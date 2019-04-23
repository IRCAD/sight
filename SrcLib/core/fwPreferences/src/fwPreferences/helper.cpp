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

#include "fwPreferences/helper.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/Os.hpp>

namespace fwPreferences
{

const std::string s_PREFERENCES_KEY = "preferences";

//----------------------------------------------------------------------------

bool setPreference(const std::string& key, const std::string& value)
{
    bool isModified = false;
    // Check preferences

    ::fwData::Composite::sptr prefs = getPreferences();
    if(prefs)
    {
        ::fwData::Composite::IteratorType iterPref = prefs->find(key);
        if ( iterPref != prefs->end() )
        {
            ::fwData::String::sptr preferences = ::fwData::String::dynamicCast(iterPref->second);
            preferences->value()               = value;
        }
        else
        {
            (*prefs)[key] = ::fwData::String::New(value);
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
    ::fwData::Composite::sptr prefs = getPreferences();
    if(prefs)
    {
        ::fwData::Composite::IteratorType iterPref = prefs->find( preferenceKey );
        if ( iterPref != prefs->end() )
        {
            ::fwData::String::sptr prefString = ::fwData::String::dynamicCast(iterPref->second);
            value                             = prefString->value();
        }
    }
    return value;
}

//-----------------------------------------------------------------------------

::boost::filesystem::path getPreferencesFile()
{
    namespace bfile = ::boost::filesystem;

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    FW_RAISE_IF("No current profile set.", !profile);

    const std::string appName     = profile->getName();
    const bfile::path appPrefDir  = ::fwTools::os::getUserDataDir("sight", appName, true);
    const bfile::path appPrefFile = appPrefDir / "preferences.json";

    FW_RAISE_IF("Unable to define user data directory", appPrefDir.empty());

    if (!bfile::exists(appPrefDir))
    {
        bfile::create_directories(appPrefDir);
    }

    FW_RAISE_IF("Preferences file '"+appPrefFile.string()+"' already exists and is not a regular file.",
                bfile::exists(appPrefFile) && !bfile::is_regular_file(appPrefFile));

    return appPrefFile;
}

//-----------------------------------------------------------------------------

// returns the preferences service (or nullptr if is does not exist). This method is not exposed.
::fwPreferences::IPreferences::sptr getPreferencesSrv()
{
    ::fwPreferences::IPreferences::sptr srv;
    const auto preferencesServicesList = ::fwServices::OSR::getServices("::fwPreferences::IPreferences");

    if(!preferencesServicesList.empty())
    {
        ::fwServices::IService::sptr prefService = *preferencesServicesList.begin();
        srv                                      = ::fwPreferences::IPreferences::dynamicCast(prefService);
    }
    SLM_DEBUG_IF("The preferences service is not found, the preferences can not be used", !srv);

    return srv;
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr getPreferences()
{
    ::fwData::Composite::sptr prefs;

    const auto prefService = getPreferencesSrv();

    if(prefService)
    {
        prefs = prefService->getInOut< ::fwData::Composite >(s_PREFERENCES_KEY);
    }
    SLM_DEBUG_IF("The preferences are not found", !prefs);

    return prefs;
}

//-----------------------------------------------------------------------------

void savePreferences()
{
    const auto prefService = getPreferencesSrv();
    SLM_WARN_IF("The preferences service is not found, the preferences can not be saved", !prefService);
    SLM_WARN_IF("The preferences service is not started, the preferences can not be saved", !prefService->isStarted());
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
    if (first == 0 && last == var.size() - 1)
    {
        value = ::fwPreferences::getPreference( var.substr(1, var.size() - 2) );
    }
    return value;
}

//-----------------------------------------------------------------------------

} // namespace fwPreferences
