/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "ExTimeLine/config.hpp"

#include "MessageTL.hpp"

#include <service/IService.hpp>

namespace thread
{

class Timer;

}

namespace ExTimeLine
{

/**
 * @brief   Service that consumes messages stored in a timeline. You can either connect the service slot to the timeline
 *          to display a message each time something is pushed, or you can specify a period in the configuration to
 *          display a message periodically.
 *
 * @section Slots Slots
 * - \b consume() : read a message from the timeline.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="Ex02TimeLine::SConsumer" >
            <in key="timeline" uid="..." />
            <id>0</id>
            <period>1000</period>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [::ExTimeLine::MessageTL]: timeline containing messages.
 * @subsection Configuration Configuration:
 * - \b id : id of the receiver.
 * - \b period (optional): time between two messages display, in milliseconds.
 */
class EXTIMELINE_CLASS_API SConsumer : public sight::service::IService
{
public:

    EXTIMELINE_API static const sight::core::com::Slots::SlotKeyType s_CONSUME_SLOT;

    SIGHT_DECLARE_SERVICE(SConsumer, sight::service::IService);

    EXTIMELINE_API SConsumer() noexcept;
    EXTIMELINE_API virtual ~SConsumer() noexcept;

protected:

    /// Configure the service
    void configuring() final;

    /// Start the timer if a period is defined.
    void starting() final;

    /// Stop the timer.
    void stopping() final;

    /// Called by the timer to consume a message periodically
    void updating() final;

    /// Called by a signal to consume a message
    void consume(sight::core::HiResClock::HiResClockType timestamp);

private:

    /// Timer used to read messages periodically
    SPTR(sight::core::thread::Timer) m_timer;

    /// Id of the receiver
    unsigned int m_receiverId;

    /// A message will be read every m_uiPeriod milliseconds.
    unsigned int m_period;

    sight::data::ptr<ExTimeLine::MessageTL, sight::data::Access::in> m_timeline {this, "timeline"};
};

} // namespace ExTimeLine
