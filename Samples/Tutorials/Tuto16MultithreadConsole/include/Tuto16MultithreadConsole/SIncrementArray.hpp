/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

    fwCoreServiceMacro(SIncrementArray, ::fwServices::IController);

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
