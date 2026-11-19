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

#include <ui/test/qt_application.hpp>

#include <ui/qt/widget/collapsible_section.hpp>

#include <doctest/doctest.h>

#include <QToolButton>

TEST_SUITE("sight::ui::qt::widget::collapsible_section")
{
    TEST_CASE("names_toggle_after_section")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};

        sight::ui::qt::widget::collapsible_section section;
        const auto buttons = section.findChildren<QToolButton*>(QString(), Qt::FindDirectChildrenOnly);
        REQUIRE(buttons.size() == 1);
        const auto* const toggle = buttons.front();
        CHECK(toggle->objectName().isEmpty());

        section.setObjectName("section_widget/section");
        CHECK(toggle->objectName() == "section_widget/section/toggle");

        // A later rename, including clearing the name, must not leave a stale locator behind.
        section.setObjectName("other/section");
        CHECK(toggle->objectName() == "other/section/toggle");
        section.setObjectName({});
        CHECK(toggle->objectName().isEmpty());
    }
}
