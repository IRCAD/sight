/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "export_with_series_set.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/jobs/base.hpp>

#include <data/series_set.hpp>

#include <io/__/service/ioTypes.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>

namespace sight::module::ui::series
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::slots::key_t FORWARD_JOB_SLOT     = "forwardJob";

//------------------------------------------------------------------------------

export_with_series_set::export_with_series_set() noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader")
{
    m_sigJobCreated  = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
    m_slotForwardJob = new_slot(FORWARD_JOB_SLOT, &export_with_series_set::forwardJob, this);
}

//------------------------------------------------------------------------------

void export_with_series_set::info(std::ostream& _sstream)
{
    _sstream << "Action for add series_set" << std::endl;
}

//------------------------------------------------------------------------------

void export_with_series_set::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->get_config();
    m_ioSelectorSrvConfig = config.get<std::string>("IOSelectorSrvConfig.<xmlattr>.name", m_ioSelectorSrvConfig);
}

//------------------------------------------------------------------------------

void export_with_series_set::updating()
{
    sight::ui::LockAction lock(this->get_sptr());

    // Create a new series_set
    auto local_series_set = std::make_shared<data::series_set>();

    {
        // Lock only here to avoid a dead lock since IOSelectorService will also lock the data object
        const auto series = m_series.lock();
        SIGHT_ASSERT("The inout key 'series' is not correctly set.", series);

        local_series_set->push_back(series.get_shared());
    }

    // Create IOSelectorService on the new series_set and execute it.

    // Get the config
    const auto ioCfg = service::extension::config::getDefault()->get_service_config(
        m_ioSelectorSrvConfig,
        "sight::module::ui::io::selector"
    );
    SIGHT_ASSERT(
        "There is no service configuration "
        << m_ioSelectorSrvConfig
        << " for module::ui::editor::selector",
        !ioCfg.empty()
    );

    // Init and execute the service
    service::base::sptr ioSelectorSrv;
    ioSelectorSrv = service::add("sight::module::ui::io::selector");
    ioSelectorSrv->set_inout(local_series_set, io::service::s_DATA_KEY);

    ioSelectorSrv->set_worker(this->worker());

    auto jobCreatedSignal = ioSelectorSrv->signal("jobCreated");
    if(jobCreatedSignal)
    {
        jobCreatedSignal->connect(m_slotForwardJob);
    }

    ioSelectorSrv->set_config(ioCfg);
    ioSelectorSrv->configure();
    ioSelectorSrv->start();
    ioSelectorSrv->update();
    ioSelectorSrv->stop();
    service::remove(ioSelectorSrv);
}

//------------------------------------------------------------------------------

void export_with_series_set::starting()
{
    this->sight::ui::action::actionServiceStarting();

    const auto series = m_series.lock();
    SIGHT_FATAL_IF("The associated object must be a data::series.", !series);
}

//------------------------------------------------------------------------------

void export_with_series_set::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

void export_with_series_set::forwardJob(core::jobs::base::sptr job)
{
    m_sigJobCreated->emit(job);
}

} // namespace sight::module::ui::series
