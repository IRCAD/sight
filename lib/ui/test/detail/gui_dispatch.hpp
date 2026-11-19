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

#include "gui_gateway.hpp"

#include <QApplication>
#include <QMetaObject>
#include <QPointer>
#include <QThread>
#include <QWidget>

#include <chrono>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>
#include <utility>

namespace sight::ui::test::detail
{

enum class dispatch_ack : std::uint8_t
{
    returned,
    modal_opened,
    popup_opened
};

/// Posts one GUI callback and acknowledges it without waiting through the nested event loop of a modal or a popup.
class gui_dispatch
{
public:

    class ticket
    {
    private:

        friend class gui_dispatch;

        mutable std::mutex m_mutex;
        call_status m_status {call_status::queued};
        gui_deadline m_deadline {};
        QPointer<QWidget> m_modal_before;
        QPointer<QWidget> m_popup_before;
        std::exception_ptr m_exception;
    };

    gui_dispatch(QApplication& _application, const gui_gateway& _gateway) :
        m_application(_application),
        m_gateway(_gateway)
    {
    }

    /// Posts _function once to the application thread.
    [[nodiscard]] std::shared_ptr<ticket> post_until(gui_deadline _deadline, std::function<void()> _function) const
    {
        if(!_function)
        {
            throw std::invalid_argument("GUI dispatch requires a callback");
        }

        if(QThread::currentThread() == m_application.thread())
        {
            throw std::logic_error("GUI dispatch cannot be posted from the application thread");
        }

        if(!m_gateway.is_loop_running())
        {
            throw std::runtime_error("the application event loop has stopped");
        }

        if(std::chrono::steady_clock::now() >= _deadline)
        {
            throw std::runtime_error("GUI dispatch timed out");
        }

        auto state = std::make_shared<ticket>();
        state->m_deadline = _deadline;

        const bool queued = QMetaObject::invokeMethod(
            &m_application,
            [state, function = std::move(_function)]() mutable
            {
                {
                    std::scoped_lock lock(state->m_mutex);
                    if(state->m_status != call_status::queued)
                    {
                        return;
                    }

                    if(std::chrono::steady_clock::now() >= state->m_deadline)
                    {
                        state->m_status = call_status::cancelled;
                        return;
                    }

                    // Captured right before the callback, so that only a modal or a popup it opens is its own.
                    state->m_modal_before = QApplication::activeModalWidget();
                    state->m_popup_before = QApplication::activePopupWidget();
                    state->m_status       = call_status::running;
                }

                std::exception_ptr exception;
                try
                {
                    function();
                }
                catch(...)
                {
                    exception = std::current_exception();
                }

                {
                    std::scoped_lock lock(state->m_mutex);
                    state->m_exception = exception;
                    state->m_status    = call_status::completed;
                }
            },
            Qt::QueuedConnection
        );

        if(!queued)
        {
            throw std::runtime_error("could not queue a GUI dispatch");
        }

        return state;
    }

    /// Waits for the callback to return, or acknowledges a new visible modal dialog or popup while it is running: both
    /// run a nested event loop (QDialog::exec(), QMenu::exec()) that only the next input ends.
    [[nodiscard]] dispatch_ack acknowledge_until(const std::shared_ptr<ticket>& _ticket, gui_deadline _deadline) const
    {
        return wait_until(_ticket, _deadline, true);
    }

    /// Waits until the callback has returned, a modal dialog it opened included, and rethrows its exception.
    void wait_returned_until(const std::shared_ptr<ticket>& _ticket, gui_deadline _deadline) const
    {
        static_cast<void>(wait_until(_ticket, _deadline, false));
    }

private:

    //------------------------------------------------------------------------------

    [[nodiscard]] dispatch_ack wait_until(
        const std::shared_ptr<ticket>& _ticket,
        gui_deadline _deadline,
        bool _acknowledge_nested
    ) const
    {
        if(_ticket == nullptr)
        {
            throw std::invalid_argument("GUI dispatch ticket must not be null");
        }

        if(QThread::currentThread() == m_application.thread())
        {
            throw std::logic_error("GUI dispatch cannot be acknowledged from the application thread");
        }

        while(std::chrono::steady_clock::now() < _deadline)
        {
            const call_status status = get_status(*_ticket);
            if(status == call_status::completed)
            {
                rethrow_exception(*_ticket);
                return dispatch_ack::returned;
            }

            if(status == call_status::cancelled)
            {
                throw gui_call_cut("GUI dispatch timed out before it started");
            }

            if(!m_gateway.is_loop_running())
            {
                cancel_if_queued(*_ticket);
                throw std::runtime_error("the application event loop has stopped");
            }

            if(_acknowledge_nested && status == call_status::running)
            {
                std::optional<dispatch_ack> nested;
                try
                {
                    nested = m_gateway.call_until(
                        _deadline,
                        [_ticket]() -> std::optional<dispatch_ack>
                        {
                            const auto is_new = [](const QWidget* _widget, const QPointer<QWidget>& _before)
                                                {
                                                    return _widget != nullptr && _widget != _before.data()
                                                           && _widget->isVisible();
                                                };
                            if(is_new(QApplication::activeModalWidget(), _ticket->m_modal_before))
                            {
                                return dispatch_ack::modal_opened;
                            }

                            if(is_new(QApplication::activePopupWidget(), _ticket->m_popup_before))
                            {
                                return dispatch_ack::popup_opened;
                            }

                            return std::nullopt;
                        });
                }
                catch(const gui_call_cut& cut)
                {
                    // Only the check was cut: the dispatch itself runs.
                    throw std::runtime_error(cut.what());
                }

                if(nested)
                {
                    if(get_status(*_ticket) == call_status::completed)
                    {
                        rethrow_exception(*_ticket);
                        return dispatch_ack::returned;
                    }

                    return *nested;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        const bool never_ran = cancel_if_queued(*_ticket) || get_status(*_ticket) == call_status::cancelled;
        if(!m_gateway.is_loop_running())
        {
            throw std::runtime_error("the application event loop has stopped");
        }

        if(never_ran)
        {
            throw gui_call_cut("GUI dispatch timed out");
        }

        throw std::runtime_error("GUI dispatch timed out");
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] static call_status get_status(const ticket& _ticket)
    {
        std::scoped_lock lock(_ticket.m_mutex);
        return _ticket.m_status;
    }

    //------------------------------------------------------------------------------

    /// True when it cancelled the callback, which then never runs.
    static bool cancel_if_queued(ticket& _ticket)
    {
        std::scoped_lock lock(_ticket.m_mutex);
        if(_ticket.m_status == call_status::queued)
        {
            _ticket.m_status = call_status::cancelled;
            return true;
        }

        return false;
    }

    //------------------------------------------------------------------------------

    static void rethrow_exception(const ticket& _ticket)
    {
        std::exception_ptr exception;
        {
            std::scoped_lock lock(_ticket.m_mutex);
            exception = _ticket.m_exception;
        }

        if(exception)
        {
            std::rethrow_exception(exception);
        }
    }

    QApplication& m_application;
    const gui_gateway& m_gateway;
};

} // namespace sight::ui::test::detail
