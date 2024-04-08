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

#ifndef __SEIAFGS1APPCFG__
#define __SEIAFGS1APPCFG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TLibCommon/CommonDef.h"
#include <vector>

// Struct for storing frame rate ino
struct frameRateInfo {
    bool command_line_value = false;
    int numerator = -1;
    int denominator = -1;
};

/// Decoder configuration class
class SEIAfgs1AppCfg
{
protected:
  std::string   m_bitstreamFileNameIn;                ///< output bitstream file name
  std::string   m_bitstreamFileNameOut;               ///< input bitstream file name
  std::string   m_parameterString;                    ///< parameter file info: <width>,<height>,<filename>
  std::string   m_frameRateString;                    ///< frame rate info: <frame_rate_num>/<frame_rate_denom>

  struct parameterFileInfo {
      unsigned width;
      unsigned height;
      std::string filename;
  };
  std::vector<struct parameterFileInfo> m_parameterFileInfo;

  frameRateInfo m_frameRateInfo;

public:
  SEIAfgs1AppCfg();
  virtual ~SEIAfgs1AppCfg();

  Bool  parseCfg        ( Int argc, TChar* argv[] );   ///< initialize option class from configuration
};


#endif  // __SEIAFGS1APPCFG__


