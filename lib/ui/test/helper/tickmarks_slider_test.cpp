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

#include "tickmarks_slider_test.hpp"

#include "select.hpp"

#include <ui/test/tester.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <QApplication>
#include <QLabel>
#include <qobject.h>
#include <QSlider>
#include <qtestsupport_core.h>

#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <qwidget.h>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

static inline std::string position_to_string(tickmarks_slider_test::position _pos)
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

static inline  QSlider* find_slider(tester& _tester, const selector& _slider)
{
    _slider.select(_tester);
    QSlider* slider = nullptr;

    _tester.doubt(
        "The slider should have a QSlider child",
        [&slider](QObject* _obj)
        {
            slider = _obj->findChild<QSlider*>();
            return slider != nullptr;
        });

    return slider;
}

//------------------------------------------------------------------------------

static inline void move_impl(
    tester& _tester,
    const selector& _slider,
    tickmarks_slider_test::position _pos,
    int _times = 1
)
{
    const QAbstractSlider::SliderAction action = _pos == tickmarks_slider_test::position::left
                                                 || _pos == tickmarks_slider_test::position::top
                                                 ? QAbstractSlider::SliderAction::SliderPageStepSub
                                                 : QAbstractSlider::SliderAction::SliderPageStepAdd;

    _slider.select(_tester);
    QSlider* const slider = find_slider(_tester, _slider);

    qApp->postEvent(
        qApp,
        new test_event(
            [action, _times, slider]
        {
            for(int i = 0 ; i < _times ; i++)
            {
                slider->triggerAction(action);
            }
        })
    );

    qApp->processEvents();
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
void tickmarks_slider_test::check_value(tester& _tester, const selector& _slider, const std::string& _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking \"" + _slider.get_description(_tester) + "\" value == " + _expected
    );

    _slider.select(_tester);

    _tester.doubt(
        _slider.get_description(_tester) + " should have the current_value property set to \"" + _expected + "\"",
        [_expected](QObject* _obj)
        {
            return _obj->property("current_value").toString().toStdString() == _expected;
        });
}

//----------------------------------------------------------------------------------

void tickmarks_slider_test::mouse_drag_test(tester& _tester, const selector& _slider, QPoint _from, QPoint _to)
{
    auto bt = _tester.add_in_backtrace("dragging \"" + _slider.get_description(_tester) + "\"");

    _slider.select(_tester);
    auto* widget = _tester.get<QWidget*>();

    qApp->postEvent(
        qApp,
        new test_event(
            [widget, _from, _to]
        {
            tester::mouse_move(widget, _from);
            QTest::mousePress(widget, Qt::LeftButton, Qt::NoModifier, _from);
            tester::mouse_move(widget, _to, 10, Qt::LeftButton, Qt::NoModifier);
            QTest::mouseRelease(widget, Qt::LeftButton, Qt::NoModifier, _to);
        })
    );
}

//------------------------------------------------------------------------------

void tickmarks_slider_test::set_current_text(
    tester& _tester,
    const selector& _slider,
    const std::string& _text
)
{
    auto bt = _tester.add_in_backtrace(
        "set current text of \"" + _slider.get_description(_tester) + "\" slider to \"" + _text + "\""
    );

    _slider.select(_tester);
    auto* widget = _tester.get<QWidget*>();

    qApp->postEvent(
        qApp,
        new test_event(
            [widget, _text]
        {
            widget->setProperty("current_value", QString::fromStdString(_text));
        })
    );

    _tester.doubt(
        _slider.get_description(_tester) + " should have the current_value property set to \"" + _text + "\"",
        [&_text](QObject* _obj)
        {
            return _obj->property("current_value").toString().toStdString() == _text;
        });
}

//------------------------------------------------------------------------------

void tickmarks_slider_test::set_current_tick(
    tester& _tester,
    const selector& _slider,
    int _tick
)
{
    auto bt = _tester.add_in_backtrace(
        "set current tick of \"" + _slider.get_description(_tester) + "\" slider to \"" + std::to_string(_tick) + "\""
    );

    _slider.select(_tester);
    auto* widget = _tester.get<QWidget*>();

    qApp->postEvent(
        qApp,
        new test_event(
            [widget, _tick]
        {
            widget->setProperty("current_tick", _tick);
        })
    );

    _tester.doubt(
        _slider.get_description(_tester) + " should have the current_tick property set to " + std::to_string(_tick),
        [&_tick](QObject* _obj)
        {
            return _obj->property("current_tick").toInt() == _tick;
        });
}

} // namespace sight::ui::test::helper
