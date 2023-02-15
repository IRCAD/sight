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

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

static void take(
    Tester& tester,
    const Select& button
)
{
    button.select(tester);
    if(tester.isA<QAction*>())
    {
        tester.yields(
            "actual " + button.getDescription(tester) + " button",
            [](QObject* old)
            {
                return Tester::getWidgetFromAction(qobject_cast<QAction*>(old));
            });
    }

    tester.doubt<QWidget*>(
        button.getDescription(tester) + " button is clickable",
        [](QWidget* obj)
        {
            return obj->isEnabled();
        });
}

//------------------------------------------------------------------------------

void Button::push(Tester& tester, const Select& button)
{
    auto bt = tester.addInBacktrace("pushing " + button.getDescription(tester) + " button");
    take(tester, button);
    tester.interact(std::make_unique<MouseClick>());
}

//------------------------------------------------------------------------------

void Button::waitForClickability(Tester& tester, const Select& button)
{
    auto bt = tester.addInBacktrace("waiting for " + button.getDescription(tester) + " to become clickable");
    take(tester, button);
}

} // namespace sight::ui::testCore::helper
