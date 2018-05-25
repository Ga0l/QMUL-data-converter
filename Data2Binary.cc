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

  // File delims
  std::set<char> delims{'/','.'};

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

    std::vector<std::string> parsedFileArg = splitpath(sources[iFile],delims);

    // TESTS
    std::ofstream file(Form("output/%s.dat",parsedFileArg[parsedFileArg.size()-2].c_str()),
                       std::ios::binary);

    std::cout << "Created " << Form("%s.dat",parsedFileArg[parsedFileArg.size()-2].c_str())
              << std::endl;

    oscheader_global hGlobal = createHeaderGlobal();
    oscheader_ch hCh         = createHeaderCh();
    oscheader_event hEvt     = createHeaderEvt();

    file.write((char*)&hGlobal, sizeof(oscheader_global));
    file.write((char*)&hCh,     sizeof(oscheader_ch));

    // Parameter to read line from input data file
    std::string line;
    std::ifstream inputFile;
    inputFile.open(sources[iFile].c_str());

    if (inputFile.is_open()) {

      //////////////////////////////////////////
      // Creating BINARY data from ASCII data //
      //////////////////////////////////////////

//      const int NumByteSamp = hCh.NumByteSamp;
      UInt_t data;

      int iLineRead = 0;

      while (getline(inputFile, line)) {

        std::stringstream test;
        std::string var;
        test << line;

        while (test >> var) {

          if (var == "Channel" || var.empty()){
            file.write((char*)&hEvt,sizeof(oscheader_event));
            break;
          }

          data = static_cast<UInt_t>(std::stoul(var));
          file.write(reinterpret_cast<const char *>(&data), sizeof(UInt_t));

        }

        iLineRead++;

      }

      inputFile.close();

    } // END if file open

    file.close();

  } // END for iFile

  /////////////////////////
  // ...

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
