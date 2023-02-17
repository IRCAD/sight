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

#include "PreferencesConfiguration.hpp"

#include "Button.hpp"
#include "Dialog.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

static std::string mapToString(const std::map<std::string, std::string>& values)
{
    std::string res = "{";
    bool first      = true;
    for(const auto& [key, value] : values)
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

void PreferencesConfiguration::fill(Tester& tester, const std::map<std::string, std::string>& values)
{
    auto bt = tester.addInBacktrace("fill preferences configuration window with " + mapToString(values));
    Dialog::take(tester, "preferences configuration window");
    QPointer<QWidget> window = tester.get<QWidget*>();
    for(const auto& e : values)
    {
        auto key   = e.first;
        auto value = e.second;
        tester.take("preferences configuration window", window);
        tester.yields('"' + key + "\" field", key);
        QString qValue = QString::fromStdString(value);
        if(tester.isA<QLineEdit*>())
        {
            tester.get<QLineEdit*>()->setText(qValue);
        }
        else if(tester.isA<QCheckBox*>())
        {
            tester.get<QCheckBox*>()->setChecked(qValue == "true");
        }
        else if(tester.isA<QComboBox*>())
        {
            tester.get<QComboBox*>()->setCurrentText(qValue);
        }
    }

    helper::Button::push(tester, Select::fromDialog("OK"));
    tester.doubt(
        "the preferences configuration window is closed",
        [&window](QObject*) -> bool
        {
            return window == nullptr || !window->isVisible();
        });
}

} // namespace sight::ui::testCore::helper
