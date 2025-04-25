/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/runtime/extension.hpp"

#include <core/base.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::core::runtime
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
T get_ptree_value(const boost::property_tree::ptree& _tree, const std::string& _path, T _default_value)
{
    boost::property_tree::ptree element;
    try
    {
        element = _tree.get_child(_path);
    }
    catch(const boost::property_tree::ptree_bad_path&)
    {
        // 3.
        return _default_value;
    }

    if(boost::optional<T> value = element.get_value_optional<T>())
    {
        // 1.
        return *value;
    }

    // 2.
    SIGHT_THROW_EXCEPTION(core::exception("Wrong value set in path: " + _path));
    return _default_value;
}

namespace property_tree
{

SIGHT_CORE_API std::string to_string(const boost::property_tree::ptree& _pt);

}

} // namespace sight::core::runtime
