/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>


#include "fwThread/Worker.hpp"
#include "fwThread/Timer.hpp"

namespace fwThread
{

std::size_t WorkerThread( ::boost::asio::io_service & io_service )
{
    OSLM_TRACE("Thread " << getCurrentThreadId() <<" Start");
    std::size_t res = io_service.run();
    OSLM_TRACE("Thread " << getCurrentThreadId() <<" Finish");
    return res;
}


/**
 * @brief Private implementation of fwThread::Worker using boost::asio.
 */
class WorkerAsio : public ::fwThread::Worker
{
public:
    typedef ::boost::asio::io_service IOServiceType;
    typedef ::boost::asio::io_service::work WorkType;
    typedef ::boost::shared_ptr< WorkType > WorkPtrType;
    typedef ::boost::thread ThreadType;


    WorkerAsio();

    virtual ~WorkerAsio();

    void stop();

    void post(TaskType handler);

    ThreadIdType getThreadId() const;

    SPTR(::fwThread::Timer) createTimer();


protected:

    /// Copy constructor forbidden
    WorkerAsio( const WorkerAsio& );

    /// Copy operator forbidden
    WorkerAsio& operator=( const WorkerAsio& );

    /// Class provides functionality to manipulate asynchronous tasks.
    IOServiceType m_ioService;

    /// Class to inform the io_service when it has work to do.
    WorkPtrType m_work;

    /// Thread created and managed by the worker.
    SPTR(ThreadType) m_thread;
};


//------------------------------------------------------------------------------


/**
 * @class TimerAsio
 * @brief Private Timer implementation using boost::asio.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class TimerAsio : public ::fwThread::Timer
{
public:
    /**
     * @brief Constructs a TimerAsio from given io_service.
     */
    TimerAsio(::boost::asio::io_service &ioSrv);

    ~TimerAsio();

    /// Starts or restarts the timer.
    void start();

    /// Stops the timer and cancel all pending operations.
    void stop();

    /// Sets time duration.
    void setDuration(TimeDurationType duration);

    /// Returns if the timer mode is 'one shot'.
    bool isOneShot() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_oneShot;
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_oneShot = oneShot;
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_running;
    }

protected:

    void cancelNoLock();
    void rearmNoLock(TimeDurationType duration);

    void call(const ::boost::system::error_code & code);


    /// Copy constructor forbidden.
    TimerAsio( const TimerAsio& );

    /// Copy operator forbidden.
    TimerAsio& operator=( const TimerAsio& );

    /// Timer object.
    ::boost::asio::deadline_timer m_timer;

    /// Time to wait until timer's expiration.
    TimeDurationType m_duration;

    /// Timer's mode.
    bool             m_oneShot;

    /// Timer's state.
    bool             m_running;
};

//------------------------------------------------------------------------------


// ---------- WorkerAsio private implementation ----------

WorkerAsio::WorkerAsio() :
    m_ioService(),
    m_work( ::boost::make_shared< WorkType >(::boost::ref(m_ioService)) )
{
    ::boost::packaged_task< ::fwThread::Worker::ExitReturnType > task( ::boost::bind(&WorkerThread, ::boost::ref(m_ioService)) );
    ::boost::unique_future< ::fwThread::Worker::ExitReturnType > ufuture = task.get_future();

    m_thread = ::boost::make_shared< ThreadType >( ::boost::move( task ) );

    m_future = ::boost::move(ufuture);
}

WorkerAsio::~WorkerAsio()
{
    if(!m_ioService.stopped())
    {
        this->stop();
    }
}

void WorkerAsio::stop()
{
    m_work.reset();
    m_thread->join();
}

SPTR(::fwThread::Timer) WorkerAsio::createTimer()
{
    return ::boost::make_shared< TimerAsio >(::boost::ref(m_ioService));
}

void WorkerAsio::post(TaskType handler)
{
    m_ioService.post(handler);
}


ThreadIdType WorkerAsio::getThreadId() const
{
    return m_thread->get_id();
}


// ---------- Worker ----------
SPTR(Worker) Worker::defaultFactory()
{
    return ::boost::make_shared< WorkerAsio >();
}


// ---------- Timer private implementation ----------

TimerAsio::TimerAsio(::boost::asio::io_service &ioSrv) :
    m_timer(ioSrv),
    m_duration(::boost::posix_time::seconds(1)),
    m_oneShot(false),
    m_running(false)
{
}

TimerAsio::~TimerAsio()
{
}

void TimerAsio::setDuration(TimeDurationType duration)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_duration = duration;
}

void TimerAsio::start()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    this->rearmNoLock(m_duration);
    m_running = true;
}

void TimerAsio::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if (m_running )
    {
        m_running = false;
        this->cancelNoLock();
    }
}

void TimerAsio::rearmNoLock(TimeDurationType duration)
{
    this->cancelNoLock();
    m_timer.expires_from_now(duration);
    m_timer.async_wait( ::boost::bind(&TimerAsio::call, this, _1));
}

void TimerAsio::call(const ::boost::system::error_code & error)
{
    if(!error)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        if (!m_oneShot)
        {
            this->rearmNoLock(m_duration);
            m_function();
        }
        else
        {
            m_function();
            m_running = false;
        }
    }
}

void TimerAsio::cancelNoLock()
{
    m_timer.cancel();
}


} //namespace fwThread

