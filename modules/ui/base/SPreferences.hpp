/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/ui/base/config.hpp"

#include <ui/base/preferences/IPreferences.hpp>

#include <filesystem>

namespace sight::module::ui::base
{

/**
 * @brief Service to load and save preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::base::SPreferences">
       <inout key="preferences" uid="..." />
       <filename>path/to/my/preferences.json</filename>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b preferences [sight::data::Object]: object containing the preferences to load/save.
 * @subsection Configuration Configuration
 * - \b filename : path to the preferences file
 */
class MODULE_UI_BASE_CLASS_API SPreferences : public ::sight::ui::base::preferences::IPreferences
{

public:
    SIGHT_DECLARE_SERVICE(SPreferences, ::sight::ui::base::preferences::IPreferences)

    ///Destructor
    MODULE_UI_BASE_API SPreferences() noexcept
    {
    }
    MODULE_UI_BASE_API virtual ~SPreferences() noexcept
    {
    }

protected:

    /// Starts service and loads preferences if exists.
    MODULE_UI_BASE_API void starting() override;

    /// Stops service and saves preferences.
    MODULE_UI_BASE_API void stopping() override;

    /// Updates service and saves preferences.
    MODULE_UI_BASE_API void updating() override;

    MODULE_UI_BASE_API void configuring() override;

    /// preference file
    std::filesystem::path m_prefFile;

private:

    /// loads preferences
    void load();
    /// saves preferences
    void save();
};

} // namespace sight::module::ui::base
