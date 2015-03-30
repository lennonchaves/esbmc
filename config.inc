# Default configuration is linux; alternately, uncomment one of the following
# variables. If building on windows, you must have g++ installed for dependancy
# generation, and build in a cygwin environment.

ifndef EXTERN_ESBMC_CONFIG # Don't overwrite configuration given by environment

# These should be autodetected; one can also override them if needed.
#LINUX=1
#WIN_MINGW32=1
#ONAMAC
#
#TARGET64=1
#TARGET32=1

# Static? (Linux only)
#STATICLINK=1

# What sat solvers to use? The following have native (in ESBMC) support.
# There's also the 'smtlib' backend that will print a benchmark, and that
# attempts to interpret a counterexample if it's hooked up with a solver
# process via a pipe; however it's slightly shakey.
USE_SOLVER_Z3=1
USE_SOLVER_BOOLECTOR=1
#USE_SOLVER_MINISAT=1
#USE_SOLVER_MATHSAT=1
#USE_SOLVER_CVC=1
#USE_SOLVER_YICES=1

endif #EXTERN_ESBMC_CONFIG

# Use eigen lib for digital filters verification
#USE_EIGEN_LIB=1

# If no OS or patform was specified, try to guess
include $(ESBMCDIR)/scripts/os_detect.inc

# Defaults:

CC=     ccache gcc
CXX=    ccache g++
DEPCXX= ccache g++
LD=     ld
AR=     ar
YACC   = bison -y
YFLAGS = -v
LEX    = flex
BLOBGENLD= ld
PARTLINKLD= ld
PARTLINKOUT= -o
PARTLINKOPTS= -r $(LDFLAGS)
FINLINKBIN= $(CXX)
FINLINKOUT= -o
FINLINKTAIL= $(CXXFLAGS) $(LIBS) $(PLATLIBS)
PATHSEP=/

# If on a 64 bit machine, build C libraries as expected. If on 32 bits, build
# everything 32 bits. This is because you can't get 64 bit headers (easily) on
# 32 bit machines, wheras you can vice versa.
FULLUNAME:=$(shell uname -a)
WEARE64BIT:=$(shell uname -a | egrep "(CYGWIN.*WOW64|x86_64)")
ifneq ($(WEARE64BIT),)
CLIB64FLAG=--64
CLIB32FLAG=--32
else
CLIB64FLAG=--32
CLIB32FLAG=--32
endif

# Extract a sat solver location from {SATDIR64,SATDIR32,HOME}, depending on
# which mode we're in {64,32} and what the user has defined.

ifdef TARGET64

ifdef SATDIR64
SATDIR=$(SATDIR64)
else
SATDIR=$(HOME)
endif

else

ifdef SATDIR32
SATDIR=$(SATDIR32)
else
SATDIR=$(HOME)
endif

endif

# Set a string for makefile scripts to use that tells them what arch we're
# building on.

ifdef TARGET64
BITWIDTH_STR='64'
else
BITWIDTH_STR='32'
endif

ifdef LINUX
include $(ESBMCDIR)/scripts/linux.inc
endif

ifdef WIN_MINGW32
include $(ESBMCDIR)/scripts/mingw.inc
endif

ifdef ONAMAC
include $(ESBMCDIR)/scripts/mac.inc
endif

# Unless someone else has set OBJDIR, default to `pwd`/.objs
OBJDIR?= .objs

# If we're cross compiling, we need a different hostobjdir.
ifdef CROSSCOMPILE
HOSTOBJDIR= .hostobjs
else
HOSTOBJDIR= $(OBJDIR)
endif

# Danger: evaluating expression creates directory :o
CREATEOBJDIR := $(shell mkdir $(OBJDIR) 2>/dev/null)
CREATEOBJDIR2 := $(shell mkdir $(HOSTOBJDIR) 2>/dev/null)