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

#include "data/TransferFunction.hpp"

#include <core/reflection/Mapper/ValueMapper.hpp>

#include <boost/algorithm/string.hpp>

namespace camp_ext
{

template <>
struct ValueMapper< sight::data::TransferFunction::TFColor >
{
    typedef sight::data::TransferFunction::TFColor ReturnType;
    static const int type = camp::stringType;
    //------------------------------------------------------------------------------

    static const std::string to(const ReturnType& source)
    {
        std::string result = "";

        std::string current = boost::lexical_cast< std::string>(source.r);
        result += current;
        result += ";";

        current = boost::lexical_cast< std::string>(source.g);
        result += current;
        result += ";";

        current = boost::lexical_cast< std::string>(source.b);
        result += current;
        result += ";";

        current = boost::lexical_cast< std::string>(source.a);
        result += current;
        result += ";";

        return result;
    }

    //------------------------------------------------------------------------------

    static ReturnType from(bool)
    {
        CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(long)
    {
        CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(double)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const camp::EnumObject&)
    {
        CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const camp::UserObject&)
    {
        CAMP_ERROR(camp::BadType(camp::userType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const std::string& source)
    {
        std::vector< std::string> result;
        ReturnType tfColor;
        ::boost::split(result, source, boost::is_any_of(";"));

        if(result.size() >= 4)
        {
            tfColor.r = ValueMapper<float>::from(result[0]);
            tfColor.g = ValueMapper<float>::from(result[1]);
            tfColor.b = ValueMapper<float>::from(result[2]);
            tfColor.a = ValueMapper<float>::from(result[3]);
        }
        else
        {
            SLM_WARN("Your tf color is not correctly setted, nb of component : " << result.size());
        }
        return tfColor;
    }
};

} //camp_ext
