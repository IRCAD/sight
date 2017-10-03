/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO16MULTITHREADCONSOLE_SINCREMENTARRAY_HPP__
#define __TUTO16MULTITHREADCONSOLE_SINCREMENTARRAY_HPP__

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwServices/IController.hpp>

namespace fwThread
{
class Timer;
}

namespace Tuto16MultithreadConsole
{

/**
 * @brief   This service increments all the values of a ::fwData::Array.
 *
 * @section Slots Slots
 * - \b startTimer(): start the internal timer and thus the periodic increment of the array values.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::Tuto16MultithreadConsole::SIncrementArray">
            <inout key="array" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b array [::fwData::Array]: array which values should be incremented.
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API SIncrementArray : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SIncrementArray)(::fwServices::IController) );

    TUTO16MULTITHREADCONSOLE_API SIncrementArray() noexcept;
    TUTO16MULTITHREADCONSOLE_API virtual ~SIncrementArray() noexcept;

protected:

    /// Initialize the timer
    virtual void starting() override;

    /// Stop and reset the timer
    virtual void stopping() override;

    /// Increment all the values of the current array
    virtual void updating() override;

    /// Do nothing
    virtual void configuring() override;

private:
    /// Slot: starts the timer
    void startTimer();

    SPTR( fwThread::Timer ) m_timer;

    unsigned int m_periodInMillisec;
};

}  // namespace Tuto16MultithreadConsole

#endif  // __TUTO16MULTITHREADCONSOLE_SINCREMENTARRAY_HPP__

