// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// Buffer class - Emulate the AFGS1 buffer that stores previously transmitted
// parameters.  These parameters may be used for prediction.
//
// Created by Segall, Andrew on 3/25/24.
//

#include <cassert>
#include "afgs1_buffer.h"

Afgs1_buffer::Afgs1_buffer()
{
    clear_buffer();
}

void Afgs1_buffer::clear_buffer()
{
    for( int i=0; i < AFGS1_MAX_BUFFERSIZE; i++ )
        buffer[i].apply_grain = -1;
}

void Afgs1_buffer::update_buffer( Afgs1_film_grain_params p )
{
    if( p.apply_grain && p.update_parameters ) {
        int idx = p.film_grain_param_set_idx;
        buffer[idx] = p;
    }
}

const Afgs1_film_grain_params Afgs1_buffer::get_params( int index )
{
    assert( index >=0 && index < AFGS1_MAX_BUFFERSIZE );
    return buffer[index];
}

// Function to determine if parameters p are already in the buffer
// TODO: Observing odd behavior when decoding bit-streams with FFMPEG.  Function currently disabled.
int Afgs1_buffer::find_params( Afgs1_film_grain_params p )
{
    for( int i=0; i < AFGS1_MAX_BUFFERSIZE; i++ )
    {
        if( buffer[i] == p )
#if AFGS1_DEBUG_DISABLE_PRED
            return -1;
#else
            return i;
#endif
    }
    return -1;
}