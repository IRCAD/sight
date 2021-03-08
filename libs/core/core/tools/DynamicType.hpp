/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include <boost/mpl/vector.hpp>

#include "core/tools/Stringizer.hpp"
#include "core/config.hpp"

namespace sight::core::tools
{

/**
 * @brief   Class defining an elementary C++ type aka unsigned char, signed char, .... signed long, float, double
 * @deprecated This class is no longer supported and will be removed in sight 22.0
 **/
class CORE_CLASS_API DynamicType
{
public:
    /// Default constructor
    [[deprecated("This class is no longer supported and will be removed in sight 22.0")]]
    CORE_API DynamicType();

    /// Constructor by copy
    [[deprecated("This class is no longer supported and will be removed in sight 22.0")]]
    CORE_API DynamicType(const DynamicType&);

    /**
     * @brief comparison operator
     */
    CORE_API bool operator==(const DynamicType&) const;

    /**
     * @brief comparison operator
     */
    CORE_API bool operator!=(const DynamicType&) const;

    /**
     * @brief  define an order (lexicographic) for dynamicType
     */
    CORE_API bool operator<( const DynamicType& ) const;

    /**
     * @brief   Set DynamicType value according given template
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    void setType();

    /**
     * @brief   Return true iff the DynamicType value represents the TYPE
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    bool isType() const;

    /**
     * @brief   Return the sizeof of the type
     */
    CORE_API unsigned char sizeOf() const;

    /**
     * @brief Register a new type to be managed within DynamicType.
     *
     * Check are performed to ensure robsutess
     */
    template<class NEWTYPE>
    static void registerNewType(const std::string& newKey);

    /// Default destrucor : do nothing
    CORE_API virtual ~DynamicType();

    /// Return a human readable string
    CORE_API const std::string& string() const;

    /**
     * @brief Return a human readable string for type (static version no instanciation ...)
     *
     * Use a  DynamicType::string<int>();
     */
    template<class TYPE>
    static const std::string string();

    /**
     * @brief return the min and max storable in the DynamicType. take care that min/max value are casted into template
     * T
     */
    template<class T>
    std::pair<T, T> minMax();

    /// return true iff the type use a fixed precision
    CORE_API bool isFixedPrecision();

    /// return true iff the type is signed
    CORE_API bool isSigned();

    typedef boost::mpl::vector<
            signed char,
            unsigned char,
            signed short,
            unsigned short,
            signed int,
            unsigned int,
            unsigned long,
            signed long,
            float,
            double
            >::type SupportedTypes;

protected:

    std::string m_value; // aka key
    unsigned char m_sizeof;

    /// Container of types managed by DynamicType
    CORE_API static std::list< std::string > m_managedTypes;

    /// Value for not specified type
    CORE_API static const std::string m_unSpecifiedType;
};

/// Helper to create object DynamicType from a given type TYPE in { (un)signed char, ... , double }
template<class TYPE>
DynamicType makeDynamicType();

/**
 * @brief   Helper to create object DynamicType from a given type from a KEYTYPE representing a type in { (un)signed
 * char, ... , double }
 *
 * Example makeDynamicType(type_id(int)) and   makeDynamicType("signed int")  should work
 */
// #include "KEYTYPEMapping.hpp" is mandatory
template<class KEYTYPE>
DynamicType makeDynamicType(const KEYTYPE& keyType);

template<>
CORE_API std::string getString(const DynamicType& dt);

template<>
CORE_API std::string getString(const std::vector<DynamicType>& dtv);

} //end namespace sight::core::tools

#include "core/tools/DynamicType.hxx"
