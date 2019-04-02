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

#include "fwGuiQml/WorkerQml.hpp"

#include "fwGuiQml/App.hpp"
#include "fwGuiQml/util/FuncSlot.hpp"

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Timer.hpp>
#include <fwThread/Worker.hpp>

#include <fwTools/Os.hpp>

#include <QDir>
#include <QEvent>
#include <QFont>
#include <QPointer>
#include <QSharedPointer>
#include <QStringList>
#include <QTimer>

#include <functional>

namespace fwGuiQml
{

class WorkerQmlTask : public QEvent
{
public:
    WorkerQmlTask( const ::fwThread::Worker::TaskType& handler ) :
        QEvent( static_cast< QEvent::Type >(s_WORKER_QML_TASK_EVENT_TYPE) ),
        m_handler( handler )
    {
        SLM_ASSERT( "QGuiApplication should be instantiated", qGuiApp );
    }

    ~WorkerQmlTask()
    {
        m_handler();
    }

    static const int s_WORKER_QML_TASK_EVENT_TYPE;

protected:

    ::fwThread::Worker::TaskType m_handler;
};

const int WorkerQmlTask::s_WORKER_QML_TASK_EVENT_TYPE = QEvent::registerEventType();

/**
 * @brief Private implementation of fwThread::Worker using boost::asio.
 */
class WorkerQml : public ::fwThread::Worker
{
public:
    WorkerQml();

    void init( int& argc, char** argv, bool guiEnabled = true );

    virtual ~WorkerQml();

    void stop();

    void post(TaskType handler);

    ::fwThread::Worker::FutureType getFuture();

    virtual ::fwThread::ThreadIdType getThreadId() const;

    virtual void processTasks();

    virtual void processTasks(PeriodType maxtime);

protected:

    int m_argc;

    QSharedPointer< QGuiApplication > m_app;

    SPTR(::fwThread::Timer) createTimer();

    /// Copy constructor forbidden
    WorkerQml( const WorkerQml& );

    /// Copy operator forbidden
    WorkerQml& operator=( const WorkerQml& );

    ::fwThread::ThreadIdType m_threadId;
};

//-----------------------------------------------------------------------------

::fwThread::Worker::sptr getQmlWorker(int& argc, char** argv, bool guiEnabled)
{
    SPTR(WorkerQml) workerQml = std::make_shared< WorkerQml >();
    workerQml->init(argc, argv, guiEnabled);

    return workerQml;
}

//-----------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using Qml.
 */
class TimerQml : public ::fwThread::Timer
{
public:
    /**
     * @brief Constructs a TimerQml from given io_service.
     */
    TimerQml();

    ~TimerQml();

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
        return m_timerQml->isSingleShot();
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_timerQml->setSingleShot(oneShot);
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_timerQml->isActive();
    }

protected Q_SLOTS:

    void call();

protected:

    /// Copy constructor forbidden.
    TimerQml( const TimerQml& );

    /// Copy operator forbidden.
    TimerQml& operator=( const TimerQml& );

    void updatedFunction();

    QPointer< QTimer > m_timerQml;

    QPointer< ::fwGuiQml::util::FuncSlot > m_qmlFunc;
};

//------------------------------------------------------------------------------

// ---------- WorkerQml private implementation ----------

WorkerQml::WorkerQml() :
    m_argc(0),
    m_app(nullptr),
    m_threadId( ::fwThread::getCurrentThreadId() )
{
}

//------------------------------------------------------------------------------

