/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <QString>
#include <QStringView>

namespace sight::ui::qt::layout::detail
{

/**
 * @brief Joins two names of the XML into one object name, "<owner>/<name>".
 *
 * A '/', '#' or '\' inside a name is escaped with '\', so that it is not taken for the separator of a locator or the
 * '#' of a role ordinal.
 *
 * @code{.cpp}
 * composed_object_name(u"toolbar", u"Load");     // "toolbar/Load"
 * composed_object_name(u"view/left", u"a#b");    // "view\/left/a\#b"
 * @endcode
 */
[[nodiscard]] inline QString composed_object_name(QStringView _owner, QStringView _name)
{
    const auto escaped = [](QStringView _component)
                         {
                             QString result;
                             result.reserve(_component.size());
                             for(const QChar character : _component)
                             {
                                 if(QStringView(u"/#\\").contains(character))
                                 {
                                     result += u'\\';
                                 }

                                 result += character;
                             }

                             return result;
                         };
    return escaped(_owner) + u'/' + escaped(_name);
}

} // namespace sight::ui::qt::layout::detail
