/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Os.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include "preferences/Plugin.hpp"

namespace preferences
{

const std::string Plugin::s_PREF_SERVICE_UID = "PREF_SERVICE_UID";

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("preferences::Plugin");

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    m_preferences = ::fwData::Composite::New();

    ::fwServices::IService::sptr prefSrv;
    prefSrv = ::fwServices::add (m_preferences,
                                 "::preferences::IPreferencesService", "::preferences::SPreferences",
                                 s_PREF_SERVICE_UID);

    try
    {
        ::fwRuntime::EConfigurationElement::sptr prefConfig = ::fwRuntime::EConfigurationElement::New( "filename" );
        ::boost::filesystem::path prefFile                  = this->getPreferencesFile();
        prefConfig->setValue(prefFile.string());

        prefSrv->setConfiguration(prefConfig);
        prefSrv->configure();
        prefSrv->start();

        if ( m_preferences->find( "PREFERENCES_VERSION" ) == m_preferences->end() )
        {
            ::fwData::String::sptr version            = ::fwData::String::New( PREFERENCES_VER );
            (*m_preferences)[ "PREFERENCES_VERSION" ] = version;
        }
    }
    catch(::fwCore::Exception& e)
    {
        OSLM_ERROR("Preferences service not started. " << e.what());
    }
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
    if(::fwTools::fwID::exist(s_PREF_SERVICE_UID))
    {
        ::fwServices::IService::sptr prefSrv = ::fwServices::get(s_PREF_SERVICE_UID);
        prefSrv->stop();
        ::fwServices::OSR::unregisterService(prefSrv);
    }
    m_preferences.reset();
}

//-----------------------------------------------------------------------------

::boost::filesystem::path Plugin::getPreferencesFile() const
{
    namespace bfile = ::boost::filesystem;

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    FW_RAISE_IF("No current profile set.", !profile);

    const std::string appName     = profile->getName();
    const bfile::path appPrefDir  = ::fwTools::os::getUserDataDir("fw4spl", appName, true);
    const bfile::path appPrefFile = appPrefDir / "preferences.json";

    FW_RAISE_IF("Unable to define User's data directory", appPrefDir.empty());

    if (!bfile::exists(appPrefDir))
    {
        bfile::create_directories(appPrefDir);
    }

    FW_RAISE_IF("Preferences file '"+appPrefFile.string()+"' already exists and is not a regular file.",
                bfile::exists(appPrefFile) && !bfile::is_regular_file(appPrefFile));

    return appPrefFile;
}

//-----------------------------------------------------------------------------

} // namespace preferences
