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

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void ComboBox::select(Tester& _tester, const Select& _combo_box, const std::string& _text)
{
    auto bt = _tester.addInBacktrace(
        "selecting " + _text + " in \"" + _combo_box.getDescription(
            _tester
        ) + "\" combo box"
    );
    _combo_box.select(_tester);
    _tester.doSomething<QComboBox*>([&_text](QComboBox* _cb){_cb->setCurrentText(QString::fromStdString(_text));});
}

//------------------------------------------------------------------------------

void ComboBox::valueEquals(Tester& _tester, const Select& _combo_box, const std::string& _expected)
{
    auto bt = _tester.addInBacktrace(
        "checking whether " + _combo_box.getDescription(
            _tester
        ) + " combo box value is \"" + _expected + '"'
    );
    _combo_box.select(_tester);
    QString q_expected = QString::fromStdString(_expected);
    _tester.doubt<QComboBox*>(
        _combo_box.getDescription(_tester) + " combo box value should be \"" + _expected + '"',
        [&q_expected](QComboBox* _obj){return _obj->currentText() == q_expected;});
}

} // namespace sight::ui::test_core::helper
