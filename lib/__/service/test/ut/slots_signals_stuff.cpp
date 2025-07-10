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

#include "slots_signals_stuff.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/registry/macros.hpp>

#include <service/macros.hpp>

#include <thread>

namespace sight::service::ut
{

SIGHT_REGISTER_DATA(buffer);

SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::s_basic_test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::s_basic_test, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::reader_test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::reader_test, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::s_show_test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::s_show_test, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::reader2_test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::reader2_test, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::s_show2_test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::s_show2_test, service::ut::Buffer);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

s_basic_test::s_basic_test()
= default;

//------------------------------------------------------------------------------

void s_basic_test::starting()
{
    std::this_thread::sleep_for(m_start_retarder);
}

//------------------------------------------------------------------------------

void s_basic_test::stopping()
{
    std::this_thread::sleep_for(m_stop_retarder);
}

//------------------------------------------------------------------------------

void s_basic_test::swapping(std::string_view /*unused*/)
{
    std::this_thread::sleep_for(m_swap_retarder);
    m_swap_finished = true;
}

//------------------------------------------------------------------------------

void s_basic_test::updating()
{
    std::this_thread::sleep_for(m_update_retarder);
    m_update_finished = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void reader_test::updating()
{
    auto buff = m_buffer.lock();
    buff->async_emit(this, data::object::MODIFIED_SIG);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::slots::key_t s_show_test::CHANGE_SLOT = "change";

//------------------------------------------------------------------------------

s_show_test::s_show_test()
{
    m_slot_change = core::com::new_slot(&s_show_test::change, this);
    core::com::has_slots::m_slots(CHANGE_SLOT, m_slot_change);
}

//------------------------------------------------------------------------------

void s_show_test::updating()
{
    std::this_thread::sleep_for(m_receive_retarder);
    const auto buffer = m_buffer.lock();
    ++m_receive_count;
}

//------------------------------------------------------------------------------

void s_show_test::change()
{
    core::mt::scoped_lock lock(m_mutex);
    ++m_change_count;
}

//------------------------------------------------------------------------------

service::connections_t s_show_test::auto_connections() const
{
    return {{BUFFER_INOUT, data::object::MODIFIED_SIG, slots::UPDATE}};
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::signals::key_t reader2_test::CHANGED_SIG = "changed";

//------------------------------------------------------------------------------

reader2_test::reader2_test() :
    m_sig_changed(std::make_shared<changed_signal_t>())
{
    // Register
    core::com::has_signals::m_signals(CHANGED_SIG, m_sig_changed);
}

//------------------------------------------------------------------------------

void reader2_test::updating()
{
    // Emit object Modified
    reader2_test::changed_signal_t::sptr sig;
    sig = this->signal<reader2_test::changed_signal_t>(reader2_test::CHANGED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::slots::key_t s_show2_test::UPDATE_BUFFER_SLOT = "updateBuffer";

//------------------------------------------------------------------------------

s_show2_test::s_show2_test()
{
    new_slot(UPDATE_BUFFER_SLOT, &s_show2_test::update_buffer, this);
}

//------------------------------------------------------------------------------

void s_show2_test::updating()
{
    const auto buffer = m_buffer.lock();
    buffer->async_emit(this, data::object::MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void s_show2_test::update_buffer()
{
    {
        std::this_thread::sleep_for(m_receive_retarder);
        const auto buffer = m_buffer.lock();
        ++m_receive_count;
    }

    this->updating();
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
