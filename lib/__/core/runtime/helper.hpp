/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
#include <core/ptree.hpp>

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
[[deprecated("Use sight::core::ptree::get_value() instead")]]
T get_ptree_value(const boost::property_tree::ptree& _tree, const std::string& _path, T _default_value)
{
    return core::ptree::get_value(_tree, _path, _default_value);
}

namespace property_tree
{

//------------------------------------------------------------------------------

[[deprecated("Use sight::core::ptree::to_string() instead")]]
inline std::string to_string(const boost::property_tree::ptree& _pt)
{
    return core::ptree::to_string(_pt);
}

}

} // namespace sight::core::runtime
