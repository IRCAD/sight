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

#include "button.hpp"

#include <QAction>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

static void take(
    tester& _tester,
    const selector& _button
)
{
    _button.select(_tester);
    if(_tester.is_a<QAction*>())
    {
        _tester.yields(
            "actual " + _button.get_description(_tester) + " button",
            [](QObject* _old)
            {
                return tester::get_widget_from_action(qobject_cast<QAction*>(_old));
            });
    }

    _tester.doubt<QWidget*>(
        _button.get_description(_tester) + " button is clickable",
        [](QWidget* _obj)
        {
            return _obj->isEnabled();
        });
}

//------------------------------------------------------------------------------

void button::push(tester& _tester, const selector& _button)
{
    auto bt = _tester.add_in_backtrace("pushing " + _button.get_description(_tester) + " button");
    take(_tester, _button);
    _tester.interact(std::make_unique<mouse_click>());
}

//------------------------------------------------------------------------------

void button::push_menu_bar_item(tester& _tester, const selector& _button)
{
    auto bt = _tester.add_in_backtrace("pushing " + _button.get_description(_tester) + " button");
    _button.select(_tester);
    _tester.do_something<QAction*>([](QAction* _obj){_obj->trigger();});
}

//------------------------------------------------------------------------------

void button::wait_for_clickability(tester& _tester, const selector& _button)
{
    auto bt = _tester.add_in_backtrace("waiting for " + _button.get_description(_tester) + " to become clickable");
    take(_tester, _button);
}

} // namespace sight::ui::test::helper
