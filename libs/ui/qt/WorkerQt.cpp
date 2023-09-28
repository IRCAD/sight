/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/qt/WorkerQt.hpp"

#include "ui/qt/util/FuncSlot.hpp"

#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/os.hpp>

#include <QApplication>
#include <QDir>
#include <QEvent>
#include <QPointer>
#include <QStringList>
#include <QTimer>

#include <functional>

namespace sight::ui::qt
{

class WorkerQtTask : public QEvent
{
public:

    explicit WorkerQtTask(core::thread::worker::task_t handler) :
        QEvent(static_cast<QEvent::Type>(s_WORKER_QT_TASK_EVENT_TYPE)),
        m_handler(std::move(handler))
    {
        SIGHT_ASSERT("Application should be instantiated", QCoreApplication::instance());
    }

    ~WorkerQtTask() override
    {
        m_handler();
    }

    static const int s_WORKER_QT_TASK_EVENT_TYPE;

private:

    core::thread::worker::task_t m_handler;
};

const int WorkerQtTask::s_WORKER_QT_TASK_EVENT_TYPE = QEvent::registerEventType();

/**
 * @brief Private implementation of core::thread::worker using boost::asio.
 */
class WorkerQt : public core::thread::worker
{
public:

    WorkerQt();

    /// Copy constructor forbidden
    WorkerQt(const WorkerQt&) = delete;

    /// Copy operator forbidden
    WorkerQt& operator=(const WorkerQt&) = delete;

    void init(int& argc, char** argv);

    ~WorkerQt() override;

    void stop() override;

    void post(task_t handler) override;

    void setApp(QSharedPointer<QCoreApplication> app, const std::string& name, const std::string& version);

    core::thread::worker::future_t get_future() override;

    core::thread::thread_id_t get_thread_id() const override;

    void process_tasks() override;

    void process_tasks(period_t maxtime) override;

private:

    int m_argc {0};
    char** m_argv {};

    QSharedPointer<QCoreApplication> m_app;

    SPTR(core::thread::timer) create_timer() override;

    core::thread::thread_id_t m_threadId;
};

//-----------------------------------------------------------------------------

core::thread::worker::sptr getQtWorker(
    int& argc,
    char** argv,
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback,
    const std::string& name,
    const std::string& version
)
{
    SPTR(WorkerQt) workerQt = std::make_shared<WorkerQt>();
    workerQt->init(argc, argv);
    workerQt->setApp(callback(argc, argv), name, version);
    return workerQt;
}

//-----------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using Qt.
 */
class TimerQt : public core::thread::timer
{
public:

    /**
     * @brief Constructs a TimerQt from given io_service.
     */
    TimerQt();

    /// Copy constructor forbidden.
    TimerQt(const TimerQt&) = delete;

    /// Copy operator forbidden.
    TimerQt& operator=(const TimerQt&) = delete;

    ~TimerQt() override;

    /// Starts or restarts the timer.
    void start() override;

    /// Stops the timer and cancel all pending operations.
    void stop() override;

    /// Sets time duration.
    void set_duration(time_duration_t duration) override;

    /// Returns if the timer mode is 'one shot'.
    bool is_one_shot() const override
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_timerQt->isSingleShot();
    }

    /// Sets timer mode.
    void set_one_shot(bool oneShot) override
    {
        core::mt::scoped_lock lock(m_mutex);
        m_timerQt->setSingleShot(oneShot);
    }

    /// Returns true if the timer is currently running.
    bool is_running() const override
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_timerQt->isActive();
    }

protected Q_SLOTS:

    void call();

private:

    void updated_function() override;

    QPointer<QTimer> m_timerQt;

    QPointer<ui::qt::util::FuncSlot> m_qtFunc;
};

//------------------------------------------------------------------------------

// ---------- WorkerQt private implementation ----------

WorkerQt::WorkerQt() :

    m_app(nullptr),
    m_threadId(core::thread::get_current_thread_id())
{
}

//------------------------------------------------------------------------------

