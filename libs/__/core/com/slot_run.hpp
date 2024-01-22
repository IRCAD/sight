/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#define FWCOM_SLOTRUN_HPP

#include "core/com/slot_base.hpp"

#include <functional>
#include <set>

namespace sight::core::thread
{

class worker;

} // namespace sight::core::thread

namespace sight::core::com
{

template<typename F>
struct slot_run;

/**
 * @brief Slot run interface (call without return value).
 *
 * @tparam A Slot's arguments types.
 */
template<typename ... A>
struct CORE_CLASS_API slot_run<void(A ...)>: slot_base
{
    /**
     * @name Typedefs
     * @{ */
    using signature_type = void (A ...);
    using self_t         = slot_run<signature_type>;
    using sptr           = std::shared_ptr<self_t>;
    using wptr           = std::weak_ptr<self_t>;
    /**  @} */

    slot_run() :
        slot_base(sizeof...(A))
    {
    }

    /**
     * @brief Run the Slot with the given parameters.
     */
    virtual void run(A ... _a) const = 0;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     *
     * @param _worker Worker that will run the Slot.
     * @param _args run arguments.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if given worker is not valid.
     */
    virtual slot_base::void_shared_future_type async_run(const SPTR(core::thread::worker)& _worker, A ... _args) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual slot_base::void_shared_future_type async_run(A ... _args) const;

    protected:

        template<typename R, typename WEAKCALL>
        static std::shared_future<R> post_weak_call(const SPTR(core::thread::worker)& _worker, WEAKCALL _f);

        /**
         * @brief Binds the given parameters to the run method within a void() function.
         *
         * @return a void() function.
         */
        virtual std::function<void()> bind_run(A ... _args) const;
};

} // namespace sight::core::com
