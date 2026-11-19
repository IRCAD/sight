/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <string_view>

namespace sight::ui::detail::registry
{

/// A launched configuration qualifies its ids as "<config>-<n>-<uid>": what follows the last '-' is the XML uid, as
/// core::id::base_id() does. An unqualified id is kept whole (npos + 1 is 0).
[[nodiscard]] inline std::string_view xml_name(std::string_view _id) noexcept
{
    return _id.substr(_id.find_last_of('-') + 1);
}

} // namespace sight::ui::detail::registry
