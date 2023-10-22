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

#include "ui/qt/worker_qt.hpp"

#include "ui/qt/util/func_slot.hpp"

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

class worker_qtTask : public QEvent
{
public:

    explicit worker_qtTask(core::thread::worker::task_t _handler) :
        QEvent(static_cast<QEvent::Type>(s_WORKER_QT_TASK_EVENT_TYPE)),
        m_handler(std::move(_handler))
    {
        SIGHT_ASSERT("application should be instantiated", QCoreApplication::instance());
    }

    ~worker_qtTask() override
    {
        m_handler();
    }

    static const int s_WORKER_QT_TASK_EVENT_TYPE;

private:

    core::thread::worker::task_t m_handler;
};

const int worker_qtTask::s_WORKER_QT_TASK_EVENT_TYPE = QEvent::registerEventType();

/**
 * @brief Private implementation of core::thread::worker using boost::asio.
 */
class worker_qt : public core::thread::worker
{
public:

    worker_qt();

    /// Copy constructor forbidden
    worker_qt(const worker_qt&) = delete;

    /// Copy operator forbidden
    worker_qt& operator=(const worker_qt&) = delete;

    void init(int& _argc, char** _argv);

    ~worker_qt() override;

    void stop() override;

    void post(task_t _handler) override;

    void setApp(QSharedPointer<QCoreApplication> _app, const std::string& _name, const std::string& _version);

    core::thread::worker::future_t get_future() override;

    core::thread::thread_id_t get_thread_id() const override;

    void process_tasks() override;

    void process_tasks(period_t _maxtime) override;

private:

    int m_argc {0};
    char** m_argv {};

    QSharedPointer<QCoreApplication> m_app;

    SPTR(core::thread::timer) create_timer() override;

    core::thread::thread_id_t m_threadId;
};

//-----------------------------------------------------------------------------

core::thread::worker::sptr get_qt_worker(
    int& _argc,
    char** _argv,
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> _callback,
    const std::string& _name,
    const std::string& _version
)
{
    SPTR(worker_qt) worker = std::make_shared<worker_qt>();
    worker->init(_argc, _argv);
    worker->setApp(_callback(_argc, _argv), _name, _version);
    return worker;
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
    void set_duration(time_duration_t _duration) override;

    /// Returns if the timer mode is 'one shot'.
    bool is_one_shot() const override
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_timerQt->isSingleShot();
    }

    /// Sets timer mode.
    void set_one_shot(bool _one_shot) override
    {
        core::mt::scoped_lock lock(m_mutex);
        m_timerQt->setSingleShot(_one_shot);
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

    QPointer<ui::qt::util::func_slot> m_qtFunc;
};

//------------------------------------------------------------------------------

// ---------- worker_qt private implementation ----------

worker_qt::worker_qt() :

    m_app(nullptr),
    m_threadId(core::thread::get_current_thread_id())
{
}

//------------------------------------------------------------------------------

void worker_qt::init(int& _argc, char** _argv)
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

    m_argc = _argc;
    m_argv = _argv;
}

//------------------------------------------------------------------------------

void worker_qt::setApp(QSharedPointer<QCoreApplication> _app, const std::string& _name, const std::string& _version)
{
    m_app = std::move(_app);
    QCoreApplication::setOrganizationName("IRCAD");
    QCoreApplication::setOrganizationDomain("https://www.ircad.fr/");
    QCoreApplication::setApplicationName(QString::fromStdString(_name));
    QCoreApplication::setApplicationVersion(QString::fromStdString(_version));
}

//------------------------------------------------------------------------------

worker_qt::~worker_qt()
{
    this->worker_qt::stop();
}

//------------------------------------------------------------------------------

core::thread::worker::future_t worker_qt::get_future()
{
    if(!m_future.valid())
    {
        SIGHT_ASSERT(
            "worker_qt loop shall be created and ran from main thread ",
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

core::thread::thread_id_t worker_qt::get_thread_id() const
{
    return m_threadId;
}

//------------------------------------------------------------------------------

void worker_qt::stop()
{
    this->post_task<void>(&QCoreApplication::quit).wait();
}

//------------------------------------------------------------------------------

SPTR(core::thread::timer) worker_qt::create_timer()
{
    return std::make_shared<TimerQt>();
}

//------------------------------------------------------------------------------

void worker_qt::post(task_t _handler)
{
    QCoreApplication::postEvent(QCoreApplication::instance(), new worker_qtTask(_handler));
}

//------------------------------------------------------------------------------

void worker_qt::process_tasks()
{
    QCoreApplication::sendPostedEvents(nullptr, ui::qt::worker_qtTask::s_WORKER_QT_TASK_EVENT_TYPE);
}

//------------------------------------------------------------------------------

void worker_qt::process_tasks(period_t _maxtime)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, int(_maxtime));
}

// ---------- Timer private implementation ----------

TimerQt::TimerQt() :
    m_timerQt(new QTimer(QCoreApplication::instance())),
    m_qtFunc(new ui::qt::util::func_slot())
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

void TimerQt::set_duration(time_duration_t _duration)
{
    core::mt::scoped_lock lock(m_mutex);
    m_timerQt->setInterval(
        static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count())
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
