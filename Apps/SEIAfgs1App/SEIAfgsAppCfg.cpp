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

#include <deque>
#include <cstring>
#include <string>
#include "SEIAfgsAppCfg.h"
#include "Utilities/program_options_lite.h"

using namespace std;
namespace po = df::program_options_lite;

Bool SEIAfgs1AppCfg::parseCfg( Int argc, TChar* argv[] )
{
  Bool do_help = false;
  Int warnUnknowParameter = 0;

  po::Options opts;
  opts.addOptions()

  ("help",                      do_help,                               false,      "this help text")
  ("c",                         po::parseConfigFile,                               "film grain configuration file name")
  ("ParameterString,p",         m_parameterString,                     string(""), "film grain parameter info <filename>,<width>,<height>,...")
  ("BitstreamFileIn,b",         m_bitstreamFileNameIn,                 string(""), "bitstream input file name")
  ("BitstreamFileOut,o",        m_bitstreamFileNameOut,                string(""), "bitstream output file name")
  ("Fps, f",                    m_frameRateString,                     string(""), "frame rate used for film grain parameter files")
  ("WarnUnknowParameter,w",     warnUnknowParameter,                   0,          "warn for unknown configuration parameters instead of failing")
  ;

  po::setDefaults(opts);
  po::ErrorReporter err;
  const list<const TChar*>& argv_unhandled = po::scanArgv(opts, argc, (const TChar**)argv, err);

  for (list<const TChar*>::const_iterator it = argv_unhandled.begin(); it != argv_unhandled.end(); it++)
  {
    std::cerr << "Unhandled argument ignored: "<< *it << std::endl;
  }

  if (argc == 1 || do_help)
  {
    po::doHelp(cout, opts);
    return false;
  }

  if (err.is_errored)
  {
    if (!warnUnknowParameter)
    {
      /* errors have already been reported to stderr */
      return false;
    }
  }

  if (m_bitstreamFileNameIn.empty())
  {
    std::cerr << "No input file specified, aborting" << std::endl;
    return false;
  }
  if (m_bitstreamFileNameOut.empty())
  {
    std::cerr << "No output file specified, aborting" << std::endl;
    return false;
  }

  if (!m_parameterString.empty()) {

    // Convert the command line to a series of tokens
    // TODO: Improve command line handling
    std::deque<std::string> v;
    char *token = strtok( const_cast<char*>(m_parameterString.c_str()), ",");
    while( token ) {
        v.push_back(string(token));
        token = strtok(nullptr, ",");
    }

    // Error checking
    if( v.size() % 3 )
    {
        std::cerr << "Parameter string must be of the form <filename>,<width>,<height>,..." << std::endl;
        return false;
    }

    // Convert to ParameterInfo structures
    while( v.size() )
    {
        parameterFileInfo* p = new parameterFileInfo;

        p->filename = v.front();
        v.pop_front();

        p->width    = stoi(v.front());
        v.pop_front();

        p->height   = stoi(v.front());
        v.pop_front();

        m_parameterFileInfo.push_back(*p);
    }

  }

    if (!m_frameRateString.empty()) {

        m_frameRateInfo.command_line_value = true;

        // Convert the command line to a series of tokens
        std::deque<std::string> v;
        char *token = strtok( const_cast<char*>(m_frameRateString.c_str()), "/");
        while( token ) {
            v.push_back(string(token));
            token = strtok(nullptr, "/");
        }

        // Error checking
        if( v.size() != 2 )
        {
            std::cerr << "Frame Rate string must be of the form <numerator>/<denominator>" << std::endl;
            return false;
        }

        // Convert to FrameRateInfo structures
        m_frameRateInfo.numerator = stoi(v.front());
        v.pop_front();

        m_frameRateInfo.denominator = stoi(v.front());
        v.pop_front();

    }

  return true;
}

SEIAfgs1AppCfg::SEIAfgs1AppCfg()
: m_bitstreamFileNameIn()
, m_bitstreamFileNameOut()
{
}

SEIAfgs1AppCfg::~SEIAfgs1AppCfg()
{
}