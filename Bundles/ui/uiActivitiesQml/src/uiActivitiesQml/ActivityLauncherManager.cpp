/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "uiActivitiesQml/ActivityLauncherManager.hpp"

#include <fwMedData/SeriesDB.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <fwServices/registry/Proxy.hpp>

static const ::fwServices::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

static const std::string s_ACTIVITY_CREATED_CHANNEL = "activityCreatedChannel";
static const std::string s_GO_TO_CHANNEL            = "GoToChannel";
static const std::string s_VALIDATION_CHANNEL       = "validationChannel";

//------------------------------------------------------------------------------

ActivityLauncherManager::ActivityLauncherManager() noexcept
{

}

//------------------------------------------------------------------------------

ActivityLauncherManager::~ActivityLauncherManager() noexcept
{
    this->uninitialize();
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::initialize()
{
    this->create();

    m_seriesDB = ::fwData::factory::New< ::fwMedData::SeriesDB >();

    this->addObject(m_seriesDB, this->getInputID(s_SERIESDB_INOUT));

    auto addActivityViewParam = [&](const std::string& replace)
                                {
                                    ::fwServices::IService::ConfigType parameterViewConfig;
                                    parameterViewConfig.add("<xmlattr>.replace", replace);
                                    parameterViewConfig.add("<xmlattr>.by", this->getInputID(replace));
                                    m_activityViewConfig.add_child("parameters.parameter", parameterViewConfig);
                                };
    addActivityViewParam(s_SERIESDB_INOUT);
    addActivityViewParam(s_VALIDATION_CHANNEL);

    this->startServices();
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::uninitialize()
{
    // stop the started services and unregister all the services
    this->destroy();
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::onServiceCreated(const QVariant& obj)
{
    ::fwQml::IQmlEditor::sptr srv(obj.value< ::fwQml::IQmlEditor* >());
    if (srv)
    {
        if (srv->isA("::uiActivitiesQml::SActivityView"))
        {
            srv->configure(m_activityViewConfig);
            // connect to launch the activity when it is created/updated.
            ::fwServices::helper::ProxyConnections activityCreatedCnt(this->getInputID(s_ACTIVITY_CREATED_CHANNEL));
            activityCreatedCnt.addSlotConnection(srv->getID(), "launchActivity");

            this->addProxyConnection(activityCreatedCnt);

            this->addService(srv, true);
        }
        else if (srv->isA("::uiActivitiesQml::SActivitySequencer"))
        {
            // create the services;
            m_activitySequencer = srv;
            m_activitySequencer->registerInOut(m_seriesDB, "seriesDB", true);

            // connect to launch the activity when it is created/updated.
            ::fwServices::helper::ProxyConnections activityCreatedCnt(this->getInputID(s_ACTIVITY_CREATED_CHANNEL));
            activityCreatedCnt.addSignalConnection(m_activitySequencer->getID(), "activityCreated");

            // When the activity is launched, the sequencer sends the information to enable "previous" and "next"
            // actions
            ::fwServices::helper::ProxyConnections validationCnt(this->getInputID(s_VALIDATION_CHANNEL));
            validationCnt.addSlotConnection(m_activitySequencer->getID(), "checkNext");

            // The activity sequencer should receive the call from the "goTo" action.
            ::fwServices::helper::ProxyConnections activityGoToCnt(this->getInputID(s_GO_TO_CHANNEL));
            activityGoToCnt.addSlotConnection(m_activitySequencer->getID(), "goTo");

            this->addProxyConnection(activityCreatedCnt);
            this->addProxyConnection(validationCnt);
            this->addProxyConnection(activityGoToCnt);
            this->addService(srv, true, true);
        }
    }
}

//------------------------------------------------------------------------------
