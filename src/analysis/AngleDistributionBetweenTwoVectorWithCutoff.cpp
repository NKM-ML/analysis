//
// Created by xiamr on 8/6/19.
//

#include <boost/range/adaptors.hpp>
#include "AngleDistributionBetweenTwoVectorWithCutoff.hpp"
#include "frame.hpp"
#include "ThrowAssert.hpp"
#include "common.hpp"
#include "VectorSelectorFactory.hpp"

using namespace std;


AngleDistributionBetweenTwoVectorWithCutoff::AngleDistributionBetweenTwoVectorWithCutoff() {
    enable_outfile = true;
}


void AngleDistributionBetweenTwoVectorWithCutoff::processFirstFrame(std::shared_ptr<Frame> &frame) {
    std::for_each(frame->atom_list.begin(), frame->atom_list.end(),
                  [this](shared_ptr<Atom> &atom) {
                      if (Atom::is_match(atom, this->ids1)) this->group1.insert(atom);
                      if (Atom::is_match(atom, this->ids2)) this->group2.insert(atom);
                  });
    throw_assert(this->group1.size() == 1, "Group1 must has only one atom");
    throw_assert(!this->group1.empty(), "Group2 must has at least one atom");
}

void AngleDistributionBetweenTwoVectorWithCutoff::process(std::shared_ptr<Frame> &frame) {
    nframe++;
    for (auto &ref : group1) {
        for (auto &atom: group2) {
            double distance = atom_distance(ref, atom, frame);
            if (cutoff1 <= distance and distance < cutoff2) {

                auto v1 = vector1->calculateVector(ref->molecule.lock(), frame);
                auto v2 = vector2->calculateVector(atom->molecule.lock(), frame);

                double cos_ = dot_multiplication(v1, v2) / sqrt(vector_norm2(v1) * vector_norm2(v2));

                cos_hist.update(cos_);

                double angle = radian * acos(cos_);

                angle_evolution.emplace(nframe, std::make_pair<int>(atom->seq, angle));

                angle_hist.update(angle);
            }
        }
    }

}

void AngleDistributionBetweenTwoVectorWithCutoff::print(std::ostream &os) {
    os << string(50, '#') << '\n';
    os << "# " << title() << '\n';
    os << "# Group1 > " << ids1 << '\n';
    os << "# Group2 > " << ids2 << '\n';

    os << "# vector1 > ";
    vector1->print(os);
    os << "# vector2 > ";
    vector2->print(os);

    os << "# angle max   > " << angle_hist.dimension_range.second << '\n';
    os << "# angle width > " << angle_hist.getWidth() << '\n';

    os << "# cutoff1 > " << cutoff1 << '\n';
    os << "# cutoff2 > " << cutoff2 << '\n';


    std::set<int> appeared_atoms;

    for (auto &it : angle_evolution) {
        appeared_atoms.insert(it.second.first);
    }

    os << string(25, '#') << "Angle(degree)" << string(25, '#') << '\n';
    os << format("#%15s", "Frame");
    for (auto i : appeared_atoms) {
        os << format(" [ %10d ]", i);
    }
    os << '\n';

    for (auto i = 1; i <= nframe; ++i) {
        os << format("%15.3d", i);
        std::map<int, double> mapping;
        for (auto it = angle_evolution.lower_bound(i); it != angle_evolution.upper_bound(i); ++it) {
            mapping.insert(it->second);
        }
        for (auto seq : appeared_atoms) {
            auto it = mapping.find(seq);
            if (it != mapping.end()) {
                os << format(" %15.3f", it->second);
            } else {
                os << string(16, ' ');
            }
        }
        os << '\n';
    }


    os << string(50, '#') << '\n';
    os << format("#%15s %15s\n", "Angle(degree)", "Probability Density(% degree-1)");
    for (auto[grid, value] : angle_hist.getDistribution()) {
        os << format("%15.3f %15.3f\n", grid, 100 * value);
    }
    os << string(50, '#') << '\n';

    os << format("#%15s %15s\n", "cos(theta)", "Probability Density(%)");
    for (auto[grid, value] : cos_hist.getDistribution()) {
        os << format("%15.3f %15.3f\n", grid, 100 * value);
    }

}

