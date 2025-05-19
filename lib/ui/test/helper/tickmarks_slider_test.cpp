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

#include "tickmarks_slider_test.hpp"

#include "select.hpp"

#include <ui/test/tester.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <QApplication>
#include <QLabel>
#include <QSlider>

#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <QWidget>

#include <utility>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

inline std::string position_to_string(tickmarks_slider_test::position _pos)
{
    switch(_pos)
    {
        case tickmarks_slider_test::position::top:
            return "top";

        case tickmarks_slider_test::position::right:
            return "right";

        case tickmarks_slider_test::position::bottom:
            return "bottom";

        case tickmarks_slider_test::position::left:
            return "left";
    }

    return "";
}

//------------------------------------------------------------------------------

inline  QSlider* take(tester& _tester, const selector& _slider)
{
    _slider.select(_tester);

    if(!_tester.is_a<QSlider*>())
    {
        _tester.yields<QSlider*>("'" + _slider.get_description(_tester) + "' is not a QSlider");
    }

    return _tester.get<QSlider*>();
}

//------------------------------------------------------------------------------

inline void move_impl(tester& _tester, const selector& _slider, tickmarks_slider_test::position _pos, int _times = 1)
{
    const QAbstractSlider::SliderAction action = _pos == tickmarks_slider_test::position::left
                                                 || _pos == tickmarks_slider_test::position::top
                                                 ? QAbstractSlider::SliderAction::SliderPageStepSub
                                                 : QAbstractSlider::SliderAction::SliderPageStepAdd;

    QSlider* const slider = take(_tester, _slider);

    for(int i = 0 ; i < _times ; i++)
    {
        slider->triggerAction(action);
    }
}

//------------------------------------------------------------------------------
void tickmarks_slider_test::move(
    tester& _tester,
    const selector& _slider,
    tickmarks_slider_test::position _pos,
    int _times
)
{
    auto bt = _tester.add_in_backtrace(
        "moving \"" + _slider.get_description(_tester) + "\" slider "
        + std::to_string(_times) + " step(s) to the " + position_to_string(_pos)
    );
    move_impl(_tester, _slider, _pos, _times);
}

//----------------------------------------------------------------------------------
void tickmarks_slider_test::check_value(tester& _tester, const selector& _slider, int _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking \"" + _slider.get_description(_tester) + "\" value == " + std::to_string(_expected)
    );
    QSlider* slider = take(_tester, _slider);
    int value       = slider->value();
    CPPUNIT_ASSERT_EQUAL(_expected, value);
}

} // namespace sight::ui::test::helper
