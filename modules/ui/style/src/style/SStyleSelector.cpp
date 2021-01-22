/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "style/SStyleSelector.hpp"

#include <core/com/Slots.hxx>
#include <core/macros.hpp>

#include <fwPreferences/helper.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <QApplication>
#include <QFile>
#include <QResource>
#include <QTextStream>

#include <algorithm>
#include <filesystem>

fwServicesRegisterMacro( ::fwServices::IController, ::style::SStyleSelector )

namespace style
{

static const core::com::Slots::SlotKeyType s_UPDATE_FROM_PREFS_SLOT = "updateFromPreferences";

//-----------------------------------------------------------------------------

SStyleSelector::SStyleSelector() noexcept
{
    newSlot(s_UPDATE_FROM_PREFS_SLOT, &SStyleSelector::updateFromPrefs, this);
}

//-----------------------------------------------------------------------------

SStyleSelector::~SStyleSelector() noexcept
{
}

//-----------------------------------------------------------------------------

void SStyleSelector::configuring()
{
}

//-----------------------------------------------------------------------------

void SStyleSelector::starting()
{
    m_styleMap["DEFAULT"] = std::filesystem::path("");

    const auto styleRc = ::fwRuntime::getModuleResourcePath("style");

    // Stores each rcc & qss

    for(auto& p: std::filesystem::directory_iterator(styleRc))
    {
        std::filesystem::path f = p;

        if(f.extension() == ".rcc")
        {
            const std::string filename = f.filename().replace_extension("").string();

            std::string name = filename;
            std::transform(filename.begin(), filename.end(), name.begin(),
                           [](unsigned char c) -> unsigned char { return std::toupper(c); });

            m_styleMap[name] = f.replace_extension("");

        }
    }

    // Apply theme from preferences if any.
    this->updateFromPrefs();
}

//-----------------------------------------------------------------------------

void SStyleSelector::stopping()
{
    m_styleMap.clear();
}

//-----------------------------------------------------------------------------

void SStyleSelector::updating()
{

}

//-----------------------------------------------------------------------------

void SStyleSelector::changeStyle(const std::string& _styleName)
{
    auto path = m_styleMap[_styleName];

    // DEFAULT (no theme) case.
    if(path.empty())
    {
        qApp->setStyleSheet("");
        ::fwPreferences::setPreference("THEME", "DEFAULT");
        ::fwPreferences::savePreferences();
        return;
    }

    // Load ressources
    const bool resourceLoaded = QResource::registerResource(path.replace_extension(".rcc").string().c_str());
    SLM_ASSERT("Cannot load resources '"+path.replace_extension(".rcc").string() + "'.", resourceLoaded);

    // Load stylesheet.
    QFile data(QString::fromStdString(path.replace_extension(".qss").string()));
    if(data.open(QFile::ReadOnly))
    {
        QTextStream styleIn(&data);
        const QString style = styleIn.readAll();
        data.close();
        qApp->setStyleSheet(style);
        ::fwPreferences::setPreference("THEME", _styleName);
        ::fwPreferences::savePreferences();
    }
}

//-----------------------------------------------------------------------------

void SStyleSelector::updateFromPrefs()
{
    // Apply previously saved style in preferences file.
    const std::string value = ::fwPreferences::getPreference("THEME");
    if(!value.empty())
    {
        this->changeStyle(value);
    }
}

//-----------------------------------------------------------------------------

}
