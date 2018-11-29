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

#ifndef __FWTOOLS_TYPEINFO_HPP__
#define __FWTOOLS_TYPEINFO_HPP__

#include "fwTools/config.hpp"

#include <typeinfo>

namespace fwTools
{
/**
 * @brief   Purpose: offer a first-class, comparable wrapper over std::type_info ( but copy construcible )
 *
 * Code imported from Loki-Library and adapted for Sight
 */

class FWTOOLS_CLASS_API TypeInfo
{
public:

    /// default constructor
    FWTOOLS_API TypeInfo(); // needed for containers

    /// copy constructor
    FWTOOLS_API TypeInfo(const std::type_info&); // non-explicit

    /**
     * @brief copy method
     * @param[in] ti the TypeInfo to copy
     * @return the copied TypeInfo
     */
    FWTOOLS_API TypeInfo& operator=(const TypeInfo& ti);

    /**
     * @brief access for the wrapped std::type_info
     * @return a const reference on the embedded  std::type_info
     */
    FWTOOLS_API const std::type_info& type_info() const;

    /**
     * @brief cast operator TypeInfo -> std::type_info
     * @return a const reference on the embedded  std::type_info
     */
    FWTOOLS_API operator const std::type_info&() const
    {
        return *pInfo_;
    }

    // Compatibility functions
    /**
     * @brief compatibility functions ( std::type_info like)
     * @return true iff TypeInfo is "before" an other, ( ordering is defined as std::type_info::before() )
     */
    FWTOOLS_API bool before(const TypeInfo& rhs) const;

    /**
     * @brief compatibility functions ( std::type_info like)
     * @return the lexical name of the typeinfo ( = td::type_info::name() )
     */
    FWTOOLS_API const char* name() const;

private:

    /// std::type_info are not copiable but its pointer yes and typeid always return same reference !!!
    const std::type_info* pInfo_;
};

// COMPARISON OPERATOR
/**
 * @name Comparison operators for TypeInfo
 * @{
 */
FWTOOLS_API bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator<(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator>(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs);
///@}

}

#endif /* __FWTOOLS_TYPEINFO_HPP__ */
