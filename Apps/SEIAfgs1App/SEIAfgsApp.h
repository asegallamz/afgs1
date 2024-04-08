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

#ifndef __SEIAFGS1APP__
#define __SEIAFGS1APP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <fstream>
#include <iostream>
#include "SEIAfgsAppCfg.h"
#include "TLibCommon/CommonDef.h"
#include "TLibCommon/TComSlice.h"
#include "TLibDecoder/NALread.h"
#include "TLibEncoder/NALwrite.h"
#include "TLibDecoder/SEIread.h"
#include "TLibEncoder/SEIwrite.h"
#include "TLibDecoder/TDecTop.h"
#include "afgs1_buffer.h"
#include "afgs1_database.h"
#include "afgs1_bitstream.h"

using namespace std;

class SEIAfgs1App : public SEIAfgs1AppCfg
{

public:
  SEIAfgs1App();
  virtual ~SEIAfgs1App()  {}

  UInt  process            (); ///< main decoding function
  Void  load_database      ();

protected:
  ParameterSetManager   m_parameterSetManager;
  SEIReader             m_seiReader;
  SEIWriter             m_seiWriter;
  TDecTop               m_cTDecTop;                     ///< decoder class
  TComSlice*            m_pcSlice;                      ///< slice header class

  Afgs1_buffer          m_afgs1Buffer;
  Afgs1_film_grain_database m_afgs1Database;

};

#endif // __SEIAFGS1APP__

