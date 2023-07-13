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

#include "Field.hpp"

#include <QLineEdit>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void Field::fill(Tester& tester, const Select& field, const std::string& text)
{
    auto bt = tester.addInBacktrace("fill " + field.getDescription(tester) + " field with \"" + text + '"');
    field.select(tester);
    tester.doSomething<QLineEdit*>([&text](QLineEdit* obj){obj->setText(QString::fromStdString(text));});
}

} // namespace sight::ui::testCore::helper
