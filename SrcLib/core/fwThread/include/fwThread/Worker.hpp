/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <boost/any.hpp>
#include <future>

#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>

#include "fwThread/config.hpp"

namespace fwThread
{
typedef std::thread::id ThreadIdType;

/// Returns the current thread id
FWTHREAD_API ThreadIdType getCurrentThreadId();

class Timer;

/**
 * @brief   This class creates and manages a task loop.
 * The default implementation create a loop in a new thread.
 */
class FWTHREAD_CLASS_API Worker : public ::fwCore::BaseObject
{
public:
    typedef ::fwCore::HiResClock::HiResClockType PeriodType;
    typedef std::function< void () > TaskType;
    typedef ::boost::any ExitReturnType;

    typedef std::shared_future< ExitReturnType > FutureType;

    fwCoreClassDefinitionsWithFactoryMacro( (Worker)(::fwCore::BaseObject), (()), defaultFactory );

    Worker()
    {
    }

    /// Waits for the last task to be processed and stops the loop
    FWTHREAD_API virtual void stop() = 0;

    /// Requests invocation of the given task handler and returns immediately.
    virtual void post(TaskType handler) = 0;

    /**
     * @brief Requests invocation of the given callable and returns a shared future.
     *
     * Returns immediately if called from another thread. Otherwise, the given callable is called
     * immediately.
     *
     * @tparam R future's value type
     * @tparam CALLABLE Any type wrappable with a boost::function< void() >
     *
     * @returns a std::shared_future associated with the result of the given callable
     */
    template< typename R, typename CALLABLE >
    std::shared_future< R > postTask(CALLABLE f);

    /// Returns the worker's thread id
    FWTHREAD_API virtual ThreadIdType getThreadId() const = 0;

    /// Creates and returns a ::fwThread::Timer running in this Worker
    FWTHREAD_API virtual SPTR(::fwThread::Timer) createTimer() = 0;

    /**
     * @brief Returns a std::shared_future associated with the execution of Worker's loop
     * @warning Calling getFuture() may be blocking if it is required by a specific implementation (for example, the Qt
     * implementation).
     */
    virtual FutureType getFuture()
    {
        return m_future;
    }

    /**
     * @brief Processes worker pending tasks for the calling thread
     * for maxtime milliseconds or until there are no more tasks to process.
     * You can call this function occasionally when your program is busy performing a long operation.
     *
     * @param maxtime milliseconds to process worker pending tasks.
     * @warning Qt implementation processes all Qt and Worker pending events, be careful.
     * @warning WxWidgets version is not yet implemented.
     */
    FWTHREAD_API virtual void processTasks(PeriodType maxtime) = 0;

    /**
     * @brief Processes all worker pending tasks for the calling thread
     * until there are no more tasks to process.
     * You can call this function occasionally when your program is busy performing a long operation.
     * @warning WxWidgets version is not yet implemented.
     */
    FWTHREAD_API virtual void processTasks() = 0;

protected:

    /// Creates and returns a new instance of Worker default implementation
    /// (boost::Asio).
    FWTHREAD_API static SPTR(Worker) defaultFactory();

    /// Copy constructor forbidden
    Worker( const Worker& );

    /// Copy operator forbidden
    Worker& operator=( const Worker& );

    /// Worker's loop future
    FutureType m_future;
};

} //namespace fwThread

#include "fwThread/Worker.hxx"
