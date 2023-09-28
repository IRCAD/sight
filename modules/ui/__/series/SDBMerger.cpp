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

#include "SDBMerger.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/jobs/base.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <io/__/service/ioTypes.hpp>

#include <service/extension/Config.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>

namespace sight::module::ui::series
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";
static const core::com::slots::key_t FORWARD_JOB_SLOT     = "forwardJob";

//------------------------------------------------------------------------------

SDBMerger::SDBMerger() noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader"),
    m_sigJobCreated(new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL)),
    m_slotForwardJob(new_slot(FORWARD_JOB_SLOT, &SDBMerger::forwardJob, this))
{
}

//------------------------------------------------------------------------------

SDBMerger::~SDBMerger() noexcept =
    default;

//------------------------------------------------------------------------------

void SDBMerger::info(std::ostream& _sstream)
{
    _sstream << "Action for add SeriesSet" << std::endl;
}

//------------------------------------------------------------------------------

void SDBMerger::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->getConfiguration();
    m_ioSelectorSrvConfig = config.get<std::string>("IOSelectorSrvConfig.<xmlattr>.name", m_ioSelectorSrvConfig);
}

//------------------------------------------------------------------------------

void SDBMerger::updating()
{
    sight::ui::LockAction lock(this->get_sptr());

    // Create a new SeriesSet
    auto local_series_set = std::make_shared<data::SeriesSet>();

    /// Create IOSelectorService on the new SeriesSet and execute it.

    // Get the config
    const auto ioCfg = service::extension::Config::getDefault()->getServiceConfig(
        m_ioSelectorSrvConfig,
        "sight::module::ui::io::SSelector"
    );
    SIGHT_ASSERT(
        "There is no service configuration "
        << m_ioSelectorSrvConfig
        << " for module::ui::editor::SSelector",
        !ioCfg.empty()
    );

    // Init and execute the service
    service::base::sptr ioSelectorSrv;
    ioSelectorSrv = service::add("sight::module::ui::io::SSelector");
    ioSelectorSrv->setInOut(local_series_set, io::service::s_DATA_KEY);
    ioSelectorSrv->set_worker(this->worker());

    auto jobCreatedSignal = ioSelectorSrv->signal("jobCreated");
    if(jobCreatedSignal)
    {
        jobCreatedSignal->connect(m_slotForwardJob);
    }

    ioSelectorSrv->setConfiguration(ioCfg);
    ioSelectorSrv->configure();
    ioSelectorSrv->start();
    ioSelectorSrv->update();
    ioSelectorSrv->stop();
    service::remove(ioSelectorSrv);

    // Lock only when needed.
    auto series_set = m_series_set.lock();
    SIGHT_ASSERT("The inout key '" << s_SERIES_SET << "' is not correctly set.", series_set);

    const auto scoped_emitter = series_set->scoped_emit();
    std::copy(local_series_set->cbegin(), local_series_set->cend(), data::inserter(*series_set));
}

//------------------------------------------------------------------------------

void SDBMerger::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SDBMerger::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SDBMerger::forwardJob(core::jobs::base::sptr job)
{
    m_sigJobCreated->emit(job);
}

//------------------------------------------------------------------------------
//

} // namespace sight::module::ui::series
