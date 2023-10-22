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

#pragma once

#include <QObject>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

template<typename T>
void Dialog::take(Tester& _tester, const std::string& _desc, const std::string& _child_name)
{
    auto bt = _tester.addInBacktrace("take " + _desc + " dialog");
    _tester.take<T>(
        _desc,
        []{return qobject_cast<T>(qApp->activeModalWidget());},
        [&_child_name](T _o)
        {
            return _o->isVisible()
            && (_child_name.empty() || _o->template findChild<QObject*>(
                    QString::fromStdString(
                        _child_name
                    )
                ) != nullptr);
        });
}

//------------------------------------------------------------------------------

template<typename T>
void Dialog::maybeTake(Tester& _tester, const std::string& _desc, const std::string& _child_name)
{
    _tester.maybeTake<T>(
        _desc,
        []{return qobject_cast<T>(qApp->activeModalWidget());},
        [&_child_name](T _o)
        {
            return _o->isVisible()
            && (_child_name.empty() || _o->template findChild<QObject*>(
                    QString::fromStdString(
                        _child_name
                    )
                ) != nullptr);
        });
}

} // namespace sight::ui::testCore::helper
