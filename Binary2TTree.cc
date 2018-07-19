#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>

#include <boost/filesystem.hpp>

#include <convert.h>

#ifndef PRINTDATA
// #define PRINTDATA
#endif

static void show_usage(std::string name);
static void processArgs(TApplication *theApp,
                        int *nFiles,
                        std::vector<std::string>& sources,
                        std::string *outputPath = NULL);

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
void printProgress (double percentage);

int quick_pow10(int n);

int main(int argc, char *argv[]) {

  // Parameters define by user
  // ...
  std::string outputPath = "0";

  std::string typeUInt  = "int";
  std::string typeUChar = "unsigned char";

  // Nb files processed
  int nFiles = 0;

  // Create TApp
  TApplication theApp("App", &argc, argv);

  // Process arguments given by user and create list of files
  std::vector<std::string> sources;
  processArgs(&theApp, &nFiles, sources);

  // Create rootfiles and TTree
  TFile *file;
  TTree *tree;
  TTree *treeHeader;

  // LOOP ON ALL FILES
  /////////////////////////

  for(int iFile=0; iFile < nFiles;iFile++) {

    // Create output file and tree
    boost::filesystem::path p(sources[iFile]);

    std::string str;
    if(outputPath != "0") str = outputPath + "/" + p.stem().string() + ".root";
    else str = p.parent_path().string() + "/" + p.stem().string() + ".root";

    file = new TFile(str.c_str(),"RECREATE");
    std::cout << "Created " << str.c_str() << std::endl;

    tree = new TTree("PMTData","PMTData");
    treeHeader = new TTree("PMTDataHeader","PMTDataHeader");

    // READ
    std::ifstream input_file(sources[iFile], std::ios::binary);

    oscheader_global hGlobal;
    input_file.read(reinterpret_cast<char *>(&hGlobal), sizeof(hGlobal));

    std::cout << hGlobal.TestWord << std::endl
              << hGlobal.Version  << std::endl
              << hGlobal.InstID   << std::endl
              << hGlobal.NumCh    << std::endl
              << hGlobal.TimeStep << std::endl
              << hGlobal.SampRate << std::endl
              << hGlobal.reserved << std::endl;

    treeHeader->Branch("GlobalHeader",
                       &hGlobal,
                       "TestWord/i:Version/i:InstID[256]/B:NumCh/i:TimeStep/D:SampRate/D:reserved[256]/B");

    const unsigned int nbCh = hGlobal.NumCh;
    oscheader_ch hCh[nbCh];

    std::string typeCh[nbCh];

    for(unsigned int iCh = 0; iCh < nbCh; iCh++) {

      input_file.read(reinterpret_cast<char *>(&hCh[iCh]), sizeof(hCh[iCh]));
      typeCh[iCh] = hCh[iCh].type;

      std::cout << hCh[iCh].TestWord << std::endl
                << hCh[iCh].NumSamp << std::endl
                << hCh[iCh].NumByteSamp << std::endl
                << hCh[iCh].NumBitSamp << std::endl
                << hCh[iCh].type << std::endl
                << hCh[iCh].Yscale << std::endl
                << hCh[iCh].Yoffset << std::endl
                << hCh[iCh].reserved << std::endl;

      treeHeader->Branch(Form("Ch%dHeader", iCh),
                         &hCh[iCh],
                         "TestWord/i:name[16]/B:NumSamp/i:NumByteSamp/i:NumBitSamp/i:type[32]/B:Yscale/F:Yoffset/F:reserved[256]/B");

    }

    treeHeader->Fill();
    treeHeader->Write();

    oscheader_event hEvt;

    unsigned int nbEvtRead = 0;

    UInt_t *data[nbCh];
    for(unsigned int iCh = 0; iCh< nbCh; iCh++) {
      data[iCh] = new UInt_t[hCh[iCh].NumSamp];
      tree->Branch(Form("DataCh%d",iCh), data[iCh], Form("Data[%d]/i",hCh[iCh].NumSamp));
    }

    while(!input_file.eof()){

      input_file.read(reinterpret_cast<char *>(&hEvt), sizeof(hEvt));

      for(unsigned int iCh = 0; iCh < nbCh; iCh++) {

        for(unsigned int iSmp = 0; iSmp<hCh[iCh].NumSamp; iSmp++) {

          UInt_t iData = 0;

          if(typeCh[iCh] == typeUInt){

            input_file.read(reinterpret_cast<char *>(&iData), sizeof(iData));

          } else if(typeCh[iCh] == typeUChar) {

            UChar_t cData[hCh[iCh].NumByteSamp];
            input_file.read(reinterpret_cast<char *>(cData), sizeof(cData));

            for (unsigned int ichar = 0; ichar < hCh[iCh].NumByteSamp; ichar++){
              iData += (UInt_t)cData[ichar] * quick_pow10(hCh[iCh].NumByteSamp-ichar-1);
            }

          } else {

            std::cout << "SMP TYPE - " << typeCh[iCh]
                      << " - NOT RECOGNIZE.... Specifiy if samples are int, char, ... "
                      << std::endl;
            return -1;

          }

          data[iCh][iSmp] = iData;

        } // END loop on smp

      } // END loop on Ch

      tree->Fill();
      nbEvtRead++;

    } // END while file reach EOF

    if(nbCh == 1) tree->Write();
    file->Close();

  } // END loop iFile

  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "##################" << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;

}

static void show_usage(std::string name){
  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-o\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;
}

static void processArgs(TApplication *theApp,
                        int *nFiles,
                        std::vector<std::string>& sources,
                        std::string *outputPath){

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    show_usage(theApp->Argv(0));
    exit(0);
  }

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-o")) {
      *outputPath = theApp->Argv(++i);
    } else {
      if (i + 1 > theApp->Argc() && *nFiles == 0) {
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        show_usage(theApp->Argv(0));
        exit(0);
      } else {
        std::cout << "Add " << arg << " to sources" << std::endl;
        sources.push_back(arg);
        (*nFiles)++;
      }
    }
  }


  if (nFiles == 0) {
    std::cout << "NO SOURCES DETECTED !" << std::endl;
    show_usage(theApp->Argv(0));
    exit(0);
  }

}

void printProgress (double percentage) {
  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush (stdout);
}

int quick_pow10(int n) {

  static int pow10[10] = {
      1, 10, 100, 1000, 10000,
      100000, 1000000, 10000000, 100000000, 1000000000
  };

  return pow10[n];
}
