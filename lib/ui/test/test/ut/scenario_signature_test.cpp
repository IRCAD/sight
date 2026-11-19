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

#include <ui/test/expectation.hpp>
#include <ui/test/scenario.hpp>

#include <doctest/doctest.h>

#include <QLabel>
#include <QObject>
#include <QSlider>
#include <QWidget>

#include <memory>

namespace sight::ui::test::detail
{

/// Whether read() accepts a getter, checked on the declaration only.
template<typename Getter, typename Widget = QWidget>
concept readable = requires(const scenario& _actions, Getter _getter)
{
    _actions.read<Widget>("target", _getter);
};

TEST_SUITE("sight::ui::test::scenario")
{
    TEST_CASE("accepts_only_getters_that_copy_a_value")
    {
        const auto copy = [](const QWidget& _widget){return _widget.size();};
        const auto label_text = [](const QLabel& _label){return _label.text();};
        const auto pointer = [](const QWidget& _widget){return &_widget;};
        const auto nothing   = [](const QWidget&){};
        const auto move_only = [](const QWidget&){return std::make_unique<int>(0);};

        CHECK(readable<decltype(copy)>);
        CHECK(readable<decltype(label_text), QLabel>);
        CHECK_FALSE(readable<decltype(label_text)>);
        CHECK_FALSE(readable<decltype(pointer)>);
        CHECK_FALSE(readable<decltype(nothing)>);
        CHECK_FALSE(readable<decltype(move_only)>);
        CHECK_FALSE(readable<decltype(copy), QObject>);

        const auto enabled = [](const QWidget& _widget){return _widget.isEnabled();};
        CHECK(widget_predicate<decltype(enabled), QWidget>);
        CHECK(widget_predicate<decltype(enabled), QSlider>);
        CHECK_FALSE(widget_predicate<decltype(nothing), QWidget>);
        CHECK_FALSE(widget_predicate<decltype(label_text), QWidget>);
    }
} // TEST_SUITE

} // namespace sight::ui::test::detail
