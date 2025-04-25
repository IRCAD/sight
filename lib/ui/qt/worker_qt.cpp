/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

class worker_qt_task : public QEvent
{
public:

    explicit worker_qt_task(core::thread::worker::task_t _handler) :
        QEvent(static_cast<QEvent::Type>(WORKER_QT_TASK_EVENT_TYPE)),
        m_handler(std::move(_handler))
    {
        SIGHT_ASSERT("application should be instantiated", QCoreApplication::instance());
    }

    ~worker_qt_task() override
    {
        m_handler();
    }

    static const int WORKER_QT_TASK_EVENT_TYPE;

private:

    core::thread::worker::task_t m_handler;
};

const int worker_qt_task::WORKER_QT_TASK_EVENT_TYPE = QEvent::registerEventType();

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

    void set_app(QSharedPointer<QCoreApplication> _app, const std::string& _name, const std::string& _version);

    core::thread::worker::future_t get_future() override;

    [[nodiscard]] core::thread::thread_id_t get_thread_id() const override;

    void set_thread_name(const std::string& _thread_name) override;

    void process_tasks() override;

    void process_tasks(period_t _maxtime) override;

private:

    int m_argc {0};
    char** m_argv {};

    QSharedPointer<QCoreApplication> m_app;

    SPTR(core::thread::timer) create_timer() override;

    core::thread::thread_id_t m_thread_id;
    core::thread::thread_native_id_t m_thread_native_id;
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
    worker->set_app(_callback(_argc, _argv), _name, _version);
    return worker;
}

//-----------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using Qt.
 */
class timer_qt : public core::thread::timer
{
public:

    /**
     * @brief Constructs a TimerQt from given io_service.
     */
    timer_qt();

    /// Copy constructor forbidden.
    timer_qt(const timer_qt&) = delete;

    /// Copy operator forbidden.
    timer_qt& operator=(const timer_qt&) = delete;

    ~timer_qt() override;

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
        return m_timer_qt->isSingleShot();
    }

    /// Sets timer mode.
    void set_one_shot(bool _one_shot) override
    {
        core::mt::scoped_lock lock(m_mutex);
        m_timer_qt->setSingleShot(_one_shot);
    }

    /// Returns true if the timer is currently running.
    bool is_running() const override
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_timer_qt->isActive();
    }

protected Q_SLOTS:

    void call();

private:

    void updated_function() override;

    QPointer<QTimer> m_timer_qt;

    QPointer<ui::qt::util::func_slot> m_qt_func;
};

//------------------------------------------------------------------------------

// ---------- worker_qt private implementation ----------

worker_qt::worker_qt() :

    m_app(nullptr),
    m_thread_id(core::thread::get_current_thread_id()),
    m_thread_native_id(core::thread::get_current_thread_native_id())
{
    worker_qt::set_thread_name("QT_WORKER");
}

//------------------------------------------------------------------------------

void worker_qt::init(int& _argc, char** _argv)
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#ifdef WIN32
    // To get Qt initialized properly, we need to find its plugins
    // This is difficult to do, especially because the location of the deps is different whether
    // you are executing the application in the build tree or in the install tree
    // Thus the strategy here is to locate the Qt6Core library and then compute the path relatively
    // This work in all cases when we use VCPkg.
    const auto qt6_core_path = core::tools::os::get_shared_library_path("Qt6Core");
    SIGHT_ASSERT("Could not find Qt6Core library", std::filesystem::is_regular_file(qt6_core_path));

    // This is the installed Qt 6 plugins path
    const auto qt6_root_path    = qt6_core_path.parent_path().parent_path();
    const auto qt6_plugins_path = qt6_root_path / "plugins";

    if(QDir plugins_dir(qt6_plugins_path); plugins_dir.exists())
    {
        SIGHT_INFO("Load Qt6 plugins path from: " + qt6_plugins_path.string());
        QCoreApplication::addLibraryPath(plugins_dir.canonicalPath());
    }
    else
    {
        // Try the vcpkg path
        const auto vcpkg_qt6_plugins_path = qt6_root_path / "Qt6" / "plugins";

        if(QDir vcpkg_plugins_dir(vcpkg_qt6_plugins_path); vcpkg_plugins_dir.exists())
        {
            SIGHT_INFO("Load Qt6 plugins path from: " + vcpkg_qt6_plugins_path.string());
            QCoreApplication::addLibraryPath(vcpkg_plugins_dir.canonicalPath());
        }
        else
        {
            SIGHT_FATAL(
                "Could not determine qt6 plugins path, tried with `"
                + qt6_plugins_path.string()
                + "` and `"
                + vcpkg_qt6_plugins_path.string()
                + "`"
            );
        }
    }
