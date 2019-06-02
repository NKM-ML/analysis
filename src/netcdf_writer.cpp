//
// Created by xiamr on 3/19/19.
//

#include <iostream>
#include "netcdf_writer.hpp"
#include "frame.hpp"
#include "atom.hpp"
#include "gmxtrr.h"


void NetCDFWriter::open(const std::string &filename, int natom) {
    if (netcdfCreate(&NC, filename.c_str(), natom, 1)) {
        std::cerr << "Error open " << filename << std::endl;
    }
}

void NetCDFWriter::close() {
    netcdfClose(&NC);
}

void NetCDFWriter::write(const std::shared_ptr<Frame> &frame) {
    if (step == 0) {
        x = new double[NC.ncatom3];

    }
    double box[6];
    box[0] = frame->a_axis;
    box[1] = frame->b_axis;
    box[2] = frame->c_axis;
    box[3] = frame->alpha;
    box[4] = frame->beta;
    box[5] = frame->gamma;
    double *p = x;
    for (auto &atom : frame->atom_list) {
        *p = atom->x;
        p++;
        *p = atom->y;
        p++;
        *p = atom->z;
        p++;
    }
    if (netcdfWriteNextFrame(&NC, x, box)) {
        std::cerr << "Error write  mdcrd frame " << std::endl;
    }
    step++;
}