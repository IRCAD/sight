/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPreferences/helper.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/Os.hpp>

namespace fwPreferences
{

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
    const bfile::path appPrefDir  = ::fwTools::os::getUserDataDir("fw4spl", appName, true);
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

} // namespace fwPreferences
