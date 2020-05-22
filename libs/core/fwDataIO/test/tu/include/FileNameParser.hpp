/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATAIO_UT_FILENAMEPARSER_HPP__
#define __FWDATAIO_UT_FILENAMEPARSER_HPP__

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
    std::string::const_iterator last  = name.end();

    bool r = phrase_parse(first, last,
                          ("Mesh-" >> ulong_long >> "-" >> ulong_long >> "pts-" >> ulong_long >>
                           "cells.trian")[ phx::ref(points) = _2, phx::ref(cells) = _3 ],
                          space);

    if (first != last) // fail if we didn't get a full match
    {
        return false;
    }

    return r;
}


#endif //__FWDATAIO_UT_FILENAMEPARSER_HPP__
