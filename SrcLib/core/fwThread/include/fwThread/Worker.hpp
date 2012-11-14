/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_WORKER_HPP__
#define __FWTHREAD_WORKER_HPP__

#include <sstream>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread.hpp>

#include <fwCore/base.hpp>

#include "fwThread/config.hpp"


namespace fwThread
{


void WorkerThread( boost::asio::io_service & io_service );


/**
 * @class   Worker.
 * @brief   This class...
 *
 * @author IRCAD (Research and Development Team).
 * @date    2012.
 */
class FWTHREAD_CLASS_API Worker : public ::fwCore::BaseObject
{
public:
    typedef ::boost::asio::io_service IOServiceType;
    typedef ::boost::asio::io_service::work WorkType;
    typedef ::boost::shared_ptr< WorkType > WorkPtrType;

    fwCoreClassDefinitionsWithFactoryMacro( (Worker)(::fwCore::BaseObject), (()), ::boost::make_shared< Worker > );

    Worker();

    ~Worker();

    FWTHREAD_API void stop();

    template< typename Handler>
    void post(Handler handler);

    FWTHREAD_API::boost::thread::id getThreadId() const;

protected:
    Worker( const Worker& );
    Worker& operator=( const Worker& );


    IOServiceType m_ioService;
    WorkPtrType m_work;
    boost::thread m_thread;
};

} //namespace fwThread

#include "fwThread/Worker.hxx"

#endif //__FWTHREAD_WORKER_HPP__
