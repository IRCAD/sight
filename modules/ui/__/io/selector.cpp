/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "selector.hpp"

#include <app/extension/config.hpp>

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/extension/factory.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <sstream>
#include <string>

namespace sight::module::ui::io
{

namespace io = sight::io;

//------------------------------------------------------------------------------

static const core::com::signals::key_t JOB_CREATED_SIGNAL   = "jobCreated";
static const core::com::signals::key_t JOB_FAILED_SIGNAL    = "jobFailed";
static const core::com::signals::key_t JOB_SUCCEEDED_SIGNAL = "jobSucceeded";

static const core::com::slots::key_t FORWARD_JOB_SLOT = "forwardJob";

//------------------------------------------------------------------------------

selector::selector() :
    m_sigJobCreated(new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL)),
    m_sigJobFailed(new_signal<JobFailedSignalType>(JOB_FAILED_SIGNAL)),
    m_sigJobSucceeded(new_signal<JobSucceededSignalType>(JOB_SUCCEEDED_SIGNAL)),
    m_slotForwardJob(new_slot(FORWARD_JOB_SLOT, &selector::forwardJob, this))
{
}

//------------------------------------------------------------------------------

selector::~selector() noexcept =
    default;

//------------------------------------------------------------------------------

void selector::configuring()
{
    const config_t srvConfig = this->get_config();

    const std::string mode = srvConfig.get<std::string>("type.<xmlattr>.mode", "reader");
    SIGHT_ASSERT(
        "The xml attribute <mode> must be 'reader' (to open file) or 'writer' (to write a new file).",
        mode == "writer" || mode == "reader"
    );
    m_mode = (mode == "writer") ? WRITER_MODE : READER_MODE;
    SIGHT_DEBUG("mode => " + mode);

    const std::string selectionMode = srvConfig.get<std::string>("selection.<xmlattr>.mode", "exclude");
    SIGHT_ASSERT(
        "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
        "'exclude' (to exclude the selection of the selector list).",
        selectionMode == "exclude" || selectionMode == "include"
    );
    m_servicesAreExcluded = (selectionMode == "exclude");
    SIGHT_DEBUG("selection mode => " + selectionMode);

    for(const auto& itSelection : boost::make_iterator_range(srvConfig.equal_range("addSelection")))
    {
        const auto service = itSelection.second.get<std::string>("<xmlattr>.service");
        m_selectedServices.push_back(service);
        SIGHT_DEBUG("add selection => " + service);

        const std::string configId = itSelection.second.get<std::string>("<xmlattr>.config", "");
        if(!configId.empty())
        {
            m_serviceToConfig[service] = configId;
            SIGHT_DEBUG(std::string("add config '") + configId + "' for service '" + service + "'");
        }
    }

    for(const auto& itCfg : boost::make_iterator_range(srvConfig.equal_range("config")))
    {
        const auto service  = itCfg.second.get<std::string>("<xmlattr>.service");
        const auto configId = itCfg.second.get<std::string>("<xmlattr>.id");

        m_serviceToConfig[service] = configId;
        SIGHT_DEBUG(std::string("add config '") + configId + "' for service '" + service + "'");
    }
}

//------------------------------------------------------------------------------

void selector::starting()
{
}

//------------------------------------------------------------------------------

void selector::stopping()
{
}

//------------------------------------------------------------------------------

