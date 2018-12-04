/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWTOOLS_STRINGIZER_HPP__
#define __FWTOOLS_STRINGIZER_HPP__

#include "fwTools/config.hpp"
#include "fwTools/TypeInfo.hpp"

#include <fwCore/Demangler.hpp>

#include  <boost/lexical_cast.hpp>
#include  <boost/mpl/if.hpp>
#include  <boost/type_traits/is_arithmetic.hpp>

#include <string>
#include <type_traits>
#include <typeinfo>

namespace fwTools
{

namespace
{
struct NumericCast
{
    //------------------------------------------------------------------------------

    template<class T>
    static std::string eval(const T& t)
    {
        // note boost::lexical_cast with char is a ASCII-code conversion
        // instead numerical casting. We provide fix with specialization
        return ::boost::lexical_cast<std::string>(t);
    }
};

struct Default
{
    //------------------------------------------------------------------------------

    template<class T>
    static std::string eval(const T& t)
    {
        return "No getString for " + ::fwCore::Demangler(typeid(t)).getClassname();
    }
};
}

/**
 * @name Conversion value to string
 * @{
 */

// inline mandatory to be supported in multi compilation unit and avoid conflict when linking
// function doesn't exist code is rewritted
/**
 * @brief       Convert the value to a string
 * @param[in]   t value
 * @return      Value converted in a string
 */
template<class T>
inline std::string getString(const T& t)
{
    typedef BOOST_DEDUCED_TYPENAME boost::mpl::if_c<  std::is_arithmetic<T>::value,
                                                      NumericCast,
                                                      Default
                                                      >::type Choice;

    return Choice::eval(t);
}

/**
 * @brief       Convert a pair of values to a string.
 *
 * partial specialization.
 *
 * @param[in]   t pair of values in type T1 and T2
 * @return      String in format [value 1, value 2]
 */
template<class T1, class T2>
inline std::string getString(const std::pair<T1, T2>& t)
{
    std::string res = ("[");
    res += getString(t.first) + "," + getString(t.second) + "]";
    return res;
}

/**
 * @brief       Convert container values to a string
 *
 * Specialization for all stl container vector,list,map
 *
 * @param[in]   begin begin of the container
 * @param[in]   end end of the container
 * @return      String in format [value1, value2,....]
 */
template<class ForwardIterator>
inline std::string getString(ForwardIterator begin, ForwardIterator end)
{
    std::string result("[");
    if (begin != end)
    {
        result += getString( *begin );
        while (++begin != end)
        {
            result += "," + getString( *begin );
        }
    }
    result += "]";
    return result;
}

/// explicit specialization of getString(const T &t)
template<>
FWTOOLS_API std::string getString(const std::string& aString);

template<>
FWTOOLS_API std::string getString(const std::type_info& ti);

template<>
FWTOOLS_API std::string getString(const TypeInfo& ti);

// char are numerically casted
template<>
FWTOOLS_API std::string getString(const signed char& c);

// char are numerically casted // signed char and char doesn't are the same :/ !!!!
template<>
FWTOOLS_API std::string getString(const char& c);

// char are numerically casted
template<>
FWTOOLS_API std::string getString(const unsigned char& c);

template<>
FWTOOLS_API std::string getString(const std::string& aString);

///@}

}

#endif // __FWTOOLS_STRINGIZER_HPP__

