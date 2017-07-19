/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EX02TIMELINE_SCONSUMER_HPP__
#define __EX02TIMELINE_SCONSUMER_HPP__

#include <fwServices/IService.hpp>

#include "Ex02TimeLine/config.hpp"

namespace fwThread
{
class Timer;
}

namespace Ex02TimeLine
{

/**
 * @brief   Service that consumes messages stored in a timeline. You can either connect the service slot to the timeline
 *          to display a message each time something is pushed, or you can specify a period in the configuration to
 *          display a message periodically.
 */
class EX02TIMELINE_CLASS_API SConsumer : public ::fwServices::IService
{
public:

    EX02TIMELINE_API static const ::fwCom::Slots::SlotKeyType s_CONSUME_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> ConsumeSlotType;

    fwCoreServiceClassDefinitionsMacro ( (SConsumer)(::fwServices::IService) );

    EX02TIMELINE_API SConsumer() noexcept;
    EX02TIMELINE_API virtual ~SConsumer() noexcept;

protected:

    /// Starts the timer if a period is defined.
    virtual void starting();

    /// Stops the timer.
    virtual void stopping();

    /// Does nothing.
    virtual void swapping();

    /// Called by the timer to consume a message periodically
    virtual void updating();

    /**
     * @brief Configures the service.
     *
     * @code{.xml}
       <service type="::fwServices::IService" impl="::Ex02TimeLine::SConsumer" autoConnect="yes">
           <id>0</id>
           <period>1000</period>
       </service>
       @endcode
     * - \b id : id of the receiver.
     * - \b period (optional): time between two messages display, in milliseconds.
     */
    virtual void configuring();

    /// Called by a signal to consume a message
    void consume(::fwCore::HiResClock::HiResClockType timestamp);

private:

    /// Timer used to read messages periodically
    SPTR( fwThread::Timer ) m_timer;

    /// Slots used when the frame have been refreshed
    ConsumeSlotType::sptr m_slotConsume;

    /// Id of the receiver
    unsigned int m_receiverId;

    /// A message will be read every m_uiPeriod milliseconds.
    unsigned int m_period;
};

}  // namespace Ex02TimeLine

#endif  // __EX02TIMELINE_SCONSUMER_HPP__
