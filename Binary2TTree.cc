#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#include <TApplication.h>

#include <convert.h>

static void show_usage(std::string name);
static void processArgs(TApplication *theApp, int *nFiles, std::vector<std::string>& sources);
static std::vector<std::string> splitpath( const std::string& str ,
                                           const std::set<char> delimiters);

int main(int argc, char *argv[]) {

  // Parameters define by user
  // ...
  UChar_t typeUInt[32]  = "int";
  UChar_t typeUChar[32] = "unsigned char";

  // Nb files processed
  int nFiles = 0;

  // Create TApp
  TApplication theApp("App", &argc, argv);

  // Process arguments given by user and create list of files
  std::vector<std::string> sources;
  processArgs(&theApp, &nFiles, sources);

  // LOOP ON ALL FILES
  /////////////////////////

  for(int iFile=0; iFile < nFiles;iFile++) {

    // TESTS
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

    oscheader_ch bufCh;

    const unsigned int nbCh = hGlobal.NumCh;
    oscheader_ch hCh[nbCh];

    for(unsigned int iCh = 0; iCh< nbCh; iCh++) {

      input_file.read(reinterpret_cast<char *>(&bufCh), sizeof(bufCh));

      std::cout << bufCh.TestWord << std::endl
                << bufCh.NumSamp << std::endl
                << bufCh.NumByteSamp << std::endl
                << bufCh.NumBitSamp << std::endl
                << bufCh.type << std::endl
                << bufCh.Yscale << std::endl
                << bufCh.Yoffset << std::endl
                << bufCh.reserved << std::endl;

      hCh[iCh] = bufCh;

    }

    oscheader_event hEvt;

    unsigned int nbEvtRead = 0;

    while(!input_file.eof()){

      std::cout << "Reading event #" << nbEvtRead << std::endl;

      input_file.read(reinterpret_cast<char *>(&hEvt), sizeof(hEvt));

      std::cout << hEvt.TestWord << std::endl
                << hEvt.unixtime << std::endl
                << hEvt.reserved << std::endl;

      for(unsigned int iCh = 0; iCh < nbCh; iCh++) {

//        if(memcmp(hCh[iCh].type, typeUInt, 32)){
//
//          UInt_t iData;
//          for(unsigned int iSmp = 0; iSmp<hCh[iCh].NumSamp; iSmp++) {
//
//            input_file.read(reinterpret_cast<char *>(&iData), sizeof(iData));
//            std::cout << iData << " ";
//            if (iSmp % 4 == 3) std::cout << std::endl;
//          }
//
//        } else if (memcmp(hCh[iCh].type, typeUChar, 32)){

        std::cout << memcmp(hCh[iCh].type, typeUChar, 32) << std::endl;
        UChar_t cData[hCh[iCh].NumByteSamp];
        for (unsigned int iSmp = 0; iSmp < hCh[iCh].NumSamp; iSmp++) {

          input_file.read(reinterpret_cast<char *>(cData), sizeof(cData));

          for (unsigned int ichar = 0; ichar < hCh[iCh].NumByteSamp; ichar++){
            std::cout << std::dec << std::setw(4) << (UInt_t)cData[ichar] << " ";
          }

//          std::cout << (UInt_t)cData << std::endl;
//          if (iSmp % 4 == 3) std::cout << std::endl;
        }

//        } else {
//          std::cout << "SMP TYPE NOT RECOGNIZE.... Specifiy if samples are int, char, ... " << std::endl;
//          exit(0);
//        }

      } // END loop on Ch

      std::cout << std::endl;
      nbEvtRead++;

    } // END while file reach EOF

  }

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

static void processArgs(TApplication *theApp, int *nFiles, std::vector<std::string>& sources){

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    show_usage(theApp->Argv(0));
    exit(0);
  }

  std::string outputPath = "0";

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-o")) {
      outputPath = theApp->Argv(++i);
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

static std::vector<std::string> splitpath( const std::string& str ,
                                           const std::set<char> delimiters) {
  std::vector<std::string> result;

  char const* pch = str.c_str();
  char const* start = pch;
  for(; *pch; ++pch)
  {
    if (delimiters.find(*pch) != delimiters.end())
    {
      if (start != pch)
      {
        std::string str(start, pch);
        result.push_back(str);
      }
      else
      {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  return result;
}
