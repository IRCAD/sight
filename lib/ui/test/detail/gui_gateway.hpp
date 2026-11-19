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

#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>
#include <Qt>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace sight::ui::test::detail
{

using gui_deadline = std::chrono::steady_clock::time_point;

enum class call_status : std::uint8_t
{
    queued,
    running,
    completed,
    cancelled
};

/// What a GUI call may hand to another thread: nothing, or a value that owns its data. No reference and no pointer
/// into the GUI, and no QObject, which cannot be moved.
/* *INDENT-OFF* */
template<typename T>
concept gui_result = std::is_void_v<T>
                     || (std::is_object_v<T> && !std::is_pointer_v<T> && std::move_constructible<T>);
/* *INDENT-ON* */

template<typename F>
using gui_call_result_t = std::invoke_result_t<std::decay_t<F>&>;

/// A GUI call that reached its deadline before the GUI thread ran it: it never runs, so it had no effect.
class gui_call_cut final : public std::runtime_error
{
public:

    using std::runtime_error::runtime_error;
};

template<typename T>
struct call_result
{
    std::optional<T> value;
};

template<>
struct call_result<void>
{
};

template<typename T>
struct call_state
{
    std::mutex mutex;
    std::condition_variable changed;
    call_status status {call_status::queued};
    gui_deadline completed_at {};
    call_result<T> result;
    std::exception_ptr exception;
};

/// Dispatches short calls to Qt's application thread without exposing widgets to the caller.
class gui_gateway
{
public:

    explicit gui_gateway(QCoreApplication& _application) :
        m_application(_application),
        m_loop_running(std::make_shared<std::atomic_bool>(true))
    {
        if(QThread::currentThread() != m_application.thread())
        {
            throw std::logic_error("the GUI gateway must be created on the application thread");
        }

        const auto loop_running = m_loop_running;
        QObject::connect(
            &m_application,
            &QCoreApplication::aboutToQuit,
            &m_application,
            [loop_running]
            {
                loop_running->store(false, std::memory_order_release);
            },
            Qt::DirectConnection
        );
    }

    gui_gateway(const gui_gateway&)            = delete;
    gui_gateway& operator=(const gui_gateway&) = delete;

    //------------------------------------------------------------------------------

    [[nodiscard]] bool is_loop_running() const noexcept
    {
        return m_loop_running->load(std::memory_order_acquire);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::shared_ptr<const std::atomic_bool> loop_state() const noexcept
    {
        return m_loop_running;
    }

    /// Runs a value-producing or void call on the application thread before an absolute deadline.
    template<typename F>
    /* *INDENT-OFF* */
    requires std::invocable<std::decay_t<F>&> && gui_result<gui_call_result_t<F> >
    /* *INDENT-ON* */
    // NOLINTNEXTLINE(modernize-use-nodiscard): void calls too.
    gui_call_result_t<F> call_until(gui_deadline _deadline, F&& _function) const
    {
        using function_t = std::decay_t<F>;
        using result_t   = gui_call_result_t<F>;

        if(QThread::currentThread() == m_application.thread())
        {
            throw std::logic_error("GUI calls cannot be made from the application thread");
        }

        if(!m_loop_running->load(std::memory_order_acquire))
        {
            throw std::runtime_error("the application event loop has stopped");
        }

        if(std::chrono::steady_clock::now() >= _deadline)
        {
            throw std::runtime_error("GUI call timed out");
        }

        auto state        = std::make_shared<call_state<result_t> >();
        auto function     = std::make_shared<function_t>(std::forward<F>(_function));
        const bool queued = QMetaObject::invokeMethod(
            &m_application,
            [state, function, _deadline]
            {
                {
                    std::lock_guard lock(state->mutex);
                    if(state->status != call_status::queued)
                    {
                        return;
                    }

                    if(std::chrono::steady_clock::now() >= _deadline)
                    {
                        state->status = call_status::cancelled;
                        state->changed.notify_all();
                        return;
                    }

                    state->status = call_status::running;
                }

                const auto complete =
                    [&state](auto&& _store_result)
                    {
                        {
                            std::lock_guard lock(state->mutex);
                            std::invoke(std::forward<decltype(_store_result)>(_store_result));
                            state->completed_at = std::chrono::steady_clock::now();
                            state->status       = call_status::completed;
                        }

                        state->changed.notify_all();
                    };

                try
                {
                    if constexpr(std::is_void_v<result_t>)
                    {
                        std::invoke(*function);
                        complete([]{});
                    }
                    else
                    {
                        auto result = std::invoke(*function);
                        complete([&state, &result]{state->result.value.emplace(std::move(result));});
                    }
                }
                catch(...)
                {
                    const std::exception_ptr exception = std::current_exception();
                    complete([&state, &exception]{state->exception = exception;});
                }
            },
            Qt::QueuedConnection
        );

        if(!queued)
        {
            throw std::runtime_error("could not queue a GUI call");
        }

        std::unique_lock lock(state->mutex);
        while(state->status != call_status::completed && state->status != call_status::cancelled
              && m_loop_running->load(std::memory_order_acquire))
        {
            const auto next_check = std::min(
                _deadline,
                std::chrono::steady_clock::now() + std::chrono::milliseconds(10)
            );
            if(state->changed.wait_until(lock, next_check) == std::cv_status::timeout
               && std::chrono::steady_clock::now() >= _deadline)
            {
                break;
            }
        }

        if(state->status == call_status::completed)
        {
            if(state->completed_at >= _deadline)
            {
                throw std::runtime_error("GUI call timed out");
            }

            if(state->exception)
            {
                std::rethrow_exception(state->exception);
            }

            if constexpr(std::is_void_v<result_t>)
            {
                return;
            }
            else
            {
                return std::move(*state->result.value);
            }
        }

        const bool never_ran = state->status == call_status::queued || state->status == call_status::cancelled;
        if(state->status == call_status::queued)
        {
            state->status = call_status::cancelled;
        }

        if(!m_loop_running->load(std::memory_order_acquire))
        {
            throw std::runtime_error("the application event loop has stopped");
        }

        if(never_ran)
        {
            throw gui_call_cut("GUI call timed out");
        }

        throw std::runtime_error("GUI call timed out");
    }

private:

    QCoreApplication& m_application;
    std::shared_ptr<std::atomic_bool> m_loop_running;
};

} // namespace sight::ui::test::detail
