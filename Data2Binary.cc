#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#include <TApplication.h>

#include <boost/filesystem.hpp>

#include <convert.h>

static void show_usage(std::string name);
static void processArgs(TApplication *theApp,
                        int *nFiles,
                        std::vector<std::string>& sources,
                        std::string *outputPath = NULL);

int main(int argc, char *argv[]) {

  // Parameters define by user
  // ...
  std::string outputPath = "0";

  // Nb files processed
  int nFiles = 0;

  // Create TApp
  TApplication theApp("App", &argc, argv);

  // Process arguments given by user and create list of files
  std::vector<std::string> sources;
  processArgs(&theApp, &nFiles, sources, &outputPath);

  // LOOP ON ALL FILES
  /////////////////////////

  for(int iFile=0; iFile < nFiles;iFile++) {

    // Parameter to read line from input data file
    std::string line;
    std::ifstream inputFile;
    inputFile.open(sources[iFile].c_str());

    // Create output file
    boost::filesystem::path p(sources[iFile]);

    std::string str;
    if(outputPath != "0") str = outputPath + "/" + p.stem().string() + ".dat";
    else str = p.parent_path().string() + "/" + p.stem().string() + ".dat";

    std::ofstream file(str.c_str(),
                       std::ios::binary);

    std::cout << "Created " << str.c_str()
              << std::endl;

    // Create relevant structures
    oscheader_global hGlobal = createHeaderGlobal();
    oscheader_ch hCh         = createHeaderCh();
    oscheader_event hEvt     = createHeaderEvt();

    // Write headers
    file.write((char*)&hGlobal, sizeof(oscheader_global));
    file.write((char*)&hCh,     sizeof(oscheader_ch));

    if (inputFile.is_open()) {

      std::cout << "Opening : " << sources[iFile].c_str() << std::endl;

      //////////////////////////////////////////
      // Creating BINARY data from ASCII data //
      //////////////////////////////////////////

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