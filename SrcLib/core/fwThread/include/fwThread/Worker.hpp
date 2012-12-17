/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_WORKER_HPP__
#define __FWTHREAD_WORKER_HPP__

#include <boost/any.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <fwCore/base.hpp>

#include "fwThread/config.hpp"


namespace fwThread
{
typedef ::boost::thread::id ThreadIdType;

/// Returns the current thread id
FWTHREAD_API ThreadIdType getCurrentThreadId();

class Timer;

/**
 * @class   Worker.
 * @brief   This class creates and manages a task loop.
 * The default implementation create a loop in a new thread.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWTHREAD_CLASS_API Worker : public ::fwCore::BaseObject
{
public:
    typedef ::boost::function< void() > TaskType;
    typedef ::boost::any                ExitReturnType;

    typedef ::boost::shared_future< ExitReturnType > FutureType;

    fwCoreClassDefinitionsWithFactoryMacro( (Worker)(::fwCore::BaseObject), (()), defaultFactory );

    Worker(){}

    /// Waits for the last task to be processed and stops the loop
    FWTHREAD_API virtual void stop() = 0;

    /// Requests invocation of the given task handler and returns immediately.
    virtual void post(TaskType handler) = 0;

    /**
     * @brief Requests invocation of the given callable and returns a shared future immediately.
     *
     * @tparam R future's value type
     * @tparam CALLABLE Any type wrappable with a boost::function< void() >
     *
     * @returns a boost::shared_future associated with the result of the given callable
     */
    template< typename R, typename CALLABLE >
    ::boost::shared_future< R > postTask(CALLABLE f);

    /// Returns the worker's thread id
    FWTHREAD_API virtual ThreadIdType getThreadId() const = 0;

    /// Creates and returns a ::fwThread::Timer running in this Worker
    FWTHREAD_API virtual SPTR(::fwThread::Timer) createTimer() = 0;

    /// Returns a boost::shared_future associated with the execution of Worker's loop
    FWTHREAD_API virtual FutureType getFuture()
    {
        return m_future;
    }

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

#endif //__FWTHREAD_WORKER_HPP__
