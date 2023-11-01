/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/ui/qt/style_selector.hpp"

#include <core/com/slots.hxx>
#include <core/macros.hpp>
#include <core/runtime/path.hpp>

#include <service/macros.hpp>

#include <ui/__/preferences.hpp>

#include <QApplication>
#include <QFile>
#include <QResource>
#include <QTextStream>

#include <algorithm>
#include <filesystem>

namespace sight::module::ui::qt
{

static const core::com::slots::key_t UPDATE_FROM_PREFS_SLOT = "update_from_preferences";

//-----------------------------------------------------------------------------

style_selector::style_selector() noexcept
{
    new_slot(UPDATE_FROM_PREFS_SLOT, &style_selector::update_from_prefs, this);
}

//-----------------------------------------------------------------------------

style_selector::~style_selector() noexcept =
    default;

//-----------------------------------------------------------------------------

void style_selector::configuring()
{
}

//-----------------------------------------------------------------------------

void style_selector::starting()
{
    m_style_map["DEFAULT"] = std::filesystem::path("");

    const auto style_rc = core::runtime::get_module_resource_path("sight::module::ui::qt");

    // Stores each rcc & qss
    for(const auto& p : std::filesystem::directory_iterator(style_rc))
    {
        std::filesystem::path f = p;

        if(f.extension() == ".rcc")
        {
            const std::string filename = f.filename().replace_extension("").string();

            std::string name = filename;
            std::transform(
                filename.begin(),
                filename.end(),
                name.begin(),
                [](unsigned char _c) -> unsigned char {return static_cast<unsigned char>(std::toupper(_c));});

            m_style_map[name] = f.replace_extension("");
        }
    }

    // Apply theme from preferences if any.
    this->update_from_prefs();
}

//-----------------------------------------------------------------------------

void style_selector::stopping()
{
    m_style_map.clear();
}

//-----------------------------------------------------------------------------

void style_selector::updating()
{
}

//-----------------------------------------------------------------------------

void style_selector::change_style(const std::string& _style_name)
{
    auto path = m_style_map[_style_name];

    sight::ui::preferences preferences;

    // DEFAULT (no theme) case.
    if(path.empty())
    {
        qApp->setStyleSheet("");

        preferences.put("THEME", "DEFAULT");
        return;
    }

    // Load ressources
    [[maybe_unused]] const bool resource_loaded = QResource::registerResource(
        path.replace_extension(
            ".rcc"
        ).string().c_str()
    );
    SIGHT_ASSERT("Cannot load resources '" + path.replace_extension(".rcc").string() + "'.", resource_loaded);

    // Load stylesheet.
    QFile data(QString::fromStdString(path.replace_extension(".qss").string()));
    if(data.open(QFile::ReadOnly))
    {
        QTextStream style_in(&data);
        const QString style = style_in.readAll();
        data.close();
        qApp->setStyleSheet(style);
        preferences.put("THEME", _style_name);
    }
}

//-----------------------------------------------------------------------------

void style_selector::update_from_prefs()
{
    // Apply previously saved style in preferences file.
    try
    {
        sight::ui::preferences preferences;
        if(const auto& theme = preferences.get_optional<std::string>("THEME"); theme)
        {
            this->change_style(*theme);
        }
    }
    catch(const sight::ui::preferences_disabled& /*e*/)
    {
        // Nothing to do..
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
