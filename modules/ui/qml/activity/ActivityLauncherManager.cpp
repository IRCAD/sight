/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "ActivityLauncherManager.hpp"

#include <core/com/Signal.hxx>

#include <data/SeriesDB.hpp>

#include <service/base.hpp>
#include <service/extension/Config.hpp>
#include <service/registry/Proxy.hpp>

#include <ui/qml/IQmlEditor.hpp>

namespace sight::module::ui::qml::activity
{

static const service::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

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

    m_seriesDB = data::factory::New<data::SeriesDB>();

    this->addObject(m_seriesDB, this->getInputID(s_SERIESDB_INOUT));

    auto addActivityViewParam = [&](const std::string& replace)
                                {
                                    service::IService::ConfigType parameterViewConfig;
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
    sight::ui::qml::IQmlEditor::sptr srv(obj.value<sight::ui::qml::IQmlEditor*>());
    if(srv)
    {
        if(srv->isA("sight::module::ui::qml::activity::SView"))
        {
            srv->configure(m_activityViewConfig);
            // connect to launch the activity when it is created/updated.
            service::helper::ProxyConnections activityCreatedCnt(this->getInputID(s_ACTIVITY_CREATED_CHANNEL));
            activityCreatedCnt.addSlotConnection(srv->getID(), "launchActivity");

            this->addProxyConnection(activityCreatedCnt);

            this->addService(srv, true);
        }
        else if(srv->isA("sight::module::ui::qml::activity::SSequencer"))
        {
            // create the services;
            m_activitySequencer = srv;
            m_activitySequencer->setInOut(m_seriesDB, "seriesDB", true);

            // connect to launch the activity when it is created/updated.
            service::helper::ProxyConnections activityCreatedCnt(this->getInputID(s_ACTIVITY_CREATED_CHANNEL));
            activityCreatedCnt.addSignalConnection(m_activitySequencer->getID(), "activityCreated");

            // When the activity is launched, the sequencer sends the information to enable "previous" and "next"
            // actions
            service::helper::ProxyConnections validationCnt(this->getInputID(s_VALIDATION_CHANNEL));
            validationCnt.addSlotConnection(m_activitySequencer->getID(), "checkNext");

            // The activity sequencer should receive the call from the "goTo" action.
            service::helper::ProxyConnections activityGoToCnt(this->getInputID(s_GO_TO_CHANNEL));
            activityGoToCnt.addSlotConnection(m_activitySequencer->getID(), "goTo");

            this->addProxyConnection(activityCreatedCnt);
            this->addProxyConnection(validationCnt);
            this->addProxyConnection(activityGoToCnt);
            this->addService(srv, true, true);
        }
    }
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::open()
{
    const auto& seriesDB = data::SeriesDB::dynamicCast(this->getObject(this->getInputID(s_SERIESDB_INOUT)));
    auto reader          = service::add("::sight::module::ui::base::io::SSelector");
    reader->setInOut(seriesDB, "data");
    const auto srvCfgFactory = service::extension::Config::getDefault();
    const auto cfgElem       = srvCfgFactory->getServiceConfig(
        "ActivityReaderConfig",
        "::sight::module::ui::base::io::SSelector"
    );
    reader->setConfiguration(core::runtime::ConfigurationElement::constCast(cfgElem));
    reader->configure();

    reader->start();
    reader->update();
    reader->stop();
    service::remove(reader);

    auto sig = seriesDB->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void ActivityLauncherManager::save()
{
    const auto& seriesDB = data::SeriesDB::dynamicCast(this->getObject(this->getInputID(s_SERIESDB_INOUT)));
    auto writer          = service::add("::sight::module::ui::base::io::SSelector");
    writer->setInOut(seriesDB, "data");
    const auto srvCfgFactory = service::extension::Config::getDefault();
    const auto cfgElem       = srvCfgFactory->getServiceConfig(
        "ActivityWriterConfig",
        "::sight::module::ui::base::io::SSelector"
    );
    writer->setConfiguration(core::runtime::ConfigurationElement::constCast(cfgElem));
    writer->configure();
    writer->start();
    writer->update();
    writer->stop();
    service::remove(writer);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::activity
