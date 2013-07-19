/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/once.hpp>
#include <boost/chrono/duration.hpp>

#include <wx/wx.h>

#include <fwCore/macros.hpp>
#include <fwCore/util/LazyInstantiator.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Timer.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include "fwGuiWx/App.hpp"
#include "fwGuiWx/WorkerWx.hpp"

namespace fwGuiWx
{
class WorkerWxTask;

wxDEFINE_EVENT(MESSAGE_EVENT, WorkerWxTask);

// Create a new application object
IMPLEMENT_APP_NO_MAIN(::fwGuiWx::App);

class WorkerWxTask : public wxEvent
{
public:

    WorkerWxTask( const ::fwThread::Worker::TaskType &handler ) :
        wxEvent( 0, MESSAGE_EVENT ),
        m_handler(handler)
    {
        SLM_ASSERT( "wxApplication should be instantiated", wxTheApp );
    }

    /// Copy constructor.
    WorkerWxTask(const WorkerWxTask& workerTask) :
        wxEvent( 0, MESSAGE_EVENT ),
        m_handler(workerTask.m_handler)
    {}

    ~WorkerWxTask()
    {}

    void process()
    {
        m_handler();
    }

    // implement the base class pure virtual
    virtual wxEvent *Clone() const
    {
        return new WorkerWxTask(*this);
    }

protected:
    ::fwThread::Worker::TaskType m_handler;
};


/**
 * @brief Private implementation of fwThread::Worker using boost::asio.
 */
class WorkerWx : public ::fwThread::Worker
{
public:
    typedef ::boost::thread ThreadType;

    WorkerWx();

    void init( int &argc, char **argv );

    void onExit();

    virtual ~WorkerWx();

    void stop();

    void post(TaskType handler);

    ::fwThread::Worker::FutureType getFuture();

    virtual ::fwThread::ThreadIdType getThreadId() const;

    void onMessage(WorkerWxTask &event);

    virtual void processTasks();

    virtual void processTasks(PeriodType maxtime);

protected:

    SPTR(::fwThread::Timer) createTimer();

    /// Copy constructor forbidden
    WorkerWx( const WorkerWx& );

    /// Copy operator forbidden
    WorkerWx& operator=( const WorkerWx& );

    ::fwThread::ThreadIdType m_threadId;

    ::boost::function1< void, WorkerWxTask& > m_onMessageHandler;
};


struct FWGUIWX_CLASS_API WorkerWxInstanciator
{
    FWGUIWX_API WorkerWxInstanciator(bool reg = true) :
        m_wxWorker(::boost::make_shared< WorkerWx >())
    {
        m_wxWorker->init( ::boost::ref(s_argc), s_argv);

        if(reg)
        {
            ::fwServices::registry::ActiveWorkers::getDefault()
                ->addWorker(::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER, m_wxWorker);
        }
    }

    FWGUIWX_API SPTR(::fwThread::Worker) getWorker();

    SPTR(WorkerWx) m_wxWorker;

    FWGUIWX_API static int    s_argc;
    FWGUIWX_API static char **s_argv;
};

int WorkerWxInstanciator::s_argc = 0 ;
char** WorkerWxInstanciator::s_argv = NULL;


SPTR(::fwThread::Worker) WorkerWxInstanciator::getWorker()
{
    return m_wxWorker;
}

::fwThread::Worker::sptr getWxWorker( int &argc, char **argv )
{

    WorkerWxInstanciator::s_argc = argc;
    WorkerWxInstanciator::s_argv = argv;

    typedef ::fwCore::util::LazyInstantiator< WorkerWxInstanciator, WorkerWxInstanciator > InstantiatorType;

    SPTR(WorkerWxInstanciator) instanciator = InstantiatorType::getInstance();

#ifdef _DEBUG
    {
        static ::boost::mutex mutex;
        ::boost::mutex::scoped_lock scoped_lock(mutex);
        static bool initialized = false;
        SLM_ASSERT("getWxWorker(argc, argv) shall be called only once", !initialized);
        initialized = true;
    }
#endif


    return instanciator->m_wxWorker;
}


//------------------------------------------------------------------------------


/**
 * @class TimerWx
 * @brief Private Timer implementation using Wx.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class TimerWx : public ::fwThread::Timer
{
public:
    /**
     * @brief Constructs a TimerWx from given io_service.
     */
    TimerWx();