void WorkerQt::init(int& argc, char** argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#ifdef WIN32
    // To get Qt initialized properly, we need to find its plugins
    // This is difficult to do, especially because the location of the deps is different whether
    // you are executing the application in the build tree or in the install tree
    // Thus the strategy here is to locate the Qt5Core library and then compute the path relatively
    // This work in all cases when we use VCPkg.
    std::filesystem::path qt5LibDir           = core::tools::os::get_shared_library_path("Qt5Core").remove_filename();
    const std::filesystem::path qt5PluginsDir = (qt5LibDir.parent_path().parent_path()) / "plugins";

    QDir pluginDir(QString::fromStdString(qt5PluginsDir.string()));
    if(pluginDir.exists())
    {
        SIGHT_INFO("Load Qt5 plugins path from: " + qt5PluginsDir.string());
        QCoreApplication::setLibraryPaths(QStringList(pluginDir.absolutePath()));
    }
    else
    {
        SIGHT_ERROR("Could not determine qt5 plugins path, tried with: " + qt5PluginsDir.string());
    }
#endif

    m_argc = argc;
    m_argv = argv;
}

//------------------------------------------------------------------------------

void WorkerQt::setApp(QSharedPointer<QCoreApplication> app, const std::string& name, const std::string& version)
{
    m_app = std::move(app);
    QCoreApplication::setOrganizationName("IRCAD");
    QCoreApplication::setOrganizationDomain("https://www.ircad.fr/");
    QCoreApplication::setApplicationName(QString::fromStdString(name));
    QCoreApplication::setApplicationVersion(QString::fromStdString(version));
}

//------------------------------------------------------------------------------

WorkerQt::~WorkerQt()
{
    this->WorkerQt::stop();
}

//------------------------------------------------------------------------------

core::thread::worker::future_t WorkerQt::get_future()
{
    if(!m_future.valid())
    {
        SIGHT_ASSERT(
            "WorkerQt loop shall be created and ran from main thread ",
            !m_future.valid() && core::thread::get_current_thread_id() == this->get_thread_id()
        );

        std::packaged_task<exit_return_type()> task([]{return QCoreApplication::exec();});

        std::future<exit_return_type> future = task.get_future();

        m_future = std::move(future);

        task();
    }

    return m_future;
}

//------------------------------------------------------------------------------

core::thread::thread_id_t WorkerQt::get_thread_id() const
{
    return m_threadId;
}

//------------------------------------------------------------------------------

void WorkerQt::stop()
{
    this->post_task<void>(&QCoreApplication::quit).wait();
}

//------------------------------------------------------------------------------

SPTR(core::thread::timer) WorkerQt::create_timer()
{
    return std::make_shared<TimerQt>();
}

//------------------------------------------------------------------------------

void WorkerQt::post(task_t handler)
{
    QCoreApplication::postEvent(QCoreApplication::instance(), new WorkerQtTask(handler));
}

//------------------------------------------------------------------------------

void WorkerQt::process_tasks()
{
    QCoreApplication::sendPostedEvents(nullptr, ui::qt::WorkerQtTask::s_WORKER_QT_TASK_EVENT_TYPE);
}

//------------------------------------------------------------------------------

void WorkerQt::process_tasks(period_t maxtime)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, int(maxtime));
}

// ---------- Timer private implementation ----------

TimerQt::TimerQt() :
    m_timerQt(new QTimer(QCoreApplication::instance())),
    m_qtFunc(new ui::qt::util::FuncSlot())
{
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

void TimerQt::set_duration(time_duration_t duration)
{
    core::mt::scoped_lock lock(m_mutex);
    m_timerQt->setInterval(
        static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(duration).count())
    );
}

//------------------------------------------------------------------------------

void TimerQt::start()
{
    core::mt::scoped_lock lock(m_mutex);
    m_timerQt->start();
}

//------------------------------------------------------------------------------

void TimerQt::stop()
{
    core::mt::scoped_lock lock(m_mutex);
    m_timerQt->stop();
}

//------------------------------------------------------------------------------

void TimerQt::call()
{
    m_function();
}

//------------------------------------------------------------------------------

void TimerQt::updated_function()
{
    m_qtFunc->set_function(m_function);
}

} //namespace sight::ui::qt
