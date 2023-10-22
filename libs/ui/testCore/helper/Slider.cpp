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

#include "Slider.hpp"

#include <QSlider>
#include <QWidget>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

static std::string position_to_string(Slider::Position _pos)
{
    switch(_pos)
    {
        case Slider::Position::TOP:
            return "top";

        case Slider::Position::RIGHT:
            return "right";

        case Slider::Position::BOTTOM:
            return "bottom";

        case Slider::Position::LEFT:
            return "left";
    }

    return "";
}

//------------------------------------------------------------------------------

static QPoint position_of(Slider::Position _pos, const QWidget* _widget)
{
    switch(_pos)
    {
        case Slider::Position::TOP:
            return Tester::topOf(_widget);

        case Slider::Position::RIGHT:
            return Tester::rightOf(_widget);

        case Slider::Position::BOTTOM:
            return Tester::bottomOf(_widget);

        case Slider::Position::LEFT:
            return Tester::leftOf(_widget);
    }

    return {};
}

//------------------------------------------------------------------------------

static QSlider* take(Tester& _tester, const Select& _slider)
{
    _slider.select(_tester);
    if(!_tester.is_a<QSlider*>())
    {
        _tester.yields<QSlider*>('"' + _slider.getDescription(_tester) + "\" actual slider");
    }

    return _tester.get<QSlider*>();
}

//------------------------------------------------------------------------------

static void move_impl(Tester& _tester, const Select& _slider, Slider::Position _pos, int _times = 1)
{
    QSlider* s      = take(_tester, _slider);
    QPoint position = position_of(_pos, s);
    for(int i = 0 ; i < _times ; i++)
    {
        _tester.interact(std::make_unique<MouseClick>(Qt::LeftButton, Qt::NoModifier, position));
    }
}

//------------------------------------------------------------------------------

void Slider::move(Tester& _tester, const Select& _slider, Slider::Position _pos)
{
    auto bt = _tester.addInBacktrace(
        "moving \"" + _slider.getDescription(_tester) + "\" slider to the " + position_to_string(_pos)
    );
    move_impl(_tester, _slider, _pos);
}

//------------------------------------------------------------------------------

void Slider::move(Tester& _tester, const Select& _slider, Slider::Position _pos, int _times)
{
    auto bt = _tester.addInBacktrace(
        "moving " + std::to_string(_times) + " times \"" + _slider.getDescription(_tester) + "\" slider to the "
        + position_to_string(_pos)
    );
    move_impl(_tester, _slider, _pos, _times);
}

//------------------------------------------------------------------------------

void Slider::set(Tester& _tester, const Select& _slider, int _value)
{
    auto bt =
        _tester.addInBacktrace("setting \"" + _slider.getDescription(_tester) + "\" to " + std::to_string(_value));
    take(_tester, _slider);
    _tester.doSomething<QSlider*>([&_value](QSlider* _s){return _s->setValue(_value);});
}

} // namespace sight::ui::test_core::helper
