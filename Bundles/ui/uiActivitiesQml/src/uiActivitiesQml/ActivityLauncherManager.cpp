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

#include <fwCom/Slot.hxx>

#include <fwMedData/SeriesDB.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <fwServices/registry/Proxy.hpp>

static const ::fwServices::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

static const std::string s_ACTIVITY_CREATED_CHANNEL         = "activityCreated";
static const std::string s_ACTIVITY_LAUNCHED_CHANNEL        = "activityLaunched";
static const std::string s_ENABLED_PREVIOUS_CHANNEL_CHANNEL = "enabledPreviousChannel";
static const std::string s_ENABLED_NEXT_CHANNEL_CHANNEL     = "enabledNextChannel";
static const std::string s_PREVIOUS_CHANNEL                 = "PreviousChannel";
static const std::string s_NEXT_CHANNEL                     = "NextChannel";

static const std::string s_PREVIOUS_SIG = "previous";
static const std::string s_NEXT_SIG     = "next";

//------------------------------------------------------------------------------

ActivityLauncherManager::ActivityLauncherManager() noexcept
{
    newSignal< VoidSignalType >(s_PREVIOUS_SIG);
    newSignal< VoidSignalType >(s_NEXT_SIG);
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

    // create the services
    m_activitySequencer = this->addService("::activities::SActivitySequencer", true);
    m_activitySequencer->registerInOut(m_seriesDB, "seriesDB", true);
    m_activitySequencer->configure(m_sequencerConfig);

    auto addActivityViewParam = [&](const std::string& replace)
                                {
                                    ::fwServices::IService::ConfigType parameterViewConfig;
                                    parameterViewConfig.add("<xmlattr>.replace", replace);
                                    parameterViewConfig.add("<xmlattr>.by", this->getInputID(replace));
                                    m_activityViewConfig.add_child("parameters.parameter", parameterViewConfig);
                                };
    addActivityViewParam(s_SERIESDB_INOUT);

    // connect to launch the activity when it is created/updated.
    ::fwServices::helper::ProxyConnections activityCreatedCnt(this->getInputID(s_ACTIVITY_CREATED_CHANNEL));
    activityCreatedCnt.addSignalConnection(m_activitySequencer->getID(), "activityCreated");

    // When the activity is launched, the sequencer sends the information to enable "previous" and "next" actions
    ::fwServices::helper::ProxyConnections activityLaunchedCnt(this->getInputID(s_ACTIVITY_LAUNCHED_CHANNEL));
    activityLaunchedCnt.addSlotConnection(m_activitySequencer->getID(), "sendInfo");

    // The activity sequencer should receive the call from the "previous" action.
    ::fwServices::helper::ProxyConnections activityPreviousCnt(this->getInputID(s_PREVIOUS_CHANNEL));
    activityPreviousCnt.addSlotConnection(m_activitySequencer->getID(), "previous");

    // The activity sequencer should receive the call from the "next" action.
    ::fwServices::helper::ProxyConnections activityNextCnt(this->getInputID(s_NEXT_CHANNEL));
    activityNextCnt.addSlotConnection(m_activitySequencer->getID(), "next");

    this->addProxyConnection(activityCreatedCnt);
    this->addProxyConnection(activityLaunchedCnt);
    this->addProxyConnection(activityPreviousCnt);
    this->addProxyConnection(activityNextCnt);

    auto proxyReg = ::fwServices::registry::Proxy::getDefault();

    proxyReg->connect(this->getInputID(s_PREVIOUS_CHANNEL), this->signal(s_PREVIOUS_SIG));
    proxyReg->connect(this->getInputID(s_NEXT_CHANNEL), this->signal(s_NEXT_SIG));

    this->startServices();
    m_activitySequencer->slot("next")->asyncRun();
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::uninitialize()
{
    // stop the started services and unregister all the services
    this->destroy();

    auto proxyReg = ::fwServices::registry::Proxy::getDefault();
    proxyReg->disconnect(this->getInputID(s_PREVIOUS_CHANNEL), this->signal(s_PREVIOUS_SIG));
    proxyReg->disconnect(this->getInputID(s_NEXT_CHANNEL), this->signal(s_NEXT_SIG));
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

            // When the activity is launched, the sequencer sends the information to enable "previous" and "next"
            // actions
            ::fwServices::helper::ProxyConnections activityLaunchedCnt(this->getInputID(s_ACTIVITY_LAUNCHED_CHANNEL));
            activityLaunchedCnt.addSignalConnection(srv->getID(), "activityLaunched");

            this->addProxyConnection(activityCreatedCnt);
            this->addProxyConnection(activityLaunchedCnt);

            this->addService(srv, true);
        }
    }
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::setActivities(const QList<QString>& list)
{
    for(const auto& elt: list)
    {
        m_sequencerConfig.add("activity", elt.toStdString());

    }
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::previous()
{
    this->signal<VoidSignalType>(s_PREVIOUS_SIG)->asyncEmit();
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::next()
{
    this->signal<VoidSignalType>(s_NEXT_SIG)->asyncEmit();
}

//------------------------------------------------------------------------------
