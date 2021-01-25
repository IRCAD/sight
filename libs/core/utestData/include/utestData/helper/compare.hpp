/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "utestData/config.hpp"

#include <core/base.hpp>

#include <set>
#include <string>

namespace sight::data
{
class Object;
}

namespace sight::utestData
{
namespace helper
{
typedef std::set<std::string> ExcludeSetType;

/**
 * @brief Returns true if given objects are equals, based on their Camp attribute values.
 * The comparison can optionally omits some Camp attributes by giving their full name or a prefix.
 *
 * @param objRef Reference object to be compared
 * @param objComp Object compared to reference object
 * @param exclude Attributes omitted from comparison
 * @param excludeByPrefix Attributes beginning with these prefixes are omitted from comparison
 */
UTESTDATA_API bool compare(SPTR(data::Object) objRef,
                           SPTR(data::Object) objComp,
                           ExcludeSetType exclude         = ExcludeSetType(),
                           ExcludeSetType excludeByPrefix = ExcludeSetType());

} // namespace helper
} // namespace sight::utestData
