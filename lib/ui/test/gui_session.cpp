/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "gui_session.hpp"

#include "detail/locator.hpp"
#include "detail/main_window.hpp"

#include "scenario.hpp"

#include <core/log/test_log.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/spy_log.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/preferences.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QMetaObject>
#include <QPointer>
#include <QWidget>
#include <QWindow>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace sight::ui::test
{

using clock = std::chrono::steady_clock;

//------------------------------------------------------------------------------

/// What a scenario failed with; a doctest assertion is not a std::exception, and doctest reports its own text.
static std::string failure_text(const std::exception_ptr& _failure)
{
    try
    {
        std::rethrow_exception(_failure);
    }
    catch(const std::exception& exception)
    {
        return exception.what();
    }
    catch(...)
    {
        return "the scenario stopped on a failed assertion";
    }
}

//------------------------------------------------------------------------------

/// Closes the windows, then stops the event loop. Returns why the application did not close, or nothing. A blocked
/// teardown would only end at the CTest timeout: each wait is bounded, and ends once the event loop stops.
static std::string close_application(
    const QPointer<QWidget>& _main_window,
    bool _failed,
    const std::atomic_bool& _stop
)
{
    const auto close_deadline = clock::now() + std::chrono::seconds(30);

    // Closing can open a dialog (unsaved changes, exit confirmation) whose nested loop waits for a user: the close is
    // posted, not run inside a waiting GUI call, and such a dialog fails the test.
    struct close_state
    {
        std::atomic_bool started {false};
        std::atomic_bool returned {false};
    };

    auto state = std::make_shared<close_state>();
    QMetaObject::invokeMethod(
        qApp,
        [state, _main_window]
        {
            state->started = true;
            if(_main_window != nullptr)
            {
                _main_window->hide();
            }

            QApplication::closeAllWindows();
            state->returned = true;
        },
        Qt::QueuedConnection
    );

    // While the close runs, a query can only run in a nested loop of the close.
    std::optional<std::string> blocking_dialog;
    bool closed = false;
    while(!closed && !blocking_dialog && !_stop && clock::now() < close_deadline)
    {
        blocking_dialog = detail::query_gui_thread<std::optional<std::string> >(
            [state]() -> std::optional<std::string>
            {
                QWidget* const modal = QApplication::activeModalWidget();
                if(state->started && !state->returned && modal != nullptr)
                {
                    return modal->windowTitle().toStdString();
                }

                return std::nullopt;
            },
            close_deadline,
            _stop
        ).value_or(std::nullopt);
        closed = state->returned;
        if(!closed && !blocking_dialog)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::string failure;
    if(!_stop && (!closed || blocking_dialog))
    {
        failure = blocking_dialog
                  ? "Closing the application opened '" + *blocking_dialog + "', which no scenario step answered"
                  : std::string("The application did not close within 30 s");
    }

    // Unlike ui::application::exit(), QCoreApplication::exit() asks for no confirmation, and it also leaves the nested
    // loop of a dialog left open.
    const int return_code = _failed || !failure.empty() ? 1 : 0;
    const auto exit_loop  = [return_code]
                            {
                                QCoreApplication::exit(return_code);
                                return true;
                            };
    static_cast<void>(detail::query_gui_thread<bool>(exit_loop, clock::now() + std::chrono::seconds(30), _stop));
    return failure;
}

namespace
{

/// The scenario thread, joined once the profile has stopped, which it waits for: the process ends if it never does.
class scenario_thread
{
public:

    explicit scenario_thread(std::function<void(const std::atomic_bool&)> _run) :
        m_quit(
            QObject::connect(
                qApp,
                &QCoreApplication::aboutToQuit,
                [loop_stopped = m_loop_stopped]{*loop_stopped = true;})),
        m_thread(
            [this, run = std::move(_run)]
            {
                run(*m_loop_stopped);
                wait_for_stop();
            })
    {
    }

    ~scenario_thread()
    {
        QObject::disconnect(m_quit);
        {
            std::scoped_lock lock(m_mutex);
            m_stopped = true;
        }

        m_condition.notify_one();
        m_thread.join();
    }

    scenario_thread(const scenario_thread&)            = delete;
    scenario_thread(scenario_thread&&)                 = delete;
    scenario_thread& operator=(const scenario_thread&) = delete;
    scenario_thread& operator=(scenario_thread&&)      = delete;

private:

    //------------------------------------------------------------------------------

    void wait_for_stop()
    {
        std::unique_lock lock(m_mutex);
        if(!m_condition.wait_for(lock, std::chrono::minutes(2), [this]{return m_stopped;}))
        {
            // std::exit() would destroy static objects that the GUI thread still uses.
            std::cerr << "\n---- the application did not stop, the process ends ----" << std::endl;
            std::_Exit(EXIT_FAILURE);
        }
    }

    std::shared_ptr<std::atomic_bool> m_loop_stopped {std::make_shared<std::atomic_bool>(false)};
    QMetaObject::Connection m_quit;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stopped {false};
    std::thread m_thread;
};

} // namespace

//------------------------------------------------------------------------------

std::filesystem::path output_root()
{
    const std::string variable       = qEnvironmentVariable("IMAGE_OUTPUT_PATH").toStdString();
    const std::filesystem::path root = variable.empty()
                                       ? std::filesystem::temp_directory_path() / "sight_ui_test"
                                       : std::filesystem::path(variable);

    // Two `_uit` binaries may share a scenario name.
    return root / boost::dll::program_location().stem().string();
}

//------------------------------------------------------------------------------

gui_session::gui_session(const std::filesystem::path& _profile_path)
{
    // A scenario never reads nor writes the preferences of the user, not even while its modules start.
    ui::preferences::ignore_filesystem(true);

    core::runtime::init();

    // The modules of the application, next to its profile.
    const auto modules = _profile_path.parent_path().parent_path();
    if(std::filesystem::is_directory(modules))
    {
        core::runtime::add_modules(modules);
    }
    else
    {
        SIGHT_ERROR("Module path " << modules << " does not exist or is not a directory.");
    }

    m_profile = core::runtime::io::profile_reader::create_profile(_profile_path);
    m_profile->start();
}

//------------------------------------------------------------------------------

gui_session::~gui_session() = default;

//------------------------------------------------------------------------------

std::string gui_session::run(const std::string& _name, const std::function<void(scenario&)>& _body)
{
    auto* const application = qobject_cast<QApplication*>(QCoreApplication::instance());
    if(application == nullptr)
    {
        throw std::logic_error("A GUI scenario requires a running QApplication");
    }

    // Every dialog opened by a scenario must be routable through Qt's real input handling.
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    // One output folder per execution: two runs at once never mix their artifacts.
    const std::string execution = core::log::test_log::execution_id();
    const auto output           = output_root() / _name
                                  / (execution.empty() ? core::tools::uuid::generate() : execution);
    scenario ui(*application, detail::locator_catalog::from_loaded_modules(), output);
    std::exception_ptr failure;
    std::string close_failure;
    {
        const scenario_thread thread(
            [&](const std::atomic_bool& _loop_stopped)
                {
            // Before the teardown, while the application still shows the failure.
            const auto report = [&]
                    {
                                failure          = std::current_exception();
                                m_failure_report = ui.report_failure(output, failure_text(failure));
                                std::cerr << m_failure_report << std::endl;
                    };
            QPointer<QWidget> main_window;
            try
            {
                main_window = detail::wait_for_main_window(_loop_stopped);
                ui.prepare_window(main_window);
                _body(ui);
            }
            catch(...)
            {
                report();
            }

            // A modal dialog the scenario left open is closed before the application, a failure or not.
            try
            {
                ui.finish();
            }
            catch(...)
            {
                // The first failure is the one reported: a later one only follows from it.
                if(failure == nullptr)
                {
                    report();
                }
                else
                {
                    std::cerr << "finish also failed: " << failure_text(std::current_exception()) << std::endl;
                }
            }

            close_failure = close_application(main_window, failure != nullptr, _loop_stopped);
                });
        m_profile->run();
        m_profile->stop();
    }

    // Rethrown on the test thread, where doctest reports it.
    if(failure != nullptr)
    {
        std::rethrow_exception(failure);
    }

    return close_failure;
}

} // namespace sight::ui::test
