/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_WORKER_HPP__
#define __FWTHREAD_WORKER_HPP__

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
 * @brief   This class creates and manages a thread.
 * Thanks to the post method it is possible to execute handlers on this thread.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWTHREAD_CLASS_API Worker : public ::fwCore::BaseObject
{
public:
    typedef ::boost::thread             ThreadType;
    typedef ::boost::function< void() > HandlerType;

    fwCoreClassDefinitionsWithFactoryMacro( (Worker)(::fwCore::BaseObject), (()), defaultFactory );

    /// Constructor: creates thread dedicated to his m_ioService
    FWTHREAD_API Worker();

    /// Destructor: stops his m_ioService if it is not stopped
    FWTHREAD_API virtual ~Worker();

    /// Stops his m_ioService
    FWTHREAD_API virtual void stop();

    /// Requests invocation of the given handler and returns immediately.
    virtual void post(HandlerType handler) = 0;

    /// Requests invocation of the given task and returns a shared future immediately.
    template< typename R, typename TASK >
    ::boost::shared_future< R > postTask(TASK f);

    /// Returns the m_thread id
    FWTHREAD_API ThreadIdType getThreadId() const;

    FWTHREAD_API virtual SPTR(::fwThread::Timer) createTimer() = 0;

protected:

    friend class Timer;

    FWTHREAD_API static SPTR(Worker) defaultFactory();

    /// Copy constructor forbidden
    Worker( const Worker& );

    /// Copy operator forbidden
    Worker& operator=( const Worker& );

    /// Thread created and managed by the worker.
    SPTR(ThreadType) m_thread;

};

} //namespace fwThread

#include "fwThread/Worker.hxx"

#endif //__FWTHREAD_WORKER_HPP__
