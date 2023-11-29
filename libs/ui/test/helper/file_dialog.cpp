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

#include "file_dialog.hpp"

#include "dialog.hpp"
#include "field.hpp"

#include <QFileDialog>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

void file_dialog::fill(tester& _tester, const std::filesystem::path& _path)
{
    auto bt = _tester.add_in_backtrace("fill " + _path.string() + " in the file window");
    dialog::take<QFileDialog*>(_tester, "file window");
    const QPointer<QFileDialog> file_window = _tester.get<QFileDialog*>();
    helper::field::fill(_tester, selector::from_dialog("fileNameEdit"), _path.string());
    _tester.interact(std::make_unique<keyboard_click>(Qt::Key_Enter));
    _tester.doubt(
        "the file window is closed",
        [&file_window](QObject*)
        {
            return file_window == nullptr || !file_window->isVisible();
        });
}

} // namespace sight::ui::test::helper
