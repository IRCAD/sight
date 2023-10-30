/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "core/thread/worker.hpp"

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
class CORE_CLASS_API timer : public core::base_object
{
public:

    /**
     * @name Typedefs
     * @{ */
    using sptr            = std::shared_ptr<timer>;
    using function_type   = std::function<void ()>;
    using time_duration_t = std::chrono::duration<double>;
    /**  @} */

    CORE_API ~timer() override = default;

    /// Starts or restarts the timer.
    CORE_API virtual void start() = 0;

    /// Stops the timer and cancel all pending operations.
    CORE_API virtual void stop()
    {
    }

    /// Sets time duration.
    CORE_API virtual void set_duration(time_duration_t _duration) = 0;

    /// Sets the function to be triggered when time duration expires.
    template<typename F>
    void set_function(F _f)
    {
        core::mt::scoped_lock lock(m_mutex);
        m_function = _f;
        updated_function();
    }

    /// Returns if the timer mode is 'one shot'.
    virtual bool is_one_shot() const = 0;

    /// Sets timer mode.
    virtual void set_one_shot(bool _one_shot) = 0;

    /// Returns true if the timer is currently running.
    virtual bool is_running() const = 0;

protected:

    /**
     * @brief Constructs a Timer.
     * By default, a the Timer mode is repetitive.
     */
    CORE_API timer();

    /// Copy constructor forbidden.
    timer(const timer&);

    /// Copy operator forbidden.
    timer& operator=(const timer&);

    /// This method is triggered when Timer's function is changed.
    CORE_API virtual void updated_function()
    {
    }

    friend struct timer_callback;

    /// Function object to execute each time the timer expires.
    function_type m_function;

    mutable core::mt::mutex m_mutex;
};

} //namespace sight::core::thread
