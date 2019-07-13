//
// Created by xiamr on 7/8/19.
//

#ifndef TINKER_DIPOLEVECTORSELECTORGRAMMAR_HPP
#define TINKER_DIPOLEVECTORSELECTORGRAMMAR_HPP

#include <memory>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/phoenix/function/adapt_function.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "common.hpp"
#include "atom.hpp"
#include "grammar.hpp"

namespace qi = boost::spirit::qi;
namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;

struct DipoleVectorSelectorNodeStruct {
    Atom::Node id;

    DipoleVectorSelectorNodeStruct(const Atom::Node &id) : id(id) {}
};

using DipoleVectorSelectorNode = std::shared_ptr<DipoleVectorSelectorNodeStruct>;

template<typename Iterator, typename Skipper>
struct DipoleVectorGrammar : qi::grammar<Iterator, DipoleVectorSelectorNode(), Skipper> {

    qi::rule<Iterator, DipoleVectorSelectorNode(), Skipper> dipole_vector_selector_rule;
    qi::rule<Iterator, Atom::Node(), Skipper> mask;
    Grammar<Iterator, Skipper> maskParser;

    DipoleVectorGrammar() : DipoleVectorGrammar::base_type(dipole_vector_selector_rule) {
        using qi::lit;
        using qi::_val;
        using qi::_1;

        mask %= "[" >> maskParser >> "]";

        dipole_vector_selector_rule = (lit("dipoleVector") >> "(" >> mask >> ")")
        [_val = make_shared_<DipoleVectorSelectorNodeStruct>(_1)];
    }
};


inline bool operator==(const DipoleVectorSelectorNode &node1, const DipoleVectorSelectorNode &node2) {
    if (node1 && node2) {
        return node1->id == node2->id;
    }
    return false;
}


#endif //TINKER_DIPOLEVECTORSELECTORGRAMMAR_HPP
