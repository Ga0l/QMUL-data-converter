#include <TMath.h>

#define WFGHTestWord 0x33333333
#define WFCHTestWord 0x55555555
#define WFEHTestWord 0xAAAAAAAA

// Lecroy Osc waveform header by Marcin

const Int_t n_oscheader_global = 4+4+256+4+8+8+256;
const Int_t n_oscheader_ch     = 4+16+4+4+4+32+4+4+256;
const Int_t n_oscheader_event  = 4+8+32;

#pragma pack(push)
#pragma pack(1)

// Global header
typedef struct {
  UInt_t TestWord; //, 4 bytes
  UInt_t Version; //, 4 bytes
  Char_t InstID[256];//256 bytes
  UInt_t NumCh; //, 4 bytes
  Double_t TimeStep; //[s],
  Double_t SampRate; //
  Char_t reserved[256];//256 bytes
} oscheader_global;

//Channel header
typedef struct {
  UInt_t TestWord; //, 32-bit
  Char_t name[16];//16 bytes
  UInt_t NumSamp; //, 32-bit
  UInt_t NumByteSamp; //, size
  UInt_t NumBitSamp; //, precision
  Char_t type[32];//32 bytes
  Float_t Yscale; //[volts per LSB], 32-bit
  Float_t Yoffset; //[volts], 32-bit
  Char_t reserved[256];//256 bytes
} oscheader_ch;

//Event header
typedef struct {
  UInt_t TestWord;//4 bytes
  Double_t unixtime; //[s], 32-bit
  Char_t reserved[32];//32 bytes
} oscheader_event;

#pragma pack(pop)

oscheader_global createHeaderGlobal();

oscheader_ch createHeaderCh();

oscheader_event createHeaderEvt();
