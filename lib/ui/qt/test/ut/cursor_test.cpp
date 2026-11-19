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

#include <ui/qt/cursor.hpp>

#include <doctest/doctest.h>

#include <QCoreApplication>

TEST_SUITE("sight::ui::qt::cursor")
{
    TEST_CASE("changes_nothing_without_a_gui_application")
    {
        // As in a unit test or a tool that links the Qt library of the user interface: Qt aborts on a cursor there.
        REQUIRE(QCoreApplication::instance() == nullptr);
        sight::ui::qt::cursor cursor;
        cursor.set_cursor(sight::ui::cursor_base::busy, true);
        cursor.set_cursor(sight::ui::cursor_base::wait, false);
        cursor.set_default_cursor();
    }
}
