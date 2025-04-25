/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/tools/date_and_time.hpp"

#include <core/base.hpp>

#include <boost/lexical_cast.hpp>

#include <regex>

namespace sight::core::tools
{

//------------------------------------------------------------------------------

boost::gregorian::date str_to_boost_date(const std::string& _date_str)
{
    boost::gregorian::date res_date(boost::gregorian::from_undelimited_string("19000101"));
    if(_date_str.size() < 8)
    {
        SIGHT_WARN(
            "The string length is too short (<8) : " << _date_str
            << ". The string is initialized with \"19000101\"."
        );
    }
    else if(_date_str.size() > 8)
    {
        SIGHT_WARN("The string length is too long (>8) : " << _date_str << ".. The string is trunked to 8 characters.");
        res_date = str_to_boost_date(_date_str.substr(0, 8));
    }
    else
    {
        std::regex is_number("[0-9]+");

        if(std::regex_match(_date_str, is_number))
        {
            res_date = boost::gregorian::date(boost::gregorian::from_undelimited_string(_date_str));
        }
        else
        {
            SIGHT_WARN(
                "The string not contains 8 numbers : " << _date_str
                << ". The string is initialized with \"19000101\"."
            );
        }
    }

    return res_date;
}

//------------------------------------------------------------------------------

boost::posix_time::time_duration str_to_boost_time(const std::string& _time_str)
{
    using boost::posix_time::time_duration;
    using boost::posix_time::hours;
    using boost::posix_time::minutes;
    using boost::posix_time::seconds;

    time_duration td;
    if(_time_str.size() < 6)
    {
        SIGHT_WARN(
            "The string length is too short (<6) : " << _time_str
            << ".  The string is initialized with \"000000\"."
        );
        td = hours(0) + minutes(0) + seconds(0);
    }
    else if(_time_str.size() > 6)
    {
        SIGHT_WARN("The string length is too short (>6) : " << _time_str << ". This string is trunked.");
        td = str_to_boost_time(_time_str.substr(0, 6));
    }
    else
    {
        std::regex is_number("[0-9]+");
        if(std::regex_match(_time_str, is_number))
        {
            auto h = boost::lexical_cast<std::uint16_t>(_time_str.substr(0, 2));
            auto m = boost::lexical_cast<std::uint16_t>(_time_str.substr(2, 2));
            auto s = boost::lexical_cast<std::uint16_t>(_time_str.substr(4, 2));
            td = hours(h) + minutes(m) + seconds(s);
        }
        else
        {
            SIGHT_WARN(
                "The string not contains 6 numbers : " << _time_str
                << ". The string is initialized with \"000000\"."
            );
            td = hours(0) + minutes(0) + seconds(0);
        }
    }

    return td;
}

//------------------------------------------------------------------------------

boost::posix_time::ptime str_to_boost_date_and_time(const std::string& _date_str, const std::string& _time_str)
{
    return {str_to_boost_date(_date_str), str_to_boost_time(_time_str)};
}

//------------------------------------------------------------------------------

std::string get_date(const boost::posix_time::ptime& _date_and_time)
{
    std::string date_and_time_str = boost::posix_time::to_iso_string(_date_and_time);
    return date_and_time_str.substr(0, 8);
}

//------------------------------------------------------------------------------

std::string get_time(const boost::posix_time::ptime& _date_and_time)
{
    std::string date_and_time_str = boost::posix_time::to_iso_string(_date_and_time);
    return date_and_time_str.substr(9, 6);
}

//------------------------------------------------------------------------------

std::string get_current_time()
{
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    std::string now_str          = boost::posix_time::to_simple_string(now);
    return now_str.substr(0, 21);
}

} // namespace sight::core::tools
