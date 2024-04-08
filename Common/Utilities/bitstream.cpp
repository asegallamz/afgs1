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

#include "bitstream.h"
#include <cstdio>
#include <cassert>
#include <climits>
#include <cstdlib>

#define BUFFER_CHUNK_SIZE 256

BitStream::BitStream() {
    bit_offset = 0;
    buffer_size = BUFFER_CHUNK_SIZE;
    bit_buffer = (uint8_t*) malloc(buffer_size);
};

BitStream::~BitStream() {
    free(bit_buffer);
}

void BitStream::write_bit(int bit) {

    // Error Checking
    assert( bit == 0 || bit == 1);

    // Check that there is room in the buffer
    // TODO: Add error checking
    if( (bit_offset >> 3 ) + 1 >= buffer_size) {
        //printf("Info: Bitstream buffer is not large enough, extending.\n");
        buffer_size += BUFFER_CHUNK_SIZE;
        bit_buffer = (uint8_t*) realloc( bit_buffer, buffer_size);
    }

    // Add the bit
    const int off = (int)bit_offset;
    const int p = off / CHAR_BIT;
    const int q = CHAR_BIT - 1 - off % CHAR_BIT;
    if (q == CHAR_BIT - 1) {
        // Zero next char and write bit
        bit_buffer[p] = bit << q;
    } else {
        bit_buffer[p] &= ~(1 << q);
        bit_buffer[p] |= bit << q;
    }
    bit_offset = off + 1;
};

void BitStream::write_literal(int value, int num_bits) {

    // Error Checking
    assert( num_bits > 0 && num_bits <= 31 );
    assert( value >= 0 && value < ( 1<<num_bits ) );

    // Add the literal
    for (int bit = num_bits - 1; bit >= 0; bit--)
        write_bit( (value >> bit) & 1);
};

uint32_t BitStream::get_position() {
    return bit_offset;
}

unsigned char BitStream::get_byte(int position) {

    // Error Checking
    assert( position >= 0 && position < (bit_offset/CHAR_BIT) );

    // Return
    return bit_buffer[position];
}

void BitStream::clear() {
    bit_offset = 0;
    buffer_size = BUFFER_CHUNK_SIZE;
    bit_buffer = (uint8_t*) realloc( bit_buffer, buffer_size);
}

void BitStream::write_stream_to_file(char* fname){

    FILE *fid = fopen( fname, "wb");
    if( fid == NULL ) {
        printf("Exiting: Error opening file %s in BitStream::write_stream_to_file.", fname);
        exit(1);
    }

    fwrite( bit_buffer, sizeof(uint8_t), bit_offset >> 3, fid );

    fclose(fid);
}