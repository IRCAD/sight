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

#include "widget_visibility.hpp"

// cspell:ignore Eventv Spontane

#include <QApplication>
#include <QEventPoint>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLineF>
#include <QList>
#include <QMouseEvent>
#include <QPoint>
#include <QPointF>
#include <QPointingDevice>
#include <QString>
#include <QStyleHints>
#include <QThread>
#include <QWheelEvent>
#include <QWidget>
#include <QWindow>

#include <QtGui/qtestsupport_gui.h>

#include <QtTest/qtestkeyboard.h>
#include <QtTest/qtestmouse.h>
#include <QtTest/qtestspontaneevent.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

namespace sight::ui::test::detail
{

/// Input that could not reach its target as a user's would. It is retryable when nothing reached the target (an
/// obstacle, a popup, an inactive window, the focus elsewhere, another receiver): the caller may wait until the
/// condition holds and inject again. Once part of it reached the target, injecting again would repeat that part.
class input_failure : public std::runtime_error
{
public:

    explicit input_failure(const std::string& _what, bool _retryable = true) :
        std::runtime_error(_what),
        m_retryable(_retryable)
    {
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool retryable() const noexcept
    {
        return m_retryable;
    }

private:

    bool m_retryable {true};
};

//------------------------------------------------------------------------------

/// Rethrows _failure as not retryable, since _delivered parts of the input already reached the target.
[[noreturn]] inline void throw_after_partial_input(const input_failure& _failure, int _delivered, const char* _parts)
{
    throw input_failure(
              std::string(_failure.what()) + ", after " + std::to_string(_delivered) + " " + _parts
              + " reached the target",
              false
    );
}

//------------------------------------------------------------------------------

inline std::string describe_object(const QObject* _object)
{
    if(_object == nullptr)
    {
        return "no window";
    }

    const std::string name = _object->objectName().toStdString();
    return name.empty() ? _object->metaObject()->className() : name;
}

//------------------------------------------------------------------------------

/// Advances and returns the time of injected mouse events, in milliseconds, on the application thread. Qt keeps the
/// timestamp of the last press from any source and makes a double click of a press that follows it on the same window
/// within mouseDoubleClickInterval(): QTest's clock is shared, so that its clicks and ours never go back in time.
inline int next_mouse_timestamp(int _advance)
{
    QTest::lastMouseTimestamp += _advance;
    return QTest::lastMouseTimestamp;
}

//------------------------------------------------------------------------------

/// Records the first widget that receives the injected event, as the application event filter sees it.
class receiver_filter : public QObject
{
public:

    receiver_filter(QWidget& _target, std::function<bool(const QEvent&)> _is_injected) :
        m_target(_target),
        m_is_injected(std::move(_is_injected))
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _watched, QEvent* _event) override
    {
        auto* const receiver = qobject_cast<QWidget*>(_watched);
        if(m_recorded || receiver == nullptr || !m_is_injected(*_event))
        {
            return false;
        }

        m_recorded        = true;
        m_received_target = receiver == &m_target || m_target.isAncestorOf(receiver);
        m_receiver_name   = describe_object(receiver);
        return false;
    }

    //------------------------------------------------------------------------------

    /// Throws unless _target, or one of its children, received the injected _input first.
    void verify(const std::string& _input) const
    {
        if(!m_recorded)
        {
            throw input_failure(_input + " was not delivered to '" + describe_object(&m_target) + "'");
        }

        if(!m_received_target)
        {
            throw input_failure(
                      _input + " on '" + describe_object(&m_target) + "' was received by '" + m_receiver_name + "'"
            );
        }
    }

private:

