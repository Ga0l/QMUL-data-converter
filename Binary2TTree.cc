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
    oscheader_ch     hCh;
    oscheader_event  hEvt;
    input_file.read((char*)&hGlobal, sizeof(hGlobal));
    input_file.read((char*)&hCh,     sizeof(hCh));
    input_file.read((char*)&hEvt,    sizeof(hEvt));

    // Debug printouts
    std::cout << hGlobal.TestWord << std::endl
              << hGlobal.Version  << std::endl
              << hGlobal.InstID   << std::endl
              << hGlobal.NumCh    << std::endl
              << hGlobal.TimeStep << std::endl
              << hGlobal.SampRate << std::endl
              << hGlobal.reserved << std::endl;
    std::cout << hCh.TestWord     << std::endl
              << hCh.NumSamp      << std::endl
              << hCh.NumByteSamp  << std::endl
              << hCh.NumBitSamp   << std::endl
              << hCh.type         << std::endl
              << hCh.Yscale       << std::endl
              << hCh.Yoffset      << std::endl
              << hCh.reserved     << std::endl;
    std::cout << hEvt.TestWord    << std::endl
              << hEvt.unixtime    << std::endl
              << hEvt.reserved    << std::endl;



    const unsigned int nbByteSmp = hCh.NumByteSamp;
    const unsigned long int nbSmp = hCh.NumSamp;

    unsigned int nbEvtRead = 0;

    while(!input_file.eof()){

      for(unsigned long int iSmp=0; iSmp<nbSmp; iSmp++){

        Char_t hData[nbByteSmp];
        input_file.read((char*)&hData, sizeof(nbByteSmp));

        // if(iSmp % 16 == 0 ) std::cout << std::endl;
        //
        // for(unsigned int iByte=0; iByte<nbByteSmp; iByte++){

        //   if(iByte % 4 == 0 ) std::cout << " ";
        //   std::cout << hData[iByte];

        // } // END for reading each byte

      } // END for reading each smp

      // std::cout << std::endl;
      // std::cout << "Evt #" << nbEvtRead << std::endl;

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
