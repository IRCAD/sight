/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "preferences/Plugin.hpp"

#include "preferences/SPreferences.hpp"

#include <fwData/String.hpp>

#include <fwPreferences/helper.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/Os.hpp>

namespace preferences
{

const std::string Plugin::s_PREF_SERVICE_UID = "PREF_SERVICE_UID";

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("preferences::Plugin");

//-----------------------------------------------------------------------------

void Plugin::start()
{
    m_preferences = ::fwData::Composite::New();

    ::fwServices::IService::sptr prefSrv;
    prefSrv = ::fwServices::add("::preferences::SPreferences", s_PREF_SERVICE_UID);
    prefSrv->registerInOut(m_preferences, fwPreferences::s_PREFERENCES_KEY);

    try
    {
        ::fwRuntime::EConfigurationElement::sptr prefConfig = ::fwRuntime::EConfigurationElement::New( "filename" );
        ::boost::filesystem::path prefFile                  = ::fwPreferences::getPreferencesFile();
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

void Plugin::stop() noexcept
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

} // namespace preferences
