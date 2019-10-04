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
        std::filesystem::path prefFile = ::fwPreferences::getPreferencesFile();
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