void selector::updating()
{
    std::vector<std::string> availableExtensionsId;
    {
        auto objLock           = m_data.lock();
        data::object::sptr obj = objLock.get_shared();

        // Retrieve implementation of type io::service::reader for this object
        if(m_mode == READER_MODE)
        {
            SIGHT_ASSERT("An inout key '" + io::service::s_DATA_KEY + "' must be defined.", obj);
            const auto classname = obj->get_classname();

            availableExtensionsId =
                service::extension::factory::get()->getImplementationIdFromObjectAndType(
                    classname,
                    "sight::io::service::reader"
                );
        }
        else // m_mode == WRITER_MODE
        {
            SIGHT_ASSERT("The inout key '" + io::service::s_DATA_KEY + "' is not correctly set.", obj);

            availableExtensionsId =
                service::extension::factory::get()->getImplementationIdFromObjectAndType(
                    obj->get_classname(),
                    "sight::io::service::writer"
                );
        }
    }

    // Filter available extensions and replace id by service description
    std::vector<std::pair<std::string, std::string> > availableExtensionsMap;
    std::vector<std::string> availableExtensionsSelector;

    for(const std::string& serviceId : availableExtensionsId)
    {
        bool serviceIsSelectedByUser =
            std::find(
                m_selectedServices.begin(),
                m_selectedServices.end(),
                serviceId
            ) != m_selectedServices.end();

        // Test if the service is considered here as available by users, if yes push in availableExtensionsSelector
        // excluded mode => add services that are not selected by users
        // included mode => add services selected by users
        if((m_servicesAreExcluded && !serviceIsSelectedByUser)
           || (!m_servicesAreExcluded && serviceIsSelectedByUser))
        {
            // Add this service
            std::string infoUser =
                service::extension::factory::get()->getServiceDescription(serviceId);

            auto iter = m_serviceToConfig.find(serviceId);
            if(iter != m_serviceToConfig.end())
            {
                infoUser = service::extension::config::getDefault()->getConfigDesc(iter->second);
            }

            if(!infoUser.empty())
            {
                availableExtensionsMap.emplace_back(serviceId, infoUser);
                availableExtensionsSelector.push_back(infoUser);
            }
            else
            {
                availableExtensionsMap.emplace_back(serviceId, serviceId);
                availableExtensionsSelector.push_back(serviceId);
            }
        }
    }

    // Sort available services (lexical string sort)
    std::sort(availableExtensionsSelector.begin(), availableExtensionsSelector.end());

    // Test if we have an extension
    if(!availableExtensionsMap.empty())
    {
        std::string extensionId           = availableExtensionsMap[0].first;
        bool extensionSelectionIsCanceled = false;

        // Selection of extension when availableExtensions.size() > 1
        if(availableExtensionsSelector.size() > 1)
        {
            sight::ui::dialog::selector::sptr selector =
                std::make_shared<sight::ui::dialog::selector>();

            if(m_mode != READER_MODE)
            {
                selector->setTitle("Writer to use");
            }
            else
            {
                selector->setTitle("Reader to use");
            }

            selector->set_choices(availableExtensionsSelector);
            std::string selection = selector->show()[0];
            if(!selection.empty())
            {
                bool extensionIdFound = false;

                using PairType = std::pair<std::string, std::string>;
                for(const PairType& pair : availableExtensionsMap)
                {
                    if(pair.second == selection)
                    {
                        extensionId      = pair.first;
                        extensionIdFound = true;
                    }
                }

                if(!extensionIdFound)
                {
                    m_sigJobFailed->async_emit();
                }

                SIGHT_ASSERT("Problem to find the selected string.", extensionIdFound);
            }
            else
            {
                extensionSelectionIsCanceled = true;
            }
        }

        if(!extensionSelectionIsCanceled)
        {
            // Get config
            bool hasConfigForService = false;
            service::config_t srvCfg;
            if(m_serviceToConfig.find(extensionId) != m_serviceToConfig.end())
            {
                hasConfigForService = true;
                srvCfg              = service::extension::config::getDefault()->get_service_config(
                    m_serviceToConfig[extensionId],
                    extensionId
                );
                SIGHT_ASSERT(
                    "No service configuration of type service::extension::config was found",
                    !srvCfg.empty()
                );
            }

            // Configure and start service
            if(m_mode == READER_MODE)
            {
                auto reader = service::add<io::service::reader>(extensionId);
                {
                    auto obj = m_data.lock();
                    reader->set_inout(obj.get_shared(), io::service::s_DATA_KEY);
                }

                reader->set_worker(this->worker());

                if(hasConfigForService)
                {
                    reader->set_config(srvCfg);
                    reader->configure();
                }

                auto jobCreatedSignal = reader->signal("jobCreated");
                if(jobCreatedSignal)
                {
                    jobCreatedSignal->connect(m_slotForwardJob);
                }

                try
                {
                    reader->start();
                    reader->openLocationDialog();

                    sight::ui::cursor cursor;
                    cursor.setCursor(sight::ui::cursor_base::BUSY);
                    reader->update();
                    cursor.setDefaultCursor();

                    reader->stop();
                    service::unregisterService(reader);
                }
                catch(std::exception& e)
                {
                    std::string msg = "Failed to read : \n" + std::string(e.what());
                    sight::ui::dialog::message::show("Reader Error", msg);
                    m_sigJobFailed->async_emit();
                }
                if(reader->hasFailed())
                {
                    m_sigJobFailed->async_emit();
                }
                else
                {
                    m_sigJobSucceeded->async_emit();
                }
            }
            else
            {
                auto writer = service::add<io::service::writer>(extensionId);
                {
                    auto obj = m_data.lock();
                    writer->set_input(obj.get_shared(), io::service::s_DATA_KEY);
                }

                writer->set_worker(this->worker());

                if(hasConfigForService)
                {
                    writer->set_config(srvCfg);
                    writer->configure();
                }

                auto jobCreatedSignal = writer->signal("jobCreated");
                if(jobCreatedSignal)
                {
                    jobCreatedSignal->connect(m_slotForwardJob);
                }

                try
                {
                    writer->start();
                    writer->openLocationDialog();

                    sight::ui::cursor cursor;
                    cursor.setCursor(sight::ui::cursor_base::BUSY);
                    writer->update();
                    cursor.setDefaultCursor();

                    writer->stop();
                    service::unregisterService(writer);
                }
                catch(std::exception& e)
                {
                    std::string msg = "Failed to write : \n" + std::string(e.what());
                    sight::ui::dialog::message::show("Writer Error", msg);
                    m_sigJobFailed->async_emit();
                }

                if(writer->hasFailed())
                {
                    m_sigJobFailed->async_emit();
                }
                else
                {
                    m_sigJobSucceeded->async_emit();
                }
            }
        }
        else
        {
            m_sigJobFailed->async_emit();
        }
    }
    else
    {
        SIGHT_WARN("selector::load : availableExtensions is empty.");
        if(m_mode == READER_MODE)
        {
            sight::ui::dialog::message messageBox;
            messageBox.setTitle("Reader not found");
            messageBox.setMessage("There are no available readers for this data type.");
            messageBox.setIcon(sight::ui::dialog::message::WARNING);
            messageBox.addButton(sight::ui::dialog::message::OK);
            messageBox.show();
        }
        else // m_mode == WRITER_MODE
        {
            sight::ui::dialog::message messageBox;
            messageBox.setTitle("Writer not found");
            messageBox.setMessage("There are no available writers for this data type.");
            messageBox.setIcon(sight::ui::dialog::message::WARNING);
            messageBox.addButton(sight::ui::dialog::message::OK);
            messageBox.show();
        }

        m_sigJobFailed->async_emit();
    }
}

//------------------------------------------------------------------------------

void selector::info(std::ostream& _sstream)
{
    // Update message
    _sstream << "selector";
}

//------------------------------------------------------------------------------

void selector::setIOMode(IOMode _mode)
{
    m_mode = _mode;
}

//------------------------------------------------------------------------------

void selector::forwardJob(core::jobs::base::sptr job)
{
    m_sigJobCreated->emit(job);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::io
