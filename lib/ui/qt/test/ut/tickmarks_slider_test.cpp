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

#include <ui/qt/widget/tickmarks_slider.hpp>

#include <doctest/doctest.h>

#include <QSlider>
#include <QWidget>

using sight::ui::qt::widget::tickmarks_slider;

//------------------------------------------------------------------------------

static QSlider& inner_slider(const tickmarks_slider& _widget)
{
    auto* const slider = _widget.findChild<QSlider*>(QString(), Qt::FindDirectChildrenOnly);
    REQUIRE(slider != nullptr);
    return *slider;
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::ui::qt::widget::tickmarks_slider")
{
    TEST_CASE("names_slider_after_owner")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        QWidget parent;
        tickmarks_slider first(&parent);
        tickmarks_slider second(&parent);
        const QSlider& first_slider  = inner_slider(first);
        const QSlider& second_slider = inner_slider(second);
        CHECK(first_slider.objectName().isEmpty());
        CHECK(second_slider.objectName().isEmpty());

        first.setObjectName("settings/first");
        second.setObjectName("settings/second");
        CHECK(parent.findChild<QSlider*>("settings/first/slider") == &first_slider);
        CHECK(parent.findChild<QSlider*>("settings/second/slider") == &second_slider);

        first.setObjectName("other/first");
        CHECK(first_slider.objectName() == "other/first/slider");
        CHECK(parent.findChild<QSlider*>("settings/first/slider") == nullptr);
        CHECK(second_slider.objectName() == "settings/second/slider");

        first.setObjectName({});
        CHECK(first_slider.objectName().isEmpty());
        CHECK(second_slider.objectName() == "settings/second/slider");
    }

    //------------------------------------------------------------------------------

    TEST_CASE("keeps_its_stylesheet_class_when_renamed")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        tickmarks_slider widget;
        QSlider& slider = inner_slider(widget);

        widget.setStyleSheet(".tickmarks_slider { min-width: 137px; }");
        slider.ensurePolished();
        CHECK(slider.property("class").toString() == "tickmarks_slider");
        CHECK(slider.minimumWidth() == 137);

        widget.setObjectName("settings/widget");
        CHECK(slider.minimumWidth() == 137);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("sets_its_value_by_text")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        tickmarks_slider widget;
        widget.set_tick_values({"zero", "one", "two"});

        widget.set_current_value("two");
        CHECK(widget.current_tick() == 2);
        CHECK(widget.current_value() == "two");

        // Not a tick: nothing changes.
        widget.set_current_value("three");
        CHECK(widget.current_value() == "two");
    }

    //------------------------------------------------------------------------------

    TEST_CASE("keeps_its_value_when_renamed")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        tickmarks_slider widget;

        widget.set_tick_values({"zero", "one", "two"});
        inner_slider(widget).setValue(2);
        CHECK(widget.current_tick() == 2);
        CHECK(widget.current_value() == "two");

        widget.setObjectName("settings/widget");
        widget.setObjectName({});
        CHECK(widget.current_tick() == 2);
    }
}