void WorkerQml::init( int& argc, char** argv, bool guiEnabled )
{
    OSLM_TRACE("Init Qml" << ::fwThread::getCurrentThreadId() <<" Start");

    // To get Qml initialized properly, we need to find its plugins
    // This is difficult to do this, especially because the location of the deps is different whether
    // you are executing the application in the build tree or in the install tree
    // Thus the strategy here is to locate the Qml5Core library and then compute the path relatively
    // This work in all cases when we use our binpkgs. If we use the system libraries, the Qml.conf file
    // of the system should do the job and the following might be useless.
    ::boost::filesystem::path qml5LibDir = ::fwTools::os::getSharedLibraryPath("Qt5Core");
    const ::boost::filesystem::path qml5PluginsDir = qml5LibDir.remove_filename() / "qt5" / "plugins";

    QDir pluginDir(QString::fromStdString(qml5PluginsDir.string()));
    if (pluginDir.exists())
    {
        QCoreApplication::setLibraryPaths(QStringList(pluginDir.absolutePath()));
    }
    else
    {
        SLM_ERROR("Could not determine qml5 plugins path, tried with: " + qml5PluginsDir.string());
    }

    m_argc = argc;
    m_app  = QSharedPointer< QGuiApplication > ( new ::fwGuiQml::App( m_argc, argv, guiEnabled ) );

    OSLM_TRACE("Init Qml" << ::fwThread::getCurrentThreadId() <<" Finish");
}

//------------------------------------------------------------------------------

WorkerQml::~WorkerQml()
{
    this->stop();
}

//------------------------------------------------------------------------------

::fwThread::Worker::FutureType WorkerQml::getFuture()
{
    if (!m_future.valid() )
    {
        SLM_ASSERT("WorkerQml loop shall be created and ran from main thread ",
                   !m_future.valid() && ::fwThread::getCurrentThreadId() == this->getThreadId() );

        std::packaged_task< ExitReturnType() > task( std::bind(&QGuiApplication::exec) );

        std::future< ExitReturnType > ufuture = task.get_future();

        m_future = std::move(ufuture);

        task();
    }

    return m_future;
}

//------------------------------------------------------------------------------

::fwThread::ThreadIdType WorkerQml::getThreadId() const
{
    return m_threadId;
}

//------------------------------------------------------------------------------

void WorkerQml::stop()
{
    this->postTask<void>(&QGuiApplication::quit).wait();
}
//------------------------------------------------------------------------------

SPTR(::fwThread::Timer) WorkerQml::createTimer()
{
    return std::make_shared< TimerQml >();
}

//------------------------------------------------------------------------------

void WorkerQml::post(TaskType handler)
{
    QGuiApplication::postEvent( qApp, new WorkerQmlTask(handler) );
}

//------------------------------------------------------------------------------

void WorkerQml::processTasks()
{
    QGuiApplication::sendPostedEvents(0, ::fwGuiQml::WorkerQmlTask::s_WORKER_QML_TASK_EVENT_TYPE);
}

//------------------------------------------------------------------------------

void WorkerQml::processTasks(PeriodType maxtime)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, int(maxtime));
}

// ---------- Timer private implementation ----------

TimerQml::TimerQml() :
    m_timerQml( new QTimer(qApp) )
{
    m_qmlFunc = new ::fwGuiQml::util::FuncSlot();
    QObject::connect(m_timerQml, SIGNAL(timeout()), m_qmlFunc, SLOT(trigger()));
}

//------------------------------------------------------------------------------

TimerQml::~TimerQml()
{
    QObject::disconnect(m_timerQml, SIGNAL(timeout()), m_qmlFunc, SLOT(trigger()));
    delete m_qmlFunc;
    m_timerQml->stop();
    delete m_timerQml;
}

//------------------------------------------------------------------------------

void TimerQml::setDuration(TimeDurationType duration)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerQml->setInterval( static_cast<int>(
                                 std::chrono::duration_cast< std::chrono::milliseconds >(duration).count())
                             );
}

//------------------------------------------------------------------------------

void TimerQml::start()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerQml->start();
}

//------------------------------------------------------------------------------

void TimerQml::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_timerQml->stop();
}

//------------------------------------------------------------------------------

void TimerQml::call()
{
    m_function();
}

//------------------------------------------------------------------------------

void TimerQml::updatedFunction()
{
    m_qmlFunc->setFunction(m_function);
}

} //namespace fwGuiQml
