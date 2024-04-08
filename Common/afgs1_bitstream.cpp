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

#include <cstdlib>
#include <cassert>

#include "afgs1_bitstream.h"

// Thunking commands for backwards compatibility
void aom_wb_write_bit( BitStream *wb, int bit){
    wb->write_bit(bit);
}

void aom_wb_write_literal( BitStream *wb, int data, int bits ) {
    wb->write_literal(data, bits);
}

// Write a single set of film grain parameters
void write_film_grain_params( const Afgs1_film_grain_params *pars,
                             BitStream *wb) {

    // Film grain parameter set id
    aom_wb_write_literal(wb, pars->film_grain_param_set_idx, 3);

    // Apply grain flag
    aom_wb_write_bit(wb, pars->apply_grain);
    if (!pars->apply_grain) return;

    // Grain seed
    aom_wb_write_literal(wb, pars->grain_seed, 16);

    // Update grain flag
    aom_wb_write_bit(wb, pars->update_parameters);
    if (!pars->update_parameters) return;

    // Resolution information
    // TODO: Determine value for apply_units_resolution_log2
    int apply_units_resolution_log2 = 0;
    assert(pars->apply_horz_resolution < 1<<12);
    assert(pars->apply_vert_resolution < 1<<12);
    aom_wb_write_literal(wb, apply_units_resolution_log2, 4);
    aom_wb_write_literal(wb, pars->apply_horz_resolution, 12);
    aom_wb_write_literal(wb, pars->apply_vert_resolution, 12);

    // Luma only flag
    assert(pars->luma_only_flag == 0);
    aom_wb_write_bit(wb, pars->luma_only_flag);
    if( !pars->luma_only_flag ) {
        // - Subsampling information (only values for 1,1 have been tested)
        assert(pars->subsampling_x == 1);
        assert(pars->subsampling_y == 1);
        aom_wb_write_bit(wb, pars->subsampling_x);
        aom_wb_write_bit(wb, pars->subsampling_y);
    }

    // Video characteristics flag
    assert(pars->video_signal_characteristics_flag == 0);
    aom_wb_write_bit(wb, pars->video_signal_characteristics_flag);

    // Predict scaling flag
    // TODO: Determine if predict scaling functionality should be used
    int predict_scaling_flag = 0;
    aom_wb_write_bit(wb, predict_scaling_flag);
    assert(predict_scaling_flag==0);

    int predict_y_scaling_flag;
    int predict_cb_scaling_flag;
    int predict_cr_scaling_flag;

    if( predict_scaling_flag)
        assert(0);
    else
        predict_y_scaling_flag = 0;
    assert(predict_y_scaling_flag==0);

    if( predict_y_scaling_flag ) {
        assert(0);
    } else {

        // Scaling functions parameters - Update
        aom_wb_write_literal(wb, pars->num_y_points, 4);
        assert( pars->num_y_points <= 14);

        if (pars->num_y_points) {

            // TODO: Determine values for bitsIncr and bitsScal dynamically
            int bitsIncr = 8;
            int bitsScal = 8;

            aom_wb_write_literal(wb, bitsIncr - 1, 3);
            aom_wb_write_literal(wb, bitsScal - 5, 2);

            for (int i = 0; i < pars->num_y_points; i++) {
                int point_value_increment = 0;
                if ( i != 0 )
                    point_value_increment =
                            pars->scaling_points_y[i][0] - pars->scaling_points_y[i - 1][0];
                else
                    point_value_increment = pars->scaling_points_y[i][0];

                aom_wb_write_literal(wb, point_value_increment, bitsIncr);
                aom_wb_write_literal(wb, pars->scaling_points_y[i][1], bitsIncr);
            }
        }
    }

    // Chroma scaling from luma flag
    if (!pars->luma_only_flag)
        aom_wb_write_bit(wb, pars->chroma_scaling_from_luma);
    else
        assert(pars->chroma_scaling_from_luma==0);

    if( pars->luma_only_flag || pars->chroma_scaling_from_luma ) {
        assert(pars->num_cb_points==0);
        assert(pars->num_cr_points==0);
    }
    else {

        if(predict_scaling_flag)
            assert(0);
        else
            predict_cb_scaling_flag = 0;

        if( predict_cb_scaling_flag ) {
            assert(0);
        } else {

            aom_wb_write_literal(wb, pars->num_cb_points, 4);  // max 10
            assert(pars->num_cb_points <= 10);

            if( pars->num_cb_points ) {

                // TODO: Determine values for bitsIncr and bitsScal dynamically
                int bitsIncr = 8;
                int bitsScal = 8;

                aom_wb_write_literal(wb, bitsIncr - 1, 3);
                aom_wb_write_literal(wb, bitsScal - 5, 2);
                aom_wb_write_literal(wb, 0, 8);

                for (int i = 0; i < pars->num_cb_points; i++) {
                    int point_value_increment = 0;
                    if ( i != 0)
                        point_value_increment =
                                pars->scaling_points_cb[i][0] - pars->scaling_points_cb[i - 1][0];
                    else
                        point_value_increment = pars->scaling_points_cb[i][0];

                    aom_wb_write_literal(wb, point_value_increment, bitsIncr);
                    aom_wb_write_literal(wb, pars->scaling_points_cb[i][1], bitsIncr);
                }
            }
        }

        // CR Scaling Function
        if( predict_scaling_flag ) {
            assert(0);
        } else {
            predict_cr_scaling_flag = 0;
        }

        if( predict_cr_scaling_flag){
            assert(0);
        } else {

            aom_wb_write_literal(wb, pars->num_cr_points, 4);  // max 10
            assert(pars->num_cr_points <= 10);

            if( pars->num_cr_points ) {

                // TODO: Determine values for bitsIncr and bitsScal dynamically
                int bitsIncr = 8;
                int bitsScal = 8;

                aom_wb_write_literal(wb, bitsIncr - 1, 3);
                aom_wb_write_literal(wb, bitsScal - 5, 2);
                aom_wb_write_literal(wb, 0, 8);

                for (int i = 0; i < pars->num_cr_points; i++) {
                    int point_value_increment = 0;
                    if ( i != 0 )
                        point_value_increment =
                                pars->scaling_points_cr[i][0] - pars->scaling_points_cr[i - 1][0];
                    else
                        point_value_increment = pars->scaling_points_cr[i][0];

                    aom_wb_write_literal(wb, point_value_increment, bitsIncr);
                    aom_wb_write_literal(wb, pars->scaling_points_cr[i][1], bitsIncr);
                }
            }
        }
    }

    // Grain scaling
    aom_wb_write_literal(wb, pars->scaling_shift - 8, 2);  // 8 + value

    // ----------------AR COEFFICIENTS -----------------------
    aom_wb_write_literal(wb, pars->ar_coeff_lag, 2);
    int numPosLuma = 2 * pars->ar_coeff_lag * (pars->ar_coeff_lag + 1);

    int numPosChroma;
    if (pars->num_y_points || predict_y_scaling_flag ){
        numPosChroma = numPosLuma + 1;
        int BitsArY = 8;
        aom_wb_write_literal(wb, BitsArY - 5, 2);
        for (int i = 0; i < numPosLuma; i++)
            aom_wb_write_literal(wb, pars->ar_coeffs_y[i] + 128, BitsArY);
    }
    else {
        numPosChroma = numPosLuma;
    }

    if (pars->num_cb_points || pars->chroma_scaling_from_luma || predict_cb_scaling_flag) {
        int BitsArCb = 8;
        aom_wb_write_literal(wb, BitsArCb - 5, 2);
        for (int i = 0; i < numPosChroma; i++)
            aom_wb_write_literal(wb, pars->ar_coeffs_cb[i] + 128, BitsArCb);
    }

    if (pars->num_cr_points || pars->chroma_scaling_from_luma || predict_cr_scaling_flag) {
        int BitsArCr = 8;
        aom_wb_write_literal(wb, BitsArCr - 5, 2 );
        for (int i = 0; i < numPosChroma; i++)
            aom_wb_write_literal(wb, pars->ar_coeffs_cr[i] + 128, 8);
    }

    aom_wb_write_literal(wb, pars->ar_coeff_shift - 6, 2);
    aom_wb_write_literal(wb, pars->grain_scale_shift, 2);

    if (pars->num_cb_points && !predict_cb_scaling_flag ) {
        aom_wb_write_literal(wb, pars->cb_mult, 8);
        aom_wb_write_literal(wb, pars->cb_luma_mult, 8);
        aom_wb_write_literal(wb, pars->cb_offset, 9);
    }

    if (pars->num_cr_points && !predict_cr_scaling_flag ) {
        aom_wb_write_literal(wb, pars->cr_mult, 8);
        aom_wb_write_literal(wb, pars->cr_luma_mult, 8);
        aom_wb_write_literal(wb, pars->cr_offset, 9);
    }

    aom_wb_write_bit(wb, pars->overlap_flag);
    aom_wb_write_bit(wb, pars->clip_to_restricted_range);

    return;
}

