### XIA2tree usage guide

This document explains how to use `XIA2tree` from the command line, how to structure the YAML configuration/calibration file, and some common analysis workflows.

---

### Basic invocation

The minimal invocation is:

```bash
XIA2tree -i /path/to/raw/file(s) -o output.root -C /path/to/config.yaml
```

This will:

- Read one or more XIA list-mode files (given with `-i`).
- Use the YAML configuration/calibration file given with `-C`.
- Write all histograms to `output.root`.

To also write all selected events to a TTree in the output file, add the `-t` flag:

```bash
XIA2tree -i data/*.data -o run1.root -C cal/LaBr250MHz.yaml -t
```

---

### Command line options (detailed)

The full CLI is:

```text
USAGE: XIA2tree [FLAGS] [OPTIONS]

FLAGS:
    -t, --tree
    -k, --keep_traces

OPTIONS:
    -i, --input <input>
    -o, --output <output>
    -C, --CalibrationFile <CalibrationFile>
    -R, --RangeFile <RangeFile>
    -u, --userSort <userSort>
    -c, --coincidenceTime <coincidenceTime>
    -S, --SplitTime <SplitTime>
    -s, --sortType <sortType>
    -T, --Trigger <Trigger>
```

#### Required options

- **`-i`, `--input <input>`**
  - One or more XIA list-mode files (`.data`, etc.).
  - Example:

    ```bash
    -i sirius-20201211-084928.data
    -i run1.data run2.data run3.data
    ```

- **`-o`, `--output <output>`**
  - Base name of the output ROOT file.
  - If `-t` is not set, this is the final ROOT file that will contain all histograms.
  - If `-t` is set, internal temporary files may be used, but the final merged file is still named from `-o`.

- **`-C`, `--CalibrationFile <CalibrationFile>`**
  - YAML file describing the DAQ setup, detector mapping and calibration coefficients.
  - See “Configuration/calibration YAML” below for details.

#### Optional options

- **`-R`, `--RangeFile <RangeFile>`**
  - YAML file specifying energy–range tables for particles.
  - Used to compute particle thickness for gating and to fill related histograms.

- **`-u`, `--userSort <userSort>`**
  - Path to a user-defined sorting plugin shared library (e.g. `libTimingInvestigation.so`).
  - The library must implement the `UserSort` interface and export a factory function (see `docs/plugins.md`).

- **`-t`, `--tree`**
  - If set, XIA2tree writes a ROOT TTree containing one entry per selected event.
  - This is useful when you need to re-analyze events in ROOT or apply different selections later.

- **`-k`, `--keep_traces`**
  - Only active together with `-t`.
  - If set, XIA2tree will include traces, if available in the raw file, in the ROOT TTree.
  - Note that this option will significantly increase processing time and should only be used if you intend to preserve the trace data.

#### Event-building and timing options

- **`-c`, `--coincidenceTime <coincidenceTime>`**
  - Width of the coincidence window (in ns) around each trigger.
  - Hits within this window relative to the trigger time are grouped into a single event (for `coincidence` and `time` sort types).
  - No effect if `-s gap` is used.

- **`-S`, `--SplitTime <SplitTime>`**
  - Time gap (in ns) used to identify separate clusters in the `Splitter` stage.
  - If the gap between two consecutive hits is greater than `SplitTime`, they belong to different clusters.
  - This usually does not need to be changed for normal runs.

- **`-s`, `--sortType <sortType>`**
  - Defines how events are built:
    - `coincidence` (default):
      - For each trigger hit, create an event with all hits within `coincidenceTime` of that trigger.
      - Trigger detector type is given by `-T`.
    - `time`:
      - Same as `coincidence`, but only uses detector ID `0` of the trigger type as triggers.
      - Often used for time-alignment or calibration runs.
    - `gap`:
      - Events are defined by time gaps only: a new event starts when the time difference between hits exceeds `SplitTime`.
      - If a trigger detector type is provided, clusters without that detector may be discarded.

- **`-T`, `--Trigger <Trigger>`**
  - Detector type used as the event trigger.
  - Typical values:
    - `labr`: LaBr$_3$:Ce detectors.
    - `deDet` (default): front $\Delta$E detectors.
    - `eDet`: back E-detectors.

