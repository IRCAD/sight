/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/base/editor/SIOSelector.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>

#include <data/Composite.hpp>
#include <data/tools/helper/Composite.hpp>

#include <services/macros.hpp>
#include <services/op/Add.hpp>
#include <services/registry/ServiceConfig.hpp>
#include <services/registry/ServiceFactory.hpp>

#include <io/base/services/IReader.hpp>
#include <io/base/services/IWriter.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <sstream>
#include <string>

namespace sight::modules::ui::base
{

namespace editor
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::ui::base::editor::IDialogEditor, ::sight::modules::ui::base::editor::SIOSelector,
                         ::sight::data::Object )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::Signals::SignalKeyType JOB_FAILED_SIGNAL    = "jobFailed";
static const core::com::Signals::SignalKeyType JOB_SUCCEEDED_SIGNAL = "jobSucceeded";

static const core::com::Slots::SlotKeyType FORWARD_JOB_SLOT = "forwardJob";

//------------------------------------------------------------------------------

SIOSelector::SIOSelector() :
    m_mode( READER_MODE ),
    m_servicesAreExcluded( true )
{
    m_sigJobCreated   = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
    m_sigJobFailed    = newSignal< JobFailedSignalType >( JOB_FAILED_SIGNAL );
    m_sigJobSucceeded = newSignal< JobSucceededSignalType >( JOB_SUCCEEDED_SIGNAL );

    m_slotForwardJob = newSlot( FORWARD_JOB_SLOT, &SIOSelector::forwardJob, this );
}

//------------------------------------------------------------------------------

SIOSelector::~SIOSelector()  noexcept
{
}

//------------------------------------------------------------------------------

void SIOSelector::configuring()
{
    const ConfigType srvConfig = this->getConfigTree();

    const std::string mode = srvConfig.get<std::string>("type.<xmlattr>.mode", "reader");
    SLM_ASSERT("The xml attribute <mode> must be 'reader' (to open file) or 'writer' (to write a new file).",
               mode == "writer" || mode == "reader" );
    m_mode = ( mode == "writer" ) ? WRITER_MODE : READER_MODE;
    SLM_DEBUG( "mode => " + mode );

    m_dataClassname = srvConfig.get<std::string>("type.<xmlattr>.class", "");

    const std::string selectionMode = srvConfig.get<std::string>("selection.<xmlattr>.mode", "exclude");
    SLM_ASSERT( "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
                "'exclude' (to exclude the selection of the selector list).",
                selectionMode == "exclude" || selectionMode == "include" );
    m_servicesAreExcluded = ( selectionMode == "exclude" );
    SLM_DEBUG( "selection mode => " + selectionMode );

    const auto selectionCfg = srvConfig.equal_range("addSelection");
    for (auto itSelection = selectionCfg.first; itSelection != selectionCfg.second; ++itSelection)
    {
        const std::string service = itSelection->second.get<std::string>("<xmlattr>.service");
        m_selectedServices.push_back(service);
        SLM_DEBUG( "add selection => " + service );

        const std::string configId = itSelection->second.get<std::string>("<xmlattr>.config", "");
        if(!configId.empty())
        {
            m_serviceToConfig[service] = configId;
            SLM_DEBUG( "add config '" + configId + "' for service '" + service + "'");
        }
    }

    const auto configCfg = srvConfig.equal_range("config");
    for (auto itCfg = configCfg.first; itCfg != configCfg.second; ++itCfg)
    {
        const std::string service  = itCfg->second.get<std::string>("<xmlattr>.service");
        const std::string configId = itCfg->second.get<std::string>("<xmlattr>.id");

        m_serviceToConfig[service] = configId;
        SLM_DEBUG( "add config '" + configId + "' for service '" + service + "'");
    }

    if (m_mode == WRITER_MODE)
    {
        this->registerObject(io::base::services::s_DATA_KEY, AccessType::INOUT);
    }
    else if (m_dataClassname.empty())
    {
        this->registerObject(io::base::services::s_DATA_KEY, AccessType::INOUT);
    }
    else
    {
        this->registerObject(io::base::services::s_DATA_KEY, AccessType::OUTPUT, false, true);
    }
}

