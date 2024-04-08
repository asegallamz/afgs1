// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// T35AFG1App - Example program to write the AFGS1 syntax.
//              Reads one or more "filmgrn1" parameter files, each with an associated width and height.
//              Generates an output AFGS1 T35 syntax and stores in an output file
//
//
// Usage: T35AFGS1App --input <params_file1>,<width>,<height> --input <param_file2>,<width>,<height>
//                    --fps <num>/<denom>
//                    --output_frame <output_frame>
//                    --output <file_name>
//
// Where: <params_file> is a "filmgrn1" parameter file
//        <width> is the image width associated with the params_file
//        <height> is the image height associated with the params_file
//        <fps_num> is the numerator of the frame rate used to generate the params file
//        <fps_denom> is the denominator of the frame rate used to generate the params file
//        <output_frame> is the frame number to be used for output
//        <file_name> is the output file name
//
// Notes: 1. The "filmgrn1" parameter file may be generated using the noise_model software available with libaom
//        2. One or more input parameters may be provided

#include <cstdio>
#include <cstdlib>
#include "afgs1_database.h"
#include "afgs1_bitstream.h"

int main(int argc, char **argv) {

    Afgs1_film_grain_database afgs_db;
    int frame_rate_num = -1;
    int frame_rate_denom = -1;
    int output_frame_num = -1;
    char *output_filename = NULL;

    // Simple command line processing.
    for( int i=0; i<argc; i++ ){

        // Process an input parameter file.  Note that the input is loaded and inserted into a database.
        if(strncmp( "--input", argv[i], 8) == 0) {

            if( i == argc){
                printf("Error: --input must be followed by parameter\n");
                return 1;
            }

            char *file_name = strtok( argv[++i], ",");
            int width = atoi(strtok( NULL, ","));
            int height = atoi(strtok( NULL, ","));

            afgs_db.load_table(file_name, width, height);

        }
        // Process the frame rate.  This is needed to determine the mapping between parameter sets
        // and frame numbers, as the filmgrn1 files stores data relative to presentation time.
        else if(strncmp( "--fps", argv[i], 6) == 0) {

            if( i == argc){
                printf("Error: --fps must be followed by parameter\n");
                return 1;
            }

            frame_rate_num = atoi( strtok(argv[++i], "/") );
            frame_rate_denom = atoi( strtok( NULL, "/") );
            assert( frame_rate_num > 0 );
            assert( frame_rate_denom > 0 );
        }
        // Process the frame number
        else if(strncmp( "--output_frame", argv[i], 15) == 0) {

            if( i == argc){
                printf("Error: --output_frame must be followed by parameter\n");
                return 1;
            }

            output_frame_num = atoi( argv[++i] );
            assert( output_frame_num > 0 );
        }
        // Process the output file name
        else if(strncmp( "--output", argv[i], 9) == 0){

            if( i == argc){
                printf("Error: --output must be followed by parameter\n");
                return 1;
            }

            output_filename = argv[++i];
        }

    }

    // Create an AFGS1 bit-stream
    // - Extract the parameters for output_frame_num from the database.
    // -- The output frame number is converted to a presentation time as defined in the filmgrn1 file.
    std::list<Afgs1_film_grain_params> afgs1_film_grain_param_sets =
            afgs_db.find_frames(output_frame_num * 10000000ULL * frame_rate_denom / frame_rate_num);

    // - Write the AFGS1 syntax to the write_buffer object
    BitStream write_buffer;
    write_film_grain_param_sets(&afgs1_film_grain_param_sets, &write_buffer);

    // - Output the buffer to a file
    write_buffer.write_stream_to_file(output_filename);

    return 0;
}
