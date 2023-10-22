/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "consumer.hpp"

#include <core/com/slots.hxx>
#include <core/thread/timer.hpp>

#include <service/macros.hpp>

#include <functional>

namespace ex_timeline
{

//------------------------------------------------------------------------------

const sight::core::com::slots::key_t consumer::CONSUME_SLOT = "consume";

//------------------------------------------------------------------------------

consumer::consumer() noexcept
{
    new_slot(CONSUME_SLOT, &consumer::consume, this);
}

//------------------------------------------------------------------------------

consumer::~consumer() noexcept =
    default;

//------------------------------------------------------------------------------

void consumer::configuring()
{
    sight::service::config_t config = this->get_config();

    m_receiverId = config.get<unsigned int>("id");
    m_period     = config.get<unsigned int>("period", 0);
}

//------------------------------------------------------------------------------

void consumer::starting()
{
    if(m_period != 0U)
    {
        m_timer = this->worker()->create_timer();
        m_timer->set_function([this](auto&& ...){updating();});
        m_timer->set_duration(std::chrono::milliseconds(m_period));
        m_timer->start();
    }
}

//------------------------------------------------------------------------------

void consumer::stopping()
{
    m_timer.reset();
}

//------------------------------------------------------------------------------

void consumer::updating()
{
    const auto timeline = m_timeline.lock();

    const auto timestamp = sight::core::hires_clock::get_time_in_milli_sec();
    const CSPTR(::ex_timeline::message_tl::buffer_t) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::ex_timeline::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (timer): CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender
        << " MESSAGE: \"" << element.szMsg.data() << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

void consumer::consume(sight::core::hires_clock::type _timestamp)
{
    const auto timeline = m_timeline.lock();

    const CSPTR(::ex_timeline::message_tl::buffer_t) buffer = timeline->getClosestBuffer(_timestamp);

    if(buffer)
    {
        const ::ex_timeline::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (slot) : CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender
        << " MESSAGE: \"" << element.szMsg.data() << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

} // namespace ex_timeline