#endif

    m_argc = _argc;
    m_argv = _argv;
}

//------------------------------------------------------------------------------

void worker_qt::set_app(QSharedPointer<QCoreApplication> _app, const std::string& _name, const std::string& _version)
{
    m_app = std::move(_app);
    QCoreApplication::setOrganizationName(SIGHT_APP_VENDOR);
    QCoreApplication::setOrganizationDomain(SIGHT_APP_VENDOR_URL);
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
    return m_thread_id;
}

//------------------------------------------------------------------------------

void worker_qt::set_thread_name([[maybe_unused]] const std::string& _thread_name)
{
#ifdef _WIN32
    core::thread::set_thread_name(_thread_name, m_thread_native_id);
#else
    // Do nothing on non Windows platform, because all threads inherit the program name/current thread name
#endif
}

//------------------------------------------------------------------------------

void worker_qt::stop()
{
    this->post_task<void>(&QCoreApplication::quit).wait();
}

//------------------------------------------------------------------------------

SPTR(core::thread::timer) worker_qt::create_timer()
{
    return std::make_shared<timer_qt>();
}

//------------------------------------------------------------------------------

void worker_qt::post(task_t _handler)
{
    QCoreApplication::postEvent(QCoreApplication::instance(), new worker_qt_task(_handler));
}

//------------------------------------------------------------------------------

void worker_qt::process_tasks()
{
    QCoreApplication::sendPostedEvents(nullptr, ui::qt::worker_qt_task::WORKER_QT_TASK_EVENT_TYPE);
}

//------------------------------------------------------------------------------

void worker_qt::process_tasks(period_t _maxtime)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, int(_maxtime));
}

// ---------- Timer private implementation ----------

timer_qt::timer_qt() :
    m_timer_qt(new QTimer(QCoreApplication::instance())),
    m_qt_func(new ui::qt::util::func_slot())
{
    QObject::connect(m_timer_qt, SIGNAL(timeout()), m_qt_func, SLOT(trigger()));
}

//------------------------------------------------------------------------------

timer_qt::~timer_qt()
{
    QObject::disconnect(m_timer_qt, SIGNAL(timeout()), m_qt_func, SLOT(trigger()));
    delete m_qt_func;
    m_timer_qt->stop();
    delete m_timer_qt;
}

//------------------------------------------------------------------------------

void timer_qt::set_duration(time_duration_t _duration)
{
    core::mt::scoped_lock lock(m_mutex);
    m_timer_qt->setInterval(
        static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count())
    );
}

//------------------------------------------------------------------------------

void timer_qt::start()
{
    core::mt::scoped_lock lock(m_mutex);
    m_timer_qt->start();
}

//------------------------------------------------------------------------------

void timer_qt::stop()
{
    core::mt::scoped_lock lock(m_mutex);
    m_timer_qt->stop();
}

//------------------------------------------------------------------------------

void timer_qt::call()
{
    m_function();
}

//------------------------------------------------------------------------------

void timer_qt::updated_function()
{
    m_qt_func->set_function(m_function);
}

} //namespace sight::ui::qt