// Write a film grain parameters payload as defined in the AFGS1 specification
void write_film_grain_payload( const Afgs1_film_grain_params* pars, BitStream *wb)
{

    int payload_bits = -1;

    // Store the current bit-stream position
    int startPosition = wb->get_position();

    // Create an initial version of the bit-stream to see how large it will be
    {
        BitStream temp;
        write_film_grain_params(pars, &temp);
        payload_bits = temp.get_position();
    }

    // Determine the payload size with the overhead of signaling the size and byte alignment info
    payload_bits += 9;
    payload_bits += (payload_bits % 8) ? 8 - (payload_bits % 8) : 0;

    // Write the size information
    int payload_size = payload_bits >> 3;
    int payload_less_than_4byte_flag = (payload_size < 4) ? 1 : 0;
    wb->write_bit(payload_less_than_4byte_flag);
    wb->write_literal(payload_size, payload_less_than_4byte_flag?2:8);

    // Write the film grain params
    write_film_grain_params(pars, wb);

    // Determine the number of bits consumed by the payload
    int currentPosition = wb->get_position();
    int payloadBits = currentPosition - startPosition;

    // Zero pad (and byte align) to the payload size
    wb->write_literal(0, payload_size * 8 - payloadBits);
    assert(payload_bits == (wb->get_position() - startPosition) );

    return;
}

