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

// cspell:ignore qproperty

#include <ui/test/qt_application.hpp>

#include <ui/qt/widget/switch_button.hpp>

#include <doctest/doctest.h>

#include <QColor>
#include <QWidget>

#include <vector>

using sight::ui::qt::widget::switch_button;

TEST_SUITE("sight::ui::qt::widget::switch_button")
{
    TEST_CASE("does_not_share_a_default_name")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        QWidget parent;
        switch_button first(&parent);
        switch_button second(&parent);

        // An unnamed switch can later receive the service's key or a role.
        CHECK(first.objectName().isEmpty());
        CHECK(second.objectName().isEmpty());
        CHECK(parent.findChildren<switch_button*>("switch_button").empty());

        first.setObjectName("properties1_srv/bool_2");
        second.setObjectName("properties2_srv/bool_2");
        CHECK(parent.findChild<switch_button*>("properties1_srv/bool_2") == &first);
        CHECK(parent.findChild<switch_button*>("properties2_srv/bool_2") == &second);

        first.setObjectName({});
        CHECK(first.objectName().isEmpty());
        CHECK(second.objectName() == "properties2_srv/bool_2");
    }

    //------------------------------------------------------------------------------

    TEST_CASE("stylesheet_class_survives_naming")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        switch_button button;
        button.setStyleSheet(".switch_button { qproperty-track_color_on: #123456; min-width: 137px; }");
        button.ensurePolished();

        // The class selector, not objectName, carries the switch styling.
        CHECK(button.property("class").toString() == "switch_button");
        CHECK(button.track_color_on() == QColor("#123456"));
        CHECK(button.minimumWidth() == 137);

        button.setObjectName("properties1_srv/bool_2");
        CHECK(button.track_color_on() == QColor("#123456"));
        CHECK(button.minimumWidth() == 137);
        button.setObjectName({});
        CHECK(button.property("class").toString() == "switch_button");
        CHECK(button.minimumWidth() == 137);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("naming_does_not_change_switch_behaviour")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        switch_button button;
        std::vector<int> states;
        QObject::connect(
            &button,
            &switch_button::state_changed,
            &button,
            [&states](int _state)
        {
            states.push_back(_state);
        });

        CHECK(button.isCheckable());
        CHECK(button.check_state() == Qt::Unchecked);
        button.setObjectName("properties1_srv/bool_2");
        button.click();
        CHECK(button.check_state() == Qt::Checked);
        REQUIRE(states.size() == 1);
        CHECK(states.back() == Qt::Checked);

        // Disabled switches must not send a change to the settings service.
        button.setEnabled(false);
        button.click();
        CHECK(button.check_state() == Qt::Checked);
        CHECK(states.size() == 1);

        button.setEnabled(true);
        button.setObjectName({});
        button.set_check_state(Qt::Unchecked);
        CHECK(button.check_state() == Qt::Unchecked);
        REQUIRE(states.size() == 2);
        CHECK(states.back() == Qt::Unchecked);
        button.set_check_state(Qt::Unchecked);
        CHECK(states.size() == 2);
    }
}
