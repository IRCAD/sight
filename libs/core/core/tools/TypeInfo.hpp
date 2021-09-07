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

#include "core/config.hpp"

#include <typeinfo>

namespace sight::core::tools
{

/**
 * @brief   Purpose: offer a first-class, comparable wrapper over std::type_info ( but copy construcible )
 *
 * Code imported from Loki-Library and adapted for Sight
 */

class CORE_CLASS_API TypeInfo
{
public:

    /// default constructor
    CORE_API TypeInfo(); // needed for containers

    /// copy constructor
    CORE_API TypeInfo(const std::type_info&); // non-explicit

    /**
     * @brief copy method
     * @param[in] ti the TypeInfo to copy
     * @return the copied TypeInfo
     */
    CORE_API TypeInfo& operator=(const TypeInfo& ti);

    /**
     * @brief access for the wrapped std::type_info
     * @return a const reference on the embedded  std::type_info
     */
    CORE_API const std::type_info& type_info() const;

    /**
     * @brief cast operator TypeInfo -> std::type_info
     * @return a const reference on the embedded  std::type_info
     */
    CORE_API operator const std::type_info& () const
    {
        return *pInfo_;
    }

    // Compatibility functions
    /**
     * @brief compatibility functions ( std::type_info like)
     * @return true iff TypeInfo is "before" an other, ( ordering is defined as std::type_info::before() )
     */
    CORE_API bool before(const TypeInfo& rhs) const;

    /**
     * @brief compatibility functions ( std::type_info like)
     * @return the lexical name of the typeinfo ( = td::type_info::name() )
     */
    CORE_API const char* name() const;

private:

    /// std::type_info are not copiable but its pointer yes and typeid always return same reference !!!
    const std::type_info* pInfo_;
};

// COMPARISON OPERATOR
/**
 * @name Comparison operators for TypeInfo
 * @{
 */
CORE_API bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
CORE_API bool operator<(const TypeInfo& lhs, const TypeInfo& rhs);
CORE_API bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);
CORE_API bool operator>(const TypeInfo& lhs, const TypeInfo& rhs);
CORE_API bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs);
CORE_API bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs);
///@}

}
