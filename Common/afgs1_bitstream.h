// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// AFGS1 bitstream syntax class - Supports writing film grain parameters using the AFGS1 syntax
//
// Created by Segall, Andrew on 3/25/24.
//

#ifndef AFGS1_BITSTREAM_H
#define AFGS1_BITSTREAM_H

#include "afgs1_params.h"
#include "Utilities/bitstream.h"

void write_film_grain_param_sets( std::list<Afgs1_film_grain_params> *sets, BitStream *wb);

#endif