//------------------------------------------------------------------------------

void SIOSelector::starting()
{
}

//------------------------------------------------------------------------------

void SIOSelector::stopping()
{
}

//------------------------------------------------------------------------------

void SIOSelector::updating()
{
    bool createOutput      = false;
    data::Object::sptr obj = this->getInOut< data::Object>(io::base::services::s_DATA_KEY);

    // Retrieve implementation of type io::base::services::IReader for this object
    std::vector< std::string > availableExtensionsId;
    if ( m_mode == READER_MODE )
    {
        std::string classname = m_dataClassname;

        SLM_ASSERT(
            "An inout key '" + io::base::services::s_DATA_KEY + "' must be defined if 'class' attribute is not defined.",
            obj || !classname.empty());

        if (obj)
        {
            SLM_WARN_IF("The 'class' attribute is defined, but the object is set as 'inout', only the object classname "
                        "is used", !classname.empty())
            classname = obj->getClassname();
        }
        createOutput          = (!obj && !m_dataClassname.empty());
        availableExtensionsId =
            services::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType(
                classname, "::sight::io::base::services::IReader");
    }
    else // m_mode == WRITER_MODE
    {
        SLM_ASSERT("The inout key '" + io::base::services::s_DATA_KEY + "' is not correctly set.", obj);

        availableExtensionsId =
            services::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType(
                obj->getClassname(), "::sight::io::base::services::IWriter");
    }

    // Filter available extensions and replace id by service description
    std::vector< std::pair < std::string, std::string > > availableExtensionsMap;
    std::vector< std::string > availableExtensionsSelector;

    for( const std::string& serviceId :  availableExtensionsId )
    {
        bool serviceIsSelectedByUser =
            std::find( m_selectedServices.begin(), m_selectedServices.end(),
                       serviceId ) != m_selectedServices.end();

        // Test if the service is considered here as available by users, if yes push in availableExtensionsSelector
        // excluded mode => add services that are not selected by users
        // included mode => add services selected by users
        if( (m_servicesAreExcluded && !serviceIsSelectedByUser) ||
            (!m_servicesAreExcluded && serviceIsSelectedByUser) )
        {
            // Add this service
            std::string infoUser =
                services::registry::ServiceFactory::getDefault()->getServiceDescription(serviceId);

            std::map< std::string, std::string >::const_iterator iter = m_serviceToConfig.find( serviceId );
            if ( iter != m_serviceToConfig.end() )
            {
                infoUser = services::registry::ServiceConfig::getDefault()->getConfigDesc(iter->second);
            }

            if (infoUser != "")
            {
                availableExtensionsMap.push_back( std::pair < std::string, std::string > (serviceId, infoUser) );
                availableExtensionsSelector.push_back( infoUser );
            }
            else
            {
                availableExtensionsMap.push_back( std::pair < std::string, std::string > (serviceId, serviceId) );
                availableExtensionsSelector.push_back( serviceId );
            }
        }
    }

    // Sort available services (lexical string sort)
    std::sort( availableExtensionsSelector.begin(), availableExtensionsSelector.end() );

    // Test if we have an extension
    if ( !availableExtensionsMap.empty() )
    {
        std::string extensionId           = availableExtensionsMap[0].first;
        bool extensionSelectionIsCanceled = false;

        // Selection of extension when availableExtensions.size() > 1
        if ( availableExtensionsSelector.size() > 1 )
        {
            sight::ui::base::dialog::SelectorDialog::sptr selector = sight::ui::base::dialog::SelectorDialog::New();

            if ( m_mode != READER_MODE )
            {
                selector->setTitle("Writer to use");
            }
            else
            {
                selector->setTitle("Reader to use");
            }
            selector->setSelections(availableExtensionsSelector);
            std::string selection = selector->show();
            if( !selection.empty() )
            {
                bool extensionIdFound = false;

                typedef std::pair < std::string, std::string > PairType;
                for(PairType pair :  availableExtensionsMap)
                {
                    if (pair.second == selection )
                    {
                        extensionId      = pair.first;
                        extensionIdFound = true;
                    }
                }
                if(!extensionIdFound)
                {
                    m_sigJobFailed->asyncEmit();
                }
                SLM_ASSERT("Problem to find the selected string.", extensionIdFound );
            }
            else
            {
                extensionSelectionIsCanceled = true;
            }
        }

        if ( !extensionSelectionIsCanceled )
        {

            // Get Config
            bool hasConfigForService = false;
            core::runtime::ConfigurationElement::csptr srvCfg;
            if ( m_serviceToConfig.find( extensionId ) != m_serviceToConfig.end() )
            {
                hasConfigForService = true;
                srvCfg              = services::registry::ServiceConfig::getDefault()->getServiceConfig(
                    m_serviceToConfig[extensionId], extensionId );
                SLM_ASSERT(
                    "No service configuration of type services::registry::ServiceConfig was found",
                    srvCfg );
            }

            // Configure and start service
            if ( m_mode == READER_MODE )
            {
                if(createOutput)
                {
                    obj = data::factory::New(m_dataClassname);
                    SLM_ASSERT("Cannot create object with classname='" + m_dataClassname + "'", obj);
                }

                io::base::services::IReader::sptr reader = services::add< io::base::services::IReader >( extensionId );
                reader->registerInOut(obj, io::base::services::s_DATA_KEY);
                reader->setWorker(m_associatedWorker);

                if ( hasConfigForService )
                {
                    reader->setConfiguration( core::runtime::ConfigurationElement::constCast(srvCfg) );
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
                    services::OSR::unregisterService(reader);

                    if (createOutput && !reader->hasFailed())
                    {
                        this->setOutput(io::base::services::s_DATA_KEY, obj);
                    }
                }
                catch (std::exception& e)
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
                // When all writers make use of getObject(), we can use the following code instead:
                //      io::base::services::IWriter::sptr writer = services::add< io::base::services::IWriter >(
                // extensionId );
                //      writer->registerInput(this->getObject(), io::base::services::s_DATA_KEY);

                auto factory                             = services::registry::ServiceFactory::getDefault();
                io::base::services::IWriter::sptr writer =
                    io::base::services::IWriter::dynamicCast(factory->create( "::sight::io::base::services::IWriter",
                                                                              extensionId));
                writer->registerInput(obj, io::base::services::s_DATA_KEY);

                writer->setWorker(m_associatedWorker);

                if ( hasConfigForService )
                {
                    writer->setConfiguration( core::runtime::ConfigurationElement::constCast(srvCfg) );
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
                    services::OSR::unregisterService(writer);
                }
                catch (std::exception& e)
                {
                    std::string msg = "Failed to write : \n" +  std::string(e.what());
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
        SLM_WARN("SIOSelector::load : availableExtensions is empty.");
        if ( m_mode == READER_MODE )
        {
            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Reader not found");
            messageBox.setMessage( "There are no available readers for this data type." );
            messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else // m_mode == WRITER_MODE
        {
            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Writer not found");
            messageBox.setMessage( "There are no available writers for this data type." );
            messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
            messageBox.show();

        }
        m_sigJobFailed->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SIOSelector::info( std::ostream& _sstream )
{
    // Update message
    _sstream << "SIOSelector";
}

//------------------------------------------------------------------------------

void SIOSelector::setIOMode( IOMode _mode )
{
    m_mode = _mode;
}

//------------------------------------------------------------------------------

void SIOSelector::forwardJob(core::jobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------
} // namespace editor

} // namespace sight::modules::ui::base
