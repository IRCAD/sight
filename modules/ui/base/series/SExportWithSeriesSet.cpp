/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SExportWithSeriesSet.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/jobs/IJob.hpp>

#include <data/SeriesSet.hpp>

#include <io/base/service/ioTypes.hpp>

#include <service/base.hpp>
#include <service/extension/Config.hpp>

#include <ui/base/Cursor.hpp>

namespace sight::module::ui::base::series
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::Slots::SlotKeyType FORWARD_JOB_SLOT       = "forwardJob";

//------------------------------------------------------------------------------

SExportWithSeriesSet::SExportWithSeriesSet() noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader")
{
    m_sigJobCreated  = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
    m_slotForwardJob = newSlot(FORWARD_JOB_SLOT, &SExportWithSeriesSet::forwardJob, this);
}

//------------------------------------------------------------------------------

void SExportWithSeriesSet::info(std::ostream& _sstream)
{
    _sstream << "Action for add SeriesSet" << std::endl;
}

//------------------------------------------------------------------------------

void SExportWithSeriesSet::configuring()
{
    this->sight::ui::base::IAction::initialize();
    auto vectConfig = m_configuration->find("IOSelectorSrvConfig");
    if(!vectConfig.empty())
    {
        auto selectorConfig = vectConfig.at(0);
        SIGHT_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_ioSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }
}

//------------------------------------------------------------------------------

void SExportWithSeriesSet::updating()
{
    sight::ui::base::LockAction lock(this->getSptr());

    // Create a new SeriesSet
    auto local_series_set = data::SeriesSet::New();

    {
        // Lock only here to avoid a dead lock since IOSelectorService will also lock the data object
        const auto series = m_series.lock();
        SIGHT_ASSERT("The inout key 'series' is not correctly set.", series);

        local_series_set->push_back(series.get_shared());
    }

    // Create IOSelectorService on the new SeriesSet and execute it.

    // Get the config
    core::runtime::ConfigurationElement::csptr ioCfg;
    ioCfg = service::extension::Config::getDefault()->getServiceConfig(
        m_ioSelectorSrvConfig,
        "sight::module::ui::base::io::SSelector"
    );
    SIGHT_ASSERT(
        "There is no service configuration "
        << m_ioSelectorSrvConfig
        << " for module::ui::base::editor::SSelector",
        ioCfg
    );

    // Init and execute the service
    service::IService::sptr ioSelectorSrv;
    ioSelectorSrv = service::add("sight::module::ui::base::io::SSelector");
    ioSelectorSrv->setInOut(local_series_set, io::base::service::s_DATA_KEY);

    ioSelectorSrv->setWorker(m_associatedWorker);

    auto jobCreatedSignal = ioSelectorSrv->signal("jobCreated");
    if(jobCreatedSignal)
    {
        jobCreatedSignal->connect(m_slotForwardJob);
    }

    ioSelectorSrv->setConfiguration(core::runtime::ConfigurationElement::constCast(ioCfg));
    ioSelectorSrv->configure();
    ioSelectorSrv->start();
    ioSelectorSrv->update();
    ioSelectorSrv->stop();
    service::remove(ioSelectorSrv);
}

//------------------------------------------------------------------------------

void SExportWithSeriesSet::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();

    const auto series = m_series.lock();
    SIGHT_FATAL_IF("The associated object must be a data::Series.", !series);
}

//------------------------------------------------------------------------------

void SExportWithSeriesSet::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SExportWithSeriesSet::forwardJob(core::jobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

} // namespace sight::module::ui::base::series
