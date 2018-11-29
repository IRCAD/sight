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

#ifndef __FWJOBS_IJOB_HPP__
#define __FWJOBS_IJOB_HPP__

#include "fwJobs/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwCore/mt/types.hpp>

#include <cstdint>
#include <functional>
#include <future>
#include <string>
#include <vector>

namespace fwJobs
{

/**
 * @brief This class is an interface for class managing job.
 */
class FWJOBS_CLASS_API IJob
{
public:

    /**
     * @brief Job's states
     *
     * State :
     * - WAITING : The job is ready to be used
     * - RUNNING : The job is currently running
     * - CANCELING : The job received a cancel request
     * - CANCELED : The job is canceled
     * - FINISHED : The job have finished to run his task
     */
    enum State
    {
        WAITING = 0,
        RUNNING,
        CANCELING,
        CANCELED,
        FINISHED
    };

    /**
     * @name Typedef used in IJob
     * @{ */
    typedef SPTR (::fwJobs::IJob) sptr;
    typedef WPTR (::fwJobs::IJob) wptr;

    /// Cancel request callback type
    typedef std::function< bool () > CancelRequestCallback;

    /// Job cancel callback type for cancel hook
    typedef std::function< void (IJob&) > JobCancelHook;

    /// Log callback type for log hook
    typedef std::function< void (IJob&, const std::string&) > LogHook;

    /// Done work callback type for done work hook
    typedef std::function< void (IJob&, std::uint64_t /*oldDoneWork*/) > DoneWorkHook;

    /// Work units callback type for total work unit hook
    typedef std::function< void (IJob&, std::uint64_t /*oldTotalWorkUnits*/) > TotalWorkUnitsHook;

    /// Cancel callback type for cancel hook
    typedef std::function< void () > CancelHook;

    /// State callback type for state hook
    typedef std::function< void ( State ) > StateHook;

    /// Job cancel callback sequence type for cancel hook
    typedef std::vector< JobCancelHook > CancelHookSeq;

    /// Done work callback sequence type for done work hook
    typedef std::vector< DoneWorkHook > DoneWorkHookSeq;

    /// Work units callback sequence type for total work unit hook
    typedef std::vector< TotalWorkUnitsHook > TotalWorkUnitsHookSeq;

    /// Log callback sequence type for log hook
    typedef std::vector< LogHook > LogHookSeq;

    /// Log callback sequence type for state hook
    typedef std::vector< StateHook > StateHookSeq;

    /// Log container type
    typedef std::vector< std::string > Logs;

    /// State signal type
    typedef ::fwCom::Signal< void () > StateSignal;

    /// Cancel request signal type
    typedef ::fwCom::Signal< void () > CancelRequestedSignal;

    /// Done work signal type
    typedef ::fwCom::Signal< void (std::uint64_t, std::uint64_t) > DoneWorkSignal;

    /// Log signal type
    typedef ::fwCom::Signal< void (std::string) > LogSignal;

    /// Future type
    typedef std::shared_future< void > SharedFuture;
    /**  @} */

    /**
     * @brief Default constructor.
     *
     * @param name The name of the job.
     */
    FWJOBS_API IJob(const std::string& name = "");

    /// Default destructor.
    FWJOBS_API virtual ~IJob();

    /// Getter on the number of done work units.
    FWJOBS_API std::uint64_t getDoneWorkUnits() const;

    /// Getter on the total number of work units.
    FWJOBS_API std::uint64_t getTotalWorkUnits() const;

    /// Getter on the current State. @see State
    FWJOBS_API State getState() const;

    ///Getter on the name of the job.
    FWJOBS_API const std::string& getName() const;

    /// Getter on the log container
    FWJOBS_API Logs getLogs() const;

    /// Getter on cancelable, returns whether the job is cancelable.
    FWJOBS_API bool isCancelable() const;

    /// Setter on cancelable.
    FWJOBS_API void setCancelable(bool cancel);

    /**
     * @brief Run the current job
     *
     * @return future on the current running job
     */
    FWJOBS_API SharedFuture run();

