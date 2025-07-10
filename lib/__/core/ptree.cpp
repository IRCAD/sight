/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <core/ptree.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <iostream>

namespace sight::core::ptree
{

//------------------------------------------------------------------------------

std::string to_string(const boost::property_tree::ptree& _pt)
{
    std::stringstream ss;

    // We specify xml settings for pretty printing
    boost::property_tree::write_xml(
        ss,
        _pt,
        boost::property_tree::xml_parser::xml_writer_settings<std::string>(' ', 4)
    );

    auto str = ss.str();
    str.erase(0, str.find('\n') + 1); // Remove the <xml?> tag which adds unnecessary noise
    return str;
}

} // namespace sight::core::ptree
