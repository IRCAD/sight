/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EX02TIMELINE_SPRODUCER_HPP__
#define __EX02TIMELINE_SPRODUCER_HPP__

#include "Ex02TimeLine/config.hpp"

#include <fwServices/IService.hpp>

#include <fwThread/Timer.hpp>

namespace Ex02TimeLine
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
class EX02TIMELINE_CLASS_API SProducer : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro( (SProducer)(::fwServices::IService) );

    EX02TIMELINE_API SProducer() noexcept;
    EX02TIMELINE_API virtual ~SProducer() noexcept;

protected:

    /// Configure the service
    virtual void configuring() final;

    /// Starts the timer.
    virtual void starting() final;

    /// Stops the timer.
    virtual void stopping() final;

    /// Called by the timer to send messages periodically
    virtual void updating() final;

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

}  // namespace Ex02TimeLine

#endif  // __EX02TIMELINE_SPRODUCER_HPP__
