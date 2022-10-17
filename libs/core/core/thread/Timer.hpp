/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/thread/Worker.hpp"

#include <core/mt/types.hpp>

#include <functional>

namespace sight::core::thread
{

/**
 * @brief The Timer class provides single-shot or repetitive timers.
 * A Timer triggers a function once after a delay, or periodically, inside the
 * worker loop. The delay or the period is defined by the duration attribute.
 *
 * Timer's function must be set before starting the timer.
 */
class CORE_CLASS_API Timer : public core::BaseObject
{
public:

    /**
     * @name Typedefs
     * @{ */
    typedef std::shared_ptr<Timer> sptr;
    typedef std::function<void ()> FunctionType;
    typedef std::chrono::duration<double> TimeDurationType;
    /**  @} */

    CORE_API ~Timer() override = default;

    /// Starts or restarts the timer.
    CORE_API virtual void start() = 0;

    /// Stops the timer and cancel all pending operations.
    CORE_API virtual void stop()
    {
    }

    /// Sets time duration.
    CORE_API virtual void setDuration(TimeDurationType duration) = 0;

    /// Sets the function to be triggered when time duration expires.
    template<typename F>
    void setFunction(F f)
    {
        core::mt::ScopedLock lock(m_mutex);
        m_function = f;
        updatedFunction();
    }

    /// Returns if the timer mode is 'one shot'.
    virtual bool isOneShot() const = 0;

    /// Sets timer mode.
    virtual void setOneShot(bool oneShot) = 0;

    /// Returns true if the timer is currently running.
    virtual bool isRunning() const = 0;

protected:

    /**
     * @brief Constructs a Timer.
     * By default, a the Timer mode is repetitive.
     */
    CORE_API Timer();

    /// Copy constructor forbidden.
    Timer(const Timer&);

    /// Copy operator forbidden.
    Timer& operator=(const Timer&);

    /// This method is triggered when Timer's function is changed.
    CORE_API virtual void updatedFunction()
    {
    }

    /// Function object to execute each time the timer expires.
    FunctionType m_function;

    mutable core::mt::Mutex m_mutex;
};

} //namespace sight::core::thread