---

### Configuration/calibration YAML

The calibration/configuration file passed with `-C` is essential: it describes the DAQ hardware layout, calibration constants and (optionally) analysis parameters.

Although experiment-specific files will vary, they share a common structure, illustrated below.

#### Top-level structure

```yaml
setup:
  detectors:
    labr: 30
    deDet: 64
    eDet: 8
    eGuard: 8

  crates:
    - crate: 0
      slots:
        - slot: 2
          channels: 16
          speed: 250
          bits: 14
          detectors:
            - channel: 0
              type: labr
              detectorID: 0
            - channel: 1
              type: labr
              detectorID: 1
        # ...

calibration:
  labr:
    quad:  [0.0, 0.0, ...]
    gain:  [1.0, 1.0, ...]
    shift: [0.0, 0.0, ...]
    time_shift: [0.0, 0.0, ...]

  deDet:
    quad:  [...]
    gain:  [...]
    shift: [...]
    time_shift: [...]

analysis:
  # Optional, see below

user_parameters:
  # Optional, experiment/user specific
```

#### `setup.detectors`

The `detectors` section gives the total number of detectors of each type:

```yaml
setup:
  detectors:
    labr: 30
    deDet: 64
    eDet: 8
    eGuard: 8
```

This allows `ConfigManager` to allocate and index internal arrays for each detector type.

#### `setup.crates`

The `crates` section maps the DAQ hardware layout:

```yaml
setup:
  crates:
    - crate: 0
      slots:
        - slot: 2
          channels: 16
          speed: 250  # MHz
          bits: 14
          detectors:
            - channel: 0
              type: labr
              detectorID: 0
            - channel: 1
              type: labr
              detectorID: 1
```

- `crate`, `slot`, `channel` identify a physical digitizer channel.
- `type` is the detector type (e.g. `labr`, `deDet`, `eDet`, `eGuard`, `ppac`, `qint`), which maps to internal enums.
- `detectorID` is the index of that detector (0-based).
- `speed` and `bits` are used to set the sampling frequency and ADC resolution.

This mapping is used to convert a raw `XIA_base_t` word into a specific detector and channel.

#### `calibration.<detector_type>`

Each detector type has its own calibration block:

```yaml
calibration:
  labr:
    quad:       [q0, q1, ...]
    gain:       [g0, g1, ...]
    shift:      [s0, s1, ...]
    time_shift: [t0, t1, ...]
```

For each detector index:

- `quad`, `gain`, `shift` are used to calibrate the raw ADC value to a physical energy.
- `time_shift` corrects detector-to-detector timing offsets.

The exact formula is encoded in `ConfigManager`, but conceptually it is:

```text
E_cal = quad * ADC^2 + gain * ADC + shift
T_cal = T_raw + time_shift
```

Other detector types (`deDet`, `eDet`, `eGuard`, `ppac`, etc.) follow the same structure.

#### `analysis` (optional)

The `analysis` section provides parameters used by the physics layer (e.g. ALFNA analysis). A typical structure is:

```yaml
analysis:
  excitation:
    a0: [ ... ]  # coefficients per ring
    a1: [ ... ]
    a2: [ ... ]

  prompt:
    lhs: -10.0
    rhs:  10.0

  background:
    lhs: -50.0
    rhs: -20.0

  particle_gate:
    lhs: 0.0
    rhs: 10.0
```

- `a0`, `a1`, `a2` give coefficients for a polynomial used to compute excitation energy from ring index and particle energy.
- `prompt` and `background` define time windows (relative to the trigger) used to form prompt/background gamma gates.
- `particle_gate` defines the allowed range of particle thickness for gating.

`OCL::UserConfiguration` reads these values and exposes helper functions to check gates and compute excitation energy.

#### `user_parameters` (optional)

This section is free-form and intended for user-specific configuration:

```yaml
user_parameters:
  my_gate_low:  1.0
  my_gate_high: 3.0
  some_flag:    true
```

The whole YAML node is passed to user sorting plugins, so they can read arbitrary experiment-specific settings without changing the core code.

---

### Typical workflows

#### 1. Time alignment / calibration run

Use this when you want to align detector times or verify timing parameters.

Example:

