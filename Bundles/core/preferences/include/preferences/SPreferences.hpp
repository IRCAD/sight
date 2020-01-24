/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "preferences/config.hpp"

#include <fwPreferences/IPreferences.hpp>

#include <filesystem>

namespace preferences
{

/**
 * @brief Service to load and save preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::preferences::SPreferences">
       <inout key="preferences" uid="..." />
       <filename>path/to/my/preferences.json</filename>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b preferences [::fwData::Object]: object containing the preferences to load/save.
 * @subsection Configuration Configuration
 * - \b filename : path to the preferences file
 */
class PREFERENCES_CLASS_API SPreferences : public ::fwPreferences::IPreferences
{

public:
    fwCoreServiceMacro(SPreferences, ::fwPreferences::IPreferences)

    ///Destructor

    SPreferences() noexcept
    {
    }
    virtual ~SPreferences() noexcept
    {
    }

protected:

    /// Starts service and loads preferences if exists.
    PREFERENCES_API virtual void starting() override;

    /// Stops service and saves preferences.
    PREFERENCES_API virtual void stopping() override;

    /// Updates service and saves preferences.
    PREFERENCES_API virtual void updating() override;

    PREFERENCES_API virtual void configuring() override;

    /// preference file
    std::filesystem::path m_prefFile;

private:

    /// loads preferences
    void load();
    /// saves preferences
    void save();
};

} // namespace preferences
