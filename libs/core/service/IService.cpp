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

#include "service/IService.hpp"

#include "service/detail/Service.hpp"
#include "service/registry.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/Worker.hpp>

#include <functional>
#include <regex>

namespace sight::service
{

IService::KeyConnectionsMap::KeyConnectionsMap(
    std::initializer_list<std::tuple<const std::string_view, core::com::Signals::key_t, core::com::Slots::key_t> > init
)
{
    for(const auto& [key, sig, slot] : init)
    {
        m_keyConnectionsMap[key].push_back(std::make_pair(sig, slot));
    }
}

//------------------------------------------------------------------------------

void IService::KeyConnectionsMap::push(
    std::string_view key,
    const core::com::Signals::key_t& sig,
    const core::com::Slots::key_t& slot
)
{
    m_keyConnectionsMap[key].push_back(std::make_pair(sig, slot));
}

//------------------------------------------------------------------------------

IService::KeyConnectionsMap::KeyConnectionsMapType::const_iterator IService::KeyConnectionsMap::find(
    std::string_view key
) const
{
    return m_keyConnectionsMap.find(key);
}

//------------------------------------------------------------------------------

IService::KeyConnectionsMap::KeyConnectionsMapType::const_iterator IService::KeyConnectionsMap::end() const
{
    return m_keyConnectionsMap.cend();
}

//------------------------------------------------------------------------------

bool IService::KeyConnectionsMap::empty() const
{
    return m_keyConnectionsMap.empty();
}

//------------------------------------------------------------------------------

std::size_t IService::KeyConnectionsMap::size() const
{
    return m_keyConnectionsMap.size();
}

//-----------------------------------------------------------------------------

IService::IService() :
    m_pimpl(std::make_unique<detail::Service>(*this))
{
    newSignal<signals::started_t>(signals::s_STARTED);
    newSignal<signals::updated_t>(signals::s_UPDATED);
    newSignal<signals::swapped_t>(signals::s_SWAPPED);
    newSignal<signals::stopped_t>(signals::s_STOPPED);

    newSlot(slots::s_START, [this](){m_pimpl->start(true);});
    newSlot(slots::s_STOP, [this](){m_pimpl->stop(true);});
    newSlot(slots::s_UPDATE, [this](){m_pimpl->update(true);});
    newSlot(
        slots::s_SWAPKEY,
        [this](std::string_view _key, data::Object::sptr _obj)
        {
            m_pimpl->swapKey(_key, _obj, true);
        });
}

//-----------------------------------------------------------------------------

IService::~IService() = default;

//-----------------------------------------------------------------------------

void IService::setWorker(core::thread::Worker::sptr worker)
{
    m_pimpl->m_worker = worker;
    core::com::HasSlots::m_slots.setWorker(m_pimpl->m_worker);
}

//-----------------------------------------------------------------------------

core::thread::Worker::sptr IService::worker() const
{
    return m_pimpl->m_worker;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const config_t& _configuration)
{
    m_pimpl->setConfiguration(_configuration);
}

//-----------------------------------------------------------------------------

const IService::ConfigType& IService::getConfiguration() const
{
    return m_pimpl->getConfiguration();
}

//-----------------------------------------------------------------------------

const IService::ConfigType& IService::getConfigTree() const
{
    return m_pimpl->getConfiguration();
}

//-----------------------------------------------------------------------------

void IService::configure()
{
    m_pimpl->configure();
}

//-----------------------------------------------------------------------------

void IService::configure(const ConfigType& serviceConfig)
{
    this->setConfiguration(serviceConfig);
    this->configure();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::start()
{
    if(!m_pimpl->m_worker || core::thread::getCurrentThreadId() == m_pimpl->m_worker->getThreadId())
    {
        return m_pimpl->start(false);
    }

    return slot(slots::s_START)->asyncRun();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stop()
{
    if(!m_pimpl->m_worker || core::thread::getCurrentThreadId() == m_pimpl->m_worker->getThreadId())
    {
        return m_pimpl->stop(false);
    }

    return slot(slots::s_STOP)->asyncRun();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::update()
{
    if(!m_pimpl->m_worker || core::thread::getCurrentThreadId() == m_pimpl->m_worker->getThreadId())
    {
        return m_pimpl->update(false);
    }

    return slot(slots::s_UPDATE)->asyncRun();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapKey(std::string_view _key, data::Object::sptr _obj)
{
    if(!m_pimpl->m_worker || core::thread::getCurrentThreadId() == m_pimpl->m_worker->getThreadId())
    {
        return m_pimpl->swapKey(_key, _obj, false);
    }

    return slot(slots::s_SWAPKEY)->asyncRun(_key, _obj);
}

//-----------------------------------------------------------------------------

IService::GlobalStatus IService::getStatus() const noexcept
{
    return m_pimpl->m_globalState;
}

//-----------------------------------------------------------------------------

IService::ConfigurationStatus IService::getConfigurationStatus() const noexcept
{
    return m_pimpl->m_configurationState;
}

//-----------------------------------------------------------------------------

bool IService::isStarted() const noexcept
{
    return m_pimpl->m_globalState == STARTED;
}

//-----------------------------------------------------------------------------

bool IService::isStopped() const noexcept
{
    return m_pimpl->m_globalState == STOPPED;
}

//-----------------------------------------------------------------------------

IService::UpdatingStatus IService::getUpdatingStatus() const noexcept
{
    return m_pimpl->m_updatingState;
}

//-----------------------------------------------------------------------------

IService::KeyConnectionsMap IService::getAutoConnections() const
{
    KeyConnectionsMap connections;
    return connections;
}

//-----------------------------------------------------------------------------

void IService::info(std::ostream& /*unused*/)
{
}

//------------------------------------------------------------------------------

void IService::notifyRegisterOut(data::Object::sptr _obj, const std::string& _id)
{
    detail::Service::notifyRegisterOut(_obj, _id);
}

//------------------------------------------------------------------------------

void IService::notifyUnregisterOut(data::Object::sptr _obj, const std::string& _id)
{
    detail::Service::notifyUnregisterOut(_obj, _id);
}

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream& operator<<(std::ostream& _ostream, IService& _service)
{
    _service.info(_ostream);
    return _ostream;
}

//-----------------------------------------------------------------------------

} // namespace sight::service
