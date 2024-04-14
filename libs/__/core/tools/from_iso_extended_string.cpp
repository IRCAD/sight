/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/tools/from_iso_extended_string.hpp"

#include <sstream>

namespace sight::core::tools
{

//------------------------------------------------------------------------------

std::string get_date_in_xsd_format(const boost::posix_time::ptime& _date_and_time)
{
    std::string date_and_time_str = boost::posix_time::to_iso_extended_string(_date_and_time);
    return date_and_time_str.substr(0, 10);
}

//------------------------------------------------------------------------------

boost::posix_time::ptime from_iso_extended_string(const std::string& _time_string)
{
    std::string tmp(_time_string);
    const auto i = tmp.find('T');
    tmp[i] = ' ';
    boost::posix_time::ptime t = boost::posix_time::time_from_string(tmp);
    return t;
}

//------------------------------------------------------------------------------

boost::posix_time::ptime from_date_in_xsd_format(const std::string& _date_string)
{
    std::stringstream ss;
    ss << _date_string << " 00:00:00";

    boost::posix_time::ptime t = boost::posix_time::time_from_string(ss.str());
    return t;
}

//------------------------------------------------------------------------------

std::string to_iso_extended_string(boost::posix_time::ptime _ptime)
{
    return boost::posix_time::to_iso_extended_string(_ptime);
}

} // namespace sight::core::tools
