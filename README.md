# QMUL-data-converter

This software is designed to convert ASCII data collected from VME SIS3316 at Queen Mary to the binary standard designed by Marcin and Nishimura-san.
There is also an additional software to convert binary data to TTree.

The ROOT file is written in a specific structure. The following table describe the structure of the 2 trees holding the DAQ information and the data per channel :

|PMTDataHeader             | PMTData                     |
|------------------------  |-----------------------------|
|TBranch* GlobalHeader     |UInt_t DataChXXX[nbSmp]      |
|TBranch* ChXXXHeader      |UInt_t DataChXXX[nbSmp]...   |
|TBranch* ChXXXHeader ...  |

The *ChXXX* value correspond to the channel number. 

A very quick and powerful way to check the data signal inside the PMTData tree can be do as followed inside ROOT CINT interpretor :
```bash
PMTData->Draw("DataChXXX:Iteration$", "Entry$==YYY", "L")
```
With fairly obviously **DataChXXX** the name of the channel you want to look for and XXX its number, and **Entry$==YYY** the entry number YYY you want to look at.

To compile :

```bash
make Binary2TTree
make Data2Binary
```


Use it like this :

**From ASCII (Queen Mary) to Binary**
```bash
./Data2Binary /path/to/file1.txt /path/to/file2.txt ...
```
This will create automatically a file inside an ouput/ directory with the same name + .dat

**From Binary to ROOT TTree**
```bash
./Binary2TTree /path/to/file1.dat /path/to/file2.dat ...
```
This will create automatically a file inside an ouput/ directory with the same name + .root.
Inside that root file it will be one TTree called **PMTData** filled with branches called *DataChX* with *X* the number of the according channel, see above in this documentation for the structure of the trees.

Want to analyze that newly created file ? Have a look at https://github.com/P3tru/PMTAnalysis
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE4OTMzNjY5MDZdfQ==
-->