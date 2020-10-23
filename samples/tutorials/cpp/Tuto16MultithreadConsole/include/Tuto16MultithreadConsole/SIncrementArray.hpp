/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwServices/IController.hpp>

namespace fwThread
{
class Timer;
}

namespace Tuto16MultithreadConsole
{

/**
 * @brief This service increments all the values of a ::fwData::Array.
 *
 * @section Slots Slots
 * - \b startTimer(): starts the internal timer and thus the periodic increment of the array values.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::Tuto16MultithreadConsole::SIncrementArray" >
            <inout key="array" uid="..." />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b array [::fwData::Array]: array which values should be incremented.
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API SIncrementArray final : public ::fwServices::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SIncrementArray, ::fwServices::IController)

    /// Initializes the slot.
    TUTO16MULTITHREADCONSOLE_API SIncrementArray() noexcept;

    /// Destroys the service.
    TUTO16MULTITHREADCONSOLE_API virtual ~SIncrementArray() noexcept;

protected:

    /// Does nothing.
    TUTO16MULTITHREADCONSOLE_API void configuring() override;

    /// Creates and starts the timer.
    TUTO16MULTITHREADCONSOLE_API void starting() override;

    /// Increments all the values of the current array.
    TUTO16MULTITHREADCONSOLE_API void updating() override;

    /// Stops and reset the timer.
    TUTO16MULTITHREADCONSOLE_API void stopping() override;

private:

    /// SLOT: starts the timer.
    void startTimer();

    /// Contains the timer.
    SPTR(fwThread::Timer) m_timer;

    /// Defines the periode of the timer.
    unsigned int m_periodInMillisec;
};

}  // namespace Tuto16MultithreadConsole.