    QWidget& m_target;
    std::function<bool(const QEvent&)> m_is_injected;
    bool m_recorded {false};
    bool m_received_target {false};
    std::string m_receiver_name;
};

//------------------------------------------------------------------------------

/// Matches the press injected with _button at _global_position.
inline std::function<bool(const QEvent&)> is_mouse_press(Qt::MouseButton _button, QPoint _global_position)
{
    return [_button, _global_position](const QEvent& _event)
           {
               // The second press of a double click reaches widgets as a MouseButtonDblClick only.
               if(_event.type() != QEvent::MouseButtonPress && _event.type() != QEvent::MouseButtonDblClick)
               {
                   return false;
               }

               const auto& mouse_event = static_cast<const QMouseEvent&>(_event);
               return mouse_event.button() == _button && mouse_event.globalPosition().toPoint() == _global_position;
           };
}

//------------------------------------------------------------------------------

/// Matches the wheel event injected at _global_position.
inline std::function<bool(const QEvent&)> is_mouse_wheel(QPoint _global_position)
{
    return [_global_position](const QEvent& _event)
           {
               return _event.type() == QEvent::Wheel
                      && static_cast<const QWheelEvent&>(_event).globalPosition().toPoint() == _global_position;
           };
}

//------------------------------------------------------------------------------

/// Matches the key press injected for _key.
inline std::function<bool(const QEvent&)> is_key_press(Qt::Key _key)
{
    return [_key](const QEvent& _event)
           {
               return _event.type() == QEvent::KeyPress
                      && static_cast<const QKeyEvent&>(_event).key() == _key;
           };
}

//------------------------------------------------------------------------------

/// Matches the first event of a touch sequence. A widget that grabs a touch gesture receives it too: the recognizer
/// makes it accept touch events (QPinchGestureRecognizer::create).
inline std::function<bool(const QEvent&)> is_touch_begin()
{
    return [](const QEvent& _event){return _event.type() == QEvent::TouchBegin;};
}

//------------------------------------------------------------------------------

/// Records whether a shortcut event is delivered: a key bound to a shortcut is consumed by Qt's shortcut map and never
/// reaches the focus widget.
class shortcut_recorder : public QObject
{
public:

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* /*_watched*/, QEvent* _event) override
    {
        m_triggered = m_triggered || _event->type() == QEvent::Shortcut;
        return false;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool triggered() const noexcept
    {
        return m_triggered;
    }

private:

    bool m_triggered {false};
};

//------------------------------------------------------------------------------

class event_filter_guard
{
public:

    event_filter_guard(QObject& _application, QObject& _filter) :
        m_application(_application),
        m_filter(_filter)
    {
        m_application.installEventFilter(&m_filter);
    }

    ~event_filter_guard()
    {
        m_application.removeEventFilter(&m_filter);
    }

private:

