/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
#include <functional>
#include <string>

namespace sight::core::thread
{

class worker;

} //namespace sight::core::thread

namespace sight::core::jobs
{

/**
 * @brief   This class encapsulate a task that will report it's progression
 * The embedded task will be run at most once. Once the job is finished or has
 * been canceled, the job (thus the task) will not be run anymore)
 */
class CORE_CLASS_API job : public base
{
public:

    /**
     * @name Typedefs
     * @{ */
    using sptr = std::shared_ptr<core::jobs::job>;
    using wptr = std::weak_ptr<core::jobs::job>;

    /// Task type
    using task = std::function<void (job&)>;

    /// Progress callback type
    using progress_callback_t = std::function<void (std::uint64_t)>;
    /**  @} */

    /**
     * @brief Construct a new job
     *
     * @param name name of the new job
     * @param task The task managed by the job
     * @param core::thread::worker An optional worker to run the task on
     */
    CORE_API job(const std::string& _name, task _task, SPTR(core::thread::worker)_worker = nullptr);

    /**
     * @brief Return a job callback with the done work number as parameter
     * This callback can only be used if the job is still instantiated.
     */
    CORE_API progress_callback_t progress_callback();

    /**
     * @name Exposed base methods
     * @{
     */
    using base::done_work;
    using base::set_total_work_units;
    using base::done;
    /**  @} */

    ///Getter on the current job worker or nullptr if no worker has been set.
    CORE_API SPTR(core::thread::worker) get_worker();

    /// Reimplements base's cancel
    CORE_API shared_future cancel() override;

protected:

    /**
     * @brief Run the task of the job and mark the job as finished
     *
     * @return future of the task
     */
    CORE_API shared_future run_impl() override;

private:

    /// The task meant to be run
    task m_task;

    /// The worker on which the task is meant to be run
    SPTR(core::thread::worker) m_worker;
};

} //namespace sight::core::jobs
