/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>

#include <camp/camptype.hpp>
#include <camp/type.hpp>
#include <camp/valuemapper.hpp>

namespace camp_ext
{

template <>
struct ValueMapper< std::uint64_t>
{
    // The corresponding CAMP type is "string"
    static const int type = camp::intType;

    // Convert from MyStringClass to std::string
    static std::string to(const std::uint64_t& source)
    {
        return boost::lexical_cast<std::string>(source);
    }

    // Convert from any type to MyStringClass
    // Be smart, just reuse ValueMapper<std::string> :)
    template <typename T>
    static std::uint64_t from(const T& source)
    {
        return boost::lexical_cast< std::uint64_t>(source);
    }
};

//-----------------------------------------------------------------------------

template<typename T>
struct wrapperTribbol
{
    //------------------------------------------------------------------------------

    static ::boost::logic::tribool get(const T&)
    {
        return ::boost::logic::tribool();
    }
};

//-----------------------------------------------------------------------------

template<>
struct wrapperTribbol<std::string >
{
    //------------------------------------------------------------------------------

    static ::boost::logic::tribool get(const std::string& source)
    {
        ::boost::logic::tribool value;

        if (source.compare("true") == 0)
        {
            value = true;
        }
        else if (source.compare("false") == 0)
        {
            value = false;
        }
        else
        {
            value = boost::logic::indeterminate;
        }

        return value;
    }
};

//-----------------------------------------------------------------------------

template <>
struct ValueMapper< ::boost::logic::tribool>
{
    // The corresponding CAMP type is "string"
    static const int type = camp::stringType;

    // Convert from MyStringClass to std::string
    static std::string to(const ::boost::logic::tribool& source)
    {
        std::string value;

        if (source)
        {
            value = "true";
        }
        else if (!source)
        {
            value = "false";
        }
        else
        {
            value = "indeterminate";
        }
        return value;
    }
    //------------------------------------------------------------------------------

    template <typename T>
    static ::boost::logic::tribool from(const T& source)
    {
        return wrapperTribbol<T>::get(source);
    }
};

//-----------------------------------------------------------------------------

template <>
struct ValueMapper< ::boost::posix_time::ptime >
{
    typedef boost::posix_time::ptime ReturnType;
    static const int type = camp::stringType;
    //------------------------------------------------------------------------------

    static const std::string to(const ReturnType& source)
    {
        return boost::posix_time::to_simple_string(source);
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
        return boost::posix_time::time_from_string(source);
    }
};

//-----------------------------------------------------------------------------

template <>
struct ValueMapper< ::boost::filesystem::path >
{
    typedef ::boost::filesystem::path ReturnType;
    static const int type = camp::stringType;
    //------------------------------------------------------------------------------

    static const std::string to(const ReturnType& source)
    {
        return source.string();
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
        return ::boost::filesystem::path(source);
    }
};

//-----------------------------------------------------------------------------

/*
 * Specialization of ValueMapper for unsigned char
 */
template <>
struct ValueMapper<unsigned char>
{
    static const int type = camp::stringType;
    //------------------------------------------------------------------------------

    static std::string to(unsigned char source)
    {
        unsigned int intValue = boost::numeric_cast<unsigned int>(source);

        return boost::lexical_cast<std::string>(intValue);
    }

    //------------------------------------------------------------------------------

    static unsigned char from(bool source)
    {
        return static_cast<unsigned char>(source);
    }
    //------------------------------------------------------------------------------

    static unsigned char from(long source)
    {
        return static_cast<unsigned char>(source);
    }
    //------------------------------------------------------------------------------

    static unsigned char from(double source)
    {
        return static_cast<unsigned char>(source);
    }
    //------------------------------------------------------------------------------

    static unsigned char from(const std::string& source)
    {
        unsigned int intValue = boost::lexical_cast<unsigned int>(source);
        return boost::numeric_cast<unsigned char>(intValue);
    }
    //------------------------------------------------------------------------------

    static unsigned char from(const camp::EnumObject& source)
    {
        return static_cast<unsigned char>(source.value());
    }
    //------------------------------------------------------------------------------

    static unsigned char from(const camp::UserObject&)
    {
        CAMP_ERROR(camp::BadType(camp::userType, camp::realType));
    }
};

//-----------------------------------------------------------------------------

template <typename T>
struct ValueMapper<std::shared_ptr<T> >
{
    typedef std::shared_ptr<T> ReturnType;
    static const int type = camp::userType;
    //------------------------------------------------------------------------------

    static const camp::UserObject to(const ReturnType& source)
    {
        return camp::UserObject(source);
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

    static ReturnType from(const std::string&)
    {
        CAMP_ERROR(camp::BadType(camp::stringType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const camp::EnumObject&)
    {
        CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));
    }
    //------------------------------------------------------------------------------

    static ReturnType from(const camp::UserObject& source)
    {
        ReturnType result;
        try
        {
            T* ptr = source.get< T* >();
            result = T::dynamicCast(ptr->getSptr());
        }
        catch(...)
        {
            std::shared_ptr<T> tmp;
            return tmp;
        }
        return result;
    }
};
}  // namespace camp_ext
