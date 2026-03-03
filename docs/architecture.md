### XIA2tree architecture and data flow

This document explains how data flows through `XIA2tree`, from raw XIA list-mode files on disk to ROOT histograms and TTrees, and how the main components fit together.

---

### High-level overview

At a high level, `XIA2tree`:

- Reads raw XIA list-mode data files (32-bit words from XIA digitizers).
- Converts raw data into calibrated detector hits using a YAML configuration/calibration file.
- Groups hits into time-ordered clusters and then into events, based on coincidence windows and split times.
- Performs physics analysis (particle identification, excitation energy, gamma–particle coincidences, etc.).
- Fills ROOT histograms and, optionally, a ROOT TTree with selected events.
- Optionally calls user-defined sorting plugins for additional analysis.

The work is split into several *tasks* connected by lock-free queues and executed concurrently in a thread pool.

---

### Main components

- **Executable**
  - `XIA2tree` (`main.cpp`): parses command line options, loads configuration, constructs the pipeline, runs all tasks in parallel, and writes/merges ROOT output.

- **Configuration and calibration**
  - `OCL::ConfigManager`: owns the detector mapping (crate/slot/channel → detector type + ID) and calibration coefficients.
  - `OCL::UserConfiguration`: owns analysis parameters, trigger type and sort mode, gates and excitation-curve parameters.
  - `ParticleRange`: provides a mapping between energy and range, used to compute particle thickness.

- **Data model**
  - `XIA_base_t`: low-level representation of a raw XIA event word.
  - `Entry_t`: calibrated detector hit (detector type, ID, calibrated energy, corrected time, etc.).
  - `Triggered_event`: a collection of `Entry_t` belonging to one event, plus information about which hit acted as the trigger (if any).

- **Pipeline tasks**
  - `Task::XIAReader`: reads raw XIA data from disk and produces a stream of `XIA_base_t` pointers.
  - `Task::Calibrator`: converts each raw word into a calibrated `Entry_t` using `OCL::ConfigManager`.
  - `Task::Buffer`: accumulates and time-sorts hits, then flushes them as large, roughly time-ordered blocks.
  - `Task::Splitter`: splits blocks into smaller time-contiguous clusters based on a split-time gap.
  - `Task::Triggers`: constructs events around trigger hits, using coincidence windows and sort type.
  - `Task::MTSort` / `Sorters`: performs analysis on triggered events, fills histograms and optional TTrees, and delegates to user plugins.

- **ROOT integration**
  - `ThreadSafeHistograms` / `OCL::Histogram` (external library): thread-safe histogram container backed by ROOT.
  - `TTreeManager`: helper for writing triggered events to a ROOT TTree.
  - `RootWriter` / `TFileMerger`: write histograms and merge multiple ROOT files into a single output file.

---

### Data flow: from disk to ROOT

The following sections describe the flow in order, referencing the main tasks and files.

#### 1. CLI parsing and setup

1. The `XIA2tree` executable parses the command line using the `CommandLineInterface` module.
2. It validates required parameters:
   - `-i` / `--input`: one or more raw list-mode files.
   - `-o` / `--output`: output ROOT filename (base name).
   - `-C` / `--CalibrationFile`: YAML configuration/calibration file.
3. It then:
   - Loads `OCL::ConfigManager` from the YAML file.
   - Constructs a `ParticleRange` from `-R` / `--RangeFile` if provided.
   - Loads `OCL::UserConfiguration` with analysis parameters (trigger type, sort type, gates, etc.).
   - Derives the histogram and (optional) tree output file names based on `-o` and the `-t` flag.

#### 2. Task graph and queues

The processing pipeline is built from tasks connected by lock-free queues:

```text
XIAReader  ->  Calibrator  ->  Buffer  ->  Splitter  ->  Triggers  ->  MTSort (xN)
```

- Each arrow represents a concurrent queue.
- All tasks run in parallel in a `ThreadPool`, so reading, calibration, event building and histogram filling can overlap.

