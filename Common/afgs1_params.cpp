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

#include "afgs1_params.h"

#include <cstdlib>
#include <cstdarg>
#include <cinttypes>

// Helper Functions
int error_info = 0;
#define AOM_CODEC_ERROR 0
void aom_internal_error(int a, int b, const char* fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    exit(1);
};

Afgs1_film_grain_params::Afgs1_film_grain_params() {

    apply_grain = 0;

};

/* Function to read film grain parameters from a "filmgrn1" file.
* Code is largely borrowed from libaom.
*
* Note: The file format is an ascii representation for readability and
* editability. Array parameters are separated from the non-array
* parameters and prefixed with a few characters to make for easy
* localization with a parameter set. Each entry is prefixed with "E"
* and the other parameters are only specified if "update-parms" is
* non-zero.
*
* filmgrn1
* E <start-time> <end-time> <apply-grain> <random-seed> <update-parms>
*  p <ar_coeff_lag> <ar_coeff_shift> <grain_scale_shift> ...
*  sY <num_y_points> <point_0_x> <point_0_y> ...
*  sCb <num_cb_points> <point_0_x> <point_0_y> ...
*  sCr <num_cr_points> <point_0_x> <point_0_y> ...
*  cY <ar_coeff_y_0> ....
*  cCb <ar_coeff_cb_0> ....
*  cCr <ar_coeff_cr_0> ....
* E <start-time> ...
*/
void Afgs1_film_grain_params::load_params(FILE* file, int64_t* start_time, int64_t* end_time){

     Afgs1_film_grain_params *pars = this;

    int num_read = fscanf(file, "E %" PRId64 " %" PRId64 " %d %hd %d\n", start_time,
                          end_time, &pars->apply_grain, &pars->grain_seed,
                          &pars->update_parameters);
    if (num_read == 0 && feof(file)) return;
    if (num_read != 5) {
        aom_internal_error(error_info, AOM_CODEC_ERROR,
                           "Unable to read entry header. Read %d != 5", num_read);
        return;
    }
    if (pars->update_parameters) {
        num_read = fscanf(file, "p %d %d %d %d %d %d %d %d %d %d %d %d\n",
                          &pars->ar_coeff_lag, &pars->ar_coeff_shift,
                          &pars->grain_scale_shift, &pars->scaling_shift,
                          &pars->chroma_scaling_from_luma, &pars->overlap_flag,
                          &pars->cb_mult, &pars->cb_luma_mult, &pars->cb_offset,
                          &pars->cr_mult, &pars->cr_luma_mult, &pars->cr_offset);
        if (num_read != 12) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable to read entry params. Read %d != 12",
                               num_read);
            return;
        }
        if (!fscanf(file, "\tsY %d ", &pars->num_y_points)) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable to read num y points");
            return;
        }
        for (int i = 0; i < pars->num_y_points; ++i) {
            if (2 != fscanf(file, "%d %d", &pars->scaling_points_y[i][0],
                            &pars->scaling_points_y[i][1])) {
                aom_internal_error(error_info, AOM_CODEC_ERROR,
                                   "Unable to read y scaling points");
                return;
            }
        }
        if (!fscanf(file, "\n\tsCb %d", &pars->num_cb_points)) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable to read num cb points");
            return;
        }
        for (int i = 0; i < pars->num_cb_points; ++i) {
            if (2 != fscanf(file, "%d %d", &pars->scaling_points_cb[i][0],
                            &pars->scaling_points_cb[i][1])) {
                aom_internal_error(error_info, AOM_CODEC_ERROR,
                                   "Unable to read cb scaling points");
                return;
            }
        }
        if (!fscanf(file, "\n\tsCr %d", &pars->num_cr_points)) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable to read num cr points");
            return;
        }
        for (int i = 0; i < pars->num_cr_points; ++i) {
            if (2 != fscanf(file, "%d %d", &pars->scaling_points_cr[i][0],
                            &pars->scaling_points_cr[i][1])) {
                aom_internal_error(error_info, AOM_CODEC_ERROR,
                                   "Unable to read cr scaling points");
                return;
            }
        }

        if (fscanf(file, "\n\tcY")) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable to read Y coeffs header (cY)");
            return;
        }
        const int n = 2 * pars->ar_coeff_lag * (pars->ar_coeff_lag + 1);
        for (int i = 0; i < n; ++i) {
            if (1 != fscanf(file, "%d", &pars->ar_coeffs_y[i])) {
                aom_internal_error(error_info, AOM_CODEC_ERROR,
                                   "Unable to read Y coeffs");
                return;
            }
        }
        if (fscanf(file, "\n\tcCb")) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable to read Cb coeffs header (cCb)");
            return;
        }
        for (int i = 0; i <= n; ++i) {
            if (1 != fscanf(file, "%d", &pars->ar_coeffs_cb[i])) {
                aom_internal_error(error_info, AOM_CODEC_ERROR,
                                   "Unable to read Cb coeffs");
                return;
            }
        }
        if (fscanf(file, "\n\tcCr")) {
            aom_internal_error(error_info, AOM_CODEC_ERROR,
                               "Unable read to Cr coeffs header (cCr)");
            return;
        }
        for (int i = 0; i <= n; ++i) {
            if (1 != fscanf(file, "%d", &pars->ar_coeffs_cr[i])) {
                aom_internal_error(error_info, AOM_CODEC_ERROR,
                                   "Unable to read Cr coeffs");
                return;
            }
        }
        (void)fscanf(file, "\n");
    }

}

