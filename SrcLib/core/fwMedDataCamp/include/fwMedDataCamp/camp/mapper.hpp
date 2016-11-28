/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATACAMP_CAMP_MAPPER_HPP__
#define __FWMEDDATACAMP_CAMP_MAPPER_HPP__

#include <fwCamp/Mapper/ValueMapper.hpp>

#include <fwMedData/NavigationSeries.hpp>

#include <boost/algorithm/string.hpp>

namespace camp_ext
{

template <>
struct ValueMapper< ::fwMedData::NavigationSeries::CoordinateType >
{
    typedef ::fwMedData::NavigationSeries::CoordinateType ReturnType;
    static const int type = camp::stringType;
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
        ReturnType coord;
        ::boost::split(result, source, ::boost::is_any_of(";"));

        if(result.size() >= 3)
        {
            coord[0] = ValueMapper<double>::from(result[0]);
            coord[1] = ValueMapper<double>::from(result[1]);
            coord[2] = ValueMapper<double>::from(result[2]);
        }
        else
        {
            OSLM_WARN("Invalid number of components: " << result.size());
        }
        return coord;
    }
};

} //camp_ext

#endif /* __FWMEDDATACAMP_CAMP_MAPPER_HPP__ */
