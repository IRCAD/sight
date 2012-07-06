/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_TEST_TU_FILENAMEPARSER_HPP_
#define _FWDATAIO_TEST_TU_FILENAMEPARSER_HPP_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/statement/sequence.hpp>
#include <boost/spirit/home/phoenix/container.hpp>



template <typename T>
bool parseMeshFileName(const std::string &name, T &points, T &cells)
{
    using boost::spirit::qi::ulong_long;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::_3;
    using boost::spirit::ascii::space;

    namespace phx = boost::phoenix;

    std::string::const_iterator first = name.begin();
    std::string::const_iterator last = name.end();

    bool r = phrase_parse(first, last,
            ("Mesh-" >> ulong_long >> "-" >> ulong_long >> "pts-" >> ulong_long >> "cells.trian")[ phx::ref(points) = _2, phx::ref(cells) = _3 ],
            space);

    if (first != last) // fail if we didn't get a full match
    {
            return false;
    }

    return r;
}


#endif //_FWDATAIO_TEST_TU_FILENAMEPARSER_HPP_
