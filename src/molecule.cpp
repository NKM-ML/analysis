//
// Created by xiamr on 3/17/19.
//

#include "config.h"
#include "common.hpp"
#include "molecule.hpp"
#include "atom.hpp"
#include "frame.hpp"
#include "forcefield.hpp"

int Molecule::seq() {
    if (atom_list.empty()) return 0;
    return atom_list.front()->seq;
}

void Molecule::calc_mass() {
    double mol_mass = 0.0;
    for (auto &atom : atom_list) {
        mol_mass += forcefield.find_mass(atom);
    }
    mass = mol_mass;
}


std::tuple<double, double, double> Molecule::calc_weigh_center(std::shared_ptr<Frame> &frame) {
    double xmid = 0.0;
    double ymid = 0.0;
    double zmid = 0.0;
    bool first_atom = true;
    double first_x, first_y, first_z;
    double mol_mass = 0.0;
    for (auto &atom : atom_list) {
        if (first_atom) {
            first_atom = false;
            first_x = atom->x;
            first_y = atom->y;
            first_z = atom->z;
            double weigh = forcefield.find_mass(atom);
            mol_mass += weigh;
            xmid = first_x * weigh;
            ymid = first_y * weigh;
            zmid = first_z * weigh;

        } else {
            double xr = atom->x - first_x;
            double yr = atom->y - first_y;
            double zr = atom->z - first_z;
            frame->image(xr, yr, zr);
            double weigh = forcefield.find_mass(atom);
            mol_mass += weigh;
            xmid += (first_x + xr) * weigh;
            ymid += (first_y + yr) * weigh;
            zmid += (first_z + zr) * weigh;
        }
    }

    return std::make_tuple(xmid / mol_mass, ymid / mol_mass, zmid / mol_mass);
}

std::tuple<double, double, double> Molecule::calc_dipole(std::shared_ptr<Frame> &frame) {
    auto mass_center = calc_weigh_center(frame);

    double dipole_x = 0.0;
    double dipole_y = 0.0;
    double dipole_z = 0.0;

    for (auto &atom : atom_list) {
        double xr = atom->x - std::get<0>(mass_center);
        double yr = atom->y - std::get<1>(mass_center);
        double zr = atom->z - std::get<2>(mass_center);

        frame->image(xr, yr, zr);

        dipole_x += atom->charge * (xr + std::get<0>(mass_center));
        dipole_y += atom->charge * (yr + std::get<1>(mass_center));
        dipole_z += atom->charge * (zr + std::get<2>(mass_center));
    }
    return std::make_tuple(dipole_x, dipole_y, dipole_z);
}

void Molecule::calc_center(std::shared_ptr<Frame> &frame) {
    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_z = 0.0;
    bool first_atom = true;
    double first_x, first_y, first_z;
    for (auto &atom : atom_list) {
        if (first_atom) {
            first_atom = false;
            sum_x = first_x = atom->x;
            sum_y = first_y = atom->y;
            sum_z = first_z = atom->z;
        } else {
            double xr = atom->x - first_x;
            double yr = atom->y - first_y;
            double zr = atom->z - first_z;
            frame->image(xr, yr, zr);
            sum_x += first_x + xr;
            sum_y += first_y + yr;
            sum_z += first_z + zr;
        }
    }
    auto len = atom_list.size();
    center_x = sum_x / len;
    center_y = sum_y / len;
    center_z = sum_z / len;
}

double min_distance(std::shared_ptr<Molecule> &mol1, std::shared_ptr<Molecule> &mol2 ,std::shared_ptr<Frame> &frame) {
    double mindistance2 = std::numeric_limits<double>::max();
    for (auto &atom1 : mol1->atom_list) {
        for (auto &atom2 : mol2->atom_list) {
            mindistance2 = std::min(mindistance2,atom_distance2(atom1, atom2,frame));
        }
    }
    return std::sqrt(mindistance2);
}

