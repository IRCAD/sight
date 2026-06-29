/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "db_merger.hpp"

#include <core/progress/monitor.hpp>

#include <data/series_set.hpp>

#include <io/__/service/io_types.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

namespace sight::module::ui::series
{

//------------------------------------------------------------------------------

db_merger::db_merger() noexcept :
    has_monitors(has_signals::signals()),
    m_io_selector_srv_config("IOSelectorServiceConfigVRRenderReader"),
    m_slot_forward_monitor(new_slot(slots::FORWARD_MONITOR, &db_merger::forward_monitor, this))
{
}

//------------------------------------------------------------------------------

void db_merger::info(std::ostream& _sstream)
{
    _sstream << "action for add series_set" << std::endl;
}

//------------------------------------------------------------------------------

void db_merger::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->get_config();
    m_io_selector_srv_config = config.get<std::string>("IOSelectorSrvConfig.<xmlattr>.name", m_io_selector_srv_config);
}

//------------------------------------------------------------------------------

void db_merger::updating()
{
    sight::ui::lock_action lock(this->get_sptr());

    // Create a new series_set
    auto local_series_set = std::make_shared<data::series_set>();

    /// Create IOSelectorService on the new series_set and execute it.

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

    auto monitor_created_signal_t = io_selector_srv->signal("monitor_created");
    if(monitor_created_signal_t)
    {
        monitor_created_signal_t->connect(m_slot_forward_monitor);
    }

    io_selector_srv->set_config(io_cfg);
    io_selector_srv->configure();
    io_selector_srv->start();
    io_selector_srv->update();
    io_selector_srv->stop();
    service::remove(io_selector_srv);

    // Lock only when needed.
    auto series_set = m_series_set.lock();
    SIGHT_ASSERT("The inout key '" << SERIES_SET << "' is not correctly set.", series_set);

    const auto scoped_emitter = series_set->scoped_emit();
    std::copy(local_series_set->cbegin(), local_series_set->cend(), data::inserter(*series_set));
}

//------------------------------------------------------------------------------

void db_merger::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void db_merger::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

void db_merger::forward_monitor(core::progress::monitor::sptr _monitor)
{
    this->async_emit(has_monitors::signals::MONITOR_CREATED, _monitor->get_sptr());
}

//------------------------------------------------------------------------------
//

} // namespace sight::module::ui::series
