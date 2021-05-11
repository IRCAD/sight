/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "core/jobs/IJob.hpp"

#include <cstdint>
#include <functional>
#include <string>

namespace thread
{
class Worker;
} //namespace thread

namespace sight::core::jobs
{

/**
 * @brief   This class encapsulate a task that will report it's progression
 * The embedded task will be run at most once. Once the job is finished or has
 * been canceled, the job (thus the task) will not be run anymore)
 */
class CORE_CLASS_API Job : public IJob
{
public:
    /**
     * @name Typedefs
     * @{ */
    typedef SPTR (core::jobs::Job) sptr;
    typedef WPTR (core::jobs::Job) wptr;

    /// Task type
    typedef std::function< void (Job&) > Task;

    /// Progress callback type
    typedef std::function< void (std::uint64_t) > ProgressCallback;
    /**  @} */

    /**
     * @brief Construct a new job and return a smart pointer of it
     *
     * @param name name of the new job
     * @param task The task managed by the job
     * @param core::thread::Worker An optional worker to run the task on
     *
     * @return
     */
    CORE_API static sptr New(const std::string& name, Task task, const SPTR(core::thread::Worker)& worker = nullptr );

    /**
     * @brief Construct a new job
     *
     * @param name name of the new job
     * @param task The task managed by the job
     * @param core::thread::Worker An optional worker to run the task on
     */
    CORE_API Job(const std::string& name, Task task, const SPTR(core::thread::Worker)& worker = nullptr );

    /**
     * @brief Return a job callback with the done work number as parameter
     * This callback can only be used if the job is still instantiated.
     */
    CORE_API ProgressCallback progressCallback();

    /**
     * @name Exposed IJob methods
     * @{
     */
    using IJob::doneWork;
    using IJob::setTotalWorkUnits;
    using IJob::done;
    /**  @} */

    ///Getter on the current job worker or nullptr if no worker has been set.
    CORE_API SPTR(core::thread::Worker) getWorker();

    /// Reimplements IJob's cancel
    CORE_API virtual SharedFuture cancel();

protected:

    /**
     * @brief Run the task of the job and mark the job as finished
     *
     * @return future of the task
     */
    CORE_API SharedFuture runImpl();

private:

    /// The task meant to be run
    Task m_task;

    /// The worker on wich the task is meant to be run
    SPTR(core::thread::Worker) m_worker;

};

} //namespace sight::core::jobs
