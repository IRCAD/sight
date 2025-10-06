/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
#include <core/progress/monitor.hpp>

#include <data/series_set.hpp>

#include <io/__/service/io_types.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>

namespace sight::module::ui::series
{

static const core::com::slots::key_t FORWARD_MONITOR_SLOT = "forwardmonitor";

//------------------------------------------------------------------------------

export_with_series_set::export_with_series_set() noexcept :
    has_monitors(m_signals),
    m_io_selector_srv_config("IOSelectorServiceConfigVRRenderReader")
{
    m_slot_forward_monitor = new_slot(FORWARD_MONITOR_SLOT, &export_with_series_set::forward_monitor, this);
}

//------------------------------------------------------------------------------

void export_with_series_set::info(std::ostream& _sstream)
{
    _sstream << "action for add series_set" << std::endl;
}

//------------------------------------------------------------------------------

void export_with_series_set::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->get_config();
    m_io_selector_srv_config = config.get<std::string>("IOSelectorSrvConfig.<xmlattr>.name", m_io_selector_srv_config);
}

//------------------------------------------------------------------------------

void export_with_series_set::updating()
{
    sight::ui::lock_action lock(this->get_sptr());

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
    const auto io_cfg = service::extension::config::get_default()->get_service_config(
        m_io_selector_srv_config,
        "sight::module::ui::io::selector"
    );
    SIGHT_ASSERT(
        "There is no service configuration "
        << m_io_selector_srv_config
        << " for module::ui::editor::selector",
        !io_cfg.empty()
    );

    // Init and execute the service
    service::base::sptr io_selector_srv;
    io_selector_srv = service::add("sight::module::ui::io::selector");
    io_selector_srv->set_inout(local_series_set, io::service::DATA_KEY);

    io_selector_srv->set_worker(this->worker());

    const auto monitor_signal =
        this->signal<has_monitors::signals::monitor_created_t>(has_monitors::signals::MONITOR_CREATED);
    if(monitor_signal)
    {
        monitor_signal->connect(m_slot_forward_monitor);
    }

    io_selector_srv->set_config(io_cfg);
    io_selector_srv->configure();
    io_selector_srv->start();
    io_selector_srv->update();
    io_selector_srv->stop();
    service::remove(io_selector_srv);
}

//------------------------------------------------------------------------------

void export_with_series_set::starting()
{
    this->sight::ui::action::action_service_starting();

    const auto series = m_series.lock();
    SIGHT_FATAL_IF("The associated object must be a data::series.", !series);
}

//------------------------------------------------------------------------------

void export_with_series_set::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

void export_with_series_set::forward_monitor(core::progress::monitor::sptr _monitor)
{
    const auto monitor_signal =
        this->signal<has_monitors::signals::monitor_created_t>(has_monitors::signals::MONITOR_CREATED);
    monitor_signal->emit(_monitor);
}

} // namespace sight::module::ui::series
