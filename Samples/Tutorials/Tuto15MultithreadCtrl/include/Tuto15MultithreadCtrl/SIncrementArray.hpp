/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO15MULTITHREADCTRL_SINCREMENTARRAY_HPP__
#define __TUTO15MULTITHREADCTRL_SINCREMENTARRAY_HPP__

#include "Tuto15MultithreadCtrl/config.hpp"

#include <fwServices/IController.hpp>

namespace fwThread
{
class Timer;
}

namespace Tuto15MultithreadCtrl
{

/**
 * @brief This service increments all the values of a ::fwData::Array.
 *
 * This service provides a slot "startTimer" to call periodically the method "updating" that increment the values.
 */
class TUTO15MULTITHREADCTRL_CLASS_API SIncrementArray : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SIncrementArray)(::fwServices::IController) );

    TUTO15MULTITHREADCTRL_API SIncrementArray() throw();
    TUTO15MULTITHREADCTRL_API virtual ~SIncrementArray() throw();

    /// Set the timer period (in milliseconds)
    void setPeriod( unsigned int period )
    {
        m_periodInMillisec = period;
    }

protected:

    /// Initialize the timer
    virtual void starting() throw(::fwTools::Failed);

    /// Stop and reset the timer
    virtual void stopping() throw(::fwTools::Failed);

    /// Increment all the values of the current array
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void configuring() throw(fwTools::Failed);

    /// Slot: starts the timer
    void startTimer();

    SPTR( fwThread::Timer ) m_timer;

    unsigned int m_periodInMillisec;

};

}  // namespace Tuto15MultithreadCtrl

#endif  // __TUTO15MULTITHREADCTRL_SINCREMENTARRAY_HPP__

