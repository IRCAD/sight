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

#include "FileDialog.hpp"

#include "Dialog.hpp"
#include "Field.hpp"

#include <QFileDialog>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void FileDialog::fill(Tester& tester, const std::filesystem::path& path)
{
    auto bt = tester.addInBacktrace("fill " + path.string() + " in the file window");
    Dialog::take<QFileDialog*>(tester, "file window");
    const QPointer<QFileDialog> fileWindow = tester.get<QFileDialog*>();
    helper::Field::fill(tester, Select::fromDialog("fileNameEdit"), path.string());
    tester.interact(std::make_unique<KeyboardClick>(Qt::Key_Enter));
    tester.doubt(
        "the file window is closed",
        [&fileWindow](QObject*)
        {
            return fileWindow == nullptr || !fileWindow->isVisible();
        });
}

} // namespace sight::ui::testCore::helper
