// This software is derived from the HM reference software, which is subject
// to the terms below.
//
// The copyright in this software is being made available under the BSD
// License, included below. This software may be subject to other third party
// and contributor rights, including patent rights, and no such rights are
// granted under this license.
//
// Copyright (c) 2010-2020, ITU/ISO/IEC
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// Any and all modifications to the software are subject to the terms below.
//
// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// SEIAFG1App - Example program to write an AFGS1 T-35 Message in an HEVC bit-stream.
//              Reads one or more "filmgrn1" parameter files, each with an associated width and height.
//              Generates an output AFGS1 T35 syntax and stores in an output file
//
//
// Usage: SEIAFGS1App --ParameterString <params_file1>,<width>,<height> --ParameterString <param_file2>,<width>,<height>
//                    --BitstreamFileIn <in_filename> --BitstreamFileOut <out_filename>
//                    --WarnUnknowParameter <warn_value>
//                    --fps <num>/<denom>
//
// Where: <params_file> is a "filmgrn1" parameter file
//        <width> is the image width associated with the params_file
//        <height> is the image height associated with the params_file
//        <in_filename> is the input bitstream filename
//        <out_filename> is the output bitstream filename
//        <warn_value> enables warnings for unknown configuration parametres instead of failing
//        <fps_num> is the numerator of the frame rate used to generate the params file
//        <fps_denom> is the denominator of the frame rate used to generate the params file
//
// Notes: 1. The "filmgrn1" parameter file may be generated using the noise_model software available with libaom
//        2. One or more input parameters may be provided

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include "SEIAfgsApp.h"
#include "program_options_lite.h"

int main(int argc, char* argv[])
{
  Int returnCode = EXIT_SUCCESS;

  // print information
  fprintf( stdout, "\n" );
  fprintf( stdout, "HM: SEIAfgs1App Version %s ", NV_VERSION );
  fprintf( stdout, NVM_ONOS );
  fprintf( stdout, NVM_COMPILEDBY );
  fprintf( stdout, NVM_BITS );

#if ENABLE_TRACING
  fprintf( stdout, "[ENABLE_TRACING] " );
#endif
  fprintf( stdout, "\n" );

  // initialize the class
  SEIAfgs1App *pcSEIApp = new SEIAfgs1App;

  // parse configuration
  if(!pcSEIApp->parseCfg( argc, argv ))
  {
    returnCode = EXIT_FAILURE;
    return returnCode;
  }

  // load the film grain database
  pcSEIApp->load_database();

  // starting time
  Double dResult;
  clock_t lBefore = clock();

  // call decoding function
  if( 0 != pcSEIApp->process() )
  {
    printf( "\n\n***ERROR*** A decoding mismatch occurred: signalled md5sum does not match\n" );
    returnCode = EXIT_FAILURE;
  }

  // ending time
  dResult = (Double)(clock()-lBefore) / CLOCKS_PER_SEC;
  printf("\n Total Time: %12.3f sec.\n", dResult);

  delete pcSEIApp;

  return returnCode;
}
