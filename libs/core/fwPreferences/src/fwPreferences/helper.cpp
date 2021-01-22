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

#include "fwPreferences/helper.hpp"

#include <core/tools/Os.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <openssl/rand.h>
#include <openssl/sha.h>

#include <iomanip>
#include <shared_mutex>

namespace fwPreferences
{

const std::string s_PREFERENCES_KEY = "preferences";

// Password management static variables
// @todo: The std::string can be replaced with a map to manage multiple user and multiple password (later)
static std::shared_mutex s_passwordMutex;
static std::string s_password;

static const std::string s_PASSWORD_HASH_KEY = "~~Private~~";
static unsigned char s_scramble_key[SHA256_DIGEST_LENGTH] {0};

//----------------------------------------------------------------------------

// Compute a sha256 paswword hash using openSSL
inline static std::string computePasswordHash( const std::string& password )
{
    // Compute SHA256 using openssl
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    // Convert the hash to an hexadecimal string
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2) << std::hex;

    for(std::uint8_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        // Cast to int to avoid ASCII code interpretation.
        stream << static_cast<int>(hash[i]);
    }

    return stream.str();
}

//----------------------------------------------------------------------------

// Quick and simple xor encryption
inline static std::string scramble( const std::string& original )
{
    std::string scrambled;
    scrambled.resize(original.size());

    for (size_t i = 0; i < original.size(); i++ )
    {
        scrambled[i] = original[i] ^ s_scramble_key[i%sizeof(s_scramble_key)];
    }

    return scrambled;
}

//----------------------------------------------------------------------------

void setPassword(const std::string& password)
{
    // Protect for writing
    std::unique_lock writeLock(s_passwordMutex);

    if(password.empty())
    {
        // Remove the password
        s_password = password;

        // Remove the password hash
        ::fwData::Composite::sptr prefs = getPreferences();
        if(prefs && prefs->find(s_PASSWORD_HASH_KEY) != prefs->end() )
        {
            setPreference(s_PASSWORD_HASH_KEY, password);
            savePreferences();
        }
    }
    else
    {
        // Save the password hash to preferences
        setPreference(s_PASSWORD_HASH_KEY, computePasswordHash(password));
        savePreferences();

        // Scramble the scramble key
        RAND_bytes(s_scramble_key, sizeof(s_scramble_key));

        // Scramble the password
        s_password = scramble(password);
    }
}

//----------------------------------------------------------------------------

const std::string getPassword()
{
    // Protect for reading
    std::shared_lock readLock(s_passwordMutex);

    // Return it
    return scramble(s_password);
}

//----------------------------------------------------------------------------

bool checkPassword(const std::string& password)
{
    // Protect for writing
    std::unique_lock writeLock(s_passwordMutex);

    const std::string& passwordHash = getPreference(s_PASSWORD_HASH_KEY);

    if(passwordHash.empty())
    {
        // No password hash is stored in the preferences or there is no preferences
        // We must check against s_password
        return password == scramble(s_password);
    }
    else if(computePasswordHash(password) == passwordHash)
    {
        // Store the verified password
        // Scramble the scramble key
        RAND_bytes(s_scramble_key, sizeof(s_scramble_key));

        // Scramble the password
        s_password = scramble(password);

        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------

bool hasPasswordHash()
{
    return !getPreference(s_PASSWORD_HASH_KEY).empty();
}

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

std::filesystem::path getPreferencesFile()
{
    namespace bfile = std::filesystem;

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    FW_RAISE_IF("No current profile set.", !profile);

    const std::string appName     = profile->getName();
    const bfile::path appPrefDir  = core::tools::os::getUserDataDir("sight", appName, true);
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
