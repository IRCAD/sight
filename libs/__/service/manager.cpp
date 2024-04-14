/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "service/manager.hpp"

#include "service/detail/service.hpp"
#include "service/detail/service_connection.hpp"

#include <core/com/signal.hxx>

namespace sight::service
{

using register_t = core::com::signal<void (data::object::sptr, const std::string&)>;

static std::mutex s_signal_mutex;
/// Signal used to notify that a new output object is available
register_t::sptr s_register_signal = std::make_shared<register_t>();
/// Signal used to notify that an output object is no longer available
register_t::sptr s_unregister_signal = std::make_shared<register_t>();

//------------------------------------------------------------------------------

void manager::set_object(
    service::base::sptr& _srv,
    data::object::sptr _obj,
    std::string_view _key,
    std::optional<std::size_t> _index,
    data::access _access,
    const bool _auto_connect,
    const bool _optional
)
{
    _srv->set_object(_obj, _key, _index, _access, _auto_connect, _optional);
}

//------------------------------------------------------------------------------

void manager::reset_object(service::base::sptr& _srv, std::string_view _key, std::optional<std::size_t> _index)
{
    _srv->reset_object(_key, _index);
}

//------------------------------------------------------------------------------

void manager::set_deferred_id(
    service::base::sptr& _srv,
    std::string_view _key,
    const std::string& _id,
    std::optional<std::size_t> _index
)
{
    _srv->set_deferred_id(_key, _id, _index);
}

//------------------------------------------------------------------------------

void manager::add_connection(service::base::sptr& _srv, const core::com::helper::proxy_connections& _proxy)
{
    _srv->m_pimpl->m_connections.add(_proxy);
}

//------------------------------------------------------------------------------

void manager::auto_connect(service::base::sptr& _srv)
{
    _srv->m_pimpl->auto_connect();
}

//------------------------------------------------------------------------------

void manager::auto_disconnect(service::base::sptr& _srv)
{
    _srv->m_pimpl->auto_disconnect();
}

//------------------------------------------------------------------------------

core::com::connection manager::connect_register_out(const core::com::slot_base::sptr& _slot)
{
    std::lock_guard<std::mutex> lock(s_signal_mutex);
    return s_register_signal->connect(_slot);
}

//------------------------------------------------------------------------------

core::com::connection manager::connect_unregister_out(const core::com::slot_base::sptr& _slot)
{
    std::lock_guard<std::mutex> lock(s_signal_mutex);
    return s_unregister_signal->connect(_slot);
}

//------------------------------------------------------------------------------

void manager::notify_register_out(data::object::sptr _obj, const std::string& _id)
{
    std::lock_guard<std::mutex> lock(s_signal_mutex);
    s_register_signal->async_emit(_obj, _id);
}

//------------------------------------------------------------------------------

void manager::notify_unregister_out(data::object::sptr _obj, const std::string& _id)
{
    std::lock_guard<std::mutex> lock(s_signal_mutex);
    s_unregister_signal->async_emit(_obj, _id);
}

//------------------------------------------------------------------------------

std::pair<bool, bool> manager::get_object_key_attrs(const service::base::sptr& _srv, const std::string& _key)
{
    return _srv->m_pimpl->get_object_key_attrs(_key);
}

} // namespace sight::service
