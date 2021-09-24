/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "ui/base/preferences/helper.hpp"

#include <core/crypto/SHA256.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/tools/Os.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <iomanip>
#include <shared_mutex>

namespace sight::ui::base::preferences
{

const std::string s_PREFERENCES_KEY = "preferences";

//----------------------------------------------------------------------------

bool setPreference(const std::string& key, const std::string& value)
{
    bool isModified = false;

    // Check preferences
    data::Composite::sptr prefs = getPreferences();
    if(prefs)
    {
        data::Composite::IteratorType iterPref = prefs->find(key);
        if(iterPref != prefs->end())
        {
            data::String::sptr preferences = data::String::dynamicCast(iterPref->second);
            preferences->value() = value;
        }
        else
        {
            (*prefs)[key] = data::String::New(std::string(value));
        }

        isModified = true;
    }

    return isModified;
}

//----------------------------------------------------------------------------

std::string getPreference(const std::string& preferenceKey)
{
    std::string value;

    // Check preferences
    data::Composite::sptr prefs = getPreferences();
    if(prefs)
    {
        data::Composite::IteratorType iterPref = prefs->find(preferenceKey);
        if(iterPref != prefs->end())
        {
            data::String::sptr prefString = data::String::dynamicCast(iterPref->second);
            value = prefString->value();
        }
    }

    return value;
}

//-----------------------------------------------------------------------------

std::filesystem::path getPreferencesFile()
{
    core::runtime::Profile::sptr profile = core::runtime::getCurrentProfile();
    SIGHT_THROW_IF("No current profile set.", !profile);

    const std::string appName = profile->getName();
    SIGHT_THROW_IF("Unable to determine application name", appName.empty());
    const std::filesystem::path appPrefDir  = core::tools::os::getUserDataDir("sight", appName, true);
    const std::filesystem::path appPrefFile = appPrefDir / "preferences.json";

    SIGHT_THROW_IF("Unable to define user data directory", appPrefDir.empty());

    if(!std::filesystem::exists(appPrefDir))
    {
        std::filesystem::create_directories(appPrefDir);
    }

    SIGHT_THROW_IF(
        "Preferences file '" + appPrefFile.string() + "' already exists and is not a regular file.",
        std::filesystem::exists(appPrefFile) && !std::filesystem::is_regular_file(appPrefFile)
    );

    return appPrefFile;
}

//-----------------------------------------------------------------------------

// returns the preferences service (or nullptr if is does not exist). This method is not exposed.
ui::base::preferences::IPreferences::sptr getPreferencesSrv()
{
    ui::base::preferences::IPreferences::sptr srv;
    const auto preferencesServicesList = service::OSR::getServices("sight::ui::base::preferences::IPreferences");

    if(!preferencesServicesList.empty())
    {
        service::IService::sptr prefService = *preferencesServicesList.begin();
        srv = ui::base::preferences::IPreferences::dynamicCast(prefService);
    }

    SIGHT_DEBUG_IF("The preferences service is not found, the preferences can not be used", !srv);

    return srv;
}

//-----------------------------------------------------------------------------

data::Composite::sptr getPreferences()
{
    data::Composite::sptr prefs;

    const auto prefService = getPreferencesSrv();

    if(prefService)
    {
        // FIXME: This is wrong but normally harmless in most use cases
        // This should disappear anyway when we stop using a composite for preferences
        // See https://git.ircad.fr/sight/sight/-/issues/53
        prefs = prefService->getInOut<data::Composite>(s_PREFERENCES_KEY).lock().get_shared();
    }

    SIGHT_DEBUG_IF("The preferences are not found", !prefs);

    return prefs;
}

//-----------------------------------------------------------------------------

void savePreferences()
{
    const auto prefService = getPreferencesSrv();
    SIGHT_WARN_IF("The preferences service is not found, the preferences can not be saved", !prefService);
    SIGHT_WARN_IF(
        "The preferences service is not started, the preferences can not be saved",
        prefService && !prefService->isStarted()
    );
    if(prefService && prefService->isStarted())
    {
        prefService->update();
    }
}

//-----------------------------------------------------------------------------

std::string getValue(const std::string& var, const char delimiter)
{
    std::string value(var);
    const size_t first = var.find(delimiter);
    const size_t last  = var.rfind(delimiter);
    if(first == 0 && last == var.size() - 1)
    {
        value = ui::base::preferences::getPreference(var.substr(1, var.size() - 2));
    }

    return value;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::preferences
