/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "modules/ui/base/Plugin.hpp"

#include <core/runtime/EConfigurationElement.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <core/tools/Os.hpp>

#include <data/String.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>

#include <ui/base/preferences/helper.hpp>

namespace sight::module::ui::base
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::sight::module::ui::base::Plugin");

const std::string Plugin::s_PREF_SERVICE_UID = "PREF_SERVICE_UID";

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    m_preferences = data::Composite::New();

    service::IService::sptr prefSrv;
    prefSrv = service::add("::sight::module::ui::base::SPreferences", s_PREF_SERVICE_UID);
    prefSrv->registerInOut(m_preferences, sight::ui::base::preferences::s_PREFERENCES_KEY);

    try
    {
        core::runtime::EConfigurationElement::sptr prefConfig = core::runtime::EConfigurationElement::New( "filename" );
        std::filesystem::path prefFile                        = sight::ui::base::preferences::getPreferencesFile();
        prefConfig->setValue(prefFile.string());

        prefSrv->setConfiguration(prefConfig);
        prefSrv->configure();
        prefSrv->start();

        if ( m_preferences->find( "PREFERENCES_VERSION" ) == m_preferences->end() )
        {
            data::String::sptr version = data::String::New( "0.1" );
            (*m_preferences)[ "PREFERENCES_VERSION" ] = version;
        }
    }
    catch(core::Exception& e)
    {
        SLM_ERROR("Preferences service not started. " << e.what());
    }
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    if(core::tools::fwID::exist(s_PREF_SERVICE_UID))
    {
        service::IService::sptr prefSrv = service::get(s_PREF_SERVICE_UID);
        prefSrv->stop();
        service::OSR::unregisterService(prefSrv);
    }
    m_preferences.reset();
}

} // namespace sight::module::ui::base
