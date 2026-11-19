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

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace sight::ui::test::detail
{

/// The visible windows reduced to their named, textual or interactive objects, each with a locator that resolves to
/// it, or marked when it breaks the naming contract. On the GUI thread only.
[[nodiscard]] std::string ui_tree();

/// One diagnostic per visible object that breaks the naming contract. On the GUI thread only.
[[nodiscard]] std::vector<std::string> naming_violations();

/// The names of visible widgets and actions nearest to a requested one, the nearest first. On the GUI thread only.
[[nodiscard]] std::vector<std::string> closest_visible_names(std::string_view _requested, std::size_t _count = 3);

} // namespace sight::ui::test::detail
