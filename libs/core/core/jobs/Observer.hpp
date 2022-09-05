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
#include "core/jobs/IJob.hpp"

#include <cstdint>
#include <string>

namespace sight::core::jobs
{

/**
 * @brief This class manages a job.
 */
class CORE_CLASS_API Observer : public IJob
{
public:

    /**
     * @name Typedefs
     * @{ */
    typedef SPTR(core::jobs::Observer) sptr;
    typedef WPTR(core::jobs::Observer) wptr;

    /// Progress callback type
    typedef std::function<void (std::uint64_t)> ProgressCallback;
    /**  @} */

    /**
     * @brief Create a new Observer sptr with a name
     *
     * @param name The name of the job
     *
     * @return A pointer to a new Observer
     */
    CORE_API static sptr New(const std::string& name, std::uint64_t workUnits = 100);

    /**
     * @brief Constructor
     *
     * @param name The job name
     */
    CORE_API Observer(const std::string& name, std::uint64_t workUnits = 100);

    /**
     * @name Exposed IJob methods
     * @{
     */
    using IJob::doneWork;
    using IJob::setTotalWorkUnits;
    using IJob::done;
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
    CORE_API ProgressCallback progressCallback();

protected:

    ///Exposing run function of IJob in the interface.
    using IJob::run;

    /**
     * @brief Runs the task
     *
     * @return future of the task
     */
    CORE_API SharedFuture runImpl() override;

    /// Task type
    typedef std::packaged_task<void ()> PackagedTask;

    /// Task observed
    PackagedTask m_finishTask;
};

} //namespace sight::core::jobs
