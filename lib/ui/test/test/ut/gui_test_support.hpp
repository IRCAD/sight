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

#include <ui/test/detail/gui_dispatch.hpp>
#include <ui/test/detail/gui_gateway.hpp>
#include <ui/test/detail/window_input.hpp>
#include <ui/test/qt_application.hpp>

#include <QAbstractButton>
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QGestureEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPinchGesture>
#include <QPoint>
#include <QTest>
#include <QTouchEvent>
#include <QWheelEvent>
#include <QWidget>

#include <chrono>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace sight::ui::test::detail
{

/// What the action threw as an EXCEPTION, empty when it did not throw: a scenario thread hands it to the test thread.
template<typename EXCEPTION = std::runtime_error, typename ACTION>
std::string failure_of(ACTION&& _action)
{
    try
    {
        std::forward<ACTION>(_action)();
    }
    catch(const EXCEPTION& exception)
    {
        return exception.what();
    }

    return {};
}

//------------------------------------------------------------------------------

/// Whether the action threw an EXCEPTION.
template<typename EXCEPTION, typename ACTION>
bool throws(ACTION&& _action)
{
    try
    {
        std::forward<ACTION>(_action)();
    }
    catch(const EXCEPTION&)
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

/// Retry a negative control only when its deadline elapsed before the state under test could be observed.
inline std::string conclusive_failure(
    const std::function<void(std::chrono::milliseconds)>& _operation,
    const std::function<bool(const std::string&)>& _is_conclusive
)
{
    std::string message;
    using std::chrono::milliseconds;
    for(const milliseconds deadline : {milliseconds(200), milliseconds(1000), milliseconds(5000), milliseconds(30000)})
    {
        try
        {
            _operation(deadline);
            return "no timeout";
        }
        catch(const std::runtime_error& exception)
        {
            message = exception.what();
        }

        if(_is_conclusive(message))
        {
            break;
        }
    }

    return message;
}

//------------------------------------------------------------------------------

inline bool keep_running_without_windows()
{
    QApplication::setQuitOnLastWindowClosed(false);
    return true;
}

//------------------------------------------------------------------------------

/// Waits for a modal dialog shown before the event loop runs. Only the loop makes it the active one on a platform.
inline bool wait_for_active_modal(const QWidget* _dialog)
{
    return QTest::qWaitFor([_dialog]{return QApplication::activeModalWidget() == _dialog;}, 30000);
}

//------------------------------------------------------------------------------

struct gui_dispatch_fixture
{
    // The platform of the test runner: offscreen for a unit test, so that no window of a test running in parallel, or
    // of the developer, covers the ones under test; the real one of the system for a GUI test, on a display of its own.
    sight::ui::test::qt_application qt {"ui_test_ut", sight::ui::test::qt_platform::system};
    QApplication& application {qt.get()};
    gui_gateway gateway {application};
    gui_dispatch dispatcher {application, gateway};

    // A test closing its window, as a user would, must not stop the loop under its scenario.
    bool keeps_running {keep_running_without_windows()};
};

class press_counter : public QWidget
{
public:

    using QWidget::QWidget;

    int presses {0};
    int double_clicks {0};

protected:

    //------------------------------------------------------------------------------

    void mousePressEvent(QMouseEvent* /*_event*/) override
    {
        ++presses;
    }

    //------------------------------------------------------------------------------

    void mouseDoubleClickEvent(QMouseEvent* /*_event*/) override
    {
        ++double_clicks;
    }
};

/// Records close requests; with confirm set, asks whether to close with a real modal question, as applications do.
class close_recorder final : public QWidget
{
public:

    int close_events {0};
    bool confirm {false};
    std::function<bool()> closing_under_modal {[]{return false;}};
    bool closed_under_modal {false};

protected:

    //------------------------------------------------------------------------------

    void closeEvent(QCloseEvent* _event) override
    {
        ++close_events;
        closed_under_modal = closing_under_modal();
        if(!confirm)
        {
            QWidget::closeEvent(_event);
            return;
        }

        QMessageBox question(
            QMessageBox::Question,
            "Close",
            "Close the window?",
            QMessageBox::Yes | QMessageBox::No,
            this
        );
        question.button(QMessageBox::No)->setObjectName("keep_open");
        _event->setAccepted(question.exec() == QMessageBox::Yes);
    }
};

//------------------------------------------------------------------------------

class position_recorder : public QWidget
{
public:

    using QWidget::QWidget;

    std::vector<QPoint> positions;

protected:

    //------------------------------------------------------------------------------

    void mousePressEvent(QMouseEvent* _event) override
    {
        positions.push_back(_event->pos());
        _event->accept();
    }
};

class drag_recorder : public QWidget
{
public:

    using QWidget::QWidget;

    int m_presses {0};
    int m_moves {0};
    int m_releases {0};
    QPoint m_last_position;

protected:

    //------------------------------------------------------------------------------

    void mousePressEvent(QMouseEvent* _event) override
    {
        if(_event->button() == Qt::LeftButton)
        {
            ++m_presses;
        }
    }

    //------------------------------------------------------------------------------

    void mouseMoveEvent(QMouseEvent* _event) override
    {
        if(_event->buttons().testFlag(Qt::LeftButton))
        {
            ++m_moves;
            m_last_position = _event->position().toPoint();
        }
    }

    //------------------------------------------------------------------------------

    void mouseReleaseEvent(QMouseEvent* _event) override
    {
        if(_event->button() == Qt::LeftButton)
        {
            ++m_releases;
        }
    }
};

class wheel_recorder : public QWidget
{
public:

    using QWidget::QWidget;

    int m_wheel_events {0};
    QPoint m_angle_delta;
    Qt::ScrollPhase m_phase {Qt::NoScrollPhase};
    std::function<void()> m_on_wheel;

protected:

    //------------------------------------------------------------------------------

    void wheelEvent(QWheelEvent* _event) override
    {
        ++m_wheel_events;
        m_angle_delta = _event->angleDelta();
        m_phase       = _event->phase();
        if(m_on_wheel)
        {
            m_on_wheel();
        }

        _event->accept();
    }
};

class touch_recorder : public QWidget
{
public:

    using QWidget::QWidget;

    int m_touch_begins {0};
    int m_touch_updates {0};
    int m_touch_ends {0};
    int m_last_point_count {0};

protected:

    //------------------------------------------------------------------------------

    bool event(QEvent* _event) override
    {
        switch(_event->type())
        {
            case QEvent::TouchBegin:
                ++m_touch_begins;
                break;

            case QEvent::TouchUpdate:
                ++m_touch_updates;
                break;

            case QEvent::TouchEnd:
                ++m_touch_ends;
                break;

            default:
                return QWidget::event(_event);
        }

        m_last_point_count = static_cast<int>(static_cast<QTouchEvent*>(_event)->points().size());
        _event->accept();
        return true;
    }
};

/// Grabs the pinch gesture without accepting touch events, as Sight's 3D view does.
class pinch_recorder : public QWidget
{
public:

    explicit pinch_recorder(QWidget* _parent) :
        QWidget(_parent)
    {
        grabGesture(Qt::PinchGesture);
    }

    bool m_finished {false};
    qreal m_total_scale_factor {1.0};

protected:

    //------------------------------------------------------------------------------

    bool event(QEvent* _event) override
    {
        if(_event->type() == QEvent::Gesture)
        {
            auto* const gesture_event = static_cast<QGestureEvent*>(_event);
            if(auto* const pinch = static_cast<QPinchGesture*>(gesture_event->gesture(Qt::PinchGesture));
               pinch != nullptr)
            {
                m_total_scale_factor = pinch->totalScaleFactor();
                m_finished           = pinch->state() == Qt::GestureFinished;
            }

            return true;
        }

        return QWidget::event(_event);
    }
};

class key_recorder : public QLineEdit
{
public:

    using QLineEdit::QLineEdit;

    int m_key_presses {0};
    int m_key_releases {0};
    Qt::Key m_last_key {Qt::Key_unknown};

protected:

    //------------------------------------------------------------------------------

    void keyPressEvent(QKeyEvent* _event) override
    {
        ++m_key_presses;
        m_last_key = static_cast<Qt::Key>(_event->key());
        QLineEdit::keyPressEvent(_event);
    }

    //------------------------------------------------------------------------------

    void keyReleaseEvent(QKeyEvent* _event) override
    {
        ++m_key_releases;
        QLineEdit::keyReleaseEvent(_event);
    }
};

//------------------------------------------------------------------------------

template<typename Fixture, typename F>
void run_scenario(Fixture& _fixture, F&& _scenario)
{
    std::exception_ptr exception;
    std::thread scenario(
        [&]
            {
        try
        {
            std::forward<F>(_scenario)();
        }
        catch(...)
        {
            exception = std::current_exception();
        }

        // Not quit(): it closes every window first, which reopens a closing confirmation and is cancelled by a
        // window that refuses to close. exit() also leaves the loop of a dialog left open.
        QMetaObject::invokeMethod(
            &_fixture.application,
            []{QCoreApplication::exit(0);},
            Qt::QueuedConnection
        );
            });

    QApplication::exec();
    scenario.join();

    if(exception)
    {
        std::rethrow_exception(exception);
    }
}

//------------------------------------------------------------------------------

/// The deadline of one operation: it only bounds a hang, which a loaded machine never reaches by accident, and a
/// passing test does not wait for it.
inline gui_deadline operation_deadline()
{
    return std::chrono::steady_clock::now() + std::chrono::seconds(30);
}

//------------------------------------------------------------------------------

/// Retries _condition on the GUI thread until _deadline; a last call the deadline cut is a condition that did not hold.
template<typename F>
bool wait_for(gui_dispatch_fixture& _fixture, gui_deadline _deadline, F _condition)
{
    while(std::chrono::steady_clock::now() < _deadline)
    {
        try
        {
            if(_fixture.gateway.call_until(_deadline, _condition))
            {
                return true;
            }
        }
        catch(const std::runtime_error&)
        {
            if(!_fixture.gateway.is_loop_running() || std::chrono::steady_clock::now() < _deadline)
            {
                throw;
            }

            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return false;
}

//------------------------------------------------------------------------------

inline bool wait_for_exposure(gui_dispatch_fixture& _fixture, QWidget& _window, gui_deadline _deadline)
{
    return wait_for(
        _fixture,
        _deadline,
        [&_window]
        {
            if(!_window.isVisible() || _window.windowHandle() == nullptr || !_window.windowHandle()->isExposed())
            {
                return false;
            }

            // A window manager may open a window behind another one: input injected there would hit that one.
            if(QApplication::topLevelAt(_window.mapToGlobal(_window.rect().center())) == &_window)
            {
                return true;
            }

            _window.raise();
            _window.activateWindow();
            return false;
        });
}

/// Without a window manager, as under Xvfb, nothing activates a window: a test sending keys asks for it, as the engine
/// does.
inline bool wait_for_active_window(gui_dispatch_fixture& _fixture, QWidget& _window, gui_deadline _deadline)
{
    return wait_for(
        _fixture,
        _deadline,
        [&_window]
        {
            if(!_window.isActiveWindow())
            {
                _window.activateWindow();
            }

            return _window.isActiveWindow();
        });
}

//------------------------------------------------------------------------------

/// Runs _input on the application thread until it succeeds, trying again while it fails retryably, as the scenario
/// action layer will.
template<typename F>
bool retry_input(gui_dispatch_fixture& _fixture, gui_deadline _deadline, F _input)
{
    return wait_for(
        _fixture,
        _deadline,
        [&_input]
        {
            try
            {
                _input();
                return true;
            }
            catch(const input_failure& failure)
            {
                if(!failure.retryable())
                {
                    throw;
                }

                return false;
            }
        });
}

//------------------------------------------------------------------------------

} // namespace sight::ui::test::detail
