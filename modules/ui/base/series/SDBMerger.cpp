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

#include "SDBMerger.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/jobs/IJob.hpp>

#include <data/Series.hpp>
#include <data/helper/SeriesDB.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/extension/Config.hpp>

#include <io/base/service/ioTypes.hpp>

#include <ui/base/Cursor.hpp>

namespace sight::module::ui::base
{
namespace series
{


static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::Slots::SlotKeyType FORWARD_JOB_SLOT = "forwardJob";

static const service::IService::KeyType s_SERIES_INOUT = "seriesDB";

//------------------------------------------------------------------------------

SDBMerger::SDBMerger( ) noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader")
{
    m_sigJobCreated  = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
    m_slotForwardJob = newSlot( FORWARD_JOB_SLOT, &SDBMerger::forwardJob, this );
}

//------------------------------------------------------------------------------

SDBMerger::~SDBMerger() noexcept
{
}

//------------------------------------------------------------------------------

void SDBMerger::info(std::ostream& _sstream )
{
    _sstream << "Action for add SeriesDB" << std::endl;
}

//------------------------------------------------------------------------------

void SDBMerger::configuring()
{

    this->sight::ui::base::IAction::initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("IOSelectorSrvConfig");
    if(!vectConfig.empty())
    {
        ConfigurationType selectorConfig = vectConfig.at(0);
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_ioSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }
}

//------------------------------------------------------------------------------

void SDBMerger::updating( )
{
    sight::ui::base::LockAction lock(this->getSptr());

    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(s_SERIES_INOUT);
    SLM_ASSERT("The inout key '" + s_SERIES_INOUT + "' is not correctly set.", seriesDB);

    // Create a new SeriesDB
    data::SeriesDB::sptr localSeriesDB = data::SeriesDB::New();

    /// Create IOSelectorService on the new SeriesDB and execute it.

    // Get the config
    core::runtime::ConfigurationElement::csptr ioCfg;
    ioCfg = service::extension::Config::getDefault()->getServiceConfig(m_ioSelectorSrvConfig,
                                                                             "::sight::module::ui::base::io::SSelector");
    SLM_ASSERT("There is no service configuration "
               << m_ioSelectorSrvConfig
               << " for module::ui::base::editor::SSelector", ioCfg);

    // Init and execute the service
    service::IService::sptr ioSelectorSrv;
    ioSelectorSrv = service::add("::sight::module::ui::base::io::SSelector");
    ioSelectorSrv->registerInOut(localSeriesDB, io::base::service::s_DATA_KEY);
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
    service::OSR::unregisterService( ioSelectorSrv );

    data::helper::SeriesDB sDBhelper(seriesDB);
    sDBhelper.merge(localSeriesDB);
    sDBhelper.notify();
}

//------------------------------------------------------------------------------

void SDBMerger::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SDBMerger::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SDBMerger::forwardJob(core::jobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------
//
} // namespace series
} // namespace sight::module::ui::base
