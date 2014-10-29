/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __POC07TIMELINE_SCONSUMER_HPP__
#define __POC07TIMELINE_SCONSUMER_HPP__

#include <fwServices/IService.hpp>

#include "PoC07TimeLine/config.hpp"

fwCorePredeclare((fwThread)(Timer));

namespace PoC07TimeLine
{

/**
 * @class   SConsumer
 * @brief   Service that consumes messages stored in a timeline. You can either connect the service slot to the timeline
 *          to display a message each time something is pushed, or you can specify a period in the configuration to
 *          display a message periodically.
 */
class POC07TIMELINE_CLASS_API SConsumer : public ::fwServices::IService
{
public:

    POC07TIMELINE_API static const ::fwCom::Slots::SlotKeyType s_CONSUME_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> ConsumeSlotType;

    fwCoreServiceClassDefinitionsMacro ( (SConsumer)(::fwServices::IService) );

    POC07TIMELINE_API SConsumer() throw();
    POC07TIMELINE_API virtual ~SConsumer() throw();

protected:

    /// Starts the timer if a period is defined.
    virtual void starting() throw(::fwTools::Failed);

    /// Stops the timer.
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing.
    virtual void swapping() throw ( ::fwTools::Failed );

    /// Does nothing.
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Called by the timer to consume a message periodically
    virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief Configures the service.
     *
     * @verbatim
       <service type="::fwServices::IService" impl="::PoC07TimeLine::SConsumer" autoConnect="yes">
           <id>0</id>
           <period>1000</period>
       </service>
       @endverbatim
     * - \b id : id of the receiver.
     * - \b period (optional): time between two messages display, in milliseconds.
     */
    virtual void configuring() throw(fwTools::Failed);

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

}  // namespace PoC07TimeLine

#endif  // __POC07TIMELINE_SCONSUMER_HPP__

