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

#include <core/runtime/path.hpp>

#include <ui/test/test.hpp>

namespace sight::example::ui::ex_settings::uit
{

/// The doctest fixture shared by all ex_settings GUI scenarios.
struct fixture : sight::ui::test::base
{
    fixture() :
        sight::ui::test::base(sight::core::runtime::working_path() / "share/sight/ex_settings/profile.xml")
    {
    }
};

} // namespace sight::example::ui::ex_settings::uit
