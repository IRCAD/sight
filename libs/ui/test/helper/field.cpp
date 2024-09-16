/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "field.hpp"

#include <QLineEdit>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

void field::fill(tester& _tester, const selector& _field, const std::string& _text)
{
    auto bt = _tester.add_in_backtrace("fill " + _field.get_description(_tester) + " field with '" + _text + "'");
    _field.select(_tester);
    _tester.do_something<QLineEdit*>([&_text](QLineEdit* _obj){_obj->setText(QString::fromStdString(_text));});
}

} // namespace sight::ui::test::helper
