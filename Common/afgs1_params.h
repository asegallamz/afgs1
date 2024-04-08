// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// Film grain parameter class - Stores a single set of film grain parameters
//
// Created by Segall, Andrew on 3/25/24.
//


#ifndef AFGS1_PARAMS_H
#define AFGS1_PARAMS_H

#include <list>
#include <cstdio>
#include <cstdint>

class Afgs1_film_grain_params {

public:
    // Variables
    int film_grain_param_set_idx;
    int apply_grain;
    short int grain_seed;
    int update_parameters;

    // --Frame characteristics
    int apply_horz_resolution;
    int apply_vert_resolution;
    int luma_only_flag;
    int subsampling_x;
    int subsampling_y;

    int video_signal_characteristics_flag;
    int bit_depth;
    int color_primaries;
    int transfer_characteristics;
    int matrix_coefficients;
    int video_full_range_flag;

    // --Scaling values (represented in 8 bits)
    int scaling_points_y[14][2];
    int num_y_points;  // value: 0..14

    int scaling_points_cb[10][2];
    int num_cb_points;  // value: 0..10

    int scaling_points_cr[10][2];
    int num_cr_points;  // value: 0..10

    // --AR Coefficients
    int scaling_shift;  // values : 8..11

    int ar_coeff_lag;  // values:  0..3

    // 8 bit values
    int ar_coeffs_y[24];
    int ar_coeffs_cb[25];
    int ar_coeffs_cr[25];

    // Shift value: AR coeffs range
    // 6: [-2, 2)
    // 7: [-1, 1)
    // 8: [-0.5, 0.5)
    // 9: [-0.25, 0.25)
    int ar_coeff_shift;  // values : 6..9

    int cb_mult;       // 8 bits
    int cb_luma_mult;  // 8 bits
    int cb_offset;     // 9 bits

    int cr_mult;       // 8 bits
    int cr_luma_mult;  // 8 bits
    int cr_offset;     // 9 bits

    int overlap_flag;

    int clip_to_restricted_range;

    int chroma_scaling_from_luma;

    int grain_scale_shift;

public:
    Afgs1_film_grain_params();

    void load_params(FILE* fp, int64_t* start_time, int64_t* end_time);

    bool operator==(const Afgs1_film_grain_params &rhs) const;

    bool operator!=(const Afgs1_film_grain_params &rhs) const;

};
#endif //AFGS_T35_AFG1_PARAMS_H