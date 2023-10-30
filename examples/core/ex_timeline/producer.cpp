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

#include "producer.hpp"

#include <core/com/signal.hxx>
#include <core/thread/timer.hpp>

namespace ex_timeline
{

//------------------------------------------------------------------------------

void producer::configuring()
{
    sight::service::config_t config = this->get_config();

    m_message       = config.get<std::string>("message");
    m_sender_id     = config.get<unsigned int>("id");
    m_period        = config.get<unsigned int>("period", 0);
    m_timeline_size = config.get<unsigned int>("timelineSize", 0);
}

//------------------------------------------------------------------------------

void producer::starting()
{
    m_timer = this->worker()->create_timer();
    m_timer->set_function([this](auto&& ...){updating();});
    m_timer->set_duration(std::chrono::milliseconds(m_period));

    m_timer->start();

    // Init timeline pool
    if(m_timeline_size != 0U)
    {
        const auto timeline = m_timeline.lock();

        // This wouldn't hurt to initialize the timeline several times since it will be erased each time
        // but this would be a mess to know who is the last to initialize
        SIGHT_ASSERT("Timeline initialized twice", timeline->get_max_element_num() == ~0U);

        timeline->set_maximum_size(m_timeline_size);

        // We only have one element per buffer in this simple example
        timeline->init_pool_size(1);
    }
}

//------------------------------------------------------------------------------

void producer::stopping()
{
    m_timer.reset();
}

//------------------------------------------------------------------------------

void producer::updating()
{
    const auto timeline = m_timeline.lock();

    const auto timestamp = sight::core::hires_clock::get_time_in_milli_sec();
    SPTR(::ex_timeline::message_tl::buffer_t) buffer = timeline->create_buffer(timestamp);

    ::ex_timeline::msg_data* data = buffer->add_element(0);

    const std::string message = m_message + " #" + std::to_string(m_msg_count++);

    data->uid_sender = m_sender_id;
    std::copy(message.begin(), message.end(), data->sz_msg.begin());

    timeline->push_object(buffer);

    //Notify
    sight::data::timeline::signals::pushed_t::sptr sig;
    sig =
        timeline->signal<sight::data::timeline::signals::pushed_t>(sight::data::timeline::signals::PUSHED);
    sig->async_emit(timestamp);
}

//------------------------------------------------------------------------------

} // namespace ex_timeline
