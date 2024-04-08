// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// Bit-stream class - Provides a simple interface to a bit buffer
//
// Created by Segall, Andrew on 3/25/24.
//

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>

class BitStream {

public:
    BitStream();
    ~BitStream();

    void write_bit( int bit );
    void write_literal( int value, int num_bits);
    uint32_t get_position();
    unsigned char get_byte(int position);
    void clear();
    void write_stream_to_file(char* fname);

private:
    uint8_t *bit_buffer;
    uint32_t bit_offset;
    uint32_t buffer_size;

};

#endif