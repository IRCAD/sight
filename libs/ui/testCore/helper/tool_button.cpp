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

#include "tool_button.hpp"

#include <QToolButton>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void tool_button::tool_tip_matches(tester& _tester, const selector& _label, const std::string& _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking if \"" + _label.get_description(_tester) + "\" tool_button tooltip is equal to \"" + _expected + '"'
    );
    _label.select(_tester);
    _tester.doubt<QToolButton*>(
        '"' + _label.get_description(_tester) + "\" tool_button tooltip should be equal to \"" + _expected + '"',
        [&_expected](QToolButton* _obj){return _obj->toolTip() == QString::fromStdString(_expected);});
}

} // namespace sight::ui::test_core::helper
