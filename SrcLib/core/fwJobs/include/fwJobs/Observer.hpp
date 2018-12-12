/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWJOBS_OBSERVER_HPP__
#define __FWJOBS_OBSERVER_HPP__

#include "fwJobs/config.hpp"
#include "fwJobs/IJob.hpp"

#include <cstdint>
#include <string>

namespace fwJobs
{

/**
 * @brief This class manages a job.
 */
class FWJOBS_CLASS_API Observer : public IJob
{
public:
    /**
     * @name Typedefs
     * @{ */
    typedef SPTR (::fwJobs::Observer) sptr;
    typedef WPTR (::fwJobs::Observer) wptr;

    /// Progress callback type
    typedef std::function< void (std::uint64_t) > ProgressCallback;
    /**  @} */

    /**
     * @brief Create a new Observer sptr with a name
     *
     * @param name The name of the job
     *
     * @return A pointer to a new Observer
     */
    FWJOBS_API static sptr New(const std::string& name, std::uint64_t workUnits = 100);

    /**
     * @brief Constructor
     *
     * @param name The job name
     */
    FWJOBS_API Observer(const std::string& name, std::uint64_t workUnits = 100);

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
    FWJOBS_API void finish();

    /**
     * @brief return a progress Callback function.
     * This function takes work units in parameter.
     * @warning This callback can only be used if the job is still instantiated.
     */
    FWJOBS_API ProgressCallback progressCallback();

protected:

    ///Exposing run function of IJob in the interface.
    using IJob::run;

    /**
     * @brief Runs the task
     *
     * @return future of the task
     */
    FWJOBS_API virtual SharedFuture runImpl();

    /// Task type
    typedef std::packaged_task< void () > PackagedTask;

    /// Task observed
    PackagedTask m_finishTask;

};

} //namespace fwJobs

#endif //__FWJOBS_OBSERVER_HPP__
