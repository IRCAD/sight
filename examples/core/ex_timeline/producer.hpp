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

#pragma once

#include "ex_timeline/config.hpp"

#include "message_tl.hpp"

#include <core/thread/timer.hpp>

#include <service/base.hpp>

/**
 * Do not mark `ex_timeline` as incorrect.
 * cspell:ignore ex_timeline
 */

namespace ex_timeline
{

/**
 * @brief   Service that produces messages and stores them into a timeline.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="ex_timeline::producer" >
            <inout key="timeline" uid="..." />
           <message>Hello</message>
           <id>0</id>
           <period>1000</period>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b timeline [ex_timeline::message_tl]: timeline where to put messages.
 * @subsection Configuration Configuration:
 * - \b message : message to be sent.
 * - \b id : id of the sender.
 * - \b period : time between two messages, in milliseconds.
 * - \b timelineSize (optional): maximum number of messages inside the timeline.
 */
class EX_TIMELINE_CLASS_API producer : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(producer, sight::service::base);

    EX_TIMELINE_API producer() noexcept           = default;
    EX_TIMELINE_API ~producer() noexcept override = default;

protected:

    /// Configure the service
    void configuring() final;

    /// Starts the timer.
    void starting() final;

    /// Stops the timer.
    void stopping() final;

    /// Called by the timer to send messages periodically
    void updating() final;

private:

    /// Timer used to send messages periodically
    SPTR(sight::core::thread::timer) m_timer;

    /// Message to be sent
    std::string m_message;

    /// Id of the sender
    unsigned int m_sender_id {0};

    /// A message will be sent every m_uiPeriod milliseconds.
    unsigned int m_period {0};

    /// Current message count
    unsigned int m_msg_count {0};

    /// Timeline size
    unsigned int m_timeline_size {0};

    sight::data::ptr<ex_timeline::message_tl, sight::data::access::inout> m_timeline {this, "timeline"};
};

} // namespace ex_timeline
