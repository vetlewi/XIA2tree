# XIA2tree
[![MacOS](https://github.com/vetlewi/XIA2tree/actions/workflows/macos.yml/badge.svg)](https://github.com/vetlewi/XIA2tree/actions/workflows/macos.yml)
[![Linux](https://github.com/vetlewi/XIA2tree/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/vetlewi/XIA2tree/actions/workflows/ubuntu.yml)
---
XIA2tree is a package for sorting XIA list mode data from experiments at the Oslo Cyclotron Laboratory (OCL).  
It reads raw digitizer data, applies detector-specific calibration and time/event building, and produces ROOT histograms and optionally ROOT TTrees for further physics analysis.

### What XIA2tree does
At a high level, XIA2tree:

- Reads one or more raw XIA list-mode files and interprets the DAQ words.
- Uses a YAML configuration/calibration file to map crate/slot/channel to detector types and to apply per-channel energy/time calibration.
- Builds time-coincident events according to user-defined trigger type, coincidence window and split time.
- Fills a rich set of ROOT histograms (and optionally a ROOT TTree) for later offline analysis, including particle-identification plots and gamma–particle coincidences.
- Optionally loads user-defined sorting plugins to add custom histograms or analysis logic.

For a more detailed description of the internal pipeline and data flow, see [`docs/architecture.md`](docs/architecture.md).

### Installing
The `XIA2tree` project can be downloaded from a custom Homebrew tap. To install follow these steps:
1) Add the custom repo(tap):
```bash
brew tap vetlewi/formula
```
2) Install XIA2tree
```bash
brew install xia2tree
```
Note that to install the three first letters are not capitalized, while when running the software from command line you
need to captialize the first three letters.

### Building
The `XIA2tree` project has a minimal number of dependencies. You only need `ROOT` and `CMake` installed on your computer.

To build:

1. Download the software (from git):

```bash
git clone https://github.com/vetlewi/XIA2tree
```

2. Configure with `CMake`:

```bash
cmake -BXIA2tree_build -SXIA2tree -DCMAKE_BUILD_TYPE=Release
```

3. Build:

```bash
cmake --build XIA2tree_build -j
```

The `XIA2tree` executable will be placed in the `XIA2tree_build` folder.

### Quick start
The minimum command to run the sort is:

```bash
XIA2tree -i /path/to/raw/file(s) -o output_file.root -C /path/to/config_calibration_file.yaml
```

This will:

- Read the input XIA list-mode file(s) given with `-i`.
- Use the YAML configuration/calibration file given with `-C`.
- Write all histograms to `output_file.root`.

If you also want all “good” events written to a TTree, enable the `-t` flag:

```bash
XIA2tree -i /path/to/raw/file(s) -o output_file.root -C /path/to/config_calibration_file.yaml -t
```

See `docs/usage.md` for step-by-step usage examples and common workflows.

### Command line options

In addition to the minimal example, there are more options:

```text
USAGE: XIA2tree [FLAGS] [OPTIONS]

FLAGS:
    -t, --tree

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

Where:

- `-i` / `--input`: one or more input files.
- `-o` / `--output`: base name of the output ROOT file.
- `-C` / `--CalibrationFile`: path to the YAML calibration/configuration file.
- `-R` / `--RangeFile`: optional YAML file with particle range tables for thickness calculations.
- `-u` / `--userSort`: optional path to a user sorting shared library (`.so`) implementing extra analysis.
- `-t` / `--tree`: enable writing all selected events to a ROOT TTree in addition to histograms.

The timing and event-building options are:

- `-c`: coincidence window (in ns). Hits within this window around the trigger are grouped into one event.
- `-S` (capital S): split time (in ns). Hits separated by a larger gap in time are put into separate clusters (usually left unchanged).
- `-s` (small s): sort type. There are three valid values:
  - `coincidence` (default): selects entries within the coincidence window given by `-c`. Trigger detector type is given by `-T`.
  - `time`: same as `coincidence` but only triggers on detector `#0` of the type given by `-T` (useful for time alignment).
  - `gap`: events are bundled together whenever there is a gap in timestamps larger than `-S`.
- `-T`: trigger detector type (default `deDet`). Can be:
  - `labr`: LaBr$_3$:Ce detectors.
  - `deDet` (default): front $\Delta$E detectors.
  - `eDet`: back E-detectors.

See `docs/architecture.md` for how these parameters affect the internal pipeline.

### Configuration and calibration file
The sorting code requires a calibration/configuration file.
This file describes the data acquisition system, the number of detectors, which detectors are connected to which DAQ channels, and the calibration of the detectors.

Examples of these files are:

- `cal/LaBr250MHz.yaml` for OSCAR configured with 250 MHz XIA modules for LaBr$_3$:Ce, while the E detector was read out with a $500$ MHz module.
- `cal/LaBr500MHz.yaml` for an OSCAR setup where the LaBr$_3$:Ce detectors are read out with 500 MHz modules and all Si detectors with 250 MHz modules.

A more detailed description of the expected YAML structure (including `setup`, `calibration`, `analysis`, and `user_parameters` sections) is available in `docs/usage.md`.

### Further documentation

- `docs/architecture.md`: internal architecture and data flow (pipeline stages, threads, queues, ROOT output).
- `docs/usage.md`: detailed usage examples, YAML configuration structure, and recommended workflows.
- `docs/plugins.md`: how to write, build and load user sorting plugins.

