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

#include "Button.hpp"

#include <QAction>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

static void take(
    Tester& _tester,
    const Select& _button
)
{
    _button.select(_tester);
    if(_tester.is_a<QAction*>())
    {
        _tester.yields(
            "actual " + _button.getDescription(_tester) + " button",
            [](QObject* _old)
            {
                return Tester::getWidgetFromAction(qobject_cast<QAction*>(_old));
            });
    }

    _tester.doubt<QWidget*>(
        _button.getDescription(_tester) + " button is clickable",
        [](QWidget* _obj)
        {
            return _obj->isEnabled();
        });
}

//------------------------------------------------------------------------------

void Button::push(Tester& _tester, const Select& _button)
{
    auto bt = _tester.addInBacktrace("pushing " + _button.getDescription(_tester) + " button");
    take(_tester, _button);
    _tester.interact(std::make_unique<MouseClick>());
}

//------------------------------------------------------------------------------

void Button::pushMenuBarItem(Tester& _tester, const Select& _button)
{
    auto bt = _tester.addInBacktrace("pushing " + _button.getDescription(_tester) + " button");
    _button.select(_tester);
    _tester.doSomething<QAction*>([](QAction* _obj){_obj->trigger();});
}

//------------------------------------------------------------------------------

void Button::waitForClickability(Tester& _tester, const Select& _button)
{
    auto bt = _tester.addInBacktrace("waiting for " + _button.getDescription(_tester) + " to become clickable");
    take(_tester, _button);
}

} // namespace sight::ui::test_core::helper
