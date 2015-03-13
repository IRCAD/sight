/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWJOBS_JOB_HPP__
#define __FWJOBS_JOB_HPP__

#include "fwJobs/config.hpp"
#include "fwJobs/IJob.hpp"

#include <cstdint>
#include <functional>
#include <string>

namespace fwThread
{
class Worker;
} //namespace fwThread

namespace fwJobs
{

/**
 * @brief   This class encapsulate a task that will report it's progression
 * The embeded task will be run at most once. Once the job is finished or has
 * been canceled, the job (thus the task) will not be run anymore)
 */
class FWJOBS_CLASS_API Job : public IJob
{
public:
    /**
     * @name Typedefs
     * @{ */
    typedef SPTR (::fwJobs::Job) sptr;
    typedef WPTR (::fwJobs::Job) wptr;

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
     * @param ::fwThread::Worker An optional worker to run the task on
     *
     * @return
     */
    FWJOBS_API static sptr New(const std::string &name, Task task, const SPTR(::fwThread::Worker) &worker = nullptr );

    /**
     * @brief Construct a new job
     *
     * @param name name of the new job
     * @param task The task managed by the job
     * @param ::fwThread::Worker An optional worker to run the task on
     */
    FWJOBS_API Job(const std::string &name, Task task, const SPTR(::fwThread::Worker) &worker = nullptr );

    /**
     * @brief Return a job callback with the done work number as parameter
     * This callback can only be used if the job is still instantiated.
     */
    FWJOBS_API ProgressCallback progressCallback();

    /**
     * @name Exposed IJob methods
     * @{
     */
    using IJob::doneWork;
    using IJob::setTotalWorkUnits;
    using IJob::done;
    /**  @} */

    ///Getter on the current job worker or nullptr if no worker has been set.
    FWJOBS_API SPTR(::fwThread::Worker) getWorker();

    /// Reimplements IJob's cancel
    FWJOBS_API virtual SharedFuture cancel();

protected:

    /**
     * @brief Run the task of the job and mark the job as finished
     *
     * @return future of the task
     */
    FWJOBS_API SharedFuture runImpl();

private:

    /// The task meant to be run
    Task m_task;

    /// The worker on wich the task is meant to be run
    SPTR(::fwThread::Worker) m_worker;

};

} //namespace fwJobs

#endif //__FWJOBS_JOB_HPP__
