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

#include "service/base.hpp"

#include "service/detail/Service.hpp"
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
    std::initializer_list<std::tuple<const std::string_view, core::com::signals::key_t, core::com::slots::key_t> > init
)
{
    for(const auto& [key, sig, slot] : init)
    {
        m_keyConnectionsMap[key].emplace_back(sig, slot);
    }
}

//------------------------------------------------------------------------------

void connections_t::push(
    std::string_view key,
    const core::com::signals::key_t& sig,
    const core::com::slots::key_t& slot
)
{
    m_keyConnectionsMap[key].emplace_back(sig, slot);
}

//------------------------------------------------------------------------------

connections_t::KeyConnectionsMapType::const_iterator connections_t::find(
    std::string_view key
) const
{
    return m_keyConnectionsMap.find(key);
}

//------------------------------------------------------------------------------

connections_t::KeyConnectionsMapType::const_iterator connections_t::end() const
{
    return m_keyConnectionsMap.cend();
}

//------------------------------------------------------------------------------

bool connections_t::empty() const
{
    return m_keyConnectionsMap.empty();
}

//------------------------------------------------------------------------------

std::size_t connections_t::size() const
{
    return m_keyConnectionsMap.size();
}

//-----------------------------------------------------------------------------

base::base() :
    m_pimpl(std::make_unique<detail::Service>(*this))
{
    new_signal<signals::started_t>(signals::STARTED);
    new_signal<signals::updated_t>(signals::UPDATED);
    new_signal<signals::swapped_t>(signals::SWAPPED);
    new_signal<signals::stopped_t>(signals::STOPPED);

    new_slot(slots::START, [this](){m_pimpl->start(true);});
    new_slot(slots::STOP, [this](){m_pimpl->stop(true);});
    new_slot(slots::UPDATE, [this](){m_pimpl->update(true);});
    new_slot(
        slots::SWAPKEY,
        [this](std::string_view _key, data::Object::sptr _obj)
        {
            m_pimpl->swapKey(_key, _obj, true);
        });
}

//-----------------------------------------------------------------------------

base::~base() = default;

//-----------------------------------------------------------------------------

void base::set_worker(core::thread::worker::sptr worker)
{
    m_pimpl->m_worker = worker;
    core::com::has_slots::m_slots.set_worker(m_pimpl->m_worker);
}

//-----------------------------------------------------------------------------

core::thread::worker::sptr base::worker() const
{
    return m_pimpl->m_worker;
}

//-----------------------------------------------------------------------------

void base::setConfiguration(const config_t& _configuration)
{
    m_pimpl->setConfiguration(_configuration);
}

//-----------------------------------------------------------------------------

const base::ConfigType& base::getConfiguration() const
{
    return m_pimpl->getConfiguration();
}

//-----------------------------------------------------------------------------

const base::ConfigType& base::getConfigTree() const
{
    return m_pimpl->getConfiguration();
}

//-----------------------------------------------------------------------------

void base::configure()
{
    m_pimpl->configure();
}

//-----------------------------------------------------------------------------

void base::configure(const ConfigType& serviceConfig)
{
    this->setConfiguration(serviceConfig);
    this->configure();
}

//-----------------------------------------------------------------------------

base::SharedFutureType base::start()
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->start(false);
    }

    return slot(slots::START)->async_run();
}

//-----------------------------------------------------------------------------

base::SharedFutureType base::stop()
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->stop(false);
    }

    return slot(slots::STOP)->async_run();
}

//-----------------------------------------------------------------------------

base::SharedFutureType base::update()
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->update(false);
    }

    return slot(slots::UPDATE)->async_run();
}

//-----------------------------------------------------------------------------

base::SharedFutureType base::swapKey(std::string_view _key, data::Object::sptr _obj)
{
    if(!m_pimpl->m_worker || core::thread::get_current_thread_id() == m_pimpl->m_worker->get_thread_id())
    {
        return m_pimpl->swapKey(_key, _obj, false);
    }

    return slot(slots::SWAPKEY)->async_run(_key, _obj);
}

//-----------------------------------------------------------------------------

base::GlobalStatus base::getStatus() const noexcept
{
    return m_pimpl->m_globalState;
}

//-----------------------------------------------------------------------------

base::ConfigurationStatus base::getConfigurationStatus() const noexcept
{
    return m_pimpl->m_configurationState;
}

//-----------------------------------------------------------------------------

bool base::isStarted() const noexcept
{
    return m_pimpl->m_globalState == STARTED;
}

//-----------------------------------------------------------------------------

bool base::isStopped() const noexcept
{
    return m_pimpl->m_globalState == STOPPED;
}

//-----------------------------------------------------------------------------

base::UpdatingStatus base::getUpdatingStatus() const noexcept
{
    return m_pimpl->m_updatingState;
}

//-----------------------------------------------------------------------------

connections_t base::getAutoConnections() const
{
    connections_t connections;
    return connections;
}

//-----------------------------------------------------------------------------

void base::info(std::ostream& /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::notifyRegisterOut(data::Object::sptr _obj, const std::string& _id)
{
    detail::Service::notifyRegisterOut(_obj, _id);
}

//------------------------------------------------------------------------------

void base::notifyUnregisterOut(data::Object::sptr _obj, const std::string& _id)
{
    detail::Service::notifyUnregisterOut(_obj, _id);
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
