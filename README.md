![](https://github.com/esbmc/esbmc/workflows/ESBMC%20Build%20CI/CD%20(Full)/badge.svg)

# The ESBMC model checker

ESBMC, the efficient SMT based model checker, is a software verification tool for C and C++ code bases. The technique is sound but incomplete -- an error found by ESBMC will be correct (modulo errors in the tool), but a successful verification does not guarantee there are no errors.

To build ESBMC, please see the BUILDING file. For getting started, we recommend first reading some of the background material / publications, to understand exactly what this technique can provide, for example our SV-COMP tool papers.

The cannonical public location of ESBMCs source is on github:

    https://github.com/esbmc/esbmc

While our main website is esbmc.org

### Features

ESBMC aims to support all of C99, and detects errors in software by simulating a finite prefix of the program execution with all possible inputs. Classes of problems that can be detected include:
 * User specified assertion failures
 * Out of bounds array access
 * Illegal pointer dereferences, such as:
   * Dereferencing null
   * Performing an out-of bounds dereference
   * Double-free of malloc'd memory
   * Misaligned memory access
 * Integer overflows
 * Divide by zero
 * Memory leaks

Concurrent software (using the pthread api) is verified by explicit exploration of interleavings, producing one symbolic execution per interleaving. By default only normal errors will be checked for; one can also specify options to check concurrent programs for:
 * Deadlock (only on pthread mutexes and convars)
 * Data races (i.e. competing writes)

By default ESBMC performs a "lazy" depth first search of interleavings -- it can also encode (explicitly) all interleavings into a single SMT formula.

A number of SMT solvers are currently supported:
 * Z3 4.0+
 * Boolector 2.0+
 * MathSAT
 * CVC4
 * Yices 2.2+

In addition, ESBMC can be configured to use the SMTLIB interactive text format with a pipe, to communicate with an arbitary solver process, although not-insignificant overheads are involved.

A limited subset of C++98 is supported too -- a library modelling the STL is also available.

### Differences from CBMC

ESBMC is based on CBMC, the C bounded model checker. The primary differences between the two are that CBMC focuses on SAT-based encodings of unrolled C programs while ESBMC targets SMT; and CBMC's concurrency support is a fully symbolic encoding of a concurrent program in one SAT formulae.

The fundemental verification technique (unrolling programs to SSA then converting to a formula) is still the same in ESBMC, although the program internal representation has been had some additional types added.

# Open source

ESBMC has now been released as open source software -- mainly distributed under the terms of the Apache License 2.0. ESBMC contains a signficant amount of other peoples software, however, please see the COPYING file for an explanation of who-owns-what, and under what terms they are distributed.

We'd be extremely happy to receive contributions to make ESBMC better (under the terms of the Apache License 2.0). If you'd like to submit anything, please file a pull request against the public github repo. General discussion and release announcements will be made on the esbmc-users@googlegroups.com mailing list. To contact us about research or collaboration, please email the esbmc@ mailing list on ecs.soton.ac.uk.

### Getting started

Before start to use ESBMC, we need install some dependencies and configure the environment properly:

(1) Install dependencies

	sudo apt-get update && sudo apt-get install gperf libgmp-dev cmake bison flex gcc-multilib linux-libc-dev libboost-all-dev ninja-build python3-setuptools

(2) Download and extract Clang 9

	wget http://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz

	tar xf clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz && mv clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04 clang9

(3) Setup boolector

	git clone https://github.com/boolector/boolector && cd boolector && git reset --hard 3.2.0 && ./contrib/setup-lingeling.sh && ./contrib/setup-btor2tools.sh && ./configure.sh --prefix $PWD/../boolector-release && cd build && make -j9 && make install

(4) Setup Z3

	wget https://github.com/Z3Prover/z3/releases/download/z3-4.8.4/z3-4.8.4.d6df51951f4c-x64-ubuntu-16.04.zip && unzip z3-4.8.4.d6df51951f4c-x64-ubuntu-16.04.zip && mv z3-4.8.4.d6df51951f4c-x64-ubuntu-16.04 z3

(5) Setup MathSAT

	wget http://mathsat.fbk.eu/download.php?file=mathsat-5.5.4-linux-x86_64.tar.gz -O mathsat.tar.gz && tar xf mathsat.tar.gz && mv mathsat-5.5.4-linux-x86_64 mathsat

(6) Setup Yices 2 (GMP)

	wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz && tar xf gmp-6.1.2.tar.xz && rm gmp-6.1.2.tar.xz && cd gmp-6.1.2 && ./configure --prefix $PWD/../gmp --disable-shared ABI=64 CFLAGS=-fPIC CPPFLAGS=-DPIC && make -j4 && make install

(7) Setup Yices 2
 
	git clone https://github.com/SRI-CSL/yices2.git && cd yices2 && git checkout Yices-2.6.1 && autoreconf -fi && ./configure --prefix $PWD/../yices --with-static-gmp=$PWD/../gmp/lib/libgmp.a && make -j9 && make static-lib && make install && cp ./build/x86_64-pc-linux-gnu-release/static_lib/libyices.a ../yices/lib

(8) Setup CVC4

	wget https://github.com/CVC4/CVC4/archive/1.7.tar.gz && tar xf 1.7.tar.gz && rm 1.7.tar.gz && cd CVC4-1.7 && ./contrib/get-antlr-3.4 && ./configure.sh --optimized --prefix=../cvc4 --static --no-static-binary && cd build && make -j8 && make install
    
After setup and install all dependencies, configure CMake and build the ESBMC according to the following commands:

 (1) Configure CMake
    
    mkdir build && cd build && cmake .. -GNinja -DBUILD_TESTING=On -DENABLE_REGRESSION=On -DClang_DIR=$PWD/../clang9 -DLLVM_DIR=$PWD/../clang9 -DBUILD_STATIC=On -DBoolector_DIR=$PWD/../boolector-release -DZ3_DIR=$PWD/../z3 -DENABLE_MATHSAT=ON -DMathsat_DIR=$PWD/../mathsat -DENABLE_YICES=On -DYices_DIR=$PWD/../yices -DCVC4_DIR=$PWD/../cvc4 -DGMP_DIR=$PWD/../gmp -DCMAKE_INSTALL_PREFIX:PATH=$PWD/../release

 (2) Build ESBMC
    
       cd build && cmake --build . && ninja install

### Documentation

A limited number of classes have been marked up with doxygen documentation headers. Comments are put in the header files declaring classes and methods. HTML documation can be generated by running:

    doxygen .doxygen

Output will be in docs/html, open index.html to get started. 
