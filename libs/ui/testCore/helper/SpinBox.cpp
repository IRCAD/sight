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

#include "SpinBox.hpp"

#include <QDoubleSpinBox>
#include <QSpinBox>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

static void change(Tester& tester, const QPoint& pos)
{
    tester.interact(std::make_unique<MouseClick>(Qt::MouseButton::LeftButton, Qt::NoModifier, pos));
}

//------------------------------------------------------------------------------

void SpinBox::increment(Tester& tester, const Select& spinBox, int times)
{
    auto bt = tester.addInBacktrace(
        "incrementing \"" + spinBox.getDescription(tester) + "\" spin box " + std::to_string(times) + "times"
    );
    spinBox.select(tester);
    auto* widget = tester.get<QWidget*>();
    for(int i = 0 ; i < times ; i++)
    {
        change(tester, Tester::rightOf(widget));
    }
}

//------------------------------------------------------------------------------

void SpinBox::decrement(Tester& tester, const Select& spinBox, int times)
{
    auto bt = tester.addInBacktrace(
        "decrementing \"" + spinBox.getDescription(tester) + "\" spin box " + std::to_string(times) + "times"
    );
    spinBox.select(tester);
    auto* widget = tester.get<QWidget*>();
    for(int i = 0 ; i < times ; i++)
    {
        change(tester, Tester::leftOf(widget));
    }
}

//------------------------------------------------------------------------------

void SpinBox::valueEquals(Tester& tester, const Select& spinBox, const std::string& expected)
{
    auto bt = tester.addInBacktrace(
        "checking whether \"" + spinBox.getDescription(tester) + "\" spin box has the value " + expected
    );
    spinBox.select(tester);
    QString qExpected = QString::fromStdString(expected);
    tester.doubt(
        spinBox.getDescription(tester) + " should have the value " + expected,
        [&qExpected](QObject* obj)
        {
            if(auto* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(obj))
            {
                return doubleSpinBox->cleanText() == qExpected;
            }

            if(auto* intSpinBox = qobject_cast<QSpinBox*>(obj))
            {
                return intSpinBox->cleanText() == qExpected;
            }

            return false;
        });
}

//------------------------------------------------------------------------------

void SpinBox::valueEquals(Tester& tester, const Select& spinBox, int expected)
{
    auto bt = tester.addInBacktrace(
        "checking whether \"" + spinBox.getDescription(tester) + "\" spin box has the value " + std::to_string(expected)
    );
    spinBox.select(tester);
    tester.doubt(
        spinBox.getDescription(tester) + " should have the value " + std::to_string(expected),
        [&expected](QObject* obj)
        {
            if(auto* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(obj))
            {
                return doubleSpinBox->value() == expected;
            }

            if(auto* intSpinBox = qobject_cast<QSpinBox*>(obj))
            {
                return intSpinBox->value() == expected;
            }

            return false;
        });
}

} // namespace sight::ui::testCore::helper
