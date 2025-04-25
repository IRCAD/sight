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

#include "preferences_configuration.hpp"

#include "button.hpp"
#include "dialog.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

static std::string map_to_string(const std::map<std::string, std::string>& _values)
{
    std::string res = "{";
    bool first      = true;
    for(const auto& [key, value] : _values)
    {
        if(!first)
        {
            res += ", ";
        }

        res += '"' + key + "\": \"" + value + '"';
    }

    return res + "}";
}

//------------------------------------------------------------------------------

void preferences_configuration::fill(tester& _tester, const std::map<std::string, std::string>& _values)
{
    auto bt = _tester.add_in_backtrace("fill preferences configuration window with " + map_to_string(_values));
    dialog::take(_tester, "preferences configuration window");
    QPointer<QWidget> window = _tester.get<QWidget*>();
    for(const auto& e : _values)
    {
        auto key   = e.first;
        auto value = e.second;
        _tester.take("preferences configuration window", window);
        _tester.yields("'" + key + "' field", key);
        QString q_value = QString::fromStdString(value);
        if(_tester.is_a<QLineEdit*>())
        {
            _tester.get<QLineEdit*>()->setText(q_value);
        }
        else if(_tester.is_a<QCheckBox*>())
        {
            _tester.get<QCheckBox*>()->setChecked(q_value == "true");
        }
        else if(_tester.is_a<QComboBox*>())
        {
            _tester.get<QComboBox*>()->setCurrentText(q_value);
        }
    }

    helper::button::push(_tester, selector::from_dialog("OK"));
    _tester.doubt(
        "the preferences configuration window is closed",
        [&window](QObject*) -> bool
        {
            return window == nullptr || !window->isVisible();
        });
}

} // namespace sight::ui::test::helper