bool Afgs1_film_grain_params::operator==(const Afgs1_film_grain_params &rhs) const {
    if( film_grain_param_set_idx == rhs.film_grain_param_set_idx &&
           apply_grain == rhs.apply_grain &&
           //grain_seed == rhs.grain_seed &&
           update_parameters == rhs.update_parameters &&
           apply_horz_resolution == rhs.apply_horz_resolution &&
           apply_vert_resolution == rhs.apply_vert_resolution &&
           luma_only_flag == rhs.luma_only_flag &&
           subsampling_x == rhs.subsampling_x &&
           subsampling_y == rhs.subsampling_y &&
           video_signal_characteristics_flag == rhs.video_signal_characteristics_flag &&
           bit_depth == rhs.bit_depth &&
           color_primaries == rhs.color_primaries &&
           transfer_characteristics == rhs.transfer_characteristics &&
           matrix_coefficients == rhs.matrix_coefficients &&
           video_full_range_flag == rhs.video_full_range_flag &&
           num_y_points == rhs.num_y_points &&
           //scaling_points_y == rhs.scaling_points_y &&
           num_cb_points == rhs.num_cb_points &&
           //scaling_points_cb == rhs.scaling_points_cb &&
           num_cr_points == rhs.num_cr_points &&
           //scaling_points_cr == rhs.scaling_points_cr &&
           scaling_shift == rhs.scaling_shift &&
           ar_coeff_lag == rhs.ar_coeff_lag &&
           //ar_coeffs_y == rhs.ar_coeffs_y &&
           //ar_coeffs_cb == rhs.ar_coeffs_cb &&
           //ar_coeffs_cr == rhs.ar_coeffs_cr &&
           ar_coeff_shift == rhs.ar_coeff_shift &&
           cb_mult == rhs.cb_mult &&
           cb_luma_mult == rhs.cb_luma_mult &&
           cb_offset == rhs.cb_offset &&
           cr_mult == rhs.cr_mult &&
           cr_luma_mult == rhs.cr_luma_mult &&
           cr_offset == rhs.cr_offset &&
           overlap_flag == rhs.overlap_flag &&
           clip_to_restricted_range == rhs.clip_to_restricted_range &&
           chroma_scaling_from_luma == rhs.chroma_scaling_from_luma &&
           grain_scale_shift == rhs.grain_scale_shift ) {

        // Check the Scaling points
        for( int i = 0; i < num_y_points; i++ ) {
            if (scaling_points_y[i][0] != rhs.scaling_points_y[i][0]) { return 0; }
            if (scaling_points_y[i][1] != rhs.scaling_points_y[i][1]) { return 0; }
        }

        for( int i = 0; i < num_cb_points; i++ ) {
            if (scaling_points_cb[i][0] != rhs.scaling_points_cb[i][0]) { return 0; }
            if (scaling_points_cb[i][1] != rhs.scaling_points_cb[i][1]) { return 0; }
        }

        for( int i = 0; i < num_cr_points; i++ ) {
            if (scaling_points_cr[i][0] != rhs.scaling_points_cr[i][0]) { return 0; }
            if (scaling_points_cr[i][1] != rhs.scaling_points_cr[i][1]) { return 0; }
        }

        // Check the AR coefficients
        int numPosLuma = 2 * ar_coeff_lag * (ar_coeff_lag + 1);
        for( int i = 0; i < numPosLuma; i++ )
            if( ar_coeffs_y[i] != rhs.ar_coeffs_y[i] ){ return 0; }

        int numPosChroma = (num_y_points ) ? numPosLuma + 1 : numPosLuma;
        for( int i = 0; i < numPosChroma; i++ )
            if( ar_coeffs_cb[i] != rhs.ar_coeffs_cb[i] ){ return 0; }

        for( int i = 0; i < numPosChroma; i++ )
            if( ar_coeffs_cr[i] != rhs.ar_coeffs_cr[i] ){ return 0; }

        return 1;
    }

    return 0;
}

bool Afgs1_film_grain_params::operator!=(const Afgs1_film_grain_params &rhs) const {
    return !(rhs == *this);
};



