/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <sight/core/config.hpp>

#include <core/exceptionmacros.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::core::ptree
{

/**
 * @brief   Returns the value of a path in a boost::property::tree.
 *  This handles three different cases that a single boost function can't discriminate.
 *  1. If the path exists, this returns the value
 *  2. If the path exists but the value type is not compatible, this throws a core::exception
 *  3. If the path does not exist, this returns the default value
 * @return  value if the path is set, default value otherwise
 * @throw   sight::core::exception if the value type is not compatible
 */
template<typename T>
T get_value(const boost::property_tree::ptree& _tree, const std::string& _path, T _default_value)
{
    auto element = _tree.get_child_optional(_path);
    if(not element.has_value())
    {
        // 3.
        return _default_value;
    }

    if(boost::optional<T> value = element->get_value_optional<T>())
    {
        // 1.
        return *value;
    }

    // 2.
    SIGHT_THROW_EXCEPTION(core::exception("Wrong value set in path: " + _path));
}

//------------------------------------------------------------------------------

template<class T>
[[nodiscard]] inline T get_and_deprecate(
    const boost::property_tree::ptree& _config,
    const std::string& _key,
    const std::string& _deprecated_key,
    const std::string_view _version,
    const T _default = {})
{
    if(const auto value = _config.get_optional<T>(_deprecated_key); value.has_value())
    {
        FW_DEPRECATED_MSG(
            std::quoted(_deprecated_key) << " is deprecated, use " << std::quoted(_key) << " instead",
            _version
        );
        return *value;
    }

    return _config.get<T>(_key, _default);
}

SIGHT_CORE_API std::string to_string(const boost::property_tree::ptree& _pt);

} // namespace sight::core