    /**
     * @brief Wait job execution ending.
     *
     * This method raise a ::fwJobs::exception::Waiting Exception if the job is
     * not running.
     *
     */
    FWJOBS_API void wait();

    /**
     * @brief Returns the job canceling status.
     *
     * @return A boolean: true if cancel have been requested.
     */
    FWJOBS_API const bool& cancelRequested() const;

    /**
     * @brief Returns a callback on job canceling status.
     * This callback can only be used if the job is still instanciated.
     * @return A callback to check if the job have been requested.
     */
    FWJOBS_API CancelRequestCallback cancelRequestedCallback() const;

    /**
     * @brief Cancel the current job and call all available cancel callbacks
     *
     * @return  A future on the current running job or an empty future if the job
     * is canceled
     */
    FWJOBS_API virtual SharedFuture cancel();

    /**
     * @brief Add cancel callback to sequence for cancel hook
     *
     * @param callback cancel callback
     */
    FWJOBS_API void addSimpleCancelHook(CancelHook callback);

    /**
     * @brief Add cancel callback to sequence for cancel hook
     *
     * @param callback job cancel callback, taking a IJob as parameter
     */
    FWJOBS_API void addCancelHook(JobCancelHook callback);

    /**
     * @brief Add job done work unit callback to sequence for done work hook
     *
     * @param callback job done work unit callback, taking a IJob as parameter
     */
    FWJOBS_API void addDoneWorkHook(DoneWorkHook callback);

    /**
     * @brief Add job work unit callback to sequence for total work unit hook
     *
     * @param callback job work unit callback, taking a IJob as parameter
     */
    FWJOBS_API void addTotalWorkUnitsHook(TotalWorkUnitsHook callback);

    /**
     * @brief Add job log callback to sequence for log hook
     *
     * @param callback job log callback, taking a std::string message as parameter
     */
    FWJOBS_API void addLogHook(LogHook callback);

    /**
     * @brief Add job state callback to sequence for state hook
     *
     * @param callback job state callback, taking a State as parameter
     */
    FWJOBS_API void addStateHook(StateHook callback);

    /**
     * @brief Log a message.
     *
     * Push the message in the log container and emit a logged
     * signal with log as parameter. finally call all log callbacks
     *
     * @param log the message to log
     */
    FWJOBS_API void log(const std::string& message);

protected:

    /**
     * @name Not implemented
     * @{ */
    IJob(IJob&);
    IJob& operator=(IJob&);
    IJob(IJob&&);
    IJob& operator=(IJob&&);
    /**  @} */

    /// Run an instanciated job
    FWJOBS_API virtual SharedFuture runImpl() = 0;

    /// Finish the job: set state to finished or canceled.
    FWJOBS_API virtual void finish();

    /// Finish the job without mutex lock: set the state to finished or canceled.
    FWJOBS_API void finishNoLock();

    /**
     * @brief Return callback to finish the job.
     *
     * @return A callback
     */
    // workaround because of vs2010 issue : http://goo.gl/WHEkQ5
    // this is fixed in vs2012 : http://goo.gl/tvO3g7
    // TODO : remove when compiler is up to date
    FWJOBS_API std::function< void() > finishCallback();

    /// Getter on the state without mutex lock
    FWJOBS_API State getStateNoLock() const;

    /// Setter on the state
    FWJOBS_API void setState( State state );

    /// Setter on the state without mutex lock
    FWJOBS_API void setStateNoLock( State state );

    /// Setter on done work units
    FWJOBS_API void doneWork( std::uint64_t units );

    /**
     * @brief Setter on done work units
     * @warning This method leaves \p lock in an unlocked state
     *
     * @param units new done work units
     * @param lock mutex to upgrade to write lock
     */
    FWJOBS_API void doneWork( std::uint64_t units, ::fwCore::mt::ReadToWriteLock& lock );

    /// Set done work units to total work units
    FWJOBS_API void done();

