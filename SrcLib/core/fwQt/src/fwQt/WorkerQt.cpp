/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwQt/WorkerQt.hpp"

#include "fwQt/util/FuncSlot.hpp"

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Timer.hpp>
#include <fwThread/Worker.hpp>

#include <fwTools/Os.hpp>

#include <QDir>
#include <QEvent>
#include <QPointer>
#include <QStringList>
#include <QTimer>

#include <functional>

namespace fwQt
{

class WorkerQtTask : public QEvent
{
public:
    WorkerQtTask( const ::fwThread::Worker::TaskType& handler ) :
        QEvent( static_cast< QEvent::Type >(s_WORKER_QT_TASK_EVENT_TYPE) ),
        m_handler( handler )
    {
        SLM_ASSERT( "Application should be instantiated", QCoreApplication::instance() );
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

    void init( int& argc, char** argv);

    virtual ~WorkerQt();

    void stop();

    void post(TaskType handler);

    void setApp(QSharedPointer<QCoreApplication> app, const std::string& name, const std::string& version);

    ::fwThread::Worker::FutureType getFuture();

    virtual ::fwThread::ThreadIdType getThreadId() const;

    virtual void processTasks();

    virtual void processTasks(PeriodType maxtime);

protected:

    int m_argc;
    char** m_argv;

    QSharedPointer< QCoreApplication > m_app;

    SPTR(::fwThread::Timer) createTimer();

    /// Copy constructor forbidden
    WorkerQt( const WorkerQt& );

    /// Copy operator forbidden
    WorkerQt& operator=( const WorkerQt& );

    ::fwThread::ThreadIdType m_threadId;
};

//-----------------------------------------------------------------------------

::fwThread::Worker::sptr getQtWorker(int& argc, char** argv,
                                     std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback,
                                     const std::string& name, const std::string& version)
{
    SPTR(WorkerQt) workerQt = std::make_shared< WorkerQt >();
    workerQt->init(argc, argv);
    workerQt->setApp(callback(argc, argv), name, version);
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

    QPointer< ::fwQt::util::FuncSlot > m_qtFunc;
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

void WorkerQt::init( int& argc, char** argv)
{
    OSLM_TRACE("Init Qt" << ::fwThread::getCurrentThreadId() <<" Start");

    // To get Qt initialized properly, we need to find its plugins
    // This is difficult to do this, especially because the location of the deps is different whether
    // you are executing the application in the build tree or in the install tree
    // Thus the strategy here is to locate the Qt5Core library and then compute the path relatively
    // This work in all cases when we use our binpkgs. If we use the system libraries, the Qt.conf file
    // of the system should do the job and the following might be useless.
    ::boost::filesystem::path qt5LibDir = ::fwTools::os::getSharedLibraryPath("Qt5Core");
    const ::boost::filesystem::path qt5PluginsDir = qt5LibDir.remove_filename() / "qt5" / "plugins";

    QDir pluginDir(QString::fromStdString(qt5PluginsDir.string()));
    if (pluginDir.exists())
    {
        QCoreApplication::setLibraryPaths(QStringList(pluginDir.absolutePath()));
    }
    else
    {
        SLM_ERROR("Could not determine qt5 plugins path, tried with: " + qt5PluginsDir.string());
    }

    m_argc = argc;
    m_argv = argv;

    OSLM_TRACE("Init Qt" << ::fwThread::getCurrentThreadId() <<" Finish");
}

//------------------------------------------------------------------------------

void WorkerQt::setApp(QSharedPointer<QCoreApplication> app, const std::string& name, const std::string& version)
{
    m_app = app;
    m_app.get()->setOrganizationName("IRCAD-IHU");
    m_app.get()->setOrganizationDomain("https://www.ircad.fr/");
    m_app.get()->setApplicationName(QString::fromStdString(name));
    m_app.get()->setApplicationVersion(QString::fromStdString(version));
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

        std::packaged_task< ExitReturnType() > task( std::bind(&QCoreApplication::exec) );

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
    this->postTask<void>(&QCoreApplication::quit).wait();
}
//------------------------------------------------------------------------------

SPTR(::fwThread::Timer) WorkerQt::createTimer()
{
    return std::make_shared< TimerQt >();
}

//------------------------------------------------------------------------------

void WorkerQt::post(TaskType handler)
{
    QCoreApplication::postEvent( QCoreApplication::instance(), new WorkerQtTask(handler) );
}

//------------------------------------------------------------------------------

void WorkerQt::processTasks()
{
    QCoreApplication::sendPostedEvents(0, ::fwQt::WorkerQtTask::s_WORKER_QT_TASK_EVENT_TYPE);
}

//------------------------------------------------------------------------------

void WorkerQt::processTasks(PeriodType maxtime)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, int(maxtime));
}

// ---------- Timer private implementation ----------

TimerQt::TimerQt() :
    m_timerQt( new QTimer(QCoreApplication::instance()) )
{
    m_qtFunc = new ::fwQt::util::FuncSlot();
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

} //namespace fwQt
