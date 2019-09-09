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

#include "ExActivitiesQml/AppManager.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>

#include <fwData/factory/new.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <QDebug>

const ::fwServices::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

//------------------------------------------------------------------------------

AppManager::AppManager() noexcept
{
    s_SERIES_DB                        = this->getID(s_SERIES_DB);
    s_ACTIVITY_CREATED_CHANNEL         = this->getID(s_ACTIVITY_CREATED_CHANNEL);
    s_ACTIVITY_LAUNCHED_CHANNEL        = this->getID(s_ACTIVITY_LAUNCHED_CHANNEL);
    s_ENABLED_PREVIOUS_CHANNEL_CHANNEL = this->getID(s_ENABLED_PREVIOUS_CHANNEL_CHANNEL);
    s_ENABLED_NEXT_CHANNEL_CHANNEL     = this->getID(s_ENABLED_NEXT_CHANNEL_CHANNEL);
    s_PREVIOUS_CHANNEL                 = this->getID(s_PREVIOUS_CHANNEL);
    s_NEXT_CHANNEL                     = this->getID(s_NEXT_CHANNEL);
}

//------------------------------------------------------------------------------

AppManager::~AppManager() noexcept
{
}

//------------------------------------------------------------------------------

void AppManager::initialize()
{
    this->create();

    m_seriesDB = ::fwData::factory::New< ::fwMedData::SeriesDB >();

    this->addObject(m_seriesDB, s_SERIESDB_INOUT);

    // create the services
    m_activitySequencer = this->addService("::activities::SActivitySequencer", true);

    m_activitySequencer->registerInOut(m_seriesDB, "seriesDB", true);
    ::fwServices::IService::ConfigType sequencerConfig;
    sequencerConfig.add("activity", "ExImageReading");
    sequencerConfig.add("activity", "ExImageDisplaying");
    m_activitySequencer->configure(sequencerConfig);

    auto addParam = [&](const std::string& replace, const std::string& by)
                    {
                        ::fwServices::IService::ConfigType parameterViewConfig;
                        parameterViewConfig.add("<xmlattr>.replace", replace);
                        parameterViewConfig.add("<xmlattr>.by", by);
                        m_activityViewConfig.add_child("parameters.parameter", parameterViewConfig);
                    };
    addParam(s_ENABLED_PREVIOUS_CHANNEL_CHANNEL, s_ENABLED_PREVIOUS_CHANNEL_CHANNEL);
    addParam(s_ENABLED_NEXT_CHANNEL_CHANNEL, s_ENABLED_NEXT_CHANNEL_CHANNEL);
    addParam(s_PREVIOUS_CHANNEL, s_PREVIOUS_CHANNEL);
    addParam(s_NEXT_CHANNEL, s_NEXT_CHANNEL);

    // connect to launch the activity when it is created/updated.
    ::fwServices::helper::ProxyConnections activityCreatedCnt(s_ACTIVITY_CREATED_CHANNEL);
    activityCreatedCnt.addSignalConnection(m_activitySequencer->getID(), "activityCreated");

    // When the activity is launched, the sequencer sends the information to enable "previous" and "next" actions
    ::fwServices::helper::ProxyConnections activityLaunchedCnt(s_ACTIVITY_LAUNCHED_CHANNEL);
    activityLaunchedCnt.addSlotConnection(m_activitySequencer->getID(), "sendInfo");

    // The activity sequencer should receive the call from the "previous" action.
    ::fwServices::helper::ProxyConnections activityPreviousCnt(s_PREVIOUS_CHANNEL);
    activityPreviousCnt.addSlotConnection(m_activitySequencer->getID(), "previous");

    // The activity sequencer should receive the call from the "next" action.
    ::fwServices::helper::ProxyConnections activityNextCnt(s_NEXT_CHANNEL);
    activityNextCnt.addSlotConnection(m_activitySequencer->getID(), "next");

    this->addProxyConnection(activityCreatedCnt);
    this->addProxyConnection(activityLaunchedCnt);
    this->addProxyConnection(activityPreviousCnt);
    this->addProxyConnection(activityNextCnt);

    this->startServices();
    m_activitySequencer->slot("next")->asyncRun();
}

//------------------------------------------------------------------------------

void AppManager::uninitialize()
{
    // stop the started services and unregister all the services
    this->stopAndUnregisterServices();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void AppManager::onServiceCreated(const QVariant& obj)
{
    ::fwQml::IQmlEditor::sptr srv(obj.value< ::fwQml::IQmlEditor* >());
    if (srv)
    {
        if (srv->isA("::guiQml::editor::SActivityView"))
        {
            srv->configure(m_activityViewConfig);
            // connect to launch the activity when it is created/updated.
            ::fwServices::helper::ProxyConnections activityCreatedCnt(s_ACTIVITY_CREATED_CHANNEL);
            activityCreatedCnt.addSlotConnection(srv->getID(), "launchActivity");

            // When the activity is launched, the sequencer sends the information to enable "previous" and "next"
            // actions
            ::fwServices::helper::ProxyConnections activityLaunchedCnt(s_ACTIVITY_LAUNCHED_CHANNEL);
            activityLaunchedCnt.addSignalConnection(srv->getID(), "activityLaunched");

            this->addProxyConnection(activityCreatedCnt);
            this->addProxyConnection(activityLaunchedCnt);

            this->addService(srv, true);
        }
//        else if (srv->isA("::uiMedDataQml::SModelSeriesList"))
//        {
//            srv->setObjectId("modelSeries", s_MODELSERIES_ID);
//            this->addService(srv, true);

//            ::fwServices::helper::ProxyConnections recSelectedCnt(s_REC_SELECTED_CHANNEL);
//            recSelectedCnt.addSignalConnection(srv->getID(),  "reconstructionSelected");
//            this->addProxyConnection(recSelectedCnt);

//            ::fwServices::helper::ProxyConnections emptySelectionCnt(s_EMPTY_SELECTION_CHANNEL);
//            emptySelectionCnt.addSignalConnection(srv->getID(),  "emptiedSelection");
//            this->addProxyConnection(emptySelectionCnt);
//        }
//        else if (srv->isA("::uiReconstructionQml::SOrganMaterialEditor") ||
//                 srv->isA("::uiReconstructionQml::SRepresentationEditor"))
//        {
//            srv->setObjectId("reconstruction", s_RECONSTRUCTION_ID);
//            this->addService(srv, true);
//        }
    }
}

//------------------------------------------------------------------------------
