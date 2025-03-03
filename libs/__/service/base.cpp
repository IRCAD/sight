/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "service/base.hpp"

#include "service/detail/service.hpp"
#include "service/manager.hpp"
#include "service/registry.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>

#include <functional>
#include <regex>

namespace sight::service
{

connections_t::connections_t(
    std::initializer_list<std::tuple<const std::string_view, core::com::signals::key_t, core::com::slots::key_t> > _init
)
{
    for(const auto& [key, sig, slot] : _init)
    {
        m_key_connections_map[key].emplace_back(sig, slot);
    }
}

//------------------------------------------------------------------------------

void connections_t::push(
    std::string_view _key,
    const core::com::signals::key_t& _sig,
    const core::com::slots::key_t& _slot
)
{
    m_key_connections_map[_key].emplace_back(_sig, _slot);
}

//------------------------------------------------------------------------------

bool connections_t::contains(std::string_view _key) const
{
    return m_key_connections_map.contains(_key);
}

//------------------------------------------------------------------------------

connections_t::key_connections_map_t::const_iterator connections_t::find(std::string_view _key) const
{
    return m_key_connections_map.find(_key);
}

//------------------------------------------------------------------------------

connections_t::key_connections_map_t::const_iterator connections_t::end() const
{
    return m_key_connections_map.cend();
}

//------------------------------------------------------------------------------

bool connections_t::empty() const
{
    return m_key_connections_map.empty();
}

//------------------------------------------------------------------------------

std::size_t connections_t::size() const
{
    return m_key_connections_map.size();
}

//------------------------------------------------------------------------------

connections_t connections_t::operator+(const connections_t& _other) const
{
    connections_t result;
    result.m_key_connections_map.insert(m_key_connections_map.begin(), m_key_connections_map.end());
    result.m_key_connections_map.insert(_other.m_key_connections_map.begin(), _other.m_key_connections_map.end());
    return result;
}

//-----------------------------------------------------------------------------

base::base() :
    m_pimpl(std::make_unique<detail::service>(*this))
{
    new_signal<signals::started_t>(signals::STARTED);
    new_signal<signals::updated_t>(signals::UPDATED);
    new_signal<signals::swapped_t>(signals::SWAPPED);
    new_signal<signals::stopped_t>(signals::STOPPED);

    new_slot(slots::START, [this](){m_pimpl->start(true);});
    new_slot(slots::STOP, [this](){m_pimpl->stop(true);});
    new_slot(slots::UPDATE, [this](){m_pimpl->update(true);});
    new_slot(
        slots::SWAP_KEY,
        [this](std::string_view _key, data::object::sptr _obj)
        {
            m_pimpl->swap_key(_key, _obj, true);
        });
}

//-----------------------------------------------------------------------------

base::~base() = default;

//-----------------------------------------------------------------------------

void base::set_worker(core::thread::worker::sptr _worker)
{
    m_pimpl->m_worker = _worker;
    core::com::has_slots::m_slots.set_worker(m_pimpl->m_worker);
}

//-----------------------------------------------------------------------------

core::thread::worker::sptr base::worker() const
{
    return m_pimpl->m_worker;
}

//-----------------------------------------------------------------------------

void base::set_config(const config_t& _configuration)
{
    m_pimpl->set_config(_configuration);
}

//-----------------------------------------------------------------------------

const base::config_t& base::get_config() const
{
    return m_pimpl->get_config();
}

//-----------------------------------------------------------------------------

void base::configure()
{
    m_pimpl->configure();
}

//-----------------------------------------------------------------------------

void base::configure(const config_t& _service_config)
{
    this->set_config(_service_config);
    this->configure();
}

//-----------------------------------------------------------------------------

base::shared_future_t base::start()
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->start(false);
    }

    return slot(slots::START)->async_run();
}

//-----------------------------------------------------------------------------

base::shared_future_t base::stop()
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->stop(false);
    }

    return slot(slots::STOP)->async_run();
}

//-----------------------------------------------------------------------------

base::shared_future_t base::update()
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->update(false);
    }

    return slot(slots::UPDATE)->async_run();
}

//-----------------------------------------------------------------------------

base::shared_future_t base::swap_key(std::string_view _key, data::object::sptr _obj)
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->swap_key(_key, _obj, false);
    }

    return slot(slots::SWAP_KEY)->async_run(_key, _obj);
}

//-----------------------------------------------------------------------------

base::global_status base::status() const noexcept
{
    return m_pimpl->m_global_state;
}

//-----------------------------------------------------------------------------

base::configuration_status base::config_status() const noexcept
{
    return m_pimpl->m_configuration_state;
}

//-----------------------------------------------------------------------------

bool base::started() const noexcept
{
    return m_pimpl->m_global_state == global_status::started;
}

//-----------------------------------------------------------------------------

bool base::stopped() const noexcept
{
    return m_pimpl->m_global_state == global_status::stopped;
}

//-----------------------------------------------------------------------------

enum base::updating_status base::updating_status() const noexcept
{
    return m_pimpl->m_updating_state;
}

//------------------------------------------------------------------------------

bool base::is_auto_connected() const
{
    return m_pimpl->is_auto_connected();
}

//-----------------------------------------------------------------------------

connections_t base::auto_connections() const
{
    connections_t connections;
    return connections;
}

//------------------------------------------------------------------------------

void base::on_property_set(std::string_view /*unused*/)
{
}

//-----------------------------------------------------------------------------

void base::info(std::ostream& /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::notify_register_out(data::object::sptr _obj, const std::string& _id)
{
    manager::notify_register_out(_obj, _id);
}

//------------------------------------------------------------------------------

void base::notify_unregister_out(data::object::sptr _obj, const std::string& _id)
{
    manager::notify_unregister_out(_obj, _id);
}

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see base::operator<<(std::ostream & _ostream, base& _service)
 * @note Invoke base::info( std::ostream )
 */
std::ostream& operator<<(std::ostream& _ostream, base& _service)
{
    _service.info(_ostream);
    return _ostream;
}

//-----------------------------------------------------------------------------

} // namespace sight::service
