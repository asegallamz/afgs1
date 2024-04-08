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

#include <list>
#include <vector>
#include <cstdio>

#include "SEIAfgs.h"
#include "SEIAfgsApp.h"
#include "TLibDecoder/AnnexBread.h"

SEIAfgs1App::SEIAfgs1App()
{
    // Initialize (and clear) the AFGS1 decoder buffer
    m_afgs1Buffer.clear_buffer();
}

Void SEIAfgs1App::load_database()
{
    for( auto p : m_parameterFileInfo )
        m_afgs1Database.load_table(p.filename.c_str(), p.width, p.height);
}

UInt SEIAfgs1App::process()
{

  // Open the input bit-stream
  ifstream bitstreamFileIn(m_bitstreamFileNameIn.c_str(), ifstream::in | ifstream::binary);
  if (!bitstreamFileIn)
  {
    std::cerr << "failed to open bitstream file " << m_bitstreamFileNameIn.c_str() << " for reading";
    exit(1);
  }

  // Define the output bitstream
  ofstream bitstreamFileOut(m_bitstreamFileNameOut.c_str(), ifstream::out | ifstream::binary);

  // Initialize the input byte stream
  InputByteStream bytestream(bitstreamFileIn);
  bitstreamFileIn.clear();
  bitstreamFileIn.seekg( 0, ios::beg );

  // Initialize the decoder for use in decoding the slice headers and determining the POC
  m_cTDecTop.create();
  m_cTDecTop.init();

  while (!!bitstreamFileIn)
  {
    /* location serves to work around a design fault in the decoder, whereby
     * the process of reading a new slice that is the first slice of a new frame
     * requires the SEIAfgsApp::process() method to be called again with the same
     * nal unit. */
    AnnexBStats stats = AnnexBStats();

    InputNALUnit nalu;
    byteStreamNALUnit(bytestream, nalu.getBitstream().getFifo(), stats);

    // call actual decoding function
    if (nalu.getBitstream().getFifo().empty())
    {
      /* this can happen if the following occur:
       *  - empty input file
       *  - two back-to-back start_code_prefixes
       *  - start_code_prefix immediately followed by EOF
       */
      std::cerr << "Warning: Attempt to process an empty NAL unit" <<  std::endl;
    }
    else
    {

      // Read the NALU
      // Note the fourth input to decode instructs the function to decode the HLS
      // while skipping CABAC and reconstruction
      int iSkipFrame = 0;
      int iPOCLastDisplay = 0;
      read( nalu );
      m_pcSlice = m_cTDecTop.getApcSlicePilot();
      m_cTDecTop.decode(nalu, iSkipFrame, iPOCLastDisplay, true);

      // Kludge - The decoder removes the emulation prevention bytes inside the read function (above).
      // Since we've already decoded the header and will simply copy this data back into the bitstream,
      // we add the prevention bytes back here.
      {
        vector<uint8_t> &buf = nalu.getBitstream().getFifo();
        vector<unsigned int> prevention_locations = nalu.getBitstream().getEmulationPreventionByteLocation();

        for (auto loc: prevention_locations) {
          buf.insert( buf.begin() + loc, 3 );
        }
        nalu.getBitstream().clearEmulationPreventionByteLocation();
      }

      // Reset the AFGS1 buffer on an IRAP
      if( nalu.isSlice() && m_pcSlice->isIRAP() )
          m_afgs1Buffer.clear_buffer();

      // Add the AFGS1 message before each slice
      // TODO: Should check if this is the first slice of a frame
      if( nalu.isSlice() && bitstreamFileOut ) {

          // Create the AFGS1 SEI message
          printf("Creating AFGS1 message (POC %d)\n", m_pcSlice->getPOC() );

          // --Determine the frame rate parameters in the bitstream (if not provided on the command line)
          if( !m_frameRateInfo.command_line_value ) {

              if( m_pcSlice->getVPS() )
              {
                  const TimingInfo *m_timing_info = m_pcSlice->getVPS()->getTimingInfo();
                  if( m_timing_info->getTimingInfoPresentFlag() ) {
                      m_frameRateInfo.numerator = m_timing_info->getTimeScale();
                      m_frameRateInfo.denominator = m_timing_info->getNumUnitsInTick();
                  }
              }

              if( m_pcSlice->getSPS() && m_pcSlice->getSPS()->getVuiParametersPresentFlag() )
              {
                  const TimingInfo *m_timing_info = m_pcSlice->getSPS()->getVuiParameters()->getTimingInfo();
                  if( m_timing_info->getTimingInfoPresentFlag() ) {
                      m_frameRateInfo.numerator = m_timing_info->getTimeScale();
                      m_frameRateInfo.denominator = m_timing_info->getNumUnitsInTick();
                  }
              }
          }

          // --Create the SEI message from the database
          SEIAfgs1 sei( &m_afgs1Database, m_pcSlice->getPOC(), m_frameRateInfo, m_afgs1Buffer );

          // Insert the SEI message into the output bit-stream
          // --Create the list of SEI messages
          SEIMessages SEIs;
          SEIs.push_back( sei.create_itut_t35_sei() );

          // --Write the start code
          static const UChar startCodePrefix[] = { 0,0,0,1 };
          bitstreamFileOut.write(reinterpret_cast<const char*>(startCodePrefix + 1), 3);

          // --Write the NALU
          OutputNALUnit outNalu(NAL_UNIT_PREFIX_SEI, nalu.m_temporalId);
          m_seiWriter.writeSEImessages(outNalu.m_Bitstream, SEIs, m_parameterSetManager.getActiveSPS(), false);
          NALUnitEBSP naluWithHeader(outNalu);
          bitstreamFileOut << naluWithHeader.m_nalUnitData.str();

          // Update the AFGS1 buffer
          sei.update_buffer( &m_afgs1Buffer );

      }

      // Copy the input NALU to the output bitstream
      if( bitstreamFileOut ){
          if (nalu.m_nalUnitType < 16) { stats.m_numZeroByteBytes--; }
          int iNumZeros = stats.m_numLeadingZero8BitsBytes + stats.m_numZeroByteBytes + stats.m_numStartCodePrefixBytes - 1;
          char ch = 0;
          for (int i = 0; i < iNumZeros; i++) { bitstreamFileOut.write(&ch, 1); }
          ch = 1; bitstreamFileOut.write(&ch, 1);
          bitstreamFileOut.write((const char*)nalu.getBitstream().getFifo().data(), nalu.getBitstream().getFifo().size());
      }

    }

  } // end bitstreamFileIn

  m_cTDecTop.destroy();
  return 0;
}
