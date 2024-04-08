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

#ifndef AFGS1_BUFFER_H
#define AFGS1_BUFFER_H

#include "afgs1_params.h"

#define AFGS1_MAX_BUFFERSIZE 8
#define AFGS1_DEBUG_DISABLE_PRED 0

class Afgs1_buffer {

public:
    Afgs1_buffer();

    void clear_buffer();
    void update_buffer( Afgs1_film_grain_params params );
    const Afgs1_film_grain_params get_params( int index );
    int find_params( Afgs1_film_grain_params params );

private:
    Afgs1_film_grain_params buffer[AFGS1_MAX_BUFFERSIZE];
};

#endif //AFGS1_BUFFER_H