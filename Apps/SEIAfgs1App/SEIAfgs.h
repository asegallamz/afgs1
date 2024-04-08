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

#ifndef __SEIAFGS1__
#define __SEIAFGS1__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SEIAfgsAppCfg.h"
#include "TLibEncoder/SEIwrite.h"
#include "afgs1_buffer.h"
#include "afgs1_database.h"
#include "afgs1_bitstream.h"

using namespace std;

class SEIAfgs1 : public SEIUserDataRegistered
{
    BitStream write_buffer;
    std::list<Afgs1_film_grain_params> afgs1_film_grain_param_sets;

public:

    // Create the list of one or more film grain parameters from the database corresponding to the input
    // presentation time.  The presentation time calculation mimics what is used to generate
    // the "filmgrn1" parameter file.
    SEIAfgs1( Afgs1_film_grain_database *afgs1_db, int poc, frameRateInfo framerate_info )
    {
        int index = poc * 10000000ULL * framerate_info.denominator / framerate_info.numerator;
        afgs1_film_grain_param_sets = afgs1_db->find_frames(index);

        // The grain seed in the database is constant for each entry.  We modulate it here.
        update_grain_seed( poc );
    }

    // Create the list of one or more film grain parameters from the database corresponding to the input
    // presentation time.  Additionally, update the film grain parameters based on the status of the buffer
    // (that emulates the AFGS1 buffer at a decoder).  For example, the film grain parameters that already
    // exist in the buffer can be signaled by setting the update_parameters flag to 0.
    SEIAfgs1( Afgs1_film_grain_database *afgs1_db, int poc, frameRateInfo framerate_info, Afgs1_buffer buffer ) : SEIAfgs1( afgs1_db, poc, framerate_info )
    {
        std::list<Afgs1_film_grain_params>::iterator it;
        for( it = afgs1_film_grain_param_sets.begin(); it != afgs1_film_grain_param_sets.end(); ++it )
        {
            int index = buffer.find_params( *it );
            if( index >= 0 ) {
                it->film_grain_param_set_idx = index;
                it->update_parameters = 0;
            }
        }
    }

    // Update the AFGS1 buffer using the SEI data
    void update_buffer( Afgs1_buffer *buffer )
    {
        for( auto p : afgs1_film_grain_param_sets )
        {
            buffer->update_buffer(p);
        }
    }

    // Update the grain seed based on the POC
    void update_grain_seed( int poc )
    {
        std::list<Afgs1_film_grain_params>::iterator it;
        for( it = afgs1_film_grain_param_sets.begin(); it != afgs1_film_grain_param_sets.end(); ++it )
        {
            it->grain_seed = ( it->grain_seed + poc ) % ( (1<<16) - 1);
        }
    }

    // Encode one or more film grain parameters in an ITU-T T35 message.
    SEIUserDataRegistered *create_itut_t35_sei()
    {
        // Convert the AFGS1 parameters to a bitstream
        // Note: This corresponds to the av1_film_grain_param_sets() syntax
        write_film_grain_param_sets(&afgs1_film_grain_param_sets, &write_buffer);

        // Create the ITU-T T35 SEI message
        SEIUserDataRegistered *sei = new SEIUserDataRegistered;
        sei->m_ituCountryCode = 0xB5;
        sei->m_userData.push_back(0x58);
        sei->m_userData.push_back(0x90);
        sei->m_userData.push_back(0x01);

        int num_bytes = write_buffer.get_position() / 8;

        for( int i=0; i < num_bytes; i++ )
            sei->m_userData.push_back( write_buffer.get_byte(i) );

        // Clear the write buffer
        write_buffer.clear();

        return sei;
    };

};
#endif // __SEIAFGS1__

