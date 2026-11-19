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

#include <ui/qt/widget/speed_dial.hpp>

#include <doctest/doctest.h>

#include <QWidget>

#include <memory>

using sight::ui::qt::widget::speed_dial;

TEST_SUITE("sight::ui::qt::widget::speed_dial")
{
    TEST_CASE("names_its_actions_after_itself")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        QWidget window;
        speed_dial dial;

        // A change of parent gives the container of the actions to the window.
        dial.setParent(&window);

        dial.setObjectName("toolbar/dial");
        const auto* const actions = window.findChild<QWidget*>("toolbar/dial/actions");
        REQUIRE(actions != nullptr);

        dial.setObjectName("other/dial");
        CHECK(actions->objectName() == "other/dial/actions");

        dial.setObjectName({});
        CHECK(actions->objectName().isEmpty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("keeps_the_name_of_its_actions_when_they_outlive_it")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        QWidget window;
        auto dial = std::make_unique<speed_dial>();
        dial->setParent(&window);
        dial->setObjectName("toolbar/dial");
        const auto* const actions = window.findChild<QWidget*>("toolbar/dial/actions");
        REQUIRE(actions != nullptr);

        dial.reset();
        CHECK(actions->objectName() == "toolbar/dial/actions");
    }
}
