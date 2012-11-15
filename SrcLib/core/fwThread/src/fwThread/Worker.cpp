/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwThread/Worker.hpp"

namespace fwThread
{

ThreadIdType getCurrentThreadId()
{
    return ::boost::this_thread::get_id();
}

void Worker::WorkerThread( ::boost::asio::io_service & io_service )
{
    OSLM_TRACE("Thread " << getCurrentThreadId() <<" Start");
    io_service.run();
    OSLM_TRACE("Thread " << getCurrentThreadId() <<" Finish");
}

Worker::Worker(): m_ioService(),
    m_work( ::boost::make_shared< WorkType >(::boost::ref(m_ioService)) ),
    m_thread( ::boost::bind(&WorkerThread, ::boost::ref(m_ioService)) )
{
}

Worker::~Worker()
{
    if(!m_ioService.stopped())
    {
        this->stop();
    }
}

void Worker::stop()
{
    m_work.reset();
    m_thread.join();
}

ThreadIdType Worker::getThreadId() const
{
    return m_thread.get_id();
}

Worker::Worker( const Worker& )
{}

Worker& Worker::operator=( const Worker& )
{
    return *this;
}

} //namespace fwThread
