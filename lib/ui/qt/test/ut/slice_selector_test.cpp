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

#include <ui/qt/slice_selector.hpp>

#include <doctest/doctest.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QWidget>

#include <cstdint>

using sight::ui::qt::slice_selector;

TEST_SUITE("sight::ui::qt::slice_selector")
{
    TEST_CASE("a_crowded_selector_keeps_room_for_its_slider")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        constexpr int handle = 14;
        for(const bool position : {false, true})
        {
            CAPTURE(position);

            // As the application style sheets do: a handle width without a min-width gives a slider no minimum length.
            QWidget window;
            window.setStyleSheet(QString("QSlider::handle { width: %1px; height: %1px; }").arg(handle));
            auto* const selector = position ? new slice_selector(true, false, 0.0, &window)
                                            : new slice_selector(true, false, std::uint8_t {1}, &window);
            auto* const layout = new QHBoxLayout(&window);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->addWidget(selector);

            const auto* const axis   = selector->findChild<QComboBox*>();
            const auto* const label  = selector->findChild<QLineEdit*>();
            const auto* const slider = selector->findChild<QSlider*>();
            REQUIRE(axis != nullptr);
            REQUIRE(label != nullptr);
            REQUIRE(slider != nullptr);

            // Less room than the other parts need: the slider must not be the one left with nothing.
            window.setFixedWidth(axis->minimumSizeHint().width() + label->minimumSizeHint().width());
            window.show();
            layout->activate();

            CHECK(slider->width() > 2 * handle);
        }
    }
}
