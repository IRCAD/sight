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



/**
 * @class   Worker.
 * @brief   This class creates and manages a thread. Thanks to the post method it is possible to execute handlers on
 * this thread.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWTHREAD_CLASS_API Worker : public ::fwCore::BaseObject
{
public:
    typedef ::boost::asio::io_service IOServiceType;
    typedef ::boost::asio::io_service::work WorkType;
    typedef ::boost::shared_ptr< WorkType > WorkPtrType;
    typedef ::boost::thread ThreadType;

    fwCoreClassDefinitionsWithFactoryMacro( (Worker)(::fwCore::BaseObject), (()), ::boost::make_shared< Worker > );

    /// Constructor: creates thread dedicated to his m_ioService
    FWTHREAD_API Worker();

    /// Destructor: stops his m_ioService if it is not stopped
    FWTHREAD_API virtual ~Worker();

    /// Stops his m_ioService
    FWTHREAD_API void stop();

    /// Requests the m_ioService to invoke on the worker thread the given handler and return immediately.
    template< typename Handler >
    void post(Handler handler);

    /// Requests the m_ioService to invoke on the worker thread the given handler and return immediately.
    template< typename R, typename TASK >
    ::boost::shared_future< R > postTask(TASK f);

    /// Returns the m_thread id
    FWTHREAD_API ThreadIdType getThreadId() const;

protected:

    friend class Timer;

    /// Copy constructor forbidden
    Worker( const Worker& );

    /// Copy operator forbidden
    Worker& operator=( const Worker& );

    /// Class provides functionality to manipulate asynchronous tasks.
    IOServiceType m_ioService;

    /// Class to inform the io_service when it has work to do.
    WorkPtrType m_work;

    /// Thread created and managed by the worker.
    ThreadType m_thread;
};

} //namespace fwThread

#include "fwThread/Worker.hxx"

#endif //__FWTHREAD_WORKER_HPP__
