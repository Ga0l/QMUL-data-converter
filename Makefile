# Basic Makefile

### Compilers
CC  = gcc
CXX = g++

DEBUG_LEVEL     = -g
EXTRA_CCFLAGS   = -W -Wall -std=c++11
CPPFLAGS        = $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)
CCFLAGS         = $(CPPFLAGS)

RM = rm -f

SRCDIR := src
INCDIR := include

### ROOT
ROOTCFLAGS := $(shell root-config --cflags) -DUSE_ROOT -fPIC
ROOTLIBS   := $(shell root-config --libs) -lSpectrum

### BOOST
BOOSTCFLAGS := -I/usr/include/boost/
BOOSTLIBS   := -lboost_system

CPPFLAGS  += -I$(ROOTSYS)/include -I$(INCDIR) -I$(HOME)/.local/include $(ROOTCFLAGS)
# CPPFLAGS  +=  $(BOOSTCFLAGS)
EXTRALIBS  = $(ROOTLIBS)
# EXTRALIBS += $(BOOSTLIBS)

SRCS = $(wildcard $(SRCDIR)/*.cc)
OBJS = $(subst .cc,.o,$(SRCS))

all: Data2Binary

Data2Binary: Data2Binary.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o Data2Binary Data2Binary.cc $(OBJS) $(EXTRALIBS)
	$(RM) Data2Binary.o $(OBJS)

Binary2TTree: Binary2TTree.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o Binary2TTree Binary2TTree.cc $(OBJS) $(EXTRALIBS)
	$(RM) Binary2TTree.o $(OBJS)

clean:
	$(RM) $(OBJS) Data2Binary Binary2TTree
