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

#ifndef __FWDATA_CAMP_MAPPER_HPP__
#define __FWDATA_CAMP_MAPPER_HPP__

#include "fwData/TransferFunction.hpp"

#include <fwCamp/Mapper/ValueMapper.hpp>

#include <boost/algorithm/string.hpp>

namespace camp_ext
{

template <>
struct ValueMapper< ::fwData::TransferFunction::TFColor >
{
    typedef ::fwData::TransferFunction::TFColor ReturnType;
    static const int type = camp::stringType;
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

    static ReturnType from(bool source)
    {
        CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));
    }
    static ReturnType from(long source)
    {
        CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));
    }
    static ReturnType from(double source)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }
    static ReturnType from(const camp::EnumObject& source)
    {
        CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));
    }
    static ReturnType from(const camp::UserObject& source)
    {
        CAMP_ERROR(camp::BadType(camp::userType, camp::mapType<ReturnType>()));
    }
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
            OSLM_WARN("Your tf color is not correctly setted, nb of component : " << result.size());
        }
        return tfColor;
    }
};


} //camp_ext

#endif /* __FWDATA_CAMP_MAPPER_HPP__ */