#include <set>
struct size {
    int width;
    int height;

    bool operator<(const size& rhs) const
    {
        return width < rhs.width;
    }

    bool operator==(const size& rhs) const
    {
        return ( width == rhs.width ) && ( height == rhs.height );
    }

};


// Write one or more film grain parameter payloads as defined in the AFGS1 specification
void write_film_grain_param_sets( std::list<Afgs1_film_grain_params> *sets, BitStream *wb)
{

    // Conformance checks
    // - Confirm that we don't have duplicate resolutions
    {

        std::set<struct size> unique;

        for( auto p: *sets ){
            struct size s{ .width=p.apply_horz_resolution, .height=p.apply_vert_resolution };
            unique.insert(s);
        }

        if (unique.size() != sets->size()) {
            printf("Error: Multiple parameter sets have the same value for resolution\n");
            exit(1);
        }

    }

    // - Confirm that we don't have duplicate film grain parameter set ids
    {

        std::set<int> unique;

        for( auto p: *sets ){
            unique.insert(p.film_grain_param_set_idx);
        }

        if (unique.size() != sets->size()) {
            printf("Error: Multiple parameter sets have the same value for film_grain_param_set_idx\n");
            exit(1);
        }

    }

    // Write the bit-stream
    // - Signal the afgs1_enable_flag
    int afgs1_enable_flag = 1;
    wb->write_bit(afgs1_enable_flag);

    if( !afgs1_enable_flag ){
        return;
    }

    // - Add reserved bits so that av1 film_grain_payload is byte aligned */
    wb->write_literal(0, 4);

    // - Write the film gain payloads
    int num_film_grain_sets_minus_1 = sets->size() - 1;
    wb->write_literal(num_film_grain_sets_minus_1, 3);
    for( auto p : *sets){
        write_film_grain_payload(&p, wb);
    }

    return;
}
