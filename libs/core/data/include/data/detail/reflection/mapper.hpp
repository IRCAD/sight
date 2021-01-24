/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "data/NavigationSeries.hpp"

#include <core/reflection/Mapper/ValueMapper.hpp>

#include <boost/algorithm/string.hpp>

namespace camp_ext
{

template <>
struct ValueMapper< sight::data::NavigationSeries::CoordinateType >
{
    typedef sight::data::NavigationSeries::CoordinateType ReturnType;
    static const int type = camp::stringType;
    //------------------------------------------------------------------------------

    static const std::string to(const ReturnType& source)
    {
        std::string result;

        std::string current = ::boost::lexical_cast<std::string>(source[0]);
        result += current;
        result += ";";

        current = ::boost::lexical_cast<std::string>(source[1]);
        result += current;
        result += ";";

        current = ::boost::lexical_cast<std::string>(source[2]);
        result += current;
        result += ";";

        return result;
    }

    //------------------------------------------------------------------------------

    static ReturnType from(bool )
    {
        SLM_ERROR("Conversion not handled");
        return ReturnType();
    }
    //------------------------------------------------------------------------------

    static ReturnType from(long )
    {
        SLM_ERROR("Conversion not handled");
        return ReturnType();
    }
    //------------------------------------------------------------------------------

    static ReturnType from(double )
    {
        SLM_ERROR("Conversion not handled");
        return ReturnType();
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const camp::EnumObject& )
    {
        SLM_ERROR("Conversion not handled");
        return ReturnType();
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const camp::UserObject& )
    {
        SLM_ERROR("Conversion not handled");
        return ReturnType();
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const std::string& source)
    {
        std::vector< std::string> result;
        ReturnType coord {0., 0., 0.};
        ::boost::split(result, source, ::boost::is_any_of(";"));

        if(result.size() >= 3)
        {
            coord[0] = ValueMapper<double>::from(result[0]);
            coord[1] = ValueMapper<double>::from(result[1]);
            coord[2] = ValueMapper<double>::from(result[2]);
        }
        else
        {
            SLM_WARN("Invalid number of components: " << result.size());
        }
        return coord;
    }
};

} //camp_ext
