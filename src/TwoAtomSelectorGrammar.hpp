//
// Created by xiamr on 7/8/19.
//

#ifndef TINKER_TWOATOMSELECTORGRAMMAR_HPP
#define TINKER_TWOATOMSELECTORGRAMMAR_HPP

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

struct TwoAtomVectorSelectorNodeStruct {
    Atom::Node id1;
    Atom::Node id2;

    TwoAtomVectorSelectorNodeStruct(const Atom::Node &id1, const Atom::Node &id2)
            : id1(id1), id2(id2) {}
};

using TwoAtomVectorSelectorNode = std::shared_ptr<TwoAtomVectorSelectorNodeStruct>;

template<typename Iterator, typename Skipper>
struct TwoAtomVectorGrammar : qi::grammar<Iterator, TwoAtomVectorSelectorNode(), Skipper> {

    qi::rule<Iterator, TwoAtomVectorSelectorNode(), Skipper> two_atom_vector_selector_rule;
    Grammar<Iterator, Skipper> maskParser;

    TwoAtomVectorGrammar() : TwoAtomVectorGrammar::base_type(two_atom_vector_selector_rule) {
        using qi::lit;
        using qi::_val;
        using qi::_1;
        using qi::_2;

        two_atom_vector_selector_rule = (lit("twoAtomVector") >> "(" >> maskParser >> "," >> maskParser >> ")")
        [_val = make_shared_<TwoAtomVectorSelectorNode>(_1, _2)];
    }
};

#endif //TINKER_TWOATOMSELECTORGRAMMAR_HPP