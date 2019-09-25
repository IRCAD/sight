/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <fwServices/IService.hpp>

#include <fwThread/Timer.hpp>

namespace ExTimeLine
{
/**
 * @brief   Service that produces messages and stores them into a timeline.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="::Ex02TimeLine::SProducer">
            <inout key="timeline" uid="..." />
           <message>Hello</message>
           <id>0</id>
           <period>1000</period>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b timeline [::Ex02TimeLine::MessageTL]: timeline where to put messages.
 * @subsection Configuration Configuration:
 * - \b message : message to be sent.
 * - \b id : id of the sender.
 * - \b period : time between two messages, in milliseconds.
 * - \b timelineSize (optional): maximum number of messages inside the timeline.
 */
class EXTIMELINE_CLASS_API SProducer : public ::fwServices::IService
{
public:

    fwCoreServiceMacro(SProducer, ::fwServices::IService);

    EXTIMELINE_API SProducer() noexcept;
    EXTIMELINE_API virtual ~SProducer() noexcept;

protected:

    /// Configure the service
    virtual void configuring() final override;

    /// Starts the timer.
    virtual void starting() final override;

    /// Stops the timer.
    virtual void stopping() final override;

    /// Called by the timer to send messages periodically
    virtual void updating() final override;

private:

    /// Timer used to send messages periodically
    SPTR( fwThread::Timer ) m_timer;

    /// Message to be sent
    std::string m_message;

    /// Id of the sender
    unsigned int m_senderId;

    /// A message will be sent every m_uiPeriod milliseconds.
    unsigned int m_period;

    /// Current message count
    unsigned int m_msgCount;

    /// Timeline size
    unsigned int m_timelineSize;
};

}  // namespace ExTimeLine