#### 3. XIAReader: reading raw data

- **Input**: list of files passed via `-i`.
- **Output**: queue of `const XIA_base_t*`.

For each file:

- The data file is memory-mapped into virtual memory.
- `XIAReader` scans the mapped region and, for each XIA event word/header, enqueues a pointer to it in its output queue.
- The task can run with or without progress UI, but its logical output is the same: a stream of raw XIA words.

#### 4. Calibrator: raw → calibrated hits

- **Input**: queue of `const XIA_base_t*` from `XIAReader`.
- **Output**: queue of `Entry_t`.

For each raw XIA word:

1. `ConfigManager::keep(raw)` is called to decide whether this word corresponds to a detector type that should be kept.
2. If kept, `ConfigManager::operator()(raw)`:
   - Looks up the crate/slot/channel in its detector table.
   - Determines detector type and index.
   - Applies energy calibration (`quad`, `gain`, `shift`) and time offsets (`time_shift` / CFD shift).
   - Produces a calibrated `Entry_t` containing:
     - Detector type and ID.
     - Calibrated energy and timestamp.
     - CFD information and any additional metadata (e.g. QDC).
3. The resulting `Entry_t` is enqueued for the next stage.

#### 5. Buffer: sorting and chunking hits

- **Input**: queue of `Entry_t`.
- **Output**: queue of `std::vector<Entry_t>`.

The buffer stage:

- Maintains an in-memory buffer of hits.
- Once the buffer exceeds a configurable size:
  - Sorts all buffered hits by time (using calibrated/CFD-corrected timestamps).
  - Searches for a large time gap (e.g. > 50 000 units).
  - Flushes the earliest part of the buffer as a `std::vector<Entry_t>` to the next stage.

This ensures that hits are more strictly ordered in time and that subsequent steps work with reasonably sized, time-ordered blocks.

#### 6. Splitter: time-gap clustering

- **Input**: queue of `std::vector<Entry_t>` from `Buffer`.
- **Output**: queue of `std::vector<Entry_t>`.

Within each incoming vector:

- The splitter scans neighboring hits and computes time differences.
- Whenever the gap between hits exceeds the configured split time (`-S`):
  - A new cluster is started.
- Each cluster is emitted as its own `std::vector<Entry_t>` into the next queue.

These clusters are small, time-contiguous groups of hits that are convenient for event building.

#### 7. Triggers: building events

- **Input**: queue of `std::vector<Entry_t>` from `Splitter`.
- **Output**: queue of `std::pair<std::vector<Entry_t>, int>` (the event and trigger index).

The trigger stage is controlled by:

- `-T` / `--Trigger`: detector type that defines triggers (e.g. `deDet`, `eDet`, `labr`).
- `-c` / `--coincidenceTime`: width of the coincidence window around each trigger.
- `-s` / `--sortType`: event definition mode (`coincidence`, `time`, `gap`).

For each incoming cluster, the trigger logic does one of:

- **`sortType = coincidence`**:
  - Find all hits of the trigger detector type.
  - For each trigger candidate, construct an event consisting of all hits whose times are within the coincidence window around that trigger.
  - Emit one `Triggered_event` per trigger candidate.

- **`sortType = time`**:
  - As for `coincidence`, but only hits from detector ID `0` of the trigger type are accepted as triggers.
  - Useful for time-alignment runs where a single detector serves as the reference.

- **`sortType = gap`**:
  - Events are defined by time gaps only.
  - If a trigger type is specified, clusters without any hit of that type may be discarded.
  - The event is essentially the whole cluster, with no distinguished trigger index.

Each event is represented as:

- A vector of `Entry_t` hits.
- An integer index pointing to the trigger hit inside that vector (or `-1` when there is no single trigger).

#### 8. MTSort and HistManager: analysis and histograms

- **Input**: queue of `Triggered_event` objects from the trigger stage.
- **Output**: filled histograms and, optionally, a TTree.

