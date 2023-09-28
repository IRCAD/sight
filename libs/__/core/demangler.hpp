/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "core/config.hpp"

#include <string>
#include <typeinfo>

namespace sight::core
{

/**
 * @brief typeid, string or object name demangler.
 *
 * @warning get_classname() use on fundamentals types (int, float, ...) will give a bad result ( ex: \::int )
 */
class CORE_CLASS_API demangler
{
public:

    /**
     * @name Constructor/Destructor
     * @{ */

    /**
     * @brief Constructor from a typeinfo object
     *
     * @param t Typeinfo to demangle
     */
    CORE_API demangler(const std::type_info& t);

    /**
     * @brief Constructor from a std::string
     *
     * @param s string to demangle
     */
    CORE_API demangler(std::string s);

    /**
     * @brief Constructor from any type
     *
     * @param s object which type has to be demangled
     */
    template<typename T>
    demangler(T& s) :
        m_name(typeid(s).name())
    {
    }

    /**
     * @brief Destructor
     */
    CORE_API virtual ~demangler();

    /**  @} */

    /**
     * @brief Returns "Object" from "::some::long::namespace::Object"
     *
     * @return demangled class name only
     */
    [[nodiscard]] CORE_API std::string get_leaf_classname() const;

    /**
     * @brief Returns "::some::long::namespace::Object" from "::some::long::namespace::Object"
     *
     * @return demangled class name with all namespaces from root
     */
    [[nodiscard]] CORE_API std::string get_classname() const;

protected:

    /**
     * @brief Store the name to demangle
     */
    const std::string m_name;

    /**
     * @brief Process the name to demangle and return the same string whatever the OS is.
     *
     * @return demangled string
     */
    [[nodiscard]] std::string demangle() const;
};

/**
 * @brief Type demangler.
 * Use Demangler class to get a demangled string for the type T.
 */
template<class T>
class type_demangler : public demangler
{
public:

    /**
     * @name Constructor/Destructor
     * @{ */

    /**
     * @brief Constructor
     * @tparam T Type to demangle
     */
    type_demangler() :
        demangler(typeid(T))
    {
    }

    /**  @} */
};

/**
 * @name Type demangling functions
 * @brief These functions are provided as type-demangling helpers. See
 * Demangler for details about returned values.
 *
 * @tparam T Type to demangle
 *
 * @{ */
template<class T>
inline std::string get_leaf_classname()
{
    return type_demangler<T>().get_leaf_classname();
}

//------------------------------------------------------------------------------

template<class T>
inline std::string get_classname()
{
    return type_demangler<T>().get_classname();
}

/**  @} */

} // namespace sight::core
