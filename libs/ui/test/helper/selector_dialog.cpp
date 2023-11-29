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

#include "selector_dialog.hpp"

#include "button.hpp"
#include "dialog.hpp"
#include "list_widget.hpp"

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

void selector_dialog::select(tester& _tester, const std::string& _selection)
{
    auto bt = _tester.add_in_backtrace("selecting \"" + _selection + "\" from the selection window");
    helper::list_widget::set_current_text(_tester, selector::from_dialog("selectionList"), _selection);
    button::push(_tester, selector::from_dialog("Ok"));
}

} // namespace sight::ui::test::helper