    QObject& m_application;
    QObject& m_filter;
};

/// Returns the application to inject GUI input into, from its thread.
inline QApplication& input_application()
{
    auto* const application = qobject_cast<QApplication*>(QCoreApplication::instance());
    if(application == nullptr)
    {
        throw std::logic_error("GUI input requires a QApplication");
    }

    if(QThread::currentThread() != application->thread())
    {
        throw std::logic_error("GUI input must be injected on the application thread");
    }

    return *application;
}

//------------------------------------------------------------------------------

inline QWindow& check_exposed_window(QWidget& _target, const char* _input)
{
    QWidget* const target_window  = _target.window();
    QWindow* const window_handle  = target_window->windowHandle();
    const std::string target_name = std::string(_input) + " target '" + describe_object(&_target) + "'";
    if(window_handle == nullptr)
    {
        throw input_failure(target_name + " has no window handle");
    }

    if(!window_handle->isExposed())
    {
        throw input_failure("the window of " + target_name + " is not exposed");
    }

    return *window_handle;
}

//------------------------------------------------------------------------------

/// QObject ownership crosses popup windows; QWidget::isAncestorOf stops at a window boundary.
inline bool is_owned_by(const QObject& _target, const QObject& _owner)
{
    for(const QObject* object = &_target ; object != nullptr ; object = object->parent())
    {
        if(object == &_owner)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

/// Checks that no modal dialog and no popup outside _target's window takes the _input meant for _target.
inline void check_not_blocked(QWidget& _target, const char* _input)
{
    QWidget* const target_window = _target.window();

    QWidget* const modal = QApplication::activeModalWidget();
    if(modal != nullptr && modal != target_window && !is_owned_by(_target, *modal))
    {
        throw input_failure(
                  std::string(_input) + " target is blocked by modal dialog '" + describe_object(modal)
                  + "'"
        );
    }

    QWidget* const popup = QApplication::activePopupWidget();
    if(popup != nullptr && popup != target_window && !popup->isAncestorOf(&_target))
    {
        throw input_failure(std::string(_input) + " target is blocked by popup '" + describe_object(popup) + "'");
    }
}

//------------------------------------------------------------------------------

/// Checks that mouse input at _position would reach _target as a user's input would, and returns its window.
inline QWindow& check_mouse_target(
    QWidget& _target,
    const QPoint& _position
)
{
    if(!_target.isVisible() || !_target.isEnabled())
    {
        throw input_failure("mouse target '" + describe_object(&_target) + "' is hidden or disabled");
    }

    if(!_target.rect().contains(_position))
    {
        throw input_failure("mouse position is outside target '" + describe_object(&_target) + "'");
    }

    QWidget* const target_window = _target.window();
    QWindow& window_handle       = check_exposed_window(_target, "mouse");
    check_not_blocked(_target, "mouse");

    const QPoint global_position = _target.mapToGlobal(_position);
    const std::string at         = "(" + std::to_string(global_position.x()) + ", "
                                   + std::to_string(global_position.y()) + ")";

    // Clipped away first: the hit test would blame whatever lies there instead.
    if(!visible_rect(_target).contains(_position))
    {
        throw input_failure("mouse target '" + describe_object(&_target) + "' is not visible at " + at);
    }

    QWidget* const top_level = QApplication::topLevelAt(global_position);
    if(top_level != target_window)
    {
        const std::string obstacle = top_level == nullptr ? "another application's window" : describe_object(top_level);
        throw input_failure(
                  "mouse target '" + describe_object(&_target) + "' is covered at " + at + " by '" + obstacle + "'"
        );
    }

    // childAt() returns nullptr where the window shows no child: the window itself receives the press there.
    QWidget* const child = target_window->childAt(target_window->mapFromGlobal(global_position));
    QWidget* const hit   = child == nullptr ? target_window : child;
    if(hit != &_target && !_target.isAncestorOf(hit))
    {
        throw input_failure(
                  "mouse target '" + describe_object(&_target) + "' is covered at " + at + " by '"
                  + describe_object(hit) + "'"
        );
    }

    return window_handle;
}

//------------------------------------------------------------------------------

/// Checks that a key would reach _target as a user's key would, and returns the window to send it to. Qt sends keys to
/// the active popup whatever window has the focus, else to the focus widget of the active window
/// (QWidgetWindow::handleKeyEvent).
inline QWindow& check_keyboard_target(QWidget& _target)
{
    if(!_target.isVisible() || !_target.isEnabled())
    {
        throw input_failure("keyboard target '" + describe_object(&_target) + "' is hidden or disabled");
    }

    QWidget* const target_window = _target.window();
    QWindow& window_handle       = check_exposed_window(_target, "keyboard");

    QWidget* receiver = nullptr;
    if(QWidget* const popup = QApplication::activePopupWidget(); popup != nullptr)
    {
        if(popup != target_window)
        {
            throw input_failure("keyboard target is blocked by popup '" + describe_object(popup) + "'");
        }

        receiver = popup->focusWidget() != nullptr ? popup->focusWidget() : popup;
    }
    else
    {
        if(!window_handle.isActive() || QApplication::activeWindow() != target_window)
        {
            throw input_failure("keyboard target window '" + describe_object(target_window) + "' is not active");
        }

        receiver = QApplication::focusWidget();
        if(receiver == nullptr)
        {
            throw input_failure("keyboard target '" + describe_object(&_target) + "' has no focus");
        }
    }

    if(receiver != &_target && !_target.isAncestorOf(receiver))
    {
        throw input_failure(
                  "keyboard target '" + describe_object(&_target) + "' does not have focus; focus is on '"
                  + describe_object(receiver) + "'"
        );
    }

    return window_handle;
}

//------------------------------------------------------------------------------

/// Gives the keyboard focus to _target as a focusing click or Tab would, without their side effects (a click also
/// triggers a button or moves a slider), so that it can be given again after a focus theft. A target a user could not
/// focus is refused, and nothing goes around a modal dialog or a popup that would keep the keys. An inactive window is
/// asked to activate, as a window manager would, and the call fails retryably until it is active: activation is
/// asynchronous, so the caller tries again while it waits.
inline void give_focus(QWidget& _target)
{
    static_cast<void>(input_application());
    if(!_target.isVisible() || !_target.isEnabled())
    {
        throw input_failure("keyboard target '" + describe_object(&_target) + "' is hidden or disabled");
    }

    QWidget* focus_widget = &_target;
    while(focus_widget->focusProxy() != nullptr)
    {
        focus_widget = focus_widget->focusProxy();
    }

    if(focus_widget->focusPolicy() == Qt::NoFocus)
    {
        throw input_failure(
                  "keyboard target '" + describe_object(&_target)
                  + "' cannot take the focus: its focus policy is NoFocus",
                  false
        );
    }

    QWindow& window_handle = check_exposed_window(_target, "keyboard");
    check_not_blocked(_target, "keyboard");

    QWidget* const target_window = _target.window();
    if(QApplication::activePopupWidget() != target_window
       && (!window_handle.isActive() || QApplication::activeWindow() != target_window))
    {
        window_handle.requestActivate();
        throw input_failure(
                  "keyboard target window '" + describe_object(target_window) + "' is not active; activation requested"
        );
    }

    _target.setFocus(Qt::OtherFocusReason);
    static_cast<void>(check_keyboard_target(_target));
}

//------------------------------------------------------------------------------

/// Injects _click_count clicks at the same place through the target window, two making a double click. Each press is
/// checked first, since a click may open a dialog or a popup, or cover or hide the target.
inline void inject_mouse_clicks(
    QWidget& _target,
    const QPoint& _position,
    int _click_count,
    Qt::MouseButton _button          = Qt::LeftButton,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier
)
{
    if(_click_count < 1)
    {
        throw std::invalid_argument("mouse input requires at least one click");
    }

    QApplication& application = input_application();

    const Qt::MouseButtons pressed_buttons(_button);
    for(int click_index = 0 ; click_index < _click_count ; ++click_index)
    {
        try
        {
            QWindow& window_handle       = check_mouse_target(_target, _position);
            const QPoint global_position = _target.mapToGlobal(_position);
            const QPointF local(window_handle.mapFromGlobal(global_position));
            const QPointF global(global_position);

            // The gap keeps separate clicks at the same place separate; the clicks of one call make a double click.
            const int advance = click_index == 0 ? QGuiApplication::styleHints()->mouseDoubleClickInterval() + 1 : 1;

            receiver_filter receiver(_target, is_mouse_press(_button, global_position));
            {
                const event_filter_guard filter_guard(application, receiver);
                qt_handleMouseEvent(
                    &window_handle,
                    local,
                    global,
                    pressed_buttons,
                    _button,
                    QEvent::MouseButtonPress,
                    _modifiers,
                    next_mouse_timestamp(advance)
                );
                qt_handleMouseEvent(
                    &window_handle,
                    local,
                    global,
                    Qt::NoButton,
                    _button,
                    QEvent::MouseButtonRelease,
                    _modifiers,
                    next_mouse_timestamp(1)
                );
            }

            receiver.verify("mouse press");
        }
        catch(const input_failure& failure)
        {
            if(click_index == 0)
            {
                throw;
            }

            throw_after_partial_input(failure, click_index, "click(s)");
        }
    }
}

//------------------------------------------------------------------------------

inline void inject_mouse_click(
    QWidget& _target,
    const QPoint& _position,
    Qt::MouseButton _button          = Qt::LeftButton,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier
)
{
    inject_mouse_clicks(_target, _position, 1, _button, _modifiers);
}

//------------------------------------------------------------------------------

inline void inject_mouse_double_click(
    QWidget& _target,
    const QPoint& _position,
    Qt::MouseButton _button          = Qt::LeftButton,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier
)
{
    inject_mouse_clicks(_target, _position, 2, _button, _modifiers);
}

//------------------------------------------------------------------------------

/// Injects a drag through the target window: a press at _from, one move with the button held, a release at _to. Both
/// positions are in _target's coordinates; _to may lie outside it, the press grabs the mouse as a user's would.
inline void inject_mouse_drag(
    QWidget& _target,
    const QPoint& _from,
    const QPoint& _to,
    Qt::MouseButton _button          = Qt::LeftButton,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier
)
{
    if(_from == _to)
    {
        throw std::invalid_argument("mouse drag requires distinct start and end positions");
    }

    QApplication& application = input_application();

    QWindow& window_handle   = check_mouse_target(_target, _from);
    const QPoint from_global = _target.mapToGlobal(_from);
    const QPoint to_global   = _target.mapToGlobal(_to);
    const QPointF from_local = window_handle.mapFromGlobal(from_global);
    const QPointF to_local   = window_handle.mapFromGlobal(to_global);
    const Qt::MouseButtons state(_button);
    receiver_filter receiver(_target, is_mouse_press(_button, from_global));
    {
        const event_filter_guard filter_guard(application, receiver);
        qt_handleMouseEvent(
            &window_handle,
            from_local,
            from_global,
            state,
            _button,
            QEvent::MouseButtonPress,
            _modifiers,
            next_mouse_timestamp(QGuiApplication::styleHints()->mouseDoubleClickInterval() + 1)
        );
        qt_handleMouseEvent(
            &window_handle,
            to_local,
            to_global,
            state,
            Qt::NoButton,
            QEvent::MouseMove,
            _modifiers,
            next_mouse_timestamp(1)
        );
        qt_handleMouseEvent(
            &window_handle,
            to_local,
            to_global,
            Qt::NoButton,
            _button,
            QEvent::MouseButtonRelease,
            _modifiers,
            next_mouse_timestamp(1)
        );
    }

    receiver.verify("mouse press");
}

//------------------------------------------------------------------------------

/// Injects a two-finger pinch through the target window: both fingers press, move together in straight lines without
/// crossing, and release. Positions are in _target's coordinates; only the presses are checked, since touch points
/// stay grabbed by the widget they pressed.
inline void inject_touch_pinch(
    QWidget& _target,
    const std::pair<QPoint, QPoint>& _first_finger,
    const std::pair<QPoint, QPoint>& _second_finger,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier
)
{
    const QLineF start(_first_finger.first, _second_finger.first);
    const QLineF end(_first_finger.second, _second_finger.second);
    if(start.length() < 1. || end.length() < 1.)
    {
        throw std::invalid_argument("touch pinch requires distinct finger positions");
    }

    QApplication& application = input_application();
    QWindow& window_handle    = check_mouse_target(_target, _first_finger.first);
    static_cast<void>(check_mouse_target(_target, _second_finger.first));

    // Process-wide: Qt keeps registered devices until exit, whatever the application instance.
    static QPointingDevice* const s_DEVICE = QTest::createTouchDevice();

    const auto send =
        [&](const QPointF& _first, const QPointF& _second, QEventPoint::State _state)
        {
            const auto point = [&](int _id, const QPointF& _position)
                               {
                                   const QPointF global = _target.mapToGlobal(_position);
                                   return QEventPoint(_id, _state, window_handle.mapFromGlobal(global), global);
                               };
            // The returned value tells whether a widget accepted the event, not where it went.
            static_cast<void>(qt_handleTouchEventv2(
                                  &window_handle,
                                  s_DEVICE,
                                  {point(0, _first), point(1, _second)},
                                  _modifiers
            ));
        };

    // QPinchGestureRecognizer ignores an update that changes the finger distance more than twofold: each step changes
    // it by at most a quarter of the shorter distance. The first update, at the press positions, starts the gesture.
    const QLineF first_path(_first_finger.first, _first_finger.second);
    const QLineF second_path(_second_finger.first, _second_finger.second);
    const qreal travel       = first_path.length() + second_path.length();
    const qreal longest_step = 0.25 * std::min(start.length(), end.length());
    const int steps          = std::max(1, static_cast<int>(std::ceil(travel / longest_step)));

    receiver_filter receiver(_target, is_touch_begin());
    {
        const event_filter_guard filter_guard(application, receiver);
        send(start.p1(), start.p2(), QEventPoint::State::Pressed);
        for(int step = 0 ; step <= steps ; ++step)
        {
            const qreal progress = static_cast<qreal>(step) / steps;
            send(first_path.pointAt(progress), second_path.pointAt(progress), QEventPoint::State::Updated);
        }

        send(end.p1(), end.p2(), QEventPoint::State::Released);
    }

    receiver.verify("touch pinch");
}

//------------------------------------------------------------------------------

inline void validate_mouse_wheel_notches(int _notches)
{
    if(_notches == 0)
    {
        throw std::invalid_argument("mouse wheel input requires at least one notch");
    }

    constexpr int maximum_notches = 255;
    if(_notches < -maximum_notches || _notches > maximum_notches)
    {
        throw std::invalid_argument("mouse wheel input cannot exceed 255 notches");
    }
}

//------------------------------------------------------------------------------

/// Injects up to 255 wheel notches through the target window, one event of 120 angle units per notch; negative values
/// scroll down. Each event is spontaneous and delivered synchronously, so Qt routes it like a real wheel event.
inline void inject_mouse_wheel(
    QWidget& _target,
    const QPoint& _position,
    int _notches,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier
)
{
    validate_mouse_wheel_notches(_notches);

    QApplication& application = input_application();

    const QPoint angle_delta(0, _notches > 0 ? QWheelEvent::DefaultDeltasPerStep : -QWheelEvent::DefaultDeltasPerStep);
    for(int notch = 0 ; notch < std::abs(_notches) ; ++notch)
    {
        try
        {
            // A notch may scroll the target away or open a popup: check again before each one.
            QWindow& window_handle       = check_mouse_target(_target, _position);
            const QPoint global_position = _target.mapToGlobal(_position);
            QWheelEvent wheel_event(
                window_handle.mapFromGlobal(QPointF(global_position)),
                QPointF(global_position),
                QPoint(),
                angle_delta,
                QGuiApplication::mouseButtons(),
                _modifiers,
                Qt::NoScrollPhase,
                false
            );

            // QWindowSystemInterface::handleWheelEvent() has no synchronous delivery in Qt 6: build the event the way
            // QGuiApplicationPrivate::processWheelEvent() does. A non-spontaneous wheel would not propagate to parents,
            // and sendEvent() clears the flag: notify() directly, as QTest does for keys.
            QSpontaneKeyEvent::setSpontaneous(&wheel_event);
            wheel_event.setTimestamp(static_cast<quint64>(next_mouse_timestamp(1)));

            receiver_filter receiver(_target, is_mouse_wheel(global_position));
            {
                const event_filter_guard filter_guard(application, receiver);
                application.notify(&window_handle, &wheel_event);
            }

            receiver.verify("mouse wheel");
        }
        catch(const input_failure& failure)
        {
            if(notch == 0)
            {
                throw;
            }

            throw_after_partial_input(failure, notch, "notch(es)");
        }
    }
}

//------------------------------------------------------------------------------

/// Injects one key press and release through the active window to the focused target. As for a user's key, Qt first
/// tries the shortcuts: a key that triggers one is delivered there instead of to the target. _text is what the key
/// types; modifiers are sent in the event, not as separate key presses.
inline void inject_key_click(
    QWidget& _target,
    Qt::Key _key,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier,
    const QString& _text             = QString()
)
{
    QApplication& application = input_application();
    QWindow& window_handle    = check_keyboard_target(_target);
    receiver_filter receiver(_target, is_key_press(_key));
    shortcut_recorder shortcut;
    {
        const event_filter_guard filter_guard(application, receiver);
        const event_filter_guard shortcut_guard(application, shortcut);
        qt_handleKeyEvent(&window_handle, QEvent::KeyPress, _key, _modifiers, _text);
        qt_handleKeyEvent(&window_handle, QEvent::KeyRelease, _key, _modifiers, _text);
    }

    if(!shortcut.triggered())
    {
        receiver.verify("key press");
    }
}

//------------------------------------------------------------------------------

/// Injects each chord in a key sequence. A later failure must not replay chords already delivered.
inline void inject_key_sequence(QWidget& _target, const QKeySequence& _sequence)
{
    int delivered = 0;
    for(int index = 0 ; index < _sequence.count() ; ++index)
    {
        const auto combination = _sequence[static_cast<uint>(index)];
        try
        {
            inject_key_click(_target, combination.key(), combination.keyboardModifiers());
        }
        catch(const input_failure& failure)
        {
            if(delivered == 0)
            {
                throw;
            }

            throw_after_partial_input(failure, delivered, "key(s)");
        }

        ++delivered;
    }
}

//------------------------------------------------------------------------------

/// Types UTF-16 characters as key events. A failure after some characters is not retryable.
inline void inject_text(QWidget& _target, const QString& _text)
{
    int delivered = 0;
    for(qsizetype index = 0 ; index < _text.size() ; )
    {
        qsizetype length = 1;
        if(_text.at(index).isHighSurrogate() && index + 1 < _text.size() && _text.at(index + 1).isLowSurrogate())
        {
            length = 2;
        }

        try
        {
            inject_key_click(_target, Qt::Key_unknown, Qt::NoModifier, _text.mid(index, length));
        }
        catch(const input_failure& failure)
        {
            if(delivered == 0)
            {
                throw;
            }

            throw_after_partial_input(failure, delivered, "character(s)");
        }

        ++delivered;
        index += length;
    }
}

} // namespace sight::ui::test::detail
