/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwCore/BaseObject.hpp"
#include "fwCore/config.hpp"
#include "fwCore/HiResClock.hpp"

namespace fwCore
{

/**
 * @brief This class provide a timer (stopwatch).
 * HiResTimer is able to measure the elapsed time with a few micro-seconds.
 * accuracy, on Linux, Mac OS and Windows
 */
class FWCORE_CLASS_API HiResTimer : public BaseObject
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (HiResTimer)(BaseObject), (()), new HiResTimer);

    /**
     * @name Constructor/Destructor
     * @{ */

    FWCORE_API HiResTimer();
    FWCORE_API virtual ~HiResTimer();

    /**  @} */

    /**
     * @brief Start the timer.
     */
    FWCORE_API void   start();

    /**
     * @brief Stop the timer.
     * stop() will not reset the timer.
     */
    FWCORE_API void   stop();

    /**
     * @brief Reset the timer and leave it in the same state it was (started or stopped).
     *
     * @param initial_value Initial value in microseconds from which the timer
     * will start to count.
     */
    FWCORE_API void   reset(::fwCore::HiResClock::HiResClockType initial_value = 0.);

    /**
     * @name Elapsed time
     * @brief These methods return the cumulated elapsed running time of the timer
     * the timer is running when start() has been invoked and stop running on
     * stop() or reset() invokation.
     * @{ */

    /**
     * @return Elapsed time in seconds
     */
    FWCORE_API ::fwCore::HiResClock::HiResClockType getElapsedTimeInSec();

    /**
     * @return Elapsed time in milliseconds
     */
    FWCORE_API ::fwCore::HiResClock::HiResClockType getElapsedTimeInMilliSec();

    /**
     * @return Elapsed time in microseconds
     */
    FWCORE_API ::fwCore::HiResClock::HiResClockType getElapsedTimeInMicroSec();

    /**  @} */
protected:

private:
    /**
     * @brief Last start time.
     */
    ::fwCore::HiResClock::HiResClockType m_startTimeInMicroSec;
    /**
     * @brief Last "break" time.
     */
    ::fwCore::HiResClock::HiResClockType m_endTimeInMicroSec;
    /**
     * @brief cummulated running time.
     */
    ::fwCore::HiResClock::HiResClockType m_cumulTimeInMicroSec;
    /**
     * @brief timer status.
     */
    bool m_stopped;
};

} //namespace fwCore
