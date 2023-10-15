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

#pragma once

#include "modules/ui/qt/config.hpp"

#include <service/controller.hpp>

#include <QString>

#include <filesystem>

namespace sight::module::ui::qt
{

/**
 * @brief Selector of theme/style.
 * Theme should be stored in the rc/ folder of this module.
 *
 * When a new theme is applied on an application, it will be saved in preference file with "THEME" key.
 * style_selector will first load & apply the theme saved in preference file.
 *
 * Theme names are generated by parsing the *.rcc (from *.qrc files) and then name are uppercased (ex: flatdark.rcc
 * becames FLATDARK).
 *
 * To change a theme while application is running please use a preferences_configuration like :
 * @code{.xml}
    <service uid=".." type="sight::module::ui::qt::preferences_configuration">
        <preference>
            <type>combobox</type>
            <name>Color Theme</name>
            <key>THEME</key>
            <values>DEFAULT,DARKSTYLE,FLATDARK</values>
            <default_value>DEFAULT</default_value>
        </preference>
    </service>
 * @endcode
 *
 * Next connect the "parametersModified" signal to the "updateFromPreferences" slot of this service.
 *
 * @section Slots Slots
 * - \b updateFromPrefs(): called when changing the preference key "THEME", apply the selected theme
 * by reading preference file.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::qt::style_selector" />
   @endcode
 */
class MODULE_UI_QT_CLASS_API style_selector : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(style_selector, sight::service::controller);

    /// Constructor, initializes slots.
    MODULE_UI_QT_API style_selector() noexcept;

    /// Destructor, clears the position map.
    MODULE_UI_QT_API ~style_selector() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /// Does nothing.
    MODULE_UI_QT_API void configuring() override;

    /**
     * @brief Starts and setups the service by parsing rc folder to find availble themes.
     */
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Does nothing
     */
    MODULE_UI_QT_API void stopping() override;

    /**
     * @brief Does nothing.
     */
    MODULE_UI_QT_API void updating() override;

private:

    /// Applies the style _stylename.
    void changeStyle(const std::string& _styleName);

    ///Slot: check preference key THEME, and update theme accordingly.
    void updateFromPrefs();

    /// Map to stores theme names and their path.
    std::map<std::string, std::filesystem::path> m_styleMap;
};

} //namespace sight::module::ui::qt
