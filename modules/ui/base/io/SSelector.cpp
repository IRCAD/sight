/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SSelector.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>

#include <data/Composite.hpp>

#include <io/base/service/IReader.hpp>
#include <io/base/service/IWriter.hpp>

#include <service/extension/Config.hpp>
#include <service/extension/Factory.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <sstream>
#include <string>

namespace sight::module::ui::base::io
{

namespace io = sight::io;

//------------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL   = "jobCreated";
static const core::com::Signals::SignalKeyType JOB_FAILED_SIGNAL    = "jobFailed";
static const core::com::Signals::SignalKeyType JOB_SUCCEEDED_SIGNAL = "jobSucceeded";

static const core::com::Slots::SlotKeyType FORWARD_JOB_SLOT = "forwardJob";

//------------------------------------------------------------------------------

SSelector::SSelector()
{
    m_sigJobCreated   = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
    m_sigJobFailed    = newSignal<JobFailedSignalType>(JOB_FAILED_SIGNAL);
    m_sigJobSucceeded = newSignal<JobSucceededSignalType>(JOB_SUCCEEDED_SIGNAL);

    m_slotForwardJob = newSlot(FORWARD_JOB_SLOT, &SSelector::forwardJob, this);
}

//------------------------------------------------------------------------------

SSelector::~SSelector() noexcept =
    default;

//------------------------------------------------------------------------------

void SSelector::configuring()
{
    const ConfigType srvConfig = this->getConfigTree();

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

    const auto selectionCfg = srvConfig.equal_range("addSelection");
    for(auto itSelection = selectionCfg.first ; itSelection != selectionCfg.second ; ++itSelection)
    {
        const auto service = itSelection->second.get<std::string>("<xmlattr>.service");
        m_selectedServices.push_back(service);
        SIGHT_DEBUG("add selection => " + service);

        const std::string configId = itSelection->second.get<std::string>("<xmlattr>.config", "");
        if(!configId.empty())
        {
            m_serviceToConfig[service] = configId;
            SIGHT_DEBUG(
                std::string("add config '").append(configId).append("' for service '").append(service).append("'")
            );
        }
    }

    const auto configCfg = srvConfig.equal_range("config");
    for(auto itCfg = configCfg.first ; itCfg != configCfg.second ; ++itCfg)
    {
        const auto service  = itCfg->second.get<std::string>("<xmlattr>.service");
        const auto configId = itCfg->second.get<std::string>("<xmlattr>.id");

        m_serviceToConfig[service] = configId;
        SIGHT_DEBUG(std::string("add config '").append(configId).append("' for service '").append(service).append("'"));
    }
}

//------------------------------------------------------------------------------

void SSelector::starting()
{
}

//------------------------------------------------------------------------------

void SSelector::stopping()
{
}

//------------------------------------------------------------------------------

void SSelector::updating()
{
    std::vector<std::string> availableExtensionsId;
    {
        auto objLock           = m_data.lock();
        data::Object::sptr obj = objLock.get_shared();

        // Retrieve implementation of type io::base::service::IReader for this object
        if(m_mode == READER_MODE)
        {
            SIGHT_ASSERT("An inout key '" + io::base::service::s_DATA_KEY + "' must be defined.", obj);
            const auto classname = obj->getClassname();

            availableExtensionsId =
                service::extension::Factory::getDefault()->getImplementationIdFromObjectAndType(
                    classname,
                    "sight::io::base::service::IReader"
                );
        }
        else // m_mode == WRITER_MODE
        {
            SIGHT_ASSERT("The inout key '" + io::base::service::s_DATA_KEY + "' is not correctly set.", obj);

            availableExtensionsId =
                service::extension::Factory::getDefault()->getImplementationIdFromObjectAndType(
                    obj->getClassname(),
                    "sight::io::base::service::IWriter"
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
                service::extension::Factory::getDefault()->getServiceDescription(serviceId);

            auto iter = m_serviceToConfig.find(serviceId);
            if(iter != m_serviceToConfig.end())
            {
                infoUser = service::extension::Config::getDefault()->getConfigDesc(iter->second);
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
            sight::ui::base::dialog::SelectorDialog::sptr selector = sight::ui::base::dialog::SelectorDialog::New();

            if(m_mode != READER_MODE)
            {
                selector->setTitle("Writer to use");
            }
            else
            {
                selector->setTitle("Reader to use");
            }

            selector->setSelections(availableExtensionsSelector);
            std::string selection = selector->show();
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
                    m_sigJobFailed->asyncEmit();
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
            // Get Config
            bool hasConfigForService = false;
            core::runtime::ConfigurationElement::csptr srvCfg;
            if(m_serviceToConfig.find(extensionId) != m_serviceToConfig.end())
            {
                hasConfigForService = true;
                srvCfg              = service::extension::Config::getDefault()->getServiceConfig(
                    m_serviceToConfig[extensionId],
                    extensionId
                );
                SIGHT_ASSERT(
                    "No service configuration of type service::extension::Config was found",
                    srvCfg
                );
            }

            // Configure and start service
            if(m_mode == READER_MODE)
            {
                auto reader = service::add<io::base::service::IReader>(extensionId);
                {
                    auto obj = m_data.lock();
                    reader->setInOut(obj.get_shared(), io::base::service::s_DATA_KEY);
                }

                reader->setWorker(m_associatedWorker);

                if(hasConfigForService)
                {
                    reader->setConfiguration(core::runtime::ConfigurationElement::constCast(srvCfg));
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

                    sight::ui::base::Cursor cursor;
                    cursor.setCursor(sight::ui::base::ICursor::BUSY);
                    reader->update();
                    cursor.setDefaultCursor();

                    reader->stop();
                    service::OSR::unregisterService(reader);
                }
                catch(std::exception& e)
                {
                    std::string msg = "Failed to read : \n" + std::string(e.what());
                    sight::ui::base::dialog::MessageDialog::show("Reader Error", msg);
                    m_sigJobFailed->asyncEmit();
                }
                if(reader->hasFailed())
                {
                    m_sigJobFailed->asyncEmit();
                }
                else
                {
                    m_sigJobSucceeded->asyncEmit();
                }
            }
            else
            {
                auto writer = service::add<io::base::service::IWriter>(extensionId);
                {
                    auto obj = m_data.lock();
                    writer->setInput(obj.get_shared(), io::base::service::s_DATA_KEY);
                }

                writer->setWorker(m_associatedWorker);

                if(hasConfigForService)
                {
                    writer->setConfiguration(core::runtime::ConfigurationElement::constCast(srvCfg));
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

                    sight::ui::base::Cursor cursor;
                    cursor.setCursor(sight::ui::base::ICursor::BUSY);
                    writer->update();
                    cursor.setDefaultCursor();

                    writer->stop();
                    service::OSR::unregisterService(writer);
                }
                catch(std::exception& e)
                {
                    std::string msg = "Failed to write : \n" + std::string(e.what());
                    sight::ui::base::dialog::MessageDialog::show("Writer Error", msg);
                    m_sigJobFailed->asyncEmit();
                }

                if(writer->hasFailed())
                {
                    m_sigJobFailed->asyncEmit();
                }
                else
                {
                    m_sigJobSucceeded->asyncEmit();
                }
            }
        }
        else
        {
            m_sigJobFailed->asyncEmit();
        }
    }
    else
    {
        SIGHT_WARN("SSelector::load : availableExtensions is empty.");
        if(m_mode == READER_MODE)
        {
            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Reader not found");
            messageBox.setMessage("There are no available readers for this data type.");
            messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else // m_mode == WRITER_MODE
        {
            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Writer not found");
            messageBox.setMessage("There are no available writers for this data type.");
            messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }

        m_sigJobFailed->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SSelector::info(std::ostream& _sstream)
{
    // Update message
    _sstream << "SSelector";
}

//------------------------------------------------------------------------------

void SSelector::setIOMode(IOMode _mode)
{
    m_mode = _mode;
}

//------------------------------------------------------------------------------

void SSelector::forwardJob(core::jobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::io
