#Commands for XIA2tree

To sort data with no calibration - use `config.yml`. For time alignment:
```
./XIA2tree -i /Users/vetlewi/wanjas_problems/sirius-20201211-084928/sirius-20201211-084928.data -o output_root_file.root -C config.yml -T deDet -S 1500 -c 1500 -s time -u libTimingInvestigation.so
```

`-S 1500`: Split time is 1500 ns
`-c 1500`: Coincidence time is 1500 ns

`-i`: Input files
`-o`: Output file
`-s`: Sort type (time, coincidence, gap)
`-C`: Configuration file
`-t`: Indicates if the program should output a tree
