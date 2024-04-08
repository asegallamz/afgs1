// This source code is subject to the terms of the BSD 3-Clause Clear License and
// the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
// License was not distributed with this source code in the LICENSE file, you can
// obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the Alliance
// for Open Media Patent License 1.0 was not distributed with this source code in
// the PATENTS file, you can obtain it at aomedia.org/license/patent-license/.
//
// Database class - Stores film grain parameters that can be referenced by resolution and
// display time.
//
// Created by Segall, Andrew on 3/25/24.
//

#ifndef AFGS1_DATABASE_H
#define AFGS1_DATABASE_H

#include <list>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include "afgs1_params.h"

class Afgs1_film_grain_database {

    struct record {
        int64_t start_time;
        int64_t end_time;
        Afgs1_film_grain_params params;
    };

    std::list<record> *list;

public:
    Afgs1_film_grain_database() {
        list = new std::list<record>;
    }

    ~Afgs1_film_grain_database() {
        delete list;
    }

    void load_table(const char* fname, int width, int height) {

        FILE *fp;
        fp = fopen(fname, "rb");

        static int FilmGrainParamSetIndex = -1;
        FilmGrainParamSetIndex++;

        // Check for magic header;
        static const char kFileMagic[9] = "filmgrn1";
        char magic[9];
        if (!fread(magic, 9, 1, fp) || memcmp(magic, kFileMagic, 8)) {
            fclose(fp);
            return;
        }

        while( !feof(fp) ) {
            struct record *record;
            record = new struct record;
            record->params.load_params(fp, &record->start_time, &record->end_time);

            // Set values
            record->params.apply_horz_resolution = width;
            record->params.apply_vert_resolution = height;
            record->params.subsampling_x = 1;
            record->params.subsampling_y = 1;
            record->params.video_signal_characteristics_flag = 0;
            record->params.film_grain_param_set_idx = FilmGrainParamSetIndex;

            // Store record
            list->push_back(*record);
        }
    }

    std::list<Afgs1_film_grain_params> find_frames( int poc ){

        std::list<Afgs1_film_grain_params> subset;

        for( std::list<record>::iterator it=list->begin(); it!=list->end(); ++it)
        {
            if( poc >= it->start_time && poc < it->end_time )
                subset.push_back(it->params);
        }

        return subset;
    }

    std::list<Afgs1_film_grain_params> all_frames(){

        std::list<Afgs1_film_grain_params> subset;

        for( std::list<record>::iterator it=list->begin(); it!=list->end(); ++it)
        {
            subset.push_back(it->params);
        }

        return subset;
    }


};
#endif //AFGS_T35_AFGS1_DATABASE_H