```bash
XIA2tree \
  -i sirius-20201211-084928.data \
  -o sirius_timeAlign.root \
  -C config.yml \
  -T labr \
  -S 1500 \
  -c 1500 \
  -s time
```

- `-s time`: only detector ID `0` of type `labr` serves as the trigger.
- `-c 1500`: coincidence window of 1500 ns around the trigger.
- `-S 1500`: clusters separated by more than 1500 ns are treated as separate groups.

Typical histograms to inspect:

- Time differences between detectors and the trigger.
- Energy spectra to check basic calibration.

Rough time alignment parameters can be extracted with the [`cal/CalibrateLaBrL.C`](cal/CalibrateLaBrL.C):
In ROOT:
1) Open the result file in ROOT: the script:
```bash
root sirius-20260207-152341.root
```
2) Load the calibration script:
```bash
root [1] .L CalibrateLaBrL.C
```
3) Run the `calibrateSpectra` function on the time alignment spectra:
```bash
root [2] calibrateSpectra(time_labr)
```

```bash
root [3] calibrateSpectra(time_si_de)
```
and 
```bash
root [4] calibrateSpectra(time_si_e)
```
Each will give back a list of time alignment coefficients.
Make sure that your reference detector have its coefficient set to zero. If `labr`, set first value to zero.

#### 2. Calibrated physics run with particle trigger

Use this for standard ALFNA-like analyses where a particle telescope defines the trigger.

Example:

```bash
XIA2tree \
  -i run*.data \
  -o run_combined.root \
  -C cal/LaBr250MHz.yaml \
  -R range_file.yaml \
  -T deDet \
  -c 800 \
  -S 2000 \
  -s coincidence \
  -t
```

- `-T deDet`: use the front ΔE detectors as trigger.
- `-c 800`: relatively narrow coincidence window for prompt particle–gamma coincidences.
- `-R range_file.yaml`: provides particle range tables for thickness calculations.

Resulting ROOT file will contain:

- ΔE–E spectra for particle identification.
- Thickness vs ring histograms.
- Gamma vs excitation energy matrices (prompt and background gated).
- Optional TTree with full events.

#### 3. Gap-based clustering (no explicit trigger)

If your use case naturally groups hits by time gaps, use `sortType = gap`:

```bash
XIA2tree \
  -i run.data \
  -o run_gap.root \
  -C config.yml \
  -S 1000 \
  -s gap
```

- Events are defined whenever the time between hits exceeds `SplitTime` (`-S`).
- There is no distinguished trigger inside each event (trigger index is `-1`).

This mode is useful for certain calibration or diagnostic runs where a trigger detector is not well-defined.

---

### Interpreting the output ROOT file

Depending on options and configuration, the final ROOT file typically contains:

- **Histograms**:
  - Per-detector energy and time spectra (both raw and calibrated).
  - ΔE–E plots and particle identification spectra.
  - Thickness, excitation energy, and multiplicity histograms.
  - Prompt/background gamma vs excitation matrices.
  - Additional histograms defined by user sorting plugins (if enabled).

- **TTree** (if `-t` is set):
  - A tree where each entry corresponds to one `Triggered_event`.
  - Branches reflect the per-hit information (e.g. detector type, ID, energy, time).
  - Exact branch layout is determined by `TTreeManager` and may evolve, but it always contains enough information to reconstruct events offline.

To inspect the output:

```bash
root run1.root
```

Then use ROOT’s browser or C++/Python interfaces to explore histograms and tree entries.

---

### Example command from `commands.md`

The repository includes an example command in `commands.md`:

```bash
./XIA2tree \
  -i /Users/vetlewi/wanjas_problems/sirius-20201211-084928/sirius-20201211-084928.data \
  -o output_root_file.root \
  -C config.yml \
  -T deDet \
  -S 1500 \
  -c 1500 \
  -s time \
  -u libTimingInvestigation.so
```

This illustrates:

- A time-alignment style run (`-s time`) with `deDet` as trigger.
- Use of a user sorting plugin (`-u libTimingInvestigation.so`) for additional analysis.
- Split and coincidence times both set to 1500 ns.

You can adapt this pattern to your own experiment by changing input paths, calibration file, and plugin library.

