/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <sight/ui/test/config.hpp>

#include "select.hpp"

#include <ui/test/tester.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <QApplication>
#include <QLabel>
#include <QSlider>

namespace sight::ui::test::helper
{

class tickmarks_slider_test
{
public:

    enum class position : std::uint8_t
    {
        top,
        right,
        bottom,
        left
    };

    struct mouse_drag
    {
        mouse_drag(
            const QPoint& _from,
            const QPoint& _to,
            Qt::MouseButton _button
        ) :
            m_from(_from),
            m_to(_to),
            m_button(_button)
        {
        }

        QPoint m_from;
        QPoint m_to;
        Qt::MouseButton m_button;
    };

    SIGHT_UI_TEST_API static void mouse_drag_test(tester& _tester, const selector& _slider, QPoint _from, QPoint _to);

    SIGHT_UI_TEST_API  static void move(
        tester& _tester,
        const selector& _slider,
        tickmarks_slider_test::position _pos,
        int _times = 1
    );

    SIGHT_UI_TEST_API static  void check_value(tester& _tester, const selector& _slider, const std::string& _expected);

    SIGHT_UI_TEST_API static void set_current_text(tester& _tester, const selector& _slider, const std::string& _text);

    SIGHT_UI_TEST_API static void set_current_tick(tester& _tester, const selector& _slider, int _tick);
};

} // namespace sight::ui::test::helper