    /**
     * @brief Setter on total work units.
     *
     * @param units New total for work units.
     */
    FWJOBS_API void setTotalWorkUnits(std::uint64_t units);

    /**
     * @brief Setter on total work units
     * @warning This method leaves \p lock in an unlocked state
     *
     * @param units new total work units
     * @param lock mutex to upgrade to write lock
     */
    FWJOBS_API void setTotalWorkUnitsUpgradeLock( std::uint64_t units, ::fwCore::mt::ReadToWriteLock& lock );

    /**
     * @brief Add job cancel callback to sequence without mutex lock for cancel hook
     *
     * @param callback job cancel callback, taking a IJob as parameter or not
     */
    FWJOBS_API void addCancelHookNoLock(JobCancelHook callback);

    /**
     * @brief Add job done work unit callback to sequence without mutex lock for done work hook
     *
     * @param callback job done work unit callback, taking a IJob as parameter
     */
    FWJOBS_API void addDoneWorkHookNoLock(DoneWorkHook callback);

    /**
     * @brief Add job work unit callback to sequence without mutex lock for total work unit hook
     *
     * @param callback job work unit callback, taking a IJob as parameter
     */
    FWJOBS_API void addTotalWorkUnitsHookNoLock(TotalWorkUnitsHook callback);

    /**
     * @brief Add job log callback to sequence without mutex lock for log hook
     *
     * @param callback job log callback, taking a std::string message as parameter
     */
    FWJOBS_API void addLogHookNoLock(LogHook callback);

    /**
     * @brief Add cancel callback to sequence without mutex lock for state hook
     *
     * @param callback cancel callback, taking a State as parameter
     */
    FWJOBS_API void addStateHookNoLock(StateHook callback);

    /**
     * @brief Add a message to thelog sequence.
     *
     * @param message the message to add to the sequence
     */
    FWJOBS_API void logNoLock(const std::string& message);

    /// Signal emitted when cancel has been requested
    SPTR(CancelRequestedSignal) m_sigCancelRequested;

    /// Signal emitted when te job has been canceled.
    SPTR(StateSignal) m_sigCanceled;

    /// Signal emitted when the job has been started.
    SPTR(StateSignal) m_sigStarted;

    /// Signal emitted when the job has been finished.
    SPTR(StateSignal) m_sigFinished;

    /// Signal emitted when done work units have been changed. Takes a std::uint64_t as parameter.
    SPTR(DoneWorkSignal) m_sigDoneWork;

    /// Signal emitted when a message has been added to logs. Takes a std::string as parameter.
    SPTR(LogSignal) m_sigLogged;

    /// Mutex to protect object access.
    mutable ::fwCore::mt::ReadWriteMutex m_mutex;

    /// Job's name
    std::string m_name;

    /// Logs container
    Logs m_logs;

    /// Number of work units already reached.
    std::uint64_t m_doneWorkUnits;

    /// Number of work units to reach to complete the job.
    std::uint64_t m_totalWorkUnits;

    /// Determines if cancellation has been requested. The job is/will be canceling if true.
    bool m_cancelRequested;

    /// Determines if the job can be cancelled. The job is cancelable if true.
    bool m_cancelable;

    /// Container of cancel callbacks. Cancel callbacks will be run when job is canceling.
    CancelHookSeq m_cancelHooks;

    /// Container of done work callbacks. These callbacks take nothing or a IJob as parameter.
    DoneWorkHookSeq m_doneWorkHooks;

    /// Container of total work unit callbacks. These callbacks take a IJob as parameter.
    TotalWorkUnitsHookSeq m_totalWorkUnitsHooks;

    /// Container of log callbacks. These callbacks take a std::string as parameter.
    LogHookSeq m_logHooks;

    /// Container of state callbacks. These callbacks take a State as parameter.
    StateHookSeq m_stateHooks;

    /// Job's future returned by 'run'
    SharedFuture m_runFuture;

    /// Job's state
    State m_state;
};

} //namespace fwJobs

#endif //__FWJOBS_IJOB_HPP__
