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
#include "core/jobs/base.hpp"

#include <cstdint>
#include <string>

namespace sight::core::jobs
{

/**
 * @brief This class manages a job.
 */
class CORE_CLASS_API observer : public base
{
public:

    /**
     * @name Typedefs
     * @{ */
    typedef SPTR(core::jobs::observer) sptr;
    typedef WPTR(core::jobs::observer) wptr;

    /// Progress callback type
    using progress_callback_t = std::function<void (std::uint64_t)>;
    /**  @} */

    /**
     * @brief Constructor
     *
     * @param name The job name
     */
    CORE_API observer(const std::string& _name, std::uint64_t _work_units = 100);

    /**
     * @name Exposed base methods
     * @{
     */
    using base::done_work;
    using base::set_total_work_units;
    using base::done;
    /**  @} */

    /**
     * @brief Call for finishing observer progress
     */
    CORE_API void finish() override;

    /**
     * @brief return a progress Callback function.
     * This function takes work units in parameter.
     * @warning This callback can only be used if the job is still instantiated.
     */
    CORE_API progress_callback_t progress_callback();

protected:

    ///Exposing run function of base in the interface.
    using base::run;

    /**
     * @brief Runs the task
     *
     * @return future of the task
     */
    CORE_API shared_future run_impl() override;

    /// Task type
    typedef std::packaged_task<void ()> packaged_task;

    /// Task observed
    packaged_task m_finish_task;
};

} //namespace sight::core::jobs
