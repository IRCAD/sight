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

#include "ComboBox.hpp"

#include <QComboBox>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void ComboBox::select(Tester& tester, const Select& comboBox, const std::string& text)
{
    auto bt = tester.addInBacktrace("selecting " + text + " in \"" + comboBox.getDescription(tester) + "\" combo box");
    comboBox.select(tester);
    tester.doSomething<QComboBox*>([&text](QComboBox* cb){cb->setCurrentText(QString::fromStdString(text));});
}

//------------------------------------------------------------------------------

void ComboBox::valueEquals(Tester& tester, const Select& comboBox, const std::string& expected)
{
    auto bt = tester.addInBacktrace(
        "checking whether " + comboBox.getDescription(
            tester
        ) + " combo box value is \"" + expected + '"'
    );
    comboBox.select(tester);
    QString qExpected = QString::fromStdString(expected);
    tester.doubt<QComboBox*>(
        comboBox.getDescription(tester) + " combo box value should be \"" + expected + '"',
        [&qExpected](QComboBox* obj){return obj->currentText() == qExpected;});
}

} // namespace sight::ui::testCore::helper
