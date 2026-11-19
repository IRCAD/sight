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

#pragma once

#include "ui/test/detail/deadline.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QMetaObject>
#include <QPointer>
#include <QWidget>
#include <QWindow>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace sight::ui::test::detail
{

/// Runs _query on the GUI thread; nothing once _deadline passes or the event loop stops, which would never run it.
template<typename T>
inline std::optional<T> query_gui_thread(
    std::function<T()> _query,
    std::chrono::steady_clock::time_point _deadline,
    const std::atomic_bool& _stop
)
{
    // Shared: the call may still be queued once this function has given up.
    auto result = std::make_shared<std::promise<T> >();
    auto future = result->get_future();
    QMetaObject::invokeMethod(
        qApp,
        [result, query = std::move(_query)]{result->set_value(query());},
        Qt::QueuedConnection
    );
    while(future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready)
    {
        if(_stop || std::chrono::steady_clock::now() >= _deadline)
        {
            return std::nullopt;
        }
    }

    return future.get();
}

//------------------------------------------------------------------------------

/// How long the GUI thread may take to answer a look: the application starts on it, as slowly as the machine is loaded.
/// It only bounds a hang, so a scale below 1 never shortens it.
inline std::chrono::steady_clock::duration answer_timeout()
{
    constexpr auto timeout = std::chrono::minutes(2);
    return std::max<std::chrono::steady_clock::duration>(timeout, scaled(timeout));
}

//------------------------------------------------------------------------------

/// The visible main window, once the platform has exposed it. Its 30 s start with the first answer of the GUI thread,
/// which starts the application first.
inline QPointer<QWidget> wait_for_main_window(const std::atomic_bool& _loop_stopped)
{
    struct sighting
    {
        QPointer<QWidget> window;
        bool exposed {false};
    };

    std::optional<std::chrono::steady_clock::time_point> deadline;
    bool shown = false;
    while(!_loop_stopped && (!deadline || std::chrono::steady_clock::now() < *deadline))
    {
        const auto seen = query_gui_thread<sighting>(
            []
            {
                for(QWidget* const widget : QApplication::topLevelWidgets())
                {
                    if(qobject_cast<QMainWindow*>(widget) != nullptr && widget->isVisible())
                    {
                        const QWindow* const handle = widget->windowHandle();
                        return sighting {.window    = widget, .exposed = handle != nullptr && handle->isExposed()};
                    }
                }

                return sighting {};
            },
            std::chrono::steady_clock::now() + answer_timeout(),
            _loop_stopped
        );
        if(!seen)
        {
            if(_loop_stopped)
            {
                break;
            }

            const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(answer_timeout()).count();
            throw std::runtime_error(
                      "the GUI thread did not answer for " + std::to_string(seconds)
                      + " s while the application started"
            );
        }

        if(!deadline)
        {
            deadline = deadline_after(std::chrono::seconds(30));
        }

        if(seen->window != nullptr)
        {
            if(seen->exposed)
            {
                return seen->window;
            }

            shown = true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    throw std::runtime_error(shown ? "the main window was never exposed" : "the main window never showed up");
}

} // namespace sight::ui::test::detail
