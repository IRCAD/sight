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

#include "uiIO/action/SSeriesDBMerger.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Series.hpp>
#include <data/tools/helper/SeriesDB.hpp>

#include <fwGui/Cursor.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwJobs/IJob.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

namespace uiIO
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiIO::action::SSeriesDBMerger, data::SeriesDB )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::Slots::SlotKeyType FORWARD_JOB_SLOT = "forwardJob";

static const ::fwServices::IService::KeyType s_SERIES_INOUT = "seriesDB";

//------------------------------------------------------------------------------

SSeriesDBMerger::SSeriesDBMerger( ) noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader")
{
    m_sigJobCreated  = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
    m_slotForwardJob = newSlot( FORWARD_JOB_SLOT, &SSeriesDBMerger::forwardJob, this );
}

//------------------------------------------------------------------------------

SSeriesDBMerger::~SSeriesDBMerger() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::info(std::ostream& _sstream )
{
    _sstream << "Action for add SeriesDB" << std::endl;
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::configuring()
{

    this->::fwGui::IActionSrv::initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("IOSelectorSrvConfig");
    if(!vectConfig.empty())
    {
        ConfigurationType selectorConfig = vectConfig.at(0);
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_ioSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::updating( )
{
    ::fwGui::LockAction lock(this->getSptr());

    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(s_SERIES_INOUT);
    SLM_ASSERT("The inout key '" + s_SERIES_INOUT + "' is not correctly set.", seriesDB);

    // Create a new SeriesDB
    data::SeriesDB::sptr localSeriesDB = data::SeriesDB::New();

    /// Create IOSelectorService on the new SeriesDB and execute it.

    // Get the config
    core::runtime::ConfigurationElement::csptr ioCfg;
    ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(m_ioSelectorSrvConfig,
                                                                                  "::uiIO::editor::SIOSelector");
    SLM_ASSERT("There is no service configuration "
               << m_ioSelectorSrvConfig
               << " for ::uiIO::editor::SIOSelector", ioCfg);

    // Init and execute the service
    ::fwServices::IService::sptr ioSelectorSrv;
    ioSelectorSrv = ::fwServices::add("::uiIO::editor::SIOSelector");
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
    ::fwServices::OSR::unregisterService( ioSelectorSrv );

    data::tools::helper::SeriesDB sDBhelper(seriesDB);
    sDBhelper.merge(localSeriesDB);
    sDBhelper.notify();
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::forwardJob(::fwJobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------
//
} // namespace action
} // namespace uiIO