`Sorters` and `MTSort` provide the analysis layer:

- `Sorters` owns:
  - A shared `ThreadSafeHistograms` instance used across analysis threads.
  - One or more `MTSort` workers, each running in its own thread.
  - Optional per-thread ROOT files for trees when `-t` is enabled.
- Each `MTSort`:
  - Pulls events from the shared event queue.
  - Uses `HistManager` to:
    - Fill:
      - Per-detector histograms (time, energy, multiplicity).
      - ΔE–E plots and particle identification histograms.
      - Excitation-energy vs gamma-energy matrices.
      - Gated prompt/background spectra.
      - Other analysis histograms.
    - Compute quantities such as:
      - Particle thickness via `ParticleRange`.
      - Excitation energy via the analysis parameters.
  - Optionally writes each selected event into a TTree via `TTreeManager` if `-t` is set.
  - Delegates to the `UserSortManager` to allow user-defined plugins to inspect and analyze each event.

At the end:

- `HistManager::Flush()` is called to flush any buffered histograms.
- `UserSortManager::Flush()` is called to allow plugins to finish and write their results.

#### 9. ROOT output and file merging

Once all tasks are finished:

1. The shared `ThreadSafeHistograms` are written to the histogram file using `RootWriter`.
2. If `-t` was requested, each `MTSort` may have produced its own tree file.
3. All produced ROOT files (histograms and trees) are merged using `TFileMerger` into the final output file specified by `-o`.
4. Temporary per-thread files are deleted after the merge.

The final output is a single ROOT file containing:

- All histograms filled during the run.
- A TTree with selected events (if enabled).

---

### Concurrency model

The pipeline is designed for high throughput on multi-core machines:

- Each task (reader, calibrator, buffer, splitter, triggers, and each sorter) runs in its own thread.
- Lock-free queues are used between stages to minimize contention.
- ROOT thread safety is enabled so that multiple threads can interact with histograms safely.
- When writing trees, each sorter writes to a separate ROOT file that is merged later, avoiding contention on a single TTree.

---

### User sorting plugins in the pipeline

User sorting plugins are integrated at the `MTSort`/`HistManager` level:

- When a `Triggered_event` is processed, `HistManager` first fills the built-in histograms.
- Then, it calls `UserSortManager::FillEvent(event)`:
  - This forwards the event to the user-defined `UserSort` object (if one is loaded).
  - The plugin receives:
    - The complete event (all hits).
    - Implicit access to the shared histogram manager and user configuration.
- At the end of the run, `UserSortManager::Flush()` is called so plugins can finalize and write any remaining results.

See `docs/plugins.md` for details on writing and loading plugins.

---

### How CLI options map to architecture

Some key command line options directly configure internal components:

- `-C` / `--CalibrationFile`:
  - Loaded by `ConfigManager` and `UserConfiguration`.
  - Controls:
    - Detector mapping (which detector is on which channel).
    - Energy and time calibration coefficients.
    - Analysis gates and excitation-curve parameters.

- `-R` / `--RangeFile`:
  - Loaded by `ParticleRange`.
  - Used by `MTSort` to convert particle energy to thickness and to apply thickness gates.

- `-T` / `--Trigger`:
  - Passed to the trigger stage.
  - Determines which detector type is used as trigger when building events.

- `-c` / `--coincidenceTime` and `-S` / `--SplitTime`:
  - Configure the width of the coincidence window in the trigger stage.
  - Configure the time gap that defines separate clusters in the splitter.

- `-s` / `--sortType`:
  - Selects how events are built:
    - By coincidence around triggers.
    - By coincidence around a single reference detector.
    - By time gaps alone.

- `-u` / `--userSort`:
  - Points to a shared library implementing the `UserSort` interface.
  - Loaded into the `UserSortManager` used by `HistManager`.

Understanding how these options map onto the pipeline will help when tuning performance and interpreting results.

