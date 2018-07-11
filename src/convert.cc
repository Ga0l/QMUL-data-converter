#include <iostream>
#include <ctime>

#include "convert.h"

oscheader_global createHeaderGlobal(){

  // Set header global
  oscheader_global hGlobal;

  // Assign values
  hGlobal.TestWord = WFGHTestWord;

  hGlobal.Version = 1;

  Char_t intID[256] = "VMESIS6136";
  std::copy(std::begin(intID),std::end(intID),std::begin(hGlobal.InstID));

  hGlobal.NumCh = 1;

  hGlobal.TimeStep = 4e-9;

  hGlobal.SampRate = 250e6;

  Char_t res[256] = "";
  std::copy(std::begin(res),std::end(res),std::begin(hGlobal.reserved));

//  // Debug printouts
//  std::cout << "createHeaderGlobal()" << std::endl;
//  std::cout << hGlobal.TestWord << std::endl
//            << hGlobal.Version  << std::endl
//            << hGlobal.InstID   << std::endl
//            << hGlobal.NumCh    << std::endl
//            << hGlobal.TimeStep << std::endl
//            << hGlobal.SampRate << std::endl
//            << hGlobal.reserved << std::endl;

  return hGlobal;
}

oscheader_ch createHeaderCh(){

  // Set header ch
  oscheader_ch hCh;

  // Assign values
  hCh.TestWord = WFCHTestWord;

  Char_t Name[16] = "Ch0";
  std::copy(std::begin(Name),std::end(Name),std::begin(hCh.name));

  hCh.NumSamp = 1024;

  hCh.NumByteSamp = 4;

  hCh.NumBitSamp = 8;

  Char_t Type[32] = "int";
  std::copy(std::begin(Type),std::end(Type),std::begin(hCh.type));

  hCh.Yscale = 1;

  hCh.Yoffset = 0;

  Char_t res[256] = "";
  std::copy(std::begin(res),std::end(res),std::begin(hCh.reserved));

//  // Debug printouts
//  std::cout << "createHeaderCh()" << std::endl;
//  std::cout << hCh.TestWord      << std::endl
//            << hCh.NumSamp       << std::endl
//            << hCh.NumByteSamp   << std::endl
//            << hCh.NumBitSamp    << std::endl
//            << hCh.type          << std::endl
//            << hCh.Yscale        << std::endl
//            << hCh.Yoffset       << std::endl
//            << hCh.reserved      << std::endl;

  return hCh;
}

oscheader_event createHeaderEvt(){

  // Set header event
  oscheader_event hEvent;

  // Assign values
  hEvent.TestWord = WFEHTestWord;
  hEvent.unixtime = (Double_t) std::time(0);
  Char_t res[32] = "";
  std::copy(std::begin(res),std::end(res),std::begin(hEvent.reserved));

//  // Debug printouts
//  std::cout << "createHeaderEvt()" << std::endl;
//  std::cout << hEvent.TestWord    << std::endl
//            << hEvent.unixtime    << std::endl
//            << hEvent.reserved    << std::endl;

  return hEvent;
}
