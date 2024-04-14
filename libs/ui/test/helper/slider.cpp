/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "slider.hpp"

#include <QSlider>
#include <QWidget>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

static std::string position_to_string(slider::position _pos)
{
    switch(_pos)
    {
        case slider::position::top:
            return "top";

        case slider::position::right:
            return "right";

        case slider::position::bottom:
            return "bottom";

        case slider::position::left:
            return "left";
    }

    return "";
}

//------------------------------------------------------------------------------

static QPoint position_of(slider::position _pos, const QWidget* _widget)
{
    switch(_pos)
    {
        case slider::position::top:
            return tester::top_of(_widget);

        case slider::position::right:
            return tester::right_of(_widget);

        case slider::position::bottom:
            return tester::bottom_of(_widget);

        case slider::position::left:
            return tester::left_of(_widget);
    }

    return {};
}

//------------------------------------------------------------------------------

static QSlider* take(tester& _tester, const selector& _slider)
{
    _slider.select(_tester);
    if(!_tester.is_a<QSlider*>())
    {
        _tester.yields<QSlider*>('"' + _slider.get_description(_tester) + "\" actual slider");
    }

    return _tester.get<QSlider*>();
}

//------------------------------------------------------------------------------

static void move_impl(tester& _tester, const selector& _slider, slider::position _pos, int _times = 1)
{
    QSlider* s      = take(_tester, _slider);
    QPoint position = position_of(_pos, s);
    for(int i = 0 ; i < _times ; i++)
    {
        _tester.interact(std::make_unique<mouse_click>(Qt::LeftButton, Qt::NoModifier, position));
    }
}

//------------------------------------------------------------------------------

void slider::move(tester& _tester, const selector& _slider, slider::position _pos)
{
    auto bt = _tester.add_in_backtrace(
        "moving \"" + _slider.get_description(_tester) + "\" slider to the " + position_to_string(_pos)
    );
    move_impl(_tester, _slider, _pos);
}

//------------------------------------------------------------------------------

void slider::move(tester& _tester, const selector& _slider, slider::position _pos, int _times)
{
    auto bt = _tester.add_in_backtrace(
        "moving " + std::to_string(_times) + " times \"" + _slider.get_description(_tester) + "\" slider to the "
        + position_to_string(_pos)
    );
    move_impl(_tester, _slider, _pos, _times);
}

//------------------------------------------------------------------------------

void slider::set(tester& _tester, const selector& _slider, int _value)
{
    auto bt =
        _tester.add_in_backtrace("setting \"" + _slider.get_description(_tester) + "\" to " + std::to_string(_value));
    take(_tester, _slider);
    _tester.do_something<QSlider*>([&_value](QSlider* _s){return _s->setValue(_value);});
}

} // namespace sight::ui::test::helper