void AngleDistributionBetweenTwoVectorWithCutoff::readInfo() {
    Atom::select2group(ids1, ids2);

    std::cout << "For first Vector\n";
    vector1 = VectorSelectorFactory::getVectorSelector();
    vector1->readInfo();

    std::cout << "For second Vector\n";
    vector2 = VectorSelectorFactory::getVectorSelector();
    vector2->readInfo();

    double angle_max = choose(0.0, 180.0, "Enter Maximum Angle to Accumulate[180.0 degree]:", Default(180.0));
    double angle_width = choose(0.0, 180.0, "Enter Width of Angle Bins [0.5 degree]:", Default(0.5));

    cutoff1 = choose(0.0, 100.0, "Cutoff1 [Angstrom]:");
    cutoff2 = choose(0.0, 100.0, "Cutoff2 [Angstrom]:");

    throw_assert(cutoff1 < cutoff2, "Cutoff1 must less than Cutoff2");

    angle_hist.initialize(angle_max, angle_width);

    init_cos_hist(angle_max, angle_width);
}


string AngleDistributionBetweenTwoVectorWithCutoff::description() {
    stringstream ss;
    string title_line = "------ " + std::string(title()) + " ------";
    ss << title_line << "\n";
    ss << " M                 = [ " << ids1 << " ]\n";
    ss << " L                 = [ " << ids2 << " ]\n";
    ss << " vector1           = " << vector1->description() << "\n";
    ss << " vector2           = " << vector2->description() << "\n";
    ss << " angle_max         = " << angle_hist.dimension_range.second << " (degree)\n";
    ss << " angle_width       = " << angle_hist.getWidth() << " (degree)\n";
    ss << " cutoff1           = " << cutoff1 << " (Ang)\n";
    ss << " cutoff2           = " << cutoff2 << " (Ang)\n";
    ss << " outfilename       = " << outfilename << "\n";
    ss << string(title_line.size(), '-') << '\n';
    return ss.str();
}

void AngleDistributionBetweenTwoVectorWithCutoff::setParameters(
        const Atom::Node &M,
        const Atom::Node &L,
        std::shared_ptr<VectorSelector> vector1,
        std::shared_ptr<VectorSelector> vector2,
        double angle_max,
        double angle_width,
        double cutoff1,
        double cutoff2,
        const std::string &outfilename) {

    this->ids1 = M;
    this->ids2 = L;

    if (!vector1) {
        throw runtime_error("vector1 not vaild");
    } else {
        this->vector1 = vector1;
    }

    if (!vector2) {
        throw runtime_error("vector2 not vaild");
    } else {
        this->vector2 = vector2;
    }

    if (angle_max < 0) {
        throw runtime_error("`angle_max` must not be negative");
    }
    if (angle_width < 0) {
        throw runtime_error("`cutoff1` must not be negative");
    }

    if (cutoff1 < 0) {
        throw runtime_error("`cutoff1` must not be negative");
    } else {
        this->cutoff1 = cutoff1;
    }

    if (cutoff2 <= 0) {
        throw runtime_error("`cutoff2` must be postive");
    } else {
        this->cutoff2 = cutoff2;
    }

    this->outfilename = outfilename;
    boost::trim(this->outfilename);
    if (this->outfilename.empty()) {
        throw runtime_error("outfilename cannot empty");
    }

    angle_hist.initialize(angle_max, angle_width);
    init_cos_hist(angle_max, angle_width);

}

void AngleDistributionBetweenTwoVectorWithCutoff::init_cos_hist(double angle_max,
                                                                double angle_width) {
    cos_hist.initialize({cos(angle_max / radian), 1}, abs(1 - cos(angle_max / radian)) / (angle_max / angle_width));
}
