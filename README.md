# XIA2tree
[![MacOS](https://github.com/vetlewi/XIA2tree/actions/workflows/macos.yml/badge.svg)](https://github.com/vetlewi/XIA2tree/actions/workflows/macos.yml)
---
A package for sorting XIA list mode date at OCL

### Building
The `XIA2tree` has a minimal number of dependencies. You only need `ROOT` and `CMake` installed on your computer.
To build:
1. Download the software (from git):
```
> git clone https://github.com/vetlewi/XIA2tree
```
2. Configure with `CMake`
```
> cmake -BXIA2tree_build -SXIA2tree -DCMAKE_BUILD_TYPE=Release
```
3. Build
```
> cmake --build XIA2tree_build -j
```
And you will find the binary in the `XIA2tree_build` folder.

### Usage
Minimum to run the sort
```
> XIA2tree -i /path/to/raw/file(s) -o output_file.root -C /path/to/config_calibration_file.yaml
```
In addition, there are more options:
```
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
Where `-i` gives the input files, `-o` is the output file.
`-C` gives the calibration file.
The flag `-t` enables writing out all good events to disk as a `ROOT` tree.

The options:
* `-c`: gives the coincidence window
* `-S` (capital S): gives the split time (this can usually be left unchanged)
* `-s` (small s): defines the sort type. There are three valid values:
  * `coincidence` (default): Selects entries within the coincidence window given by time set by `-c`. Trigger detector type is given by `-T`.
  * `time`: Same as `coincidence` but only triggers on detector #0 of the type given by `-T`.
  * `gap`: Events are bundled together whenever there is a gap in timestamps larger than the value set by `-S`.
* `-T`: Trigger (default dEdet), can be
  * `labr`: LaBr$_3$:Ce detectors
  * `deDet` (default): Front $\Delta$E detectors 
  * `eDet`: Back E-detectors

### Configuration and calibration file
The sorting code requires a calibration/configuration file.
This file describes the data acquisition system, the number of detectors, what detector are connected to what DAQ channel, and the calibration of the detectors
Examples of these files are:
* `cal/LaBr250MHz.yaml` for OSCAR configured with 250 MHz XIA modules for LaBr$_3$:Ce, while the E detector was read out with a $500$ MHz module.
* `cal/LaBr500MHz.yaml` is for a OSCAR setup where the LaBr$_3$:Ce detectors are read out with 500 MHz modules and all Si detectors with 250 MHz modules.

