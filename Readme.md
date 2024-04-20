# AFGS1
Implementation of the standalone Alliance for Open Media Film Grain Synthesis (AFGS1) specification.  This repository 
includes a library for creating AFGS1 bit-streams as well as two example applications.  A first application demonstrates
how to read "filmgrn1" parameter files and generate an AFGS1 payload.  A second application demonstrates how to read 
"filmgrn1" parameter files, generate an AFGS1 payload, and insert the payload into an HEVC bit-stream as an SEI message
using Recommendation ITU-T T.35 based signaling.

## Building

### Prerequisites

1. [CMake](https://cmake.org) version 3.16 or higher.
2. [Git](https://git-scm.com/).

### Get the code

The software can be downloaded with or without support for building the HEVC bit-stream example application.  Depending
on the desired behavior, please do one of the following:

#### Build library and example applications without HEVC support

~~~
    $ git clone git@github.com:asegallamz/libafgs1.git
    # By default, the above command stores the source in the libafgs1 directory:
    $ cd libafgs1
~~~

#### Build library and example applications with HEVC support

~~~
    $ git clone --recurse-submodule git@github.com:asegallamz/libafgs1.git
    # By default, the above command stores the source in the libafgs1 directory:
    $ cd libafgs1
~~~

### Basic build

The build procedure uses CMake and supports a number of options.  It is recommended to use the following process:

~~~
    $ mkdir build
    $ cd build
    $ cmake --DCMAKE_BUILD_TYPE=debug ..
    $ make
~~~

The above will generate a makefile and build the AFGS1 library and the first example application.  Alternatively,
the HEVC bit-stream insertion functionality can be enabled by using the following process:

~~~
    $ mkdir build
    $ cd build
    $ cmake --DCMAKE_BUILD_TYPE=debug -DBUILD_SEI_APP=ON ..
    $ make
~~~

## Code Overview

The software is organized into three main components.  An introduction is provided below.  

### libAFGS1
Support for the AFGS1 standard is provided in the libAFGS1 library
that is contained in the Common directory.  This is generally organized as follows:
- afgs1_params.* provides support to store the AFGS1 film grain parameters and read these parameters from a "filmgrn1" paramter file.  These "filmgrn1" parameter files can be generated using the *noise_model* utility provided in libaom.
- afgs1_bitstream.* provides support for writing the AFGS1 syntax using the film grain parameters.
- afgs1_database.* is a helper class that can manage multiple film grain parameters.  This allows for the selection of film grain parameters for a specific frame from the timeline of parameters provided in the "filmgrn1" file.
- afgs1_buffer.* is a helper class to emulate the buffering of AFGs1 parameters at a decoder.

### T35Afgs1App
The T35Afgs1App is an example application for writing the AFGS1 syntax.  It is located in the Apps/T35Afgs1App 
directory.  The example is meant to be simple, and information on how to run the program is provided in the 
comments at the top of T35Afgs1App.cpp.

*Note: The application should/will be renamed going forward, as it no longer encapsulates
the AFGS1 syntax in a T.35 message.  Instead, the application generates the AFGS1 payload
(corresponding to the av1_film_grain_param_sets() function) that could be carried in any transport mechanism - as 
defined in the AFGS1 specification.  An example of the ITU-T T.35 encapsulation is now included in the SEIAfgs1App
(see below).*

### SEIAfgs1App
The SEIAfgs1App is an application capable of inserting AFGS1 messages into an HEVC bit-stream.  The AFGS1 syntax is
encapsulated in an SEI message using the Recommendation ITU-T T.35 message syntax.  The application is located in 
the Apps/SEIAfgs1App directory.  Information on how to run the program is provided in the comments at the top of 
SEIAfgsMain.cpp.