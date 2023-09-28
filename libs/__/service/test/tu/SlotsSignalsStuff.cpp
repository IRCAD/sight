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

#include "SlotsSignalsStuff.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/registry/macros.hpp>

#include <service/macros.hpp>

#include <thread>

namespace sight::service::ut
{

SIGHT_REGISTER_DATA(Buffer);

SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::SBasicTest);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::SBasicTest, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::SReaderTest);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::SReaderTest, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::SShowTest);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::SShowTest, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::SReader2Test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::SReader2Test, service::ut::Buffer);
SIGHT_REGISTER_SERVICE(sight::service::ut::basic_srv, sight::service::ut::SShow2Test);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::SShow2Test, service::ut::Buffer);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SBasicTest::SBasicTest()
= default;

//------------------------------------------------------------------------------

void SBasicTest::starting()
{
    std::this_thread::sleep_for(m_startRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::stopping()
{
    std::this_thread::sleep_for(m_stopRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::swapping(std::string_view /*unused*/)
{
    std::this_thread::sleep_for(m_swapRetarder);
    m_swapFinished = true;
}

//------------------------------------------------------------------------------

void SBasicTest::updating()
{
    std::this_thread::sleep_for(m_updateRetarder);
    m_updateFinished = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void SReaderTest::updating()
{
    auto buff = m_buffer.lock();

    // Emit object Modified
    data::Object::ModifiedSignalType::sptr sig;
    sig = buff->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);

    {
        core::com::connection::blocker block(sig->get_connection(this->slot(service::signals::UPDATED)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::slots::key_t SShowTest::CHANGE_SLOT = "change";

//------------------------------------------------------------------------------

SShowTest::SShowTest()
{
    m_slotChange = core::com::new_slot(&SShowTest::change, this);
    core::com::has_slots::m_slots(CHANGE_SLOT, m_slotChange);
}

//------------------------------------------------------------------------------

void SShowTest::updating()
{
    std::this_thread::sleep_for(m_receiveRetarder);
    const auto buffer = m_buffer.lock();
    ++m_receiveCount;
}

//------------------------------------------------------------------------------

void SShowTest::change()
{
    core::mt::scoped_lock lock(m_mutex);
    ++m_changeCount;
}

//------------------------------------------------------------------------------

service::connections_t SShowTest::getAutoConnections() const
{
    return {{s_BUFFER_INOUT, data::Object::MODIFIED_SIG, slots::UPDATE}};
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::signals::key_t SReader2Test::CHANGED_SIG = "changed";

//------------------------------------------------------------------------------

SReader2Test::SReader2Test() :
    m_sigChanged(std::make_shared<ChangedSignalType>())
{
    // Register
    core::com::has_signals::m_signals(CHANGED_SIG, m_sigChanged);
}

//------------------------------------------------------------------------------

void SReader2Test::updating()
{
    // Emit object Modified
    SReader2Test::ChangedSignalType::sptr sig;
    sig = this->signal<SReader2Test::ChangedSignalType>(SReader2Test::CHANGED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::slots::key_t SShow2Test::UPDATE_BUFFER_SLOT = "updateBuffer";

//------------------------------------------------------------------------------

SShow2Test::SShow2Test()
{
    new_slot(UPDATE_BUFFER_SLOT, &SShow2Test::updateBuffer, this);
}

//------------------------------------------------------------------------------

void SShow2Test::updating()
{
    const auto buffer = m_buffer.lock();

    // Emit object Modified
    data::Object::ModifiedSignalType::sptr sig;
    sig = buffer->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_BUFFER_SLOT)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void SShow2Test::updateBuffer()
{
    {
        std::this_thread::sleep_for(m_receiveRetarder);
        const auto buffer = m_buffer.lock();
        ++m_receiveCount;
    }

    this->updating();
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
