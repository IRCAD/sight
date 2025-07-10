/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

    enum class position
    {
        top,
        right,
        bottom,
        left
    };

    struct mouse_drag
    {
        mouse_drag(
            const QPoint& from,
            const QPoint& to,
            Qt::MouseButton button
        ) :
            m_from(from),
            m_to(to),
            m_button(button)
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
        int = 1
    );
    SIGHT_UI_TEST_API static  void check_value(tester& _tester, const selector& _slider, int _expected);
};

}
