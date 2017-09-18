/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/WorkerQt.hpp"

#include "fwGuiQt/App.hpp"
#include "fwGuiQt/util/FuncSlot.hpp"

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Timer.hpp>
#include <fwThread/Worker.hpp>

#include <QDir>
#include <QEvent>
#include <QFont>
#include <QPointer>
#include <QSharedPointer>
#include <QStringList>
#include <QTimer>

#include <functional>

namespace fwGuiQt
{

class WorkerQtTask : public QEvent
{
public:
    WorkerQtTask( const ::fwThread::Worker::TaskType& handler ) :
        QEvent( static_cast< QEvent::Type >(s_WORKER_QT_TASK_EVENT_TYPE) ),
        m_handler( handler )
    {
        SLM_ASSERT( "QApplication should be instantiated", qApp );
    }

    ~WorkerQtTask()
    {
        m_handler();
    }

    static const int s_WORKER_QT_TASK_EVENT_TYPE;

protected:

    ::fwThread::Worker::TaskType m_handler;
};

const int WorkerQtTask::s_WORKER_QT_TASK_EVENT_TYPE = QEvent::registerEventType();

/**
 * @brief Private implementation of fwThread::Worker using boost::asio.
 */
class WorkerQt : public ::fwThread::Worker
{
public:
    WorkerQt();

    void init( int& argc, char** argv, bool guiEnabled = true );

    virtual ~WorkerQt();

    void stop();

    void post(TaskType handler);

    ::fwThread::Worker::FutureType getFuture();

    virtual ::fwThread::ThreadIdType getThreadId() const;

    virtual void processTasks();

    virtual void processTasks(PeriodType maxtime);

protected:

    int m_argc;

    QSharedPointer< QApplication > m_app;

    SPTR(::fwThread::Timer) createTimer();

    /// Copy constructor forbidden
    WorkerQt( const WorkerQt& );

    /// Copy operator forbidden
    WorkerQt& operator=( const WorkerQt& );

    ::fwThread::ThreadIdType m_threadId;
};

//-----------------------------------------------------------------------------

::fwThread::Worker::sptr getQtWorker(int& argc, char** argv, bool guiEnabled)
{
    SPTR(WorkerQt) workerQt = std::make_shared< WorkerQt >();
    workerQt->init(argc, argv, guiEnabled);

    return workerQt;
}

//-----------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using Qt.
 */
class TimerQt : public ::fwThread::Timer
{
public:
    /**
     * @brief Constructs a TimerQt from given io_service.
     */
    TimerQt();

    ~TimerQt();

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
        return m_timerQt->isSingleShot();
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_timerQt->setSingleShot(oneShot);
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_timerQt->isActive();
    }

protected Q_SLOTS:

    void call();

protected:

    /// Copy constructor forbidden.
    TimerQt( const TimerQt& );

    /// Copy operator forbidden.
    TimerQt& operator=( const TimerQt& );

    void updatedFunction();

    QPointer< QTimer > m_timerQt;

    QPointer< ::fwGuiQt::util::FuncSlot > m_qtFunc;
};

//------------------------------------------------------------------------------

// ---------- WorkerQt private implementation ----------

WorkerQt::WorkerQt() :
    m_argc(0),
    m_app(nullptr),
    m_threadId( ::fwThread::getCurrentThreadId() )
{
}

//------------------------------------------------------------------------------

void WorkerQt::init( int& argc, char** argv, bool guiEnabled )
{
    OSLM_TRACE("Init Qt" << ::fwThread::getCurrentThreadId() <<" Start");

    QDir pluginDir("./plugins");
    if (pluginDir.exists())
    {
        QCoreApplication::setLibraryPaths(QStringList(pluginDir.absolutePath()));
    }
    m_argc = argc;
    m_app  = QSharedPointer< QApplication > ( new ::fwGuiQt::App( m_argc, argv, guiEnabled ) );

    OSLM_TRACE("Init Qt" << ::fwThread::getCurrentThreadId() <<" Finish");
}

//------------------------------------------------------------------------------

WorkerQt::~WorkerQt()
{
    this->stop();
}

//------------------------------------------------------------------------------

::fwThread::Worker::FutureType WorkerQt::getFuture()
{
    if (!m_future.valid() )
    {
        SLM_ASSERT("WorkerQt loop shall be created and ran from main thread ",
                   !m_future.valid() && ::fwThread::getCurrentThreadId() == this->getThreadId() );

        std::packaged_task< ExitReturnType() > task( std::bind(&QApplication::exec) );

        std::future< ExitReturnType > ufuture = task.get_future();

        m_future = std::move(ufuture);

        task();
    }

    return m_future;
}

//------------------------------------------------------------------------------

::fwThread::ThreadIdType WorkerQt::getThreadId() const
{
    return m_threadId;
}

//------------------------------------------------------------------------------

void WorkerQt::stop()
{
    this->postTask<void>(&QApplication::quit).wait();
}
//------------------------------------------------------------------------------

SPTR(::fwThread::Timer) WorkerQt::createTimer()
{
    return std::make_shared< TimerQt >();
}

//------------------------------------------------------------------------------

void WorkerQt::post(TaskType handler)
{
    QApplication::postEvent( qApp, new WorkerQtTask(handler) );
}

//------------------------------------------------------------------------------

void WorkerQt::processTasks()
{
    QApplication::sendPostedEvents(0, ::fwGuiQt::WorkerQtTask::s_WORKER_QT_TASK_EVENT_TYPE);
}

//------------------------------------------------------------------------------

void WorkerQt::processTasks(PeriodType maxtime)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, int(maxtime));
}

// ---------- Timer private implementation ----------

TimerQt::TimerQt() :
    m_timerQt( new QTimer(qApp) )
{
    m_qtFunc = new ::fwGuiQt::util::FuncSlot();
    QObject::connect(m_timerQt, SIGNAL(timeout()), m_qtFunc, SLOT(trigger()));
}

//------------------------------------------------------------------------------

TimerQt::~TimerQt()
{
    QObject::disconnect(m_timerQt, SIGNAL(timeout()), m_qtFunc, SLOT(trigger()));
    delete m_qtFunc;
    m_timerQt->stop();
    delete m_timerQt;
}

//------------------------------------------------------------------------------

void TimerQt::setDuration(TimeDurationType duration)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerQt->setInterval( static_cast<int>(
                                std::chrono::duration_cast< std::chrono::milliseconds >(duration).count())
                            );
}

//------------------------------------------------------------------------------

void TimerQt::start()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerQt->start();
}

//------------------------------------------------------------------------------

void TimerQt::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerQt->stop();
}

//------------------------------------------------------------------------------

void TimerQt::call()
{
    m_function();
}

//------------------------------------------------------------------------------

void TimerQt::updatedFunction()
{
    m_qtFunc->setFunction(m_function);
}

} //namespace fwGuiQt

