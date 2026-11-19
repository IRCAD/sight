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

// cspell:ignore qgetenv

#include "scenario.hpp"

#include "detail/adaptors.hpp"
#include "detail/application_objects.hpp"
#include "detail/deadline.hpp"
#include "detail/file_dialog.hpp"
#include "detail/gui_dispatch.hpp"
#include "detail/image.hpp"
#include "detail/locator.hpp"
#include "detail/test_window.hpp"
#include "detail/ui_tree.hpp"
#include "detail/widget_text.hpp"
#include "detail/widget_visibility.hpp"
#include "detail/window_input.hpp"

#include "gui_session.hpp"
#include "locator.hpp"

#include <core/com/connection.hpp>
#include <core/com/slot.hpp>
#include <core/log/test_log.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/uuid.hpp>

#include <service/base.hpp>
#include <service/registry.hpp>

#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractScrollArea>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QIODevice>
#include <QImage>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMetaObject>
#include <QOpenGLWidget>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPoint>
#include <QPointer>
#include <QRect>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSpinBox>
#include <QString>
#include <QTextEdit>
#include <QThread>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>
#include <QWindow>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace sight::ui::test
{

using detail::locator;
using detail::locator_catalog;
using detail::locator_segment;

/// What a signal_watch observed. The GUI thread records, the scenario thread waits.
struct detail::signal_watch_state
{
    signal_watch_state(
        std::string _expression,
        std::shared_ptr<const std::atomic_bool> _loop_running,
        const QThread* _gui_thread
    ) :
        expression(std::move(_expression)),
        loop_running(std::move(_loop_running)),
        gui_thread(_gui_thread)
    {
    }

    signal_watch_state(const signal_watch_state&)            = delete;
    signal_watch_state(signal_watch_state&&)                 = delete;
    signal_watch_state& operator=(const signal_watch_state&) = delete;
    signal_watch_state& operator=(signal_watch_state&&)      = delete;

    ~signal_watch_state()
    {
        QObject::disconnect(signal_connection);
        QObject::disconnect(destroyed_connection);
        service_connection.disconnect();
    }

    /// True once the service under watch is gone. A widget tells through its signal, a service only by expiring.
    [[nodiscard]] bool target_destroyed() const
    {
        return destroyed || (watches_service && service.expired());
    }

    //------------------------------------------------------------------------------

    void mark(bool signal_watch_state::* _flag)
    {
        const std::scoped_lock lock(mutex);
        this->*_flag = true;
        changed.notify_all();
    }

    //------------------------------------------------------------------------------

    void wait(std::chrono::steady_clock::time_point _deadline)
    {
        std::unique_lock lock(mutex);
        if(consumed)
        {
            throw std::logic_error("signal watch has already been consumed");
        }

        // A stopping event loop notifies nothing: its flag is checked every 10 ms.
        const auto settled = [this]
                             {
                                 return emitted || target_destroyed() || !loop_running->load(std::memory_order_acquire);
                             };
        while(!settled() && std::chrono::steady_clock::now() < _deadline)
        {
            changed.wait_until(
                lock,
                std::min(_deadline, std::chrono::steady_clock::now() + std::chrono::milliseconds(10)),
                settled
            );
        }

        // A service may have emitted with async_emit before going away: its emission then waits on the worker of the
        // watch, and is delivered before the destruction counts. A stopped loop delivers nothing.
        bool delivered = true;
        if(!emitted && watches_service && service.expired() && loop_running->load(std::memory_order_acquire))
        {
            lock.unlock();
            delivered = deliver_queued_emissions(_deadline);
            lock.lock();
        }

        if(emitted)
        {
            // Two waiters may have slept together: the emission satisfies one of them.
            if(std::exchange(consumed, true))
            {
                throw std::logic_error("signal watch has already been consumed");
            }

            return;
        }

        if(!loop_running->load(std::memory_order_acquire))
        {
            throw std::runtime_error("the application event loop has stopped");
        }

        if(!delivered)
        {
            throw std::runtime_error(
                      "signal on '" + expression + "' timed out: its service is gone, but an emission may still wait "
                                                   "on the worker of the watch"
            );
        }

        if(target_destroyed())
        {
            throw std::runtime_error("signal target '" + expression + "' was destroyed");
        }

        throw std::runtime_error("signal on '" + expression + "' timed out");
    }

    /// Waits, until _deadline or the loop stops, for the tasks the worker of the service slot already holds.
    /// False when the worker may still hold some.
    [[nodiscard]] bool deliver_queued_emissions(std::chrono::steady_clock::time_point _deadline) const
    {
        const auto worker = service_slot->get_worker();
        if(worker == nullptr)
        {
            return true;
        }

        const auto barrier = worker->post_barrier();
        while(loop_running->load(std::memory_order_acquire))
        {
            const auto next_check = std::min(
                _deadline,
                std::chrono::steady_clock::now() + std::chrono::milliseconds(10)
            );
            if(barrier.wait_until(next_check) == std::future_status::ready)
            {
                return true;
            }

            if(next_check == _deadline)
            {
                return false;
            }
        }

        return false;
    }

    const std::string expression;
    const std::shared_ptr<const std::atomic_bool> loop_running;
    const QThread* const gui_thread;
    QMetaObject::Connection signal_connection;
    QMetaObject::Connection destroyed_connection;

    // Only for a service: it stays connected through its own signals, on any thread.
    bool watches_service {false};
    std::weak_ptr<const service::base> service;
    core::com::slot_base::sptr service_slot;
    core::com::connection service_connection;

    std::mutex mutex;
    std::condition_variable changed;
    bool emitted {false};
    bool destroyed {false};
    bool consumed {false};
};

//------------------------------------------------------------------------------

void signal_watch::wait(duration_t _timeout)
{
    if(!m_state)
    {
        throw std::logic_error("cannot wait on a moved signal watch");
    }

    if(QThread::currentThread() == m_state->gui_thread)
    {
        throw std::logic_error("GUI signal waits cannot run on the application thread");
    }

    if(_timeout <= duration_t::zero())
    {
        throw std::invalid_argument("GUI signal wait timeout must be positive");
    }

    m_state->wait(detail::deadline_after(_timeout));
}

//------------------------------------------------------------------------------

namespace
{

enum class target_state : std::uint8_t
{
    ready,
    missing,
    ambiguous,
    disabled,
    unreachable,
    unstable // NOLINT(readability-trailing-comma)
};

/// Where to click to act on a target: the target itself, or a menu or toolbar extension that leads to an action.
struct click_plan
{
    QPointer<QObject> target {};
    QPointer<QWidget> widget {};
    QPoint position {};
    QRect geometry {};
    bool reaches_target {true};
};

struct resolution
{
    target_state state {target_state::missing};
    click_plan plan {};
    /// What the locator resolved to, for the step log.
    std::string path {};
    std::vector<std::string> match_scopes {};
    std::size_t hidden_matches {0};
};

struct widget_observation
{
    target_state state {target_state::missing};
    std::vector<std::string> match_scopes {};
    std::size_t hidden_matches {0};
    bool found {false};
    bool matched {false};
    std::string value {};
    /// What the locator resolved to, for the step log.
    std::string path {};
};

/// What one step of scroll_to found: the target visible, or why it could not scroll towards it.
struct scroll_step
{
    target_state state {target_state::missing};
    std::vector<std::string> match_scopes {};
    bool visible {false};
    std::string obstacle {};
};

/// Point of the target widget to click.
using click_position   = std::function<QPoint(const QWidget&)>;
using action_injection = std::function<void(QWidget&, const click_plan&)>;

} // namespace

//------------------------------------------------------------------------------

/// Where input goes when nothing else is said.
static QPoint centre_of(const QWidget& _widget)
{
    return _widget.rect().center();
}

//------------------------------------------------------------------------------

static QRect global_geometry(const QWidget& _widget)
{
    return {_widget.mapToGlobal(QPoint(0, 0)), _widget.size()};
}

//------------------------------------------------------------------------------

static click_plan plan_on(QObject& _target, QWidget& _widget, QPoint _position, bool _reaches_target)
{
    return {
        .target         = &_target,
        .widget         = &_widget,
        .position       = _position,
        .geometry       = global_geometry(_widget),
        .reaches_target = _reaches_target
    };
}

//------------------------------------------------------------------------------

/// True when two resolutions planned the same click: same target, same widget at the same place.
static bool same_plan(const click_plan& _first, const click_plan& _second)
{
    return !_first.target.isNull() && _first.target == _second.target && _first.widget == _second.widget
           && _first.position == _second.position && _first.geometry == _second.geometry
           && _first.reaches_target == _second.reaches_target;
}

//------------------------------------------------------------------------------

/// A state an action holds itself, whether a toolbar shows it or a closed menu hides it.
static bool answered_by_actions(detail::widget_query _query)
{
    using enum detail::widget_query;
    return _query == enabled || _query == disabled || _query == checked || _query == unchecked || _query == is_visible
           || _query == is_hidden;
}

//------------------------------------------------------------------------------

/// Whether an action is in the state a query asks for, and the state it is in.
static std::pair<bool, std::string> action_state(const QAction& _action, detail::widget_query _query)
{
    using enum detail::widget_query;
    if(_query == enabled || _query == disabled)
    {
        const bool is_enabled = _action.isEnabled();
        return {is_enabled == (_query == enabled), is_enabled ? "enabled" : "disabled"};
    }

    if(_query == is_visible || _query == is_hidden)
    {
        const bool shown = _action.isVisible();
        return {shown == (_query == is_visible), shown ? "visible" : "not visible"};
    }

    if(!_action.isCheckable())
    {
        throw std::invalid_argument("value(bool) requires a checkable button or action, found an action that is not");
    }

    const bool is_checked = _action.isChecked();
    return {is_checked == (_query == checked), is_checked ? "checked" : "unchecked"};
}

//------------------------------------------------------------------------------

/// Keys, drags and gestures go to a widget: an action only takes a click on the button or menu entry showing it.
static void require_widget_target(const click_plan& _plan, std::string_view _operation)
{
    if(qobject_cast<QWidget*>(_plan.target.data()) == nullptr)
    {
        throw std::invalid_argument(std::string(_operation) + " requires a widget target, not an action");
    }
}

//------------------------------------------------------------------------------

/// The click on an action that a visible menu, menu bar or toolbar shows, if one does.
static std::optional<click_plan> visible_action_plan(QAction& _action)
{
    for(QObject* object : _action.associatedObjects())
    {
        QWidget* const widget = qobject_cast<QWidget*>(object);
        if(widget == nullptr || !detail::is_visible(*widget))
        {
            continue;
        }

        if(auto* const menu = qobject_cast<QMenu*>(widget); menu != nullptr && menu->actions().contains(&_action))
        {
            if(const QRect rect = menu->actionGeometry(&_action); !rect.isEmpty())
            {
                return plan_on(_action, *menu, rect.center(), true);
            }
        }

        if(auto* const menu_bar = qobject_cast<QMenuBar*>(widget);
           menu_bar != nullptr && menu_bar->actions().contains(&_action))
        {
            if(const QRect rect = menu_bar->actionGeometry(&_action); !rect.isEmpty())
            {
                return plan_on(_action, *menu_bar, rect.center(), true);
            }
        }

        if(auto* const toolbar = qobject_cast<QToolBar*>(widget); toolbar != nullptr)
        {
            if(QWidget* const button = toolbar->widgetForAction(&_action);
               button != nullptr && detail::is_visible(*button))
            {
                return plan_on(_action, *button, button->rect().center(), true);
            }
        }

        if(auto* const button = qobject_cast<QToolButton*>(widget);
           button != nullptr && button->actions().contains(&_action))
        {
            return plan_on(_action, *button, button->rect().center(), true);
        }
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

/// Plans the click on _action, or on the menu or toolbar extension that shows it. A popup already shown but not yet
/// exposed (exposure is asynchronous on real platforms) is opening: clicking what opened it again would close it.
static resolution action_resolution(QAction& _action, std::vector<QAction*>& _visited)
{
    if(std::ranges::find(_visited, &_action) != _visited.end())
    {
        return {.state = target_state::unreachable};
    }

    // Shown now: clicked where it is.
    _visited.push_back(&_action);
    if(auto plan = visible_action_plan(_action); plan.has_value())
    {
        return {.state = target_state::ready, .plan = *plan};
    }

    // Hidden: its menu is opened first, from wherever that menu is shown, or the toolbar extension that hides it.
    for(QObject* object : _action.associatedObjects())
    {
        if(auto* const menu = qobject_cast<QMenu*>(object); menu != nullptr)
        {
            if(menu->isVisible())
            {
                return {.state = target_state::unstable};
            }

            if(resolution opener = action_resolution(*menu->menuAction(), _visited);
               opener.state != target_state::unreachable)
            {
                // With no popup open, the action is triggered once what opens its menu is reachable; a submenu of an
                // open popup is opened as a user does.
                opener.plan.reaches_target = opener.state == target_state::ready
                                             && opener.plan.reaches_target
                                             && QApplication::activePopupWidget() == nullptr;
                if(opener.plan.reaches_target)
                {
                    opener.plan.target = &_action;
                }

                return opener;
            }
        }

        if(auto* const toolbar = qobject_cast<QToolBar*>(object); toolbar != nullptr && detail::is_visible(*toolbar))
        {
            auto* const extension = toolbar->findChild<QToolButton*>("qt_toolbar_ext_button");
            if(extension != nullptr && extension->menu() != nullptr && extension->menu()->isVisible())
            {
                return {.state = target_state::unstable};
            }

            // In a main window with room for it, the toolbar expands instead of opening a menu: its button is coming,
            // and
            // clicking the extension again would fold the toolbar.
            if(extension != nullptr && extension->isChecked())
            {
                return {.state = target_state::unstable};
            }

            if(extension != nullptr && detail::is_visible(*extension))
            {
                // Triggered once the extension is reachable, unless a popup is open.
                const QPoint centre = extension->rect().center();
                const bool direct   = QApplication::activePopupWidget() == nullptr;
                return {.state = target_state::ready, .plan = plan_on(_action, *extension, centre, direct)};
            }
        }
    }

    return {.state = target_state::unreachable};
}

//------------------------------------------------------------------------------

/// The named ancestors of an object, outermost first: the locator that would find it.
static std::string format_object_scope(const QObject& _object)
{
    std::vector<std::string> names;
    for(const QObject* parent = &_object ; parent != nullptr ; parent = parent->parent())
    {
        const QString name = parent->objectName();
        if(!name.isEmpty())
        {
            names.push_back(name.toStdString());
        }
    }

    std::ranges::reverse(names);
    std::string scope;
    for(const std::string& name : names)
    {
        if(!scope.empty())
        {
            scope += " >> ";
        }

        scope += name;
    }

    return scope;
}

//------------------------------------------------------------------------------

/// The scope of each match, to name them in a failure.
template<typename Object>
static std::vector<std::string> format_match_scopes(const std::vector<Object*>& _matches)
{
    std::vector<std::string> scopes;
    scopes.reserve(_matches.size());
    for(const Object* match : _matches)
    {
        scopes.push_back(format_object_scope(*match));
    }

    return scopes;
}

//------------------------------------------------------------------------------

/// Resolves the locator to one enabled widget, else one enabled action, and plans the click. On the GUI thread only.
static resolution resolve(const locator& _target, const click_position& _position)
{
    // A widget first: it is what a user clicks.
    if(const std::vector<QWidget*> widgets = _target.resolve_widgets(); !widgets.empty())
    {
        if(widgets.size() != 1)
        {
            return {.state = target_state::ambiguous, .match_scopes = format_match_scopes(widgets)};
        }

        QWidget& widget = *widgets.front();
        if(!widget.isEnabled())
        {
            return {.state = target_state::disabled};
        }

        return {.state = target_state::ready, .plan = plan_on(widget, widget, _position(widget), true)};
    }

    // Else an action, clicked through the widget that shows it.
    const std::vector<QAction*> actions = _target.resolve_actions();
    if(actions.empty())
    {
        return {.state = target_state::missing, .hidden_matches = _target.count_hidden_widgets()};
    }

    if(actions.size() != 1)
    {
        return {.state = target_state::ambiguous, .match_scopes = format_match_scopes(actions)};
    }

    if(!actions.front()->isEnabled())
    {
        return {.state = target_state::disabled};
    }

    std::vector<QAction*> visited;
    return action_resolution(*actions.front(), visited);
}

//------------------------------------------------------------------------------

/// resolve(), with the path of the target for the step log.
static resolution resolve_with_path(const locator& _target, const click_position& _position)
{
    resolution resolved = resolve(_target, _position);
    if(resolved.state == target_state::ready && resolved.plan.target != nullptr)
    {
        resolved.path = format_object_scope(*resolved.plan.target);
    }

    return resolved;
}

//------------------------------------------------------------------------------

/// The nearest scroll area whose viewport holds _widget.
static QAbstractScrollArea* enclosing_scroll_area(const QWidget& _widget)
{
    for(QWidget* parent = _widget.parentWidget() ; parent != nullptr ; parent = parent->parentWidget())
    {
        if(auto* const area = qobject_cast<QAbstractScrollArea*>(parent);
           area != nullptr && area->viewport()->isAncestorOf(&_widget))
        {
            return area;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

/// One step of scroll_to, on the GUI thread: nothing once the target is in view, else one wheel notch on the vertical
/// scroll bar of the innermost scroll area that clips it, towards it. A target that fits is brought entirely into view,
/// since a click goes to its centre; a taller one until it fills the viewport.
static scroll_step scroll_towards(const locator& _target)
{
    // A widget scrolled out of view is not visible: find it among all.
    std::vector<QWidget*> widgets = _target.resolve_widgets(true);

    // An action has no widget of its own: a user scrolls to the button that shows it in a toolbar.
    if(widgets.empty())
    {
        const std::vector<QAction*> actions = _target.resolve_actions();
        if(actions.size() > 1)
        {
            return {.state = target_state::ambiguous, .match_scopes = format_match_scopes(actions)};
        }

        for(QAction* const action : actions)
        {
            for(QObject* const object : action->associatedObjects())
            {
                if(auto* const toolbar = qobject_cast<QToolBar*>(object); toolbar != nullptr)
                {
                    if(QWidget* const button = toolbar->widgetForAction(action); button != nullptr)
                    {
                        widgets.push_back(button);
                    }
                }
            }
        }
    }

    if(widgets.size() != 1)
    {
        return {
            .state        = widgets.empty() ? target_state::missing : target_state::ambiguous,
            .match_scopes = format_match_scopes(widgets)
        };
    }

    const QWidget& widget = *widgets.front();
    if(widget.isVisible())
    {
        for(QAbstractScrollArea* area = enclosing_scroll_area(widget) ; area != nullptr ;
            area = enclosing_scroll_area(*area))
        {
            const QWidget& viewport = *area->viewport();
            const int top           = widget.mapTo(&viewport, QPoint(0, 0)).y();
            const int bottom        = top + widget.height();
            const bool below        = bottom > viewport.height() && top > 0;
            const bool above        = top < 0 && bottom < viewport.height();
            // A notch away from the user scrolls down.
            const int notches = below ? -1 : (above ? 1 : 0);
            if(notches == 0)
            {
                continue;
            }

            QScrollBar& bar   = *area->verticalScrollBar();
            const bool at_end = bar.value() == (notches < 0 ? bar.maximum() : bar.minimum());
            if(at_end || !detail::is_visible(bar))
            {
                // As far as a user can scroll: what shows of it is what a user reaches.
                if(detail::is_visible(widget))
                {
                    return {.state = target_state::ready, .visible = true};
                }

                return {.state = target_state::ready, .obstacle = "its scroll area cannot scroll towards it"};
            }

            detail::inject_mouse_wheel(bar, bar.rect().center(), notches);
            return {.state = target_state::ready};
        }

        if(detail::is_visible(widget))
        {
            return {.state = target_state::ready, .visible = true};
        }
    }

    return {.state = target_state::ready, .obstacle = "it is hidden, not scrolled out of view"};
}

//------------------------------------------------------------------------------

static void validate_relative_position(relative_position _position)
{
    if(!std::isfinite(_position.x) || !std::isfinite(_position.y))
    {
        throw std::invalid_argument("relative coordinates must be finite");
    }

    if(_position.x < 0. || _position.x > 1. || _position.y < 0. || _position.y > 1.)
    {
        throw std::invalid_argument("relative click coordinates must be in [0, 1]");
    }
}

//------------------------------------------------------------------------------

/// Inputs are bounded (a drag goes at most ten target sizes away), so the result fits the widget's coordinates.
static QPoint map_relative_position(const QWidget& _widget, relative_position _position)
{
    const QRect rect = _widget.rect();
    return {
        rect.left() + static_cast<int>(std::floor(_position.x * (rect.width() - 1))),
        rect.top() + static_cast<int>(std::floor(_position.y * (rect.height() - 1)))
    };
}

//------------------------------------------------------------------------------

/// Converts UTF-8 from a scenario, refusing invalid text rather than typing replacement characters.
static QString utf8_text(std::string_view _text, const std::string& _what)
{
    if(_text.size() > static_cast<std::size_t>(std::numeric_limits<qsizetype>::max()))
    {
        throw std::invalid_argument(_what + " is too large");
    }

    const QByteArray bytes(_text.data(), static_cast<qsizetype>(_text.size()));
    QString text = QString::fromUtf8(bytes);
    if(text.toUtf8() != bytes)
    {
        throw std::invalid_argument(_what + " must be valid UTF-8");
    }

    return text;
}

//------------------------------------------------------------------------------

static std::string quoted_list(const std::vector<std::string>& _names)
{
    std::string list;
    for(const std::string& name : _names)
    {
        list += (list.empty() ? "'" : ", '") + name + "'";
    }

    return list;
}

//------------------------------------------------------------------------------

/// Why a target cannot be acted on now, as a failure tells it; empty when it can.
static std::string describe_state(
    target_state _state,
    std::string_view _expression,
    const std::vector<std::string>& _match_scopes = {},
    std::size_t _hidden_matches                   = 0
)
{
    const auto hidden_note = [_hidden_matches]
                             {
                                 return _hidden_matches == 0
                                        ? std::string()
                                        : std::format(
                                     "; {} hidden {}",
                                     _hidden_matches,
                                     _hidden_matches == 1 ? "match" : "matches"
                                        );
                             };

    switch(_state)
    {
        case target_state::ready:
            return {};

        case target_state::missing:
            return "no visible widget named '" + std::string(_expression) + "'" + hidden_note();

        case target_state::ambiguous:
            return "several GUI objects named '" + std::string(_expression) + "'"
                   + (_match_scopes.empty() ? std::string() : "; matches: " + quoted_list(_match_scopes));

        case target_state::disabled:
            return "widget '" + std::string(_expression) + "' is disabled";

        case target_state::unreachable:
            return "action '" + std::string(_expression)
                   + "' has no reachable widget; shortcut-only actions require press(shortcut)";

        case target_state::unstable:
            return "widget '" + std::string(_expression) + "' is changing";
    }

    throw std::logic_error("unknown GUI target state");
}

//------------------------------------------------------------------------------

/// Reports what was observed of a target, in a state a retry may have to qualify.
template<class OBSERVATION>
requires requires(const OBSERVATION& _observation)
{
    _observation.match_scopes;
    _observation.hidden_matches;
}

//------------------------------------------------------------------------------

static std::string describe_state(target_state _state, std::string_view _expression, const OBSERVATION& _observation)
{
    return describe_state(_state, _expression, _observation.match_scopes, _observation.hidden_matches);
}

//------------------------------------------------------------------------------

namespace
{

/// Several matches may be a transition, a view replaced by another: they fail once they last a second.
class ambiguity_grace
{
public:

    //------------------------------------------------------------------------------

    void check(target_state _state, std::string_view _expression, const std::vector<std::string>& _match_scopes)
    {
        if(_state != target_state::ambiguous)
        {
            m_since.reset();
            return;
        }

        const auto now = std::chrono::steady_clock::now();
        if(!m_since)
        {
            m_since = now;
        }
        else if(now - *m_since >= detail::scaled(std::chrono::seconds(1)))
        {
            throw std::runtime_error(describe_state(_state, _expression, _match_scopes));
        }
    }

private:

    std::optional<detail::gui_deadline> m_since;
};

} // namespace

//------------------------------------------------------------------------------

/// The text a widget shows, or a failure naming the widget that shows none.
static QString widget_text(const QWidget& _widget, std::string_view _operation)
{
    if(auto text = detail::displayed_text(_widget); text.has_value())
    {
        return *std::move(text);
    }

    throw std::invalid_argument(
              std::string(_operation) + " requires a text-capable widget, found "
              + _widget.metaObject()->className()
    );
}

//------------------------------------------------------------------------------

static std::string quote_text(const QString& _text)
{
    return "\"" + _text.toStdString() + "\"";
}

//------------------------------------------------------------------------------

/// The value of a spin box, slider or combo box, none for another widget.
static std::optional<double> numeric_widget_value(const QWidget& _widget)
{
    if(const auto* const spin_box = qobject_cast<const QDoubleSpinBox*>(&_widget); spin_box != nullptr)
    {
        return spin_box->value();
    }

    if(const auto* const spin_box = qobject_cast<const QSpinBox*>(&_widget); spin_box != nullptr)
    {
        return spin_box->value();
    }

    if(const auto* const slider = qobject_cast<const QAbstractSlider*>(&_widget); slider != nullptr)
    {
        return slider->value();
    }

    if(const auto* const combo = qobject_cast<const QComboBox*>(&_widget); combo != nullptr)
    {
        return combo->currentIndex();
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

/// The shortest text that reads back as _value: 0.1, not 0.10000000000000001.
static std::string format_number(double _value)
{
    return std::format("{}", _value);
}

//------------------------------------------------------------------------------

using widget_probe = std::function<std::pair<bool, std::string>(const QWidget&)>;

/// Probes the text a user reads on the widget.
static widget_probe text_probe(std::string_view _matcher, std::function<bool(const QString&)> _matches)
{
    return [matcher = std::string(_matcher), matches = std::move(_matches)](const QWidget& _widget)
           {
               const QString actual = widget_text(_widget, matcher);
               return std::pair {matches(actual), quote_text(actual)};
           };
}

//------------------------------------------------------------------------------

/// Probes a widget state, named both ways for the failure message.
static widget_probe state_probe(
    bool (* _state)(const QWidget&),
    bool _expected,
    std::string _true_name,
    std::string _false_name
)
{
    return [_state, _expected, true_name = std::move(_true_name), false_name = std::move(_false_name)](
        const QWidget& _widget)
           {
               const bool state = _state(_widget);
               return std::pair {state == _expected, state ? true_name : false_name};
           };
}

//------------------------------------------------------------------------------

static bool widget_enabled(const QWidget& _widget)
{
    return _widget.isEnabled();
}

//------------------------------------------------------------------------------

static bool widget_visible(const QWidget& _widget)
{
    return detail::is_visible(_widget);
}

//------------------------------------------------------------------------------

/// Writes what a failed image assertion saw, then reports it. Failing to write never hides the mismatch.
[[noreturn]] static void image_failure(
    const std::filesystem::path& _directory,
    const QImage& _actual,
    const QImage& _expected,
    const std::string& _message
)
{
    const std::string path = detail::qt_path(_directory).toStdString();
    try
    {
        std::filesystem::create_directories(_directory);
        if(!_actual.isNull())
        {
            detail::save_image(_actual, _directory / "actual.png");
        }

        if(!_expected.isNull())
        {
            detail::save_image(_expected, _directory / "expected.png");
            if(!_actual.isNull())
            {
                detail::save_image(detail::image_difference(_actual, _expected), _directory / "diff.png");
            }
        }

        std::ofstream metrics;
        metrics.exceptions(std::ios::failbit | std::ios::badbit);
        metrics.open(_directory / "metrics.txt");
        metrics << _message << '\n';
        metrics.close();
    }
    catch(const std::exception& failure)
    {
        throw std::runtime_error(_message + "; artifact error in " + path + ": " + failure.what());
    }

    throw std::runtime_error(_message + "; image artifacts: " + path);
}

//------------------------------------------------------------------------------

/// What a widget expectation sees of its target now, on the GUI thread.
static widget_observation observe_widget(
    const locator& _target,
    const widget_probe& _probe,
    detail::widget_query _query,
    bool _include_hidden
)
{
    const std::vector<QWidget*> widgets = _target.resolve_widgets(_include_hidden);
    if(widgets.empty() && answered_by_actions(_query))
    {
        // As for clicks, a widget takes precedence over its backing action.
        const auto actions = _target.resolve_actions();
        if(actions.size() == 1)
        {
            const auto [matched, value] = action_state(*actions.front(), _query);
            return {
                .state   = target_state::ready,
                .found   = true,
                .matched = matched,
                .value   = value,
                .path    = format_object_scope(*actions.front()),
            };
        }

        return {
            .state          = actions.empty() ? target_state::missing : target_state::ambiguous,
            .match_scopes   = format_match_scopes(actions),
            .hidden_matches = _target.count_hidden_widgets(),
        };
    }

    // Gone means no widget at all: several still present is no ambiguity, only not yet.
    if(_query == detail::widget_query::gone)
    {
        return {
            .state   = target_state::ready,
            .found   = !widgets.empty(),
            .matched = widgets.empty(),
            .value   = std::to_string(widgets.size()) + " still present",
        };
    }

    if(widgets.size() != 1)
    {
        return {
            .state          = widgets.empty() ? target_state::missing : target_state::ambiguous,
            .match_scopes   = format_match_scopes(widgets),
            .hidden_matches = widgets.empty() && !_include_hidden
                              ? _target.count_hidden_widgets() : 0,
        };
    }

    const auto [matched, value] = _probe(*widgets.front());
    return {
        .state   = target_state::ready,
        .found   = true,
        .matched = matched,
        .value   = value,
        .path    = format_object_scope(*widgets.front()),
    };
}

//------------------------------------------------------------------------------

namespace
{

/// A frame read on the GUI thread, by value: a call still running at the deadline outlives its caller.
struct frame_observation
{
    widget_observation widget;
    QImage pixels;
};

} // namespace

//------------------------------------------------------------------------------

/// The frame of the scene of a locator now, on the GUI thread.
static frame_observation grab_frame(const locator& _target)
{
    const auto widgets = _target.resolve_widgets();
    if(widgets.size() != 1)
    {
        return {
            .widget             = {
                .state          = widgets.empty() ? target_state::missing : target_state::ambiguous,
                .match_scopes   = format_match_scopes(widgets),
                .hidden_matches = widgets.empty() ? _target.count_hidden_widgets() : 0,
            },
            .pixels = {}
        };
    }

    auto* const scene = dynamic_cast<QOpenGLWidget*>(widgets.front());
    if(scene == nullptr)
    {
        throw std::invalid_argument("image requires a QOpenGLWidget scene");
    }

    if(!scene->isValid())
    {
        return {
            .widget = {.state = target_state::ready, .value = "scene has no valid OpenGL context"},
            .pixels = {}
        };
    }

    // This renders and reads the composed FBO, independently of screen contents or covering windows.
    QImage pixels = scene->grabFramebuffer();
    if(pixels.isNull())
    {
        throw std::runtime_error("grabFramebuffer returned an empty image");
    }

    return {
        .widget = {.state = target_state::ready, .path = format_object_scope(*scene)},
        .pixels = std::move(pixels)
    };
}

//------------------------------------------------------------------------------

/// Acts on the active modal dialog, on the GUI thread.
static void act_on_modal(const std::function<void(QWidget&)>& _act)
{
    QWidget* const modal = QApplication::activeModalWidget();
    if(modal == nullptr || !detail::is_visible(*modal))
    {
        throw detail::input_failure("no modal dialog is shown");
    }

    _act(*modal);
}

//------------------------------------------------------------------------------

struct scenario::implementation
{
    explicit implementation(
        QApplication& _application,
        std::optional<locator_catalog> _catalog = std::nullopt,
        std::filesystem::path _image_output     = {}) :
        application(_application),
        gateway(_application),
        dispatcher(_application, gateway),
        catalog(std::move(_catalog)),
        image_output(std::move(_image_output))
    {
    }

    //------------------------------------------------------------------------------

    void ensure_scenario_thread() const
    {
        if(QThread::currentThread() == application.thread())
        {
            throw std::logic_error("GUI scenario actions cannot run on the application thread");
        }
    }

    //------------------------------------------------------------------------------

    /// Parses a locator, and fails at once on a name the XML does not declare.
    [[nodiscard]] locator checked_locator(std::string_view _expression) const
    {
        locator target(_expression);

        // Inside a modal dialog, Qt's own dialogs keep Qt's names: the XML knows nothing of them.
        if(!catalog.has_value() || target.in_modal_dialog())
        {
            return target;
        }

        // Only typos fail here. A configuration launched twice in the XML may be launched once at a time, by two
        // activities: whether a name is ambiguous is for the resolution to tell, with the scopes it found.
        for(const locator_segment& segment : target.segments())
        {
            if(catalog->accepts(segment))
            {
                continue;
            }

            std::string message =
                "unknown XML name '" + segment.object_name + "' in locator '" + std::string(_expression) + "'";
            if(const std::vector<std::string> candidates = catalog->candidates(segment); !candidates.empty())
            {
                message += "; candidates: " + quoted_list(candidates);
            }

            throw std::invalid_argument(message);
        }

        return target;
    }

    //------------------------------------------------------------------------------

    /// Tries again every 10 ms until an attempt returns true, then returns true, or until the deadline, then returns
    /// false. The first attempt gets a deadline the GUI answers, however short the timeout, and a GUI too busy at the
    /// deadline to run a call gets one more look time to answer: what the GUI shows counts, never a busy GUI. A GUI
    /// call
    /// cut by the deadline is the timeout of the wait, and _cut gets its message; any earlier failure throws.
    bool retry(
        detail::gui_deadline _deadline,
        const std::function<bool(detail::gui_deadline)>& _attempt,
        std::string* _cut = nullptr
    ) const
    {
        auto deadline = _deadline;

        // A failure before the deadline, or once the event loop has stopped, is not the end of the wait.
        const auto ends_wait = [&]
                               {
                                   return gateway.is_loop_running() && std::chrono::steady_clock::now() >= deadline;
                               };
        auto attempt_deadline = detail::first_attempt_deadline(deadline);
        bool extended         = false;
        do
        {
            try
            {
                if(_attempt(std::exchange(attempt_deadline, deadline)))
                {
                    return true;
                }
            }
            catch(const detail::gui_call_cut& cut)
            {
                if(!ends_wait())
                {
                    throw;
                }

                // The GUI was too busy at the deadline to run the call, which had no effect: the wait goes on once.
                if(!extended)
                {
                    extended         = true;
                    deadline         = std::max(deadline, detail::deadline_after(detail::LOOK_TIMEOUT));
                    attempt_deadline = deadline;
                    continue;
                }

                if(_cut != nullptr)
                {
                    *_cut = cut.what();
                }

                return false;
            }
            catch(const std::runtime_error& failure)
            {
                if(!ends_wait())
                {
                    throw;
                }

                if(_cut != nullptr)
                {
                    *_cut = failure.what();
                }

                return false;
            }

            // Polls without flooding the application's event loop.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        while(std::chrono::steady_clock::now() < deadline);

        return false;
    }

    //------------------------------------------------------------------------------

    /// Waits for one visible widget and retries its observation until it matches.
    void wait_for_widget(
        std::string_view _expression,
        std::string_view _operation,
        std::string_view _description,
        const std::function<std::pair<bool, std::string>(const QWidget&)>& _probe,
        duration_t _timeout,
        detail::widget_query _query = detail::widget_query::probe,
        bool _never_existed_ok      = false
)
    {
        const auto deadline  = operation_deadline(_timeout, "GUI " + std::string(_operation));
        const locator target = checked_locator(_expression);
        const std::string expression(_expression);
        ambiguity_grace ambiguity;
        const bool include_hidden = _query == detail::widget_query::is_visible
                                    || _query == detail::widget_query::is_hidden
                                    || _query == detail::widget_query::gone;
        std::string last_failure = !include_hidden
                                   ? describe_state(target_state::missing, expression)
                                   : "no widget named '" + expression + "'";
        const std::string description(_description);
        const bool done = retry(
            deadline,
            [&](detail::gui_deadline _attempt_deadline)
            {
                // By value: a call still running at the deadline outlives this frame.
                const auto observe = std::bind_front(
                    &observe_widget,
                    target,
                    _probe,
                    _query,
                    include_hidden
                );
                const widget_observation observation = gateway.call_until(_attempt_deadline, observe);
                // Found once: the step log names it, and gone() may now trust its absence.
                if(observation.found)
                {
                    seen_locators.insert(expression);
                    step_path = observation.path;
                }

                if(observation.state == target_state::ready && observation.matched)
                {
                    // A typo must not pass as gone: the locator has to have matched once.
                    if(_query == detail::widget_query::gone && !_never_existed_ok
                       && !seen_locators.contains(expression))
                    {
                        throw std::invalid_argument(
                                  "gone on '" + expression
                                  + "' requires a previous match; use never_existed_ok() to allow none"
                        );
                    }

                    return true;
                }

                // Not yet: the failure will tell what the last observation saw.
                ambiguity.check(observation.state, expression, observation.match_scopes);
                if(observation.state != target_state::ready)
                {
                    last_failure = observation.state == target_state::missing
                                   && include_hidden
                                   ? "no widget named '" + expression + "'"
                                   : describe_state(observation.state, expression, observation);
                }
                else
                {
                    last_failure = observation.value.empty() ? description
                                                             : description + " (last value: " + observation.value + ")";
                }

                record_obstacle(last_failure);
                return false;
            });
        if(done)
        {
            return;
        }

        throw std::runtime_error(
                  std::string(_operation) + " on '" + expression + "' timed out: "
                  + with_closest_names(target, last_failure)
        );
    }

    //------------------------------------------------------------------------------

    /// The predicate gets the deadline of the whole wait: a GUI call inside never waits past it.
    void eventually(
        std::string_view _description,
        const std::function<bool(detail::gui_deadline)>& _predicate,
        duration_t _timeout
    ) const
    {
        const auto deadline = operation_deadline(_timeout, "eventually");
        if(_description.empty())
        {
            throw std::invalid_argument("eventually requires a description");
        }

        // The predicate runs here, on the scenario thread: it may call the verbs.
        std::string cut;
        if(retry(deadline, _predicate, &cut))
        {
            return;
        }

        const std::string cause = cut.empty() ? std::string() : ": " + cut;
        throw std::runtime_error("eventually '" + std::string(_description) + "' timed out" + cause);
    }

    //------------------------------------------------------------------------------

    /// Each step is one notch, decided again from where the target is now: none is replayed, and the next one only
    /// follows once the previous one has scrolled.
    void scroll_to(std::string_view _expression, duration_t _timeout)
    {
        const auto deadline  = operation_deadline(_timeout, "GUI scroll");
        const locator target = checked_locator(_expression);
        const std::string expression(_expression);
        ambiguity_grace ambiguity;
        std::string last_failure = "no widget named '" + expression + "'";
        const bool visible       = retry(
            deadline,
            [&](detail::gui_deadline _attempt_deadline)
            {
                // One notch, then what it did is looked at again.
                try
                {
                    const scroll_step step = gateway.call_until(
                        _attempt_deadline,
                        [target]{return scroll_towards(target);});
                    ambiguity.check(step.state, expression, step.match_scopes);
                    if(step.state == target_state::ready)
                    {
                        seen_locators.insert(expression);
                        if(step.visible)
                        {
                            return true;
                        }

                        last_failure = step.obstacle.empty() ? "still out of view" : step.obstacle;
                    }
                    else
                    {
                        last_failure = step.state == target_state::missing
                                       ? "no widget named '" + expression + "'"
                                       : describe_state(step.state, expression, step.match_scopes);
                    }
                }
                catch(const detail::input_failure& failure)
                {
                    if(!failure.retryable())
                    {
                        throw;
                    }

                    last_failure = failure.what();
                }

                record_obstacle(last_failure);
                return false;
            });
        if(visible)
        {
            return;
        }

        throw std::runtime_error(
                  "scroll to '" + expression + "' timed out: " + with_closest_names(target, last_failure)
        );
    }

    //------------------------------------------------------------------------------

    /// Injects the planned action if the target still resolves to the same plan on the GUI thread. Returns false when
    /// the attempt delivered nothing and may be repeated; a handler that opens a modal dialog or a popup stays pending.
    bool dispatch_action(
        const locator& _target,
        const std::string& _expression,
        const std::string& _action,
        const click_position& _position,
        const click_plan& _plan,
        const action_injection& _inject,
        detail::gui_deadline _deadline,
        std::string& _last_failure
)
    {
        std::shared_ptr<detail::gui_dispatch::ticket> ticket;
        try
        {
            // Resolved again in the same GUI call as the input: nothing can move in between.
            ticket = dispatcher.post_until(
                _deadline,
                [_target, _expression, _action, _position, _plan, _inject]
                {
                    const resolution resolved = resolve(_target, _position);
                    if(resolved.state != target_state::ready || !same_plan(_plan, resolved.plan))
                    {
                        const target_state state =
                            resolved.state == target_state::ready ? target_state::unstable : resolved.state;
                        throw detail::input_failure(
                                  _action + " on '" + _expression + "' was not delivered: "
                                  + describe_state(state, _expression, resolved)
                        );
                    }

                    _inject(*resolved.plan.widget, resolved.plan);
                });

            // Delivered: a handler still running, as a modal dialog's, is waited for by finish().
            if(dispatcher.acknowledge_until(ticket, _deadline) != detail::dispatch_ack::returned)
            {
                pending.push_back(std::move(ticket));
            }

            return true;
        }
        catch(const detail::input_failure& failure)
        {
            if(!failure.retryable())
            {
                throw;
            }

            _last_failure = failure.what();
            return false;
        }
        catch(const detail::gui_call_cut&)
        {
            // Never ran: nothing to wait for.
            throw;
        }
        catch(const std::runtime_error&)
        {
            // Delivered, but its handler still runs at the deadline: it is never sent again.
            if(ticket != nullptr && std::chrono::steady_clock::now() >= _deadline)
            {
                pending.push_back(std::move(ticket));
            }

            throw;
        }
    }

    //------------------------------------------------------------------------------

    /// Waits for the target to be found, stable and reachable, then injects the input once. Every verb acting on a
    /// target goes through here.
    void perform_action(
        std::string_view _expression,
        const click_position& _position,
        std::string_view _action,
        const action_injection& _inject,
        duration_t _timeout
)
    {
        const auto deadline  = operation_deadline(_timeout, "GUI action");
        const locator target = checked_locator(_expression);
        const std::string expression(_expression);
        const std::string action(_action);
        ambiguity_grace ambiguity;
        std::string last_failure = describe_state(target_state::missing, expression);
        std::string delivery_failure;
        const bool delivered = retry(
            deadline,
            [&](detail::gui_deadline _attempt_deadline)
            {
                // By value: a call still running at the deadline outlives this frame.
                const auto resolve_now = [target, _position]{return resolve_with_path(target, _position);};
                const resolution first = gateway.call_until(_attempt_deadline, resolve_now);
                ambiguity.check(first.state, expression, first.match_scopes);
                if(first.state != target_state::ready)
                {
                    last_failure = describe_state(first.state, expression, first);
                }
                else
                {
                    seen_locators.insert(expression);
                    step_path = first.path;
                    // A ready target fails for the reason of the last attempt that could not deliver, if any.
                    last_failure = delivery_failure.empty()
                                   ? "found, but the application did not complete the action in time"
                                   : delivery_failure;
                    // The same object, with the same geometry, in two resolutions one event-loop pass apart.
                    const resolution second = gateway.call_until(_attempt_deadline, resolve_now);
                    if(second.state != target_state::ready || !same_plan(first.plan, second.plan))
                    {
                        last_failure = describe_state(
                            second.state == target_state::ready ? target_state::unstable : second.state,
                            expression,
                            second
                        );
                    }
                    else if(dispatch_action(
                                target,
                                expression,
                                action,
                                _position,
                                first.plan,
                                _inject,
                                _attempt_deadline,
                                delivery_failure
                    ))
                    {
                        if(first.plan.reaches_target)
                        {
                            return true;
                        }

                        // A menu was opened on the way: the action itself is planned again.
                        last_failure = "opening a menu to reach action '" + expression + "'";
                    }
                    else
                    {
                        last_failure = delivery_failure;
                    }
                }

                record_obstacle(last_failure);
                return false;
            });
        if(delivered)
        {
            return;
        }

        throw std::runtime_error(
                  action + " on '" + expression + "' timed out: " + with_closest_names(target, last_failure)
        );
    }

    //------------------------------------------------------------------------------

    /// Clicks what opens a color dialog, then sets the color and accepts it, both under one deadline.
    void set_color(
        std::string_view _expression,
        const action_injection& _open,
        const QColor& _color,
        duration_t _timeout
)
    {
        if(!_color.isValid())
        {
            throw std::invalid_argument("color must be valid");
        }

        const auto deadline = detail::deadline_after(_timeout);
        perform_action(_expression, centre_of, "set color", _open, _timeout);
        perform_on_modal(
            "set color",
            deadline,
            [_color](QWidget& _modal){detail::adaptors::choose_color(_modal, _color);});
    }

    //------------------------------------------------------------------------------

    /// Clicks, double clicks or right clicks a target; a menu on the way is opened with one left click.
    void mouse_action(
        std::string_view _expression,
        const click_position& _position,
        Qt::MouseButton _button,
        int _click_count,
        duration_t _timeout
)
    {
        const char* const label = _click_count == 2 ? "double click" : _button
                                  == Qt::RightButton ? "right click" : "click";
        perform_action(
            _expression,
            _position,
            label,
            [_button, _click_count](QWidget& _widget, const click_plan& _plan)
            {
                // A button or an action: through the Qt API, which a click's timing cannot make miss.
                if(_button == Qt::LeftButton && _click_count == 1 && !_plan.target.isNull()
                   && detail::adaptors::click_through_qt(*_plan.target, _widget, _plan.position, _plan.reaches_target))
                {
                    return;
                }

                const bool opening_menu = !_plan.reaches_target;
                detail::inject_mouse_clicks(
                    _widget,
                    _plan.position,
                    opening_menu ? 1 : _click_count,
                    opening_menu ? Qt::LeftButton : _button
                );
            },
            _timeout
        );
    }

    //------------------------------------------------------------------------------

    /// Sets a slider or spin box by keys, or by dragging its handle.
    void set_int(std::string_view _expression, int _value, bool _by_drag, duration_t _timeout)
    {
        const std::string expression(_expression);
        perform_action(
            _expression,
            [_by_drag](const QWidget& _widget){return detail::adaptors::slider_point(_widget, _by_drag);},
            "set",
            [_value, _by_drag, expression](QWidget& _widget, const click_plan& _plan)
            {
                detail::adaptors::set_int(*_plan.target, _widget, _plan.position, _value, _by_drag, expression);
            },
            _timeout
        );
    }

    //------------------------------------------------------------------------------

    /// Acts on the active modal dialog, dispatched like any input and tried again until one is shown and the action
    /// delivered, or the deadline.
    void perform_on_modal(
        const std::string& _action,
        detail::gui_deadline _deadline,
        const std::function<void(QWidget&)>& _act
)
    {
        ensure_scenario_thread();
        std::string last_failure = "no modal dialog is shown";
        const bool acted         = retry(
            _deadline,
            [&](detail::gui_deadline _attempt_deadline)
            {
                std::shared_ptr<detail::gui_dispatch::ticket> ticket;
                try
                {
                    // The dialog is looked for in the call that acts on it.
                    ticket = dispatcher.post_until(
                        _attempt_deadline,
                        [_act]{act_on_modal(_act);});
                    if(dispatcher.acknowledge_until(ticket, _attempt_deadline) != detail::dispatch_ack::returned)
                    {
                        pending.push_back(std::move(ticket));
                    }

                    return true;
                }
                catch(const detail::input_failure& failure)
                {
                    if(!failure.retryable())
                    {
                        throw;
                    }

                    last_failure = failure.what();
                    record_obstacle(last_failure);
                }
                catch(const detail::gui_call_cut&)
                {
                    throw;
                }
                catch(const std::runtime_error&)
                {
                    if(ticket != nullptr && std::chrono::steady_clock::now() >= _attempt_deadline)
                    {
                        pending.push_back(std::move(ticket));
                    }

                    throw;
                }

                return false;
            });
        if(acted)
        {
            return;
        }

        throw std::runtime_error(_action + " timed out: " + last_failure);
    }

    //------------------------------------------------------------------------------

    /// Runs a call on a worker of the application, as the gateway does on the GUI thread: the deadline bounds the wait,
    /// and a stopped event loop ends it. By value: a call still running at the deadline outlives this frame.
    template<typename R>
    R call_on_worker(core::thread::worker& _worker, std::function<R()> _call, detail::gui_deadline _deadline) const
    {
        const std::shared_future<R> result = _worker.post_task<R>(std::move(_call));
        while(result.wait_until(std::min(_deadline, std::chrono::steady_clock::now() + std::chrono::milliseconds(10)))
              != std::future_status::ready)
        {
            if(!gateway.is_loop_running())
            {
                throw std::runtime_error("the application event loop has stopped");
            }

            if(std::chrono::steady_clock::now() >= _deadline)
            {
                throw std::runtime_error("a worker of the application did not answer in time");
            }
        }

        return result.get();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] detail::gui_deadline operation_deadline(duration_t _timeout, const std::string& _operation) const
    {
        ensure_scenario_thread();
        if(_timeout <= duration_t::zero())
        {
            throw std::invalid_argument(_operation + " timeout must be positive");
        }

        return detail::deadline_after(_timeout);
    }

    //------------------------------------------------------------------------------

    /// Waits for two identical consecutive frames that _matches accepts. _last keeps the last frame, for the failure.
    QImage wait_for_image(
        std::string_view _expression,
        detail::gui_deadline _deadline,
        const std::function<bool(const QImage&)>& _matches,
        QImage& _last
)
    {
        const locator target = checked_locator(_expression);

        // A frame counts once the next one is the same: a scene still rendering never passes.
        detail::frame_stability stability;
        ambiguity_grace ambiguity;
        std::string obstacle = "no frame captured";

        // The GUI answers the two frames of the first comparison, however short the timeout.
        const auto first_attempt = detail::first_attempt_deadline(_deadline);
        int looks                = 0;
        std::string cut;
        const bool matched = retry(
            _deadline,
            [&](detail::gui_deadline _attempt_deadline)
            {
                const frame_observation frame = gateway.call_until(
                    looks++ < 2 ? std::max(first_attempt, _attempt_deadline) : _attempt_deadline,
                    [target]{return grab_frame(target);});

                // No pixels: the scene is missing, ambiguous or not ready, and stability starts over.
                ambiguity.check(frame.widget.state, _expression, frame.widget.match_scopes);
                if(frame.pixels.isNull())
                {
                    stability.reset();
                    obstacle = frame.widget.state == target_state::ready
                               ? frame.widget.value
                               : describe_state(frame.widget.state, _expression, frame.widget);
                }
                else
                {
                    _last     = frame.pixels.convertToFormat(QImage::Format_RGB32);
                    step_path = frame.widget.path;
                    seen_locators.insert(std::string(_expression));
                    if(!stability.observe(_last))
                    {
                        obstacle = "waiting for two consecutive identical RGB frames";
                    }
                    else if(_matches(_last))
                    {
                        return true;
                    }
                    else
                    {
                        obstacle = "stable frame does not satisfy the image assertion";
                    }
                }

                record_obstacle(obstacle);
                return false;
            },
            &cut
        );
        if(matched)
        {
            return _last;
        }

        if(!cut.empty())
        {
            obstacle = cut;
        }

        throw std::runtime_error("image on '" + std::string(_expression) + "' timed out: " + obstacle);
    }

    //------------------------------------------------------------------------------

    /// A stable frame, whatever it shows; a timeout leaves the last frame in the artifacts.
    QImage capture(std::string_view _expression, detail::gui_deadline _deadline)
    {
        QImage actual;
        try
        {
            return wait_for_image(_expression, _deadline, [](const QImage&){return true;}, actual);
        }
        catch(const std::runtime_error& failure)
        {
            image_failure(next_image_directory(), actual, {}, failure.what());
        }
    }

    //------------------------------------------------------------------------------

    /// Waits for a stable frame whose comparison with _reference is _accepted; a timeout writes the artifacts.
    void wait_for_comparison(
        std::string_view _expression,
        detail::gui_deadline _deadline,
        const QImage& _reference,
        const image_threshold& _threshold,
        const std::function<bool(const detail::image_metrics&)>& _accepted,
        const std::string& _expectation
)
    {
        QImage actual;
        detail::image_metrics metrics;
        try
        {
            static_cast<void>(wait_for_image(
                                  _expression,
                                  _deadline,
                                  [&](const QImage& _image)
                {
                    metrics = detail::compare_images(_image, _reference, _threshold);
                    return _accepted(metrics);
                },
                                  actual
            ));
        }
        catch(const std::runtime_error& failure)
        {
            std::string message = failure.what() + _expectation;

            // Even an unstable last frame is useful evidence. Without one there is nothing to measure.
            if(!actual.isNull())
            {
                message += "; " + detail::compare_images(actual, _reference, _threshold).describe();
            }

            image_failure(next_image_directory(), actual, _reference, message);
        }

        record_step("  image: " + metrics.describe());
    }

    //------------------------------------------------------------------------------

    void record_thresholds(const image_threshold& _threshold)
    {
        detail::validate_threshold(_threshold);
        record_step(
            std::format(
                "  image thresholds: channel {}, ratio {}{}",
                _threshold.channel,
                _threshold.max_bad_ratio,
                _threshold.reason.empty() ? std::string() : "; " + _threshold.reason
            )
        );
    }

    //------------------------------------------------------------------------------

    std::filesystem::path output_directory()
    {
        if(image_output.empty())
        {
            image_output = output_root() / ("capture-" + core::tools::uuid::generate());
        }

        return image_output;
    }

    //------------------------------------------------------------------------------

    std::filesystem::path next_image_directory()
    {
        return output_directory() / std::format("image-{:04}", ++image_number);
    }

    //------------------------------------------------------------------------------

    void choose_paths(
        const std::vector<std::filesystem::path>& _paths,
        std::string_view _filter,
        bool _folder,
        duration_t _timeout
)
    {
        // All the substeps share one deadline.
        const auto deadline = operation_deadline(_timeout, "file dialog");
        if(_paths.empty())
        {
            throw std::invalid_argument("file selection requires at least one path");
        }

        auto dialog = std::make_shared<detail::file_dialog>(_paths, utf8_text(_filter, "file filter"), _folder);
        perform_on_modal("prepare file dialog", deadline, [dialog](QWidget& _modal){dialog->prepare(_modal);});
        perform_on_modal("accept file dialog", deadline, [dialog](QWidget& _modal){dialog->accept(_modal);});
        wait_for_file_dialog_to_close(dialog, deadline);
    }

    //------------------------------------------------------------------------------

    void cancel_file_dialog(duration_t _timeout)
    {
        const auto deadline = operation_deadline(_timeout, "file dialog");
        auto dialog         = std::make_shared<detail::file_dialog>(
            std::vector<std::filesystem::path> {},
            QString {},
            false
        );
        perform_on_modal("cancel file dialog", deadline, [dialog](QWidget& _modal){dialog->cancel(_modal);});
        wait_for_file_dialog_to_close(dialog, deadline);
    }

    //------------------------------------------------------------------------------

    /// A button is clicked once: only the overwrite question it may open is acted on afterwards.
    void wait_for_file_dialog_to_close(
        const std::shared_ptr<detail::file_dialog>& _dialog,
        detail::gui_deadline _deadline
)
    {
        bool confirmed = false;
        std::string cut;
        const bool closed = retry(
            _deadline,
            [&](detail::gui_deadline _attempt_deadline)
            {
                // Closed, or asking whether to replace a file.
                const auto state = gateway.call_until(_attempt_deadline, [_dialog]{return _dialog->observe();});
                if(state == detail::file_dialog::status::closed)
                {
                    return true;
                }

                if(state == detail::file_dialog::status::overwrite && !confirmed)
                {
                    perform_on_modal(
                        "confirm file overwrite",
                        _deadline,
                        [_dialog](QWidget& _modal){_dialog->confirm_overwrite(_modal);});
                    confirmed = true;
                }

                return false;
            },
            &cut
        );
        if(closed)
        {
            return;
        }

        const std::string cause = cut.empty() ? std::string() : ": " + cut;
        throw std::runtime_error("the original file dialog did not close after its button was clicked" + cause);
    }

    //------------------------------------------------------------------------------

    void finish(duration_t _timeout)
    {
        // The handlers that opened a modal dialog or a popup return once it closed.
        const auto deadline = operation_deadline(_timeout, "GUI action");
        while(!pending.empty())
        {
            dispatcher.wait_returned_until(pending.front(), deadline);
            pending.erase(pending.begin());
        }

        // Update mode always fails, once every image of the scenario has its candidate.
        if(!saved_candidates.empty())
        {
            std::string saved;
            for(const auto& candidate : saved_candidates)
            {
                saved += "\n  " + detail::qt_path(candidate).toStdString();
            }

            throw std::runtime_error(
                      "reference update requested; " + std::to_string(saved_candidates.size())
                      + (saved_candidates.size() == 1 ? " candidate" : " candidates")
                      + " saved, test intentionally failed:" + saved
            );
        }
    }

    //------------------------------------------------------------------------------

    /// Adds a line to the step log, and to the test log.
    void record_step(std::string _line)
    {
        core::log::test_log::phase(_line);
        steps.push_back(std::move(_line));
    }

    //------------------------------------------------------------------------------

    /// Logs what keeps a step waiting, each time it changes: a slow step then tells what it waited for.
    void record_obstacle(const std::string& _obstacle)
    {
        constexpr std::size_t max_obstacles_per_step = 20;
        if(_obstacle != step_obstacle && step_obstacle_count < max_obstacles_per_step)
        {
            step_obstacle = _obstacle;
            ++step_obstacle_count;
            record_step("  waiting: " + _obstacle);
        }
    }

    //------------------------------------------------------------------------------

    /// A missing target also reports the names nearest to it. Its deadline has passed, so this one only bounds the
    /// call: a failure to find them does not hide the failure being reported.
    [[nodiscard]] std::string with_closest_names(const locator& _target, const std::string& _failure) const
    {
        if(!_failure.starts_with("no visible widget named") && !_failure.starts_with("no widget named"))
        {
            return _failure;
        }

        try
        {
            if(_target.segments().empty())
            {
                return _failure;
            }

            const std::string name                 = _target.segments().back().object_name;
            const std::vector<std::string> closest = gateway.call_until(
                detail::deadline_after(detail::LOOK_TIMEOUT),
                [name]{return detail::closest_visible_names(name);});
            return closest.empty() ? _failure : _failure + "; closest names: " + quoted_list(closest);
        }
        catch(const std::exception&)
        {
            return _failure;
        }
    }

    //------------------------------------------------------------------------------

    /// Writes what shows a failure, and returns the files written.
    std::vector<std::string> write_failure_artifacts(const std::filesystem::path& _directory)
    {
        std::vector<std::string> artifacts;
        std::filesystem::create_directories(_directory);

        // The steps first: they need nothing from the GUI.
        const std::filesystem::path steps_path = _directory / "steps.txt";
        if(std::ofstream steps_file(steps_path); steps_file)
        {
            for(const std::string& step : steps)
            {
                steps_file << step << '\n';
            }

            artifacts.push_back(steps_path.string());
        }

        struct capture
        {
            std::string tree;
            std::vector<std::string> images;
        };

        // Painted by the widgets, never grabbed from the screen: another window or a locked session cannot show.
        const capture captured = gateway.call_until(
            detail::deadline_after(detail::LOOK_TIMEOUT),
            [_directory]
            {
                capture result {.tree = detail::ui_tree(), .images = {}};
                int other_window      = 0;
                for(QWidget* const window : QApplication::topLevelWidgets())
                {
                    if(!detail::is_visible(*window))
                    {
                        continue;
                    }

                    const std::filesystem::path path = qobject_cast<const QMainWindow*>(window) != nullptr
                                                       ? _directory / "failure.png"
                                                       : _directory / std::format("failure_{}.png", ++other_window);
                    if(window->grab().save(QString::fromStdU16String(path.u16string())))
                    {
                        result.images.push_back(path.string());
                    }
                }

                return result;
            });
        artifacts.insert(artifacts.end(), captured.images.begin(), captured.images.end());

        // The tree of named widgets, as dump() prints it.
        const std::filesystem::path tree_path = _directory / "ui_tree.txt";
        if(std::ofstream tree_file(tree_path); tree_file)
        {
            tree_file << captured.tree;
            artifacts.push_back(tree_path.string());
        }

        return artifacts;
    }

    class step_scope;

    QApplication& application;
    /// Runs calls on the GUI thread and returns their results; the dispatcher does the same for input.
    detail::gui_gateway gateway;
    detail::gui_dispatch dispatcher;
    /// The XML names, to fail at once on a typo; none for engine tests.
    std::optional<locator_catalog> catalog;
    /// Where image failures and candidates go, one numbered folder per image.
    std::filesystem::path image_output;
    std::size_t image_number {0};
    /// The locators that matched once: gone() refuses one that never did.
    std::unordered_set<std::string> seen_locators;
    /// Input whose handler has not returned yet, as one that opened a modal dialog.
    std::vector<std::shared_ptr<detail::gui_dispatch::ticket> > pending;

    /// The step log, and the state of the step running now.
    std::vector<std::string> steps;
    /// What expect_image saved in update mode, to report when the scenario finishes.
    std::vector<std::filesystem::path> saved_candidates;
    int step_count {0};
    int step_depth {0};
    std::string step_path;
    std::string step_obstacle;
    std::size_t step_obstacle_count {0};
    std::string failed_step;
};

//------------------------------------------------------------------------------

/// One line of the step log per action or assertion of the scenario: one that calls another stays one step.
/// How a step reads in the log: `click("name")`, `set("name", 35)`.
static std::string step_text(std::string_view _verb, std::string_view _expression, std::string_view _argument = {})
{
    return _argument.empty()
           ? std::format("{}(\"{}\")", _verb, _expression)
           : std::format("{}(\"{}\", {})", _verb, _expression, _argument);
}

//------------------------------------------------------------------------------

class scenario::implementation::step_scope final
{
public:

    step_scope(implementation& _implementation, std::string _description) :
        m_implementation(_implementation),
        m_is_outermost(++_implementation.step_depth == 1),
        m_description(std::move(_description))
    {
        // Only the outermost step is numbered and logged.
        if(m_is_outermost)
        {
            m_number = ++m_implementation.step_count;
            m_implementation.step_path.clear();
            m_implementation.step_obstacle.clear();
            m_implementation.step_obstacle_count = 0;
            m_implementation.failed_step.clear();
        }
    }

    /// A step that reads as `verb("expression", argument)`.
    step_scope(
        implementation& _implementation,
        std::string_view _verb,
        std::string_view _expression,
        std::string_view _argument = {}) :
        step_scope(_implementation, step_text(_verb, _expression, _argument))
    {
    }

    step_scope(const step_scope&)            = delete;
    step_scope(step_scope&&)                 = delete;
    step_scope& operator=(const step_scope&) = delete;
    step_scope& operator=(step_scope&&)      = delete;

    ~step_scope()
    {
        --m_implementation.step_depth;
        if(!m_is_outermost)
        {
            return;
        }

        try
        {
            // Leaving through an exception means the step failed.
            const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - m_start;
            if(std::uncaught_exceptions() > m_exceptions)
            {
                m_implementation.failed_step =
                    std::format("step {}: {} failed after {:.1f} s", m_number, m_description, elapsed.count());
                m_implementation.record_step(m_implementation.failed_step);
            }
            else
            {
                const std::string& path = m_implementation.step_path;
                m_implementation.record_step(
                    std::format(
                        "step {}: {}{} {:.3f} s",
                        m_number,
                        m_description,
                        path.empty() ? std::string() : " -> " + path + ",",
                        elapsed.count()
                    )
                );
            }
        }
        catch(...) // NOLINT(bugprone-empty-catch)
        {
            // The log must not end the scenario, least of all during a failure.
        }
    }

private:

    implementation& m_implementation;
    bool m_is_outermost;
    std::string m_description;
    int m_number {0};
    int m_exceptions {std::uncaught_exceptions()};
    std::chrono::steady_clock::time_point m_start {std::chrono::steady_clock::now()};
};

//------------------------------------------------------------------------------

scenario::scenario(QApplication& _application, std::filesystem::path _image_output_directory) :
    m_implementation(std::make_unique<implementation>(_application, std::nullopt, std::move(_image_output_directory)))
{
}

//------------------------------------------------------------------------------

scenario::scenario(
    QApplication& _application,
    const locator_catalog& _catalog,
    std::filesystem::path _image_output_directory
) :
    m_implementation(std::make_unique<implementation>(_application, _catalog, std::move(_image_output_directory)))
{
}

//------------------------------------------------------------------------------

scenario::~scenario() = default;

//------------------------------------------------------------------------------

void scenario::prepare_window(QPointer<QWidget> _window)
{
    const implementation::step_scope step(*m_implementation, "prepare window");
    const auto deadline = m_implementation->operation_deadline(DEFAULT_TIMEOUT, "window");
    detail::prepare_test_window(m_implementation->gateway, _window, deadline);
}

//------------------------------------------------------------------------------

signal_watch scenario::arm_signal_watch(
    std::string_view _expression,
    signal_connector _connect,
    duration_t _timeout
) const
{
    const implementation::step_scope step(*m_implementation, "watch_signal", _expression);
    auto state = std::make_shared<detail::signal_watch_state>(
        std::string(_expression),
        m_implementation->gateway.loop_state(),
        m_implementation->application.thread()
    );
    // Connected on the GUI thread, before the action that emits: an emission cannot be missed. By value: a call still
    // running at the deadline outlives this frame.
    read_widget(
        _expression,
        [state, connector = std::move(_connect)](const QWidget& _widget)
        {
            // Weak: the connections never keep a watch alive, its destruction disconnects them.
            const std::weak_ptr<detail::signal_watch_state> weak = state;
            const auto mark                                      = [weak](bool detail::signal_watch_state::* _flag)
                                                                   {
                                                                       if(const auto locked = weak.lock(); locked)
                                                                       {
                                                                           locked->mark(_flag);
                                                                       }
                                                                   };

            state->signal_connection = connector(_widget, [mark]{mark(&detail::signal_watch_state::emitted);});
            if(!state->signal_connection)
            {
                throw std::invalid_argument("could not connect signal on '" + state->expression + "'");
            }

            // A destroyed widget ends the wait at once, instead of at the deadline.
            state->destroyed_connection = QObject::connect(
                &_widget,
                &QObject::destroyed,
                [mark]{mark(&detail::signal_watch_state::destroyed);});
        },
        _timeout
    );
    return signal_watch(std::move(state));
}

//------------------------------------------------------------------------------

service_locator scenario::service(std::string_view _expression) const
{
    m_implementation->ensure_scenario_thread();
    static_cast<void>(m_implementation->checked_locator(_expression));
    return service_locator(std::string(_expression));
}

//------------------------------------------------------------------------------

signal_watch scenario::watch_signal(
    const service_locator& _service,
    std::string_view _signal,
    duration_t _timeout
) const
{
    const std::string& expression = _service.expression();
    const implementation::step_scope step(*m_implementation, "watch_signal", expression, _signal);
    auto state = std::make_shared<detail::signal_watch_state>(
        expression,
        m_implementation->gateway.loop_state(),
        m_implementation->application.thread()
    );

    // By value: a call still running at the deadline outlives this frame.
    const auto arm = [name = detail::parse_scoped_uid(expression), signal_key = std::string(_signal), state]() -> bool
                     {
                         const auto services = detail::find_services(name);
                         if(services.empty())
                         {
                             return false;
                         }

                         if(services.size() > 1)
                         {
                             throw std::invalid_argument(
                                       std::to_string(services.size()) + " services bear the name '" + state->expression
                                       + "': scope it with the view of its configuration"
                             );
                         }

                         const auto signal = services.front()->signal(signal_key);
                         if(signal == nullptr)
                         {
                             throw std::invalid_argument(
                                       "service '" + state->expression + "' has no signal '" + signal_key + "'"
                             );
                         }

                         // Weak: the connection never keeps a watch alive, its destruction disconnects it.
                         const std::weak_ptr<detail::signal_watch_state> weak = state;
                         auto slot                                            = core::com::new_slot(
                             [weak]
            {
                if(const auto locked = weak.lock(); locked)
                {
                    locked->mark(&detail::signal_watch_state::emitted);
                }
            });
                         slot->set_worker(core::thread::get_default_worker());
                         state->service_connection = signal->connect(slot);
                         state->service_slot       = std::move(slot);
                         state->service            = services.front();
                         state->watches_service    = true;
                         return true;
                     };
    // A service of a configuration launched later is waited for.
    m_implementation->eventually(
        "service '" + expression + "' is running",
        [this, &arm](detail::gui_deadline _deadline){return m_implementation->gateway.call_until(_deadline, arm);},
        _timeout
    );
    return signal_watch(std::move(state));
}

//------------------------------------------------------------------------------

data::object::csptr scenario::try_object(std::string_view _expression, detail::gui_deadline _deadline) const
{
    m_implementation->ensure_scenario_thread();

    // A typo in the scope or the uid fails at once.
    const detail::scoped_uid name = detail::parse_scoped_uid(_expression);
    if(!name.scope.empty())
    {
        static_cast<void>(m_implementation->checked_locator(name.scope));
    }

    if(m_implementation->catalog.has_value() && !m_implementation->catalog->knows_object(name.uid))
    {
        throw std::invalid_argument("the XML declares no object '" + name.uid + "'");
    }

    // The GUI thread tells the ids of the name in its scope; a service is read on its own worker, which alone changes
    // its
    // data.
    const auto lookup = m_implementation->gateway.call_until(_deadline, [name]{return detail::look_up_objects(name);});
    std::map<core::thread::worker::sptr, sight::service::service_vector_t> by_worker;
    for(const auto& running : lookup.services)
    {
        by_worker[running->worker()].insert(running);
    }

    std::vector<data::object::csptr> objects;
    const auto add = [&objects](const std::vector<data::object::csptr>& _found)
                     {
                         for(const auto& object : _found)
                         {
                             if(std::ranges::find(objects, object) == objects.end())
                             {
                                 objects.push_back(object);
                             }
                         }
                     };
    add(lookup.created);
    for(const auto& [worker, services] : by_worker)
    {
        std::function<std::vector<data::object::csptr>()> read =
            [services, ids = lookup.ids]{return detail::deferred_objects(services, ids);};
        add(worker == nullptr ? read() : m_implementation->call_on_worker(*worker, read, _deadline));
    }

    if(objects.size() > 1)
    {
        throw std::invalid_argument(
                  std::to_string(objects.size()) + " objects bear the name '" + std::string(_expression)
                  + "': scope it with the view of its configuration"
        );
    }

    return objects.empty() ? nullptr : objects.front();
}

//------------------------------------------------------------------------------

data::object::csptr scenario::wait_for_object(std::string_view _expression, duration_t _timeout) const
{
    const implementation::step_scope step(*m_implementation, "data", _expression);
    data::object::csptr object;
    m_implementation->eventually(
        "object '" + std::string(_expression) + "' exists",
        [this, &object, _expression](detail::gui_deadline _deadline)
        {
            object = try_object(_expression, _deadline);
            return object != nullptr;
        },
        _timeout
    );
    return object;
}

//------------------------------------------------------------------------------

bool scenario::exists(std::string_view _expression, duration_t _timeout) const
{
    const implementation::step_scope step(*m_implementation, "exists", _expression);
    const auto deadline  = m_implementation->operation_deadline(_timeout, "exists");
    const locator target = m_implementation->checked_locator(_expression);
    do
    {
        // Its own deadline: "no" must be an answer of the GUI.
        const bool found = m_implementation->gateway.call_until(
            detail::deadline_after(detail::LOOK_TIMEOUT),
            [target]{return !target.resolve_widgets().empty() || !target.resolve_actions().empty();});
        if(found)
        {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    while(std::chrono::steady_clock::now() < deadline);

    return false;
}

//------------------------------------------------------------------------------

void scenario::hold(duration_t _duration, std::string_view _why)
{
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count();
    const implementation::step_scope step(*m_implementation, std::format("hold({} ms, \"{}\")", milliseconds, _why));
    m_implementation->ensure_scenario_thread();
    if(_why.empty())
    {
        throw std::invalid_argument("hold requires a reason: anything else waits on a condition");
    }

    if(_duration <= duration_t::zero())
    {
        throw std::invalid_argument("hold duration must be positive");
    }

    // Not scaled: the time is what is tested. A stopped application ends it early.
    const auto end = std::chrono::steady_clock::now() + _duration;
    while(std::chrono::steady_clock::now() < end)
    {
        if(!m_implementation->gateway.is_loop_running())
        {
            throw std::runtime_error("the application event loop has stopped");
        }

        std::this_thread::sleep_for(
            std::min<duration_t>(end - std::chrono::steady_clock::now(), std::chrono::milliseconds(10))
        );
    }
}

//------------------------------------------------------------------------------

void scenario::read_widget(
    std::string_view _expression,
    std::function<void(const QWidget&)> _reader,
    duration_t _timeout
) const
{
    const implementation::step_scope step(*m_implementation, "read", _expression);
    m_implementation->wait_for_widget(
        _expression,
        "read",
        {},
        [reader = std::move(_reader)](const QWidget& _widget)
        {
            reader(_widget);
            return std::pair<bool, std::string> {true, {}};
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::expect_widget(
    std::string_view _expression,
    std::string_view _description,
    std::function<std::pair<bool, std::string>(const QWidget&)> _probe,
    duration_t _timeout,
    detail::widget_query _query,
    bool _never_existed_ok
) const
{
    const implementation::step_scope step(*m_implementation,
                                          step_text("expect", _expression) + ": " + std::string(_description));
    if(_description.empty())
    {
        throw std::invalid_argument("GUI expectation description must not be empty");
    }

    m_implementation->wait_for_widget(_expression, "expect", _description, _probe, _timeout, _query, _never_existed_ok);
}

//------------------------------------------------------------------------------

widget_expectation scenario::expect(std::string_view _expression) const
{
    return {*this, _expression};
}

//------------------------------------------------------------------------------

QImage scenario::image(std::string_view _expression, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "image", _expression);
    return m_implementation->capture(_expression, m_implementation->operation_deadline(_timeout, "image"));
}

//------------------------------------------------------------------------------

QImage scenario::read_png(const std::filesystem::path& _path, duration_t _timeout) const
{
    // The last 12 bytes of every PNG: the length, type and CRC of its empty IEND chunk.
    static const QByteArray s_END = QByteArray::fromHex("0000000049454e44ae426082");

    QImage image;
    this->eventually(
        "a complete PNG in " + detail::qt_path(_path).toStdString(),
        [&]
        {
            QFile png(detail::qt_path(_path));
            return png.open(QIODevice::ReadOnly) && png.size() >= s_END.size()
                   && png.seek(png.size() - s_END.size()) && png.read(s_END.size()) == s_END
                   && image.load(detail::qt_path(_path));
        },
        _timeout
    );
    return image;
}

//------------------------------------------------------------------------------

std::filesystem::path scenario::output_directory()
{
    return m_implementation->output_directory();
}

//------------------------------------------------------------------------------

void scenario::expect_image(
    std::string_view _expression,
    const std::filesystem::path& _reference,
    const image_threshold& _threshold,
    duration_t _timeout
)
{
    const std::string reference = detail::qt_path(_reference).toStdString();
    const implementation::step_scope step(*m_implementation, "expect_image", _expression, reference);
    const auto deadline = m_implementation->operation_deadline(_timeout, "image");
    m_implementation->record_thresholds(_threshold);

    if(qgetenv("SIGHT_UI_TEST_UPDATE_REFERENCES") == "1")
    {
        // Never open the canonical reference for writing, even when it already matches. The scenario goes on, so
        // that each of its images gets a candidate, and fails when it finishes.
        const QImage candidate = m_implementation->capture(_expression, deadline);
        auto output            = m_implementation->next_image_directory() / "candidate.png";
        detail::save_image(candidate, output);
        m_implementation->saved_candidates.push_back(std::move(output));
        return;
    }

    const QImage expected(detail::qt_path(_reference));
    if(expected.isNull())
    {
        throw std::invalid_argument("cannot read image reference: " + reference);
    }

    m_implementation->wait_for_comparison(
        _expression,
        deadline,
        expected,
        _threshold,
        [](const detail::image_metrics& _metrics){return _metrics.matches();},
        {
        });
}

//------------------------------------------------------------------------------

void scenario::expect_image_changes(
    std::string_view _expression,
    const std::function<void()>& _action,
    const image_threshold& _threshold,
    duration_t _timeout
)
{
    const implementation::step_scope step(*m_implementation, "expect_image_changes", _expression);
    const auto deadline = m_implementation->operation_deadline(_timeout, "image");
    m_implementation->record_thresholds(_threshold);

    const QImage before = m_implementation->capture(_expression, deadline);
    _action();

    // The action has its own deadlines, so the time it took is not taken from the observation.
    m_implementation->wait_for_comparison(
        _expression,
        m_implementation->operation_deadline(_timeout, "image"),
        before,
        _threshold,
        // A resized scene is not evidence that the tested rendering action worked.
        [](const detail::image_metrics& _metrics){return _metrics.same_size && !_metrics.matches();},
        "; expected an image change"
    );
}

//------------------------------------------------------------------------------

widget_expectation::widget_expectation(const scenario& _scenario, std::string_view _expression) :
    m_scenario(&_scenario),
    m_expression(_expression)
{
}

//------------------------------------------------------------------------------

widget_expectation& widget_expectation::within(duration_t _timeout)
{
    if(_timeout <= duration_t::zero())
    {
        throw std::invalid_argument("GUI expectation timeout must be positive");
    }

    m_timeout = _timeout;
    return *this;
}

//------------------------------------------------------------------------------

void scenario::eventually_impl(
    std::string_view _description,
    std::function<bool(std::chrono::steady_clock::time_point)> _predicate,
    duration_t _timeout
) const
{
    const implementation::step_scope step(*m_implementation, "eventually(\"" + std::string(_description) + "\")");
    m_implementation->eventually(_description, _predicate, _timeout);
}

//------------------------------------------------------------------------------

void widget_expectation::check(
    std::string_view _description,
    std::function<std::pair<bool, std::string>(const QWidget&)> _probe,
    detail::widget_query _query
) const
{
    // Every matcher comes here, with the query telling which widgets it looks at.
    if(m_never_existed_ok && _query != detail::widget_query::gone)
    {
        throw std::invalid_argument("never_existed_ok() applies only to gone()");
    }

    m_scenario->expect_widget(m_expression, _description, std::move(_probe), m_timeout, _query, m_never_existed_ok);
}

//------------------------------------------------------------------------------

widget_expectation& widget_expectation::never_existed_ok()
{
    m_never_existed_ok = true;
    return *this;
}

//------------------------------------------------------------------------------

void widget_expectation::text(std::string_view _expected) const
{
    const QString expected = QString::fromStdString(std::string(_expected));
    check(
        "text is " + quote_text(expected),
        text_probe("text", [expected](const QString& _text){return _text == expected;})
    );
}

//------------------------------------------------------------------------------

void widget_expectation::text_contains(std::string_view _expected) const
{
    const QString expected = QString::fromStdString(std::string(_expected));
    check(
        "text contains " + quote_text(expected),
        text_probe("text_contains", [expected](const QString& _text){return _text.contains(expected);})
    );
}

//------------------------------------------------------------------------------

void widget_expectation::text_matches(std::string_view _pattern) const
{
    const QString pattern = QString::fromStdString(std::string(_pattern));

    // The whole text: \A(?:pattern)\z also tries every alternative, which an anchored search does not.
    const QRegularExpression expression(QRegularExpression::anchoredPattern(pattern));
    if(!expression.isValid())
    {
        throw std::invalid_argument(
                  "text_matches received an invalid regular expression: " + expression.errorString().toStdString()
        );
    }

    check(
        "text matches /" + pattern.toStdString() + "/",
        text_probe("text_matches", [expression](const QString& _text){return expression.match(_text).hasMatch();})
    );
}

//------------------------------------------------------------------------------

void widget_expectation::number(double _expected, double _tolerance) const
{
    if(!std::isfinite(_expected) || !std::isfinite(_tolerance) || _tolerance < 0.)
    {
        throw std::invalid_argument("number requires a finite value and a non-negative tolerance");
    }

    check(
        "number is " + format_number(_expected) + " within " + format_number(_tolerance),
        [_expected, _tolerance](const QWidget& _widget)
        {
            // A value widget gives its value, any other its text read as a number.
            if(const std::optional<double> actual = numeric_widget_value(_widget); actual.has_value())
            {
                return std::pair {std::abs(*actual - _expected) <= _tolerance, format_number(*actual)};
            }

            const QString text  = widget_text(_widget, "number");
            bool converted      = false;
            const double parsed = text.toDouble(&converted);
            const bool matches  = converted && std::abs(parsed - _expected) <= _tolerance;
            return std::pair {matches, quote_text(text)};
        });
}

//------------------------------------------------------------------------------

void widget_expectation::value(int _expected) const
{
    value(static_cast<double>(_expected));
}

//------------------------------------------------------------------------------

void widget_expectation::value(double _expected) const
{
    if(!std::isfinite(_expected))
    {
        throw std::invalid_argument("value requires a finite number");
    }

    // The value itself, not its text: no locale nor format to agree on.
    check(
        "value is " + format_number(_expected),
        [_expected](const QWidget& _widget)
        {
            const std::optional<double> actual = numeric_widget_value(_widget);
            if(!actual.has_value())
            {
                throw std::invalid_argument(
                          "value requires a slider, spin box or combo box, found "
                          + std::string(_widget.metaObject()->className())
                );
            }

            return std::pair {*actual == _expected, format_number(*actual)};
        });
}

//------------------------------------------------------------------------------

/// Whether a checkable button is in the expected state; an action without a widget answers through its state.
static widget_probe checked_probe(bool _expected)
{
    return [_expected](const QWidget& _widget)
           {
               const auto* const button = qobject_cast<const QAbstractButton*>(&_widget);
               if(button == nullptr || !button->isCheckable())
               {
                   throw std::invalid_argument(
                             "a checked state requires a checkable button or action, found "
                             + std::string(_widget.metaObject()->className())
                   );
               }

               const bool checked = button->isChecked();
               return std::pair {checked == _expected, std::string(checked ? "checked" : "unchecked")};
           };
}

//------------------------------------------------------------------------------

void widget_expectation::checked() const
{
    check("is checked", checked_probe(true), detail::widget_query::checked);
}

//------------------------------------------------------------------------------

void widget_expectation::unchecked() const
{
    check("is unchecked", checked_probe(false), detail::widget_query::unchecked);
}

//------------------------------------------------------------------------------

void widget_expectation::enabled() const
{
    check("is enabled", state_probe(&widget_enabled, true, "enabled", "disabled"), detail::widget_query::enabled);
}

//------------------------------------------------------------------------------

void widget_expectation::disabled() const
{
    check("is disabled", state_probe(&widget_enabled, false, "enabled", "disabled"), detail::widget_query::disabled);
}

//------------------------------------------------------------------------------

void widget_expectation::visible() const
{
    check("is visible", state_probe(&widget_visible, true, "visible", "not visible"), detail::widget_query::is_visible);
}

//------------------------------------------------------------------------------

void widget_expectation::hidden() const
{
    check("is hidden", state_probe(&widget_visible, false, "visible", "not visible"), detail::widget_query::is_hidden);
}

//------------------------------------------------------------------------------

void widget_expectation::gone() const
{
    // The engine answers gone() from the resolution alone: the probe never runs.
    check("is gone", [](const QWidget&){return std::pair<bool, std::string> {false, {}};}, detail::widget_query::gone);
}

//------------------------------------------------------------------------------

void widget_expectation::tooltip(std::string_view _expected) const
{
    const QString expected = QString::fromStdString(std::string(_expected));
    check(
        "tooltip is " + quote_text(expected),
        [expected](const QWidget& _widget)
        {
            const QString actual = _widget.toolTip();
            return std::pair {actual == expected, quote_text(actual)};
        });
}

//------------------------------------------------------------------------------

void widget_expectation::rows(int _expected) const
{
    if(_expected < 0)
    {
        throw std::invalid_argument("rows requires a non-negative count");
    }

    check(
        "has " + std::to_string(_expected) + " root-level rows",
        [_expected](const QWidget& _widget)
        {
            const auto* const view = qobject_cast<const QAbstractItemView*>(&_widget);
            if(view == nullptr)
            {
                throw std::invalid_argument(
                          "rows requires an item view, found " + std::string(_widget.metaObject()->className())
                );
            }

            const QAbstractItemModel* const model = view->model();
            const int actual                      = model == nullptr ? 0 : model->rowCount(view->rootIndex());
            return std::pair {actual == _expected, std::to_string(actual)};
        });
}

//------------------------------------------------------------------------------

void scenario::click(std::string_view _expression, relative_position _position, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "click", _expression);
    validate_relative_position(_position);
    m_implementation->mouse_action(
        _expression,
        [_position](const QWidget& _widget){return map_relative_position(_widget, _position);},
        Qt::LeftButton,
        1,
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::click(std::string_view _expression, pixel_position _position, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "click", _expression);
    if(_position.x < 0 || _position.y < 0)
    {
        throw std::invalid_argument("pixel click coordinates cannot be negative");
    }

    m_implementation->mouse_action(
        _expression,
        [_position](const QWidget&){return QPoint(_position.x, _position.y);},
        Qt::LeftButton,
        1,
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::double_click(std::string_view _expression, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "double_click", _expression);
    m_implementation->mouse_action(_expression, centre_of, Qt::LeftButton, 2, _timeout);
}

//------------------------------------------------------------------------------

void scenario::right_click(std::string_view _expression, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "right_click", _expression);
    m_implementation->mouse_action(_expression, centre_of, Qt::RightButton, 1, _timeout);
}

//------------------------------------------------------------------------------

void scenario::drag(std::string_view _expression, relative_position _from, relative_position _to, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "drag", _expression);

    // The start is on the target; the end may leave it, as a drag out of a widget does.
    validate_relative_position(_from);
    constexpr double farthest = 10.;
    if(!std::isfinite(_to.x) || !std::isfinite(_to.y) || std::abs(_to.x) > farthest || std::abs(_to.y) > farthest)
    {
        throw std::invalid_argument("a drag destination must be finite and within ten target sizes");
    }

    m_implementation->perform_action(
        _expression,
        [_from](const QWidget& _widget){return map_relative_position(_widget, _from);},
        "drag",
        [_to](QWidget& _widget, const click_plan& _plan)
        {
            require_widget_target(_plan, "drag");

            detail::inject_mouse_drag(_widget, _plan.position, map_relative_position(_widget, _to));
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::wheel(std::string_view _expression, int _notches, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "wheel", _expression);
    // One event per notch, at the centre: Sight widgets count events.
    detail::validate_mouse_wheel_notches(_notches);
    m_implementation->perform_action(
        _expression,
        centre_of,
        "wheel",
        [_notches](QWidget& _widget, const click_plan& _plan)
        {
            detail::inject_mouse_wheel(_widget, _plan.position, _notches);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::scroll_to(std::string_view _expression, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "scroll_to", _expression);
    m_implementation->scroll_to(_expression, _timeout);
}

//------------------------------------------------------------------------------

void scenario::pinch(std::string_view _expression, double _scale, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "pinch", _expression);
    constexpr double largest_scale = 10.;
    if(!std::isfinite(_scale) || _scale <= 0. || _scale > largest_scale)
    {
        throw std::invalid_argument("pinch scale must be in (0, 10]");
    }

    m_implementation->perform_action(
        _expression,
        centre_of,
        "pinch",
        [_scale](QWidget& _widget, const click_plan& _plan)
        {
            require_widget_target(_plan, "pinch");

            // Two fingers on a horizontal line through the centre, two thirds of the target apart, moved by the scale.
            const QRect rect            = _widget.rect();
            const int minimum_dimension = std::min(rect.width(), rect.height());
            const int start_distance    = 2 * (minimum_dimension / 3);
            if(start_distance < 2)
            {
                throw std::invalid_argument("touch pinch requires a widget at least three pixels wide and high");
            }

            const int centre_x            = rect.width() / 2;
            const int centre_y            = rect.height() / 2;
            const int start_half_distance = start_distance / 2;
            const int end_half_distance   = static_cast<int>(std::lround(start_distance * _scale / 2.));

            const QPoint first_start(centre_x - start_half_distance, centre_y);
            const QPoint second_start(centre_x + start_half_distance, centre_y);
            const QPoint first_end(centre_x - end_half_distance, centre_y);
            const QPoint second_end(centre_x + end_half_distance, centre_y);
            detail::inject_touch_pinch(
                _widget,
                {first_start, first_end},
                {second_start, second_end
                });
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::type(std::string_view _expression, std::string_view _text, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "type", _expression, '"' + std::string(_text) + '"');
    if(_text.empty())
    {
        return;
    }

    const QString text = utf8_text(_text, "typed text");

    // The keys go to the target itself: its handler runs as for a user, whatever has the focus.
    m_implementation->perform_action(
        _expression,
        centre_of,
        "type",
        [text](QWidget& _widget, const click_plan& _plan)
        {
            require_widget_target(_plan, "type");
            detail::adaptors::type_text(_widget, text);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::press(std::string_view _expression, std::string_view _key_sequence, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "press", _expression, _key_sequence);
    if(_key_sequence.empty())
    {
        throw std::invalid_argument("key sequence must not be empty");
    }

    // Portable names, as "Ctrl+S", and only keys that exist.
    const QKeySequence sequence = QKeySequence::fromString(
        utf8_text(_key_sequence, "key sequence"),
        QKeySequence::PortableText
    );
    if(sequence.isEmpty())
    {
        throw std::invalid_argument("key sequence is invalid");
    }

    for(int index = 0 ; index < sequence.count() ; ++index)
    {
        if(sequence[static_cast<uint>(index)].key() == Qt::Key_unknown)
        {
            throw std::invalid_argument("key sequence contains an unknown key");
        }
    }

    m_implementation->perform_action(
        _expression,
        centre_of,
        "press",
        [sequence](QWidget& _widget, const click_plan& _plan)
        {
            require_widget_target(_plan, "press");

            detail::give_focus(_widget);
            detail::inject_key_sequence(_widget, sequence);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, bool _checked, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "set", _expression, _checked ? "true" : "false");
    const std::string expression(_expression);
    m_implementation->perform_action(
        _expression,
        [](const QWidget& _widget){return detail::adaptors::checkable_point(_widget);},
        "set",
        [_checked, expression](QWidget& _widget, const click_plan& _plan)
        {
            detail::adaptors::set_checked(*_plan.target, _widget, _plan.position, _checked, expression);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, int _value, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "set", _expression, std::to_string(_value));
    m_implementation->set_int(_expression, _value, false, _timeout);
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, int _value, by_drag_t /*unused*/, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "set", _expression, std::to_string(_value) + ", by_drag");
    m_implementation->set_int(_expression, _value, true, _timeout);
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, double _value, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "set", _expression, std::to_string(_value));
    if(!std::isfinite(_value))
    {
        throw std::invalid_argument("spin box value must be finite");
    }

    // Typed in the spin box, as a user enters a value it cannot reach by its steps.
    const std::string expression(_expression);
    m_implementation->perform_action(
        _expression,
        centre_of,
        "set",
        [_value, expression](QWidget&, const click_plan& _plan)
        {
            detail::adaptors::set_double(*_plan.target, _value, expression);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, std::string_view _value, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "set", _expression, '"' + std::string(_value) + '"');
    // Replaces the text of a line edit, or picks it in a combo box.
    const QString text = utf8_text(_value, "set text");
    const std::string expression(_expression);
    m_implementation->perform_action(
        _expression,
        centre_of,
        "set",
        [text, expression](QWidget&, const click_plan& _plan)
        {
            detail::adaptors::set_text(*_plan.target, text, expression);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, const char* _value, duration_t _timeout)
{
    if(_value == nullptr)
    {
        throw std::invalid_argument("set text must not be null");
    }

    set(_expression, std::string_view(_value), _timeout);
}

//------------------------------------------------------------------------------

void scenario::set(const item_locator& _target, const QColor& _color, duration_t _timeout)
{
    const std::string color = _color.name().toStdString();
    const implementation::step_scope step(*m_implementation, "set", _target.view_expression(), color);
    // The button in the item opens the dialog.
    m_implementation->set_color(
        _target.view_expression(),
        [_target](QWidget&, const click_plan& _plan){detail::adaptors::click_item_widget(*_plan.target, _target);},
        _color,
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::set(std::string_view _expression, const QColor& _color, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "set", _expression, _color.name().toStdString());
    // A click on the button opens the dialog.
    m_implementation->set_color(
        _expression,
        [](QWidget& _widget, const click_plan& _plan){detail::inject_mouse_click(_widget, _plan.position);},
        _color,
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::select(const item_locator& _target, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "select", _target.view_expression());

    // Its ancestors are expanded on the way, by their branch indicators.
    m_implementation->perform_action(
        _target.view_expression(),
        centre_of,
        "select",
        [_target](QWidget&, const click_plan& _plan){detail::adaptors::select_item(*_plan.target, _target);},
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::select(std::string_view _expression, std::string_view _text, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "select", _expression, '"' + std::string(_text) + '"');
    // Through the Qt API of the combo box, as Qt does once a user picked an item: see select_combo_item().
    const QString text = utf8_text(_text, "selection text");
    const std::string expression(_expression);
    m_implementation->perform_action(
        _expression,
        centre_of,
        "select",
        [text, expression](QWidget&, const click_plan& _plan)
        {
            detail::adaptors::select_text(*_plan.target, text, expression);
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::choose_file(const std::filesystem::path& _path, std::string_view _filter, duration_t _timeout)
{
    const std::string path = detail::qt_path(_path).toStdString();
    const implementation::step_scope step(*m_implementation, "choose_file", path, _filter);
    m_implementation->choose_paths({_path}, _filter, false, _timeout);
}

//------------------------------------------------------------------------------

void scenario::choose_files(
    const std::vector<std::filesystem::path>& _paths,
    std::string_view _filter,
    duration_t _timeout
)
{
    const implementation::step_scope step(*m_implementation, "choose_files", _filter);
    m_implementation->choose_paths(_paths, _filter, false, _timeout);
}

//------------------------------------------------------------------------------

void scenario::choose_folder(const std::filesystem::path& _path, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "choose_folder", detail::qt_path(_path).toStdString());
    m_implementation->choose_paths({_path}, {}, true, _timeout);
}

//------------------------------------------------------------------------------

void scenario::cancel_file_dialog(duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "cancel_file_dialog");
    m_implementation->cancel_file_dialog(_timeout);
}

//------------------------------------------------------------------------------

void scenario::close(std::string_view _expression, duration_t _timeout)
{
    const implementation::step_scope step(*m_implementation, "close", _expression);
    m_implementation->perform_action(
        _expression,
        centre_of,
        "close",
        [](QWidget& _widget, const click_plan& _plan)
        {
            if(_plan.target != &_widget)
            {
                throw std::invalid_argument("close needs a widget locator, not an action");
            }

            // A title bar button cannot close a window under a modal dialog.
            QWindow& window = detail::check_exposed_window(_widget, "close");
            detail::check_not_blocked(_widget, "close");

            // In Qt 6, QWindow::close() is the request a title bar button sends (QPlatformWindow::close(), delivered
            // synchronously), so a confirmation dialog opened by closeEvent() is acknowledged and waited for like
            // one opened by a click. Qt only marks its QCloseEvent non-spontaneous, which no Qt widget reads, and
            // would let it through a modal dialog, which the check above refuses.
            window.close();
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

void scenario::finish(duration_t _timeout)
{
    m_implementation->finish(_timeout);
}

//------------------------------------------------------------------------------

std::string scenario::dump()
{
    m_implementation->ensure_scenario_thread();
    std::string tree = m_implementation->gateway.call_until(
        detail::deadline_after(detail::LOOK_TIMEOUT),
        []{return detail::ui_tree();});
    std::cout << tree << std::flush;
    return tree;
}

//------------------------------------------------------------------------------

std::vector<std::string> scenario::naming_violations()
{
    m_implementation->ensure_scenario_thread();
    return m_implementation->gateway.call_until(
        detail::deadline_after(detail::LOOK_TIMEOUT),
        []{return detail::naming_violations();});
}

//------------------------------------------------------------------------------

void scenario::expect_valid_names()
{
    const implementation::step_scope step(*m_implementation, "expect_valid_names()");
    const std::vector<std::string> violations = naming_violations();
    if(violations.empty())
    {
        return;
    }

    // One line per object: the failure tells each, with where it is.
    std::string message = std::format("{} object(s) break the naming contract:", violations.size());
    for(const std::string& violation : violations)
    {
        message += "\n  " + violation;
    }

    throw std::runtime_error(message);
}

//------------------------------------------------------------------------------

const std::vector<std::string>& scenario::step_log() const noexcept
{
    return m_implementation->steps;
}

//------------------------------------------------------------------------------

std::string scenario::report_failure(const std::filesystem::path& _directory, std::string_view _what) const
{
    const std::string& failed_step = m_implementation->failed_step;
    std::string report             = failed_step.empty()
                                     ? std::format("after step {}: {}", m_implementation->step_count, _what)
                                     : std::format("{}\n  {}", failed_step, _what);
    try
    {
        const std::vector<std::string> artifacts = m_implementation->write_failure_artifacts(_directory);
        report += "\n  artifacts: ";
        for(const std::string& artifact : artifacts)
        {
            report += artifact + (&artifact == &artifacts.back() ? "" : ", ");
        }
    }
    catch(const std::exception& exception)
    {
        // The application may be stuck, or gone: the failure still has to be told.
        report += std::string("\n  artifacts not written: ") + exception.what();
    }

    core::log::test_log::phase(report);
    return report;
}

} // namespace sight::ui::test
