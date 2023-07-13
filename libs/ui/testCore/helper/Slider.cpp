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

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

static std::string positionToString(Slider::Position pos)
{
    switch(pos)
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

static QPoint positionOf(Slider::Position pos, const QWidget* widget)
{
    switch(pos)
    {
        case Slider::Position::TOP:
            return Tester::topOf(widget);

        case Slider::Position::RIGHT:
            return Tester::rightOf(widget);

        case Slider::Position::BOTTOM:
            return Tester::bottomOf(widget);

        case Slider::Position::LEFT:
            return Tester::leftOf(widget);
    }

    return {};
}

//------------------------------------------------------------------------------

static QSlider* take(Tester& tester, const Select& slider)
{
    slider.select(tester);
    if(!tester.isA<QSlider*>())
    {
        tester.yields<QSlider*>('"' + slider.getDescription(tester) + "\" actual slider");
    }

    return tester.get<QSlider*>();
}

//------------------------------------------------------------------------------

static void moveImpl(Tester& tester, const Select& slider, Slider::Position pos, int times = 1)
{
    QSlider* s      = take(tester, slider);
    QPoint position = positionOf(pos, s);
    for(int i = 0 ; i < times ; i++)
    {
        tester.interact(std::make_unique<MouseClick>(Qt::LeftButton, Qt::NoModifier, position));
    }
}

//------------------------------------------------------------------------------

void Slider::move(Tester& tester, const Select& slider, Slider::Position pos)
{
    auto bt = tester.addInBacktrace(
        "moving \"" + slider.getDescription(tester) + "\" slider to the " + positionToString(pos)
    );
    moveImpl(tester, slider, pos);
}

//------------------------------------------------------------------------------

void Slider::move(Tester& tester, const Select& slider, Slider::Position pos, int times)
{
    auto bt = tester.addInBacktrace(
        "moving " + std::to_string(times) + " times \"" + slider.getDescription(tester) + "\" slider to the "
        + positionToString(pos)
    );
    moveImpl(tester, slider, pos, times);
}

//------------------------------------------------------------------------------

void Slider::set(Tester& tester, const Select& slider, int value)
{
    auto bt = tester.addInBacktrace("setting \"" + slider.getDescription(tester) + "\" to " + std::to_string(value));
    take(tester, slider);
    tester.doSomething<QSlider*>([&value](QSlider* s){return s->setValue(value);});
}

} // namespace sight::ui::testCore::helper
