/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiIO/action/SExportWithSeriesDB.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/jobs/IJob.hpp>

#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

#include <fwIO/ioTypes.hpp>

#include <services/macros.hpp>
#include <services/op/Add.hpp>
#include <services/registry/ServiceConfig.hpp>

#include <ui/base/Cursor.hpp>

namespace uiIO
{
namespace action
{

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::uiIO::action::SExportWithSeriesDB, ::sight::data::Object )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::Slots::SlotKeyType FORWARD_JOB_SLOT = "forwardJob";

static const services::IService::KeyType s_SERIES_INOUT = "series";

//------------------------------------------------------------------------------

SExportWithSeriesDB::SExportWithSeriesDB( ) noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader")
{
    m_sigJobCreated  = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
    m_slotForwardJob = newSlot( FORWARD_JOB_SLOT, &SExportWithSeriesDB::forwardJob, this );
}

//------------------------------------------------------------------------------

SExportWithSeriesDB::~SExportWithSeriesDB() noexcept
{
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::info(std::ostream& _sstream )
{
    _sstream << "Action for add SeriesDB" << std::endl;
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::configuring()
{
    this->::ui::base::IActionSrv::initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("IOSelectorSrvConfig");
    if(!vectConfig.empty())
    {
        ConfigurationType selectorConfig = vectConfig.at(0);
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_ioSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::updating( )
{
    ui::base::LockAction lock(this->getSptr());

    data::Series::sptr series = this->getInOut< data::Series >(s_SERIES_INOUT);
    SLM_ASSERT("The inout key '" + s_SERIES_INOUT + "' is not correctly set.", series);

    // Create a new SeriesDB
    data::SeriesDB::sptr localSeriesDB = data::SeriesDB::New();
    localSeriesDB->getContainer().push_back(series);

    /// Create IOSelectorService on the new SeriesDB and execute it.

    // Get the config
    core::runtime::ConfigurationElement::csptr ioCfg;
    ioCfg = services::registry::ServiceConfig::getDefault()->getServiceConfig(m_ioSelectorSrvConfig,
                                                                              "::uiIO::editor::SIOSelector");
    SLM_ASSERT("There is no service configuration "
               << m_ioSelectorSrvConfig
               << " for ::uiIO::editor::SIOSelector", ioCfg);

    // Init and execute the service
    services::IService::sptr ioSelectorSrv;
    ioSelectorSrv = services::add( "::uiIO::editor::SIOSelector");
    ioSelectorSrv->registerInOut(localSeriesDB, ::fwIO::s_DATA_KEY);

    ioSelectorSrv->setWorker(m_associatedWorker);

    auto jobCreatedSignal = ioSelectorSrv->signal("jobCreated");
    if(jobCreatedSignal)
    {
        jobCreatedSignal->connect(m_slotForwardJob);
    }

    ioSelectorSrv->setConfiguration( core::runtime::ConfigurationElement::constCast(ioCfg) );
    ioSelectorSrv->configure();
    ioSelectorSrv->start();
    ioSelectorSrv->update();
    ioSelectorSrv->stop();
    services::OSR::unregisterService( ioSelectorSrv );
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::starting()
{
    this->::ui::base::IActionSrv::actionServiceStarting();

    data::Series::sptr series = this->getInOut< data::Series >(s_SERIES_INOUT);
    SLM_FATAL_IF( "The associated object must be a data::Series.", !series);
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::stopping()
{
    this->::ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::forwardJob(core::jobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------
//
} // namespace action
} // namespace uiIO