    ~TimerWx();

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
        return m_timerWx->IsOneShot();
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_isOneShot = oneShot;
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_timerWx->IsRunning();
    }

protected:

    void call(wxTimerEvent& event);

    /// Copy constructor forbidden.
    TimerWx( const TimerWx& );

    /// Copy operator forbidden.
    TimerWx& operator=( const TimerWx& );

    SPTR(wxTimer) m_timerWx;
    bool m_isOneShot;
    TimeDurationType m_duration;
};

//------------------------------------------------------------------------------


// ---------- WorkerWx private implementation ----------

WorkerWx::WorkerWx() :
    m_threadId( ::fwThread::getCurrentThreadId() )
{
}

void WorkerWx::init( int &argc, char **argv )
{
    OSLM_TRACE("Init Wx" << ::fwThread::getCurrentThreadId() <<" Start");
    wxEntryStart( argc, argv ) ;
    wxTheApp->OnInit();
    m_onMessageHandler = ::boost::bind( &WorkerWx::onMessage, this, _1 );
    wxTheApp->Bind( MESSAGE_EVENT, m_onMessageHandler );
    OSLM_TRACE("Init Wx" << ::fwThread::getCurrentThreadId() <<" Finish");
}

WorkerWx::~WorkerWx()
{
    this->stop();
    wxEntryCleanup();
}

int onRun()
{
    SLM_ASSERT("wxTheApp not initialized", wxTheApp);
    return wxTheApp->OnRun();
}

::fwThread::Worker::FutureType WorkerWx::getFuture()
{
    if (! m_future.valid() )
    {
        SLM_ASSERT("WorkerWx loop shall be created and ran from main thread ",
            ! m_future.valid() && ::fwThread::getCurrentThreadId() == this->getThreadId() );

        ::boost::packaged_task< ExitReturnType > task( ::boost::bind(&onRun) );
        ::boost::future< ExitReturnType > ufuture = task.get_future();
        m_future = ::boost::move(ufuture);
        task();
    }
    return m_future;
}

::fwThread::ThreadIdType WorkerWx::getThreadId() const
{
    return m_threadId;
}

void WorkerWx::onExit()
{
    wxTheApp->Unbind( MESSAGE_EVENT, m_onMessageHandler );
    wxTheApp->ExitMainLoop();
    wxTheApp->OnExit();
}

void WorkerWx::stop()
{
    this->postTask<void>(::boost::bind( &WorkerWx::onExit, this)).wait();
}

SPTR(::fwThread::Timer) WorkerWx::createTimer()
{
    return ::boost::make_shared< TimerWx >();
}

void WorkerWx::post(TaskType handler)
{
    wxTheApp->AddPendingEvent( WorkerWxTask(handler) );
}

void WorkerWx::onMessage(WorkerWxTask &event)
{
    event.process();
    event.Skip(true);
    event.StopPropagation();
}

void WorkerWx::processTasks()
{
    SLM_FATAL("ProcessTasks not yet implemented in WorkerWx");
}

void WorkerWx::processTasks(PeriodType maxtime)
{
    FwCoreNotUsedMacro(maxtime);
    SLM_FATAL("ProcessTasks not yet implemented in WorkerWx");
}

// ---------- Timer private implementation ----------

TimerWx::TimerWx() :
    m_timerWx( new wxTimer(wxTheApp) ), m_isOneShot(false)
{
    wxTheApp->Bind(wxEVT_TIMER, &TimerWx::call, this, m_timerWx->GetId());
}

TimerWx::~TimerWx()
{
    wxTheApp->Unbind(wxEVT_TIMER, &TimerWx::call, this, m_timerWx->GetId());
    m_timerWx->Stop();
}

void TimerWx::setDuration(TimeDurationType duration)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_duration = duration;
}

void TimerWx::start()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerWx->Start( 
                ::boost::chrono::duration_cast< ::boost::chrono::milliseconds >(m_duration).count(), 
                m_isOneShot);
}

void TimerWx::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerWx->Stop();
}

void TimerWx::call(wxTimerEvent& event)
{
    m_function();
}


} //namespace fwGuiWx

