/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "Tuto05MultithreadConsoleCpp/config.hpp"

#include <services/IController.hpp>

namespace thread
{
class Timer;
}

namespace Tuto05MultithreadConsoleCpp
{

/**
 * @brief This service increments all the values of a data::Array.
 *
 * @section Slots Slots
 * - \b startTimer(): starts the internal timer and thus the periodic increment of the array values.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::Tuto05MultithreadConsoleCpp::SIncrementArray" >
            <inout key="array" uid="..." />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b array [data::Array]: array which values should be incremented.
 */
class TUTO05MULTITHREADCONSOLECPP_CLASS_API SIncrementArray final : public services::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SIncrementArray, services::IController)

    /// Initializes the slot.
    TUTO05MULTITHREADCONSOLECPP_API SIncrementArray() noexcept;

    /// Destroys the service.
    TUTO05MULTITHREADCONSOLECPP_API virtual ~SIncrementArray() noexcept;

protected:

    /// Does nothing.
    TUTO05MULTITHREADCONSOLECPP_API void configuring() override;

    /// Creates and starts the timer.
    TUTO05MULTITHREADCONSOLECPP_API void starting() override;

    /// Increments all the values of the current array.
    TUTO05MULTITHREADCONSOLECPP_API void updating() override;

    /// Stops and reset the timer.
    TUTO05MULTITHREADCONSOLECPP_API void stopping() override;

private:

    /// SLOT: starts the timer.
    void startTimer();

    /// Contains the timer.
    SPTR(core::thread::Timer) m_timer;

    /// Defines the periode of the timer.
    unsigned int m_periodInMillisec;
};

}  // namespace Tuto05MultithreadConsoleCpp.
