# QMUL-data-converter

This software is designed to convert ASCII data collected from VME SIS6136 at Queen Mary
to the binary standard designed by Marcin and Nishimura-san.
There is also an additional software to convert binary data to TTree.

To compile :

```bash
make Binary2TTree
make Data2Binary
```


Use it like this :

**From ASCII (Queen Mary) to Binary**
```bash
./Data2Binary /path/to/file.txt
```
This will create automatically a file inside an ouput/ directory with the same name + .dat

**From Binary to ROOT TTree**
```bash
./Binary2TTree /path/to/file.dat
```
This will create automatically a file inside an ouput/ directory with the same name + .root.
Inside that root file it will be one TTree called *PMTData* filled with branches called *DataChX* with *X* the number of the according channel.
