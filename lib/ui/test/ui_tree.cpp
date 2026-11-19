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

#include "detail/ui_tree.hpp"

#include "detail/locator.hpp"
#include "detail/widget_visibility.hpp"

#include <ui/qt/naming.hpp>

#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QScrollBar>
#include <QSpinBox>
#include <QTabBar>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>
#include <QWidgetAction>
#include <QWindow>

#include <algorithm>
#include <format>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sight::ui::test::detail
{

//------------------------------------------------------------------------------

/// A name that Qt gives its own inner widgets, which nothing in a test refers to.
static bool is_qt_name(const QString& _name)
{
    return _name.startsWith(QStringLiteral("qt_"));
}

//------------------------------------------------------------------------------

/// A control whose inner widgets are its own business: they are reached through it.
static bool is_control(const QWidget& _widget)
{
    return qobject_cast<const QAbstractSpinBox*>(&_widget) != nullptr
           || qobject_cast<const QComboBox*>(&_widget) != nullptr
           || qobject_cast<const QAbstractItemView*>(&_widget) != nullptr
           || qobject_cast<const QTabBar*>(&_widget) != nullptr;
}

//------------------------------------------------------------------------------

/// A dialog of Qt keeps Qt's names, which are not a matter of the naming contract.
static bool is_qt_dialog(const QWidget& _window)
{
    return qobject_cast<const QFileDialog*>(&_window) != nullptr
           || qobject_cast<const QColorDialog*>(&_window) != nullptr
           || qobject_cast<const QFontDialog*>(&_window) != nullptr;
}

//------------------------------------------------------------------------------

/// A widget a user acts on, or reads as text. A menu bar or a toolbar holds what a user acts on: its actions.
static bool is_interactive(const QWidget& _widget)
{
    if(const auto* const button = qobject_cast<const QToolButton*>(&_widget); button != nullptr)
    {
        // A tool button that shows an action is that action, listed as such; Qt's own, such as the toolbar extension,
        // is not a matter for a test.
        return button->defaultAction() == nullptr && !is_qt_name(button->objectName());
    }

    return qobject_cast<const QAbstractButton*>(&_widget) != nullptr
           || (qobject_cast<const QAbstractSlider*>(&_widget) != nullptr
               && qobject_cast<const QScrollBar*>(&_widget) == nullptr)
           || qobject_cast<const QLineEdit*>(&_widget) != nullptr
           || is_control(_widget);
}

//------------------------------------------------------------------------------

/// The state a test looks at, as ` key=value` pairs.
static std::string describe_widget(const QWidget& _widget)
{
    std::string state;
    if(const auto* const label = qobject_cast<const QLabel*>(&_widget); label != nullptr)
    {
        state += std::format(" text=\"{}\"", label->text().toStdString());
    }
    else if(const auto* const line_edit = qobject_cast<const QLineEdit*>(&_widget); line_edit != nullptr)
    {
        state += std::format(" text=\"{}\"", line_edit->text().toStdString());
    }
    else if(const auto* const button = qobject_cast<const QAbstractButton*>(&_widget); button != nullptr)
    {
        if(!button->text().isEmpty())
        {
            state += std::format(" text=\"{}\"", button->text().toStdString());
        }

        if(button->isCheckable())
        {
            state += std::format(" checked={}", button->isChecked());
        }
    }
    else if(const auto* const slider = qobject_cast<const QAbstractSlider*>(&_widget); slider != nullptr)
    {
        state += std::format(" value={}", slider->value());
    }
    else if(const auto* const spin_box = qobject_cast<const QSpinBox*>(&_widget); spin_box != nullptr)
    {
        state += std::format(" value={}", spin_box->value());
    }
    else if(const auto* const double_spin_box = qobject_cast<const QDoubleSpinBox*>(&_widget);
            double_spin_box != nullptr)
    {
        state += std::format(" value={}", double_spin_box->value());
    }
    else if(const auto* const combo_box = qobject_cast<const QComboBox*>(&_widget); combo_box != nullptr)
    {
        state += std::format(" text=\"{}\"", combo_box->currentText().toStdString());
    }
    else if(const auto* const view = qobject_cast<const QAbstractItemView*>(&_widget);
            view != nullptr && view->model() != nullptr)
    {
        state += std::format(" rows={}", view->model()->rowCount(view->rootIndex()));
    }

    if(!_widget.isEnabled())
    {
        state += " enabled=false";
    }

    return state;
}

//------------------------------------------------------------------------------

/// How many visible widgets a locator expression resolves to, none when it is not a locator.
static std::vector<QWidget*> resolve(const std::string& _expression)
{
    try
    {
        return locator(_expression).resolve_widgets();
    }
    catch(const std::invalid_argument&)
    {
        return {};
    }
}

//------------------------------------------------------------------------------

/// The shortest locator that resolves to this widget: enclosing names are added until it is the only match, and when
/// they tell nothing apart, one role repeats in one scope, the fewest of them with an ordinal.
static std::string locator_of(const QWidget& _widget, bool& _is_weak)
{
    std::string scoped = _widget.objectName().toStdString();
    std::vector<std::string> candidates;
    for(const QWidget* parent = _widget.parentWidget() ; ; parent = parent->parentWidget())
    {
        const std::vector<QWidget*> matches = resolve(scoped);
        if(matches.size() == 1 && matches.front() == &_widget)
        {
            return scoped;
        }

        candidates.push_back(scoped);
        if(parent == nullptr)
        {
            for(const std::string& candidate : candidates)
            {
                const std::vector<QWidget*> same = resolve(candidate);
                if(same.size() == matches.size())
                {
                    const auto found = std::ranges::find(same, &_widget);
                    if(found != same.end())
                    {
                        _is_weak = true;
                        return std::format("{}#{}", candidate, std::distance(same.begin(), found) + 1);
                    }
                }
            }

            return scoped;
        }

        if(!parent->objectName().isEmpty() && !is_qt_name(parent->objectName()))
        {
            scoped = parent->objectName().toStdString() + " >> " + scoped;
        }
    }
}

//------------------------------------------------------------------------------

namespace
{

/// One walk of the visible objects: the dump of a scenario, the naming violations, or both.
class tree_walk final
{
public:

    explicit tree_walk(bool _with_text) :
        m_with_text(_with_text)
    {
    }

    //------------------------------------------------------------------------------

    void window(const QWidget& _window)
    {
        const bool in_qt_dialog = is_qt_dialog(_window);
        write(window_line(_window));

        // A dialog or a menu is an element a test reaches by name, a plain window is not.
        const bool must_be_named = qobject_cast<const QDialog*>(&_window) != nullptr
                                   || qobject_cast<const QMenu*>(&_window) != nullptr;
        check(_window, !must_be_named, in_qt_dialog);
        write("\n");

        if(const auto* const menu = qobject_cast<const QMenu*>(&_window); menu != nullptr)
        {
            actions(menu->actions(), 1, in_qt_dialog);
        }

        children(_window, 1, in_qt_dialog);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] const std::string& tree() const noexcept
    {
        return m_tree;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::vector<std::string> violations() && noexcept
    {
        return std::move(m_violations);
    }

private:

    //------------------------------------------------------------------------------

    void write(std::string_view _text)
    {
        if(m_with_text)
        {
            m_tree += _text;
        }
    }

    //------------------------------------------------------------------------------

    /// Marks an object breaking the contract: in the dump on its line, in the list with where it is.
    void mark(const QObject& _object, std::string_view _reason, bool _after_weak)
    {
        write(std::format("{}{}: breaks the naming contract", _after_weak ? "; " : "   <- ", _reason));
        m_violations.push_back(std::format("{}: {} in {}", _reason, describe_object(_object), ancestry(_object)));
    }

    //------------------------------------------------------------------------------

    /// Checks the name of an object: given by a sanctioned path, or none when it may have none.
    void check(const QObject& _object, bool _may_be_unnamed, bool _in_qt_dialog, bool _after_weak = false)
    {
        // Qt names the widgets of its own dialogs.
        if(_in_qt_dialog)
        {
            return;
        }

        if(!has_name(_object))
        {
            if(!_may_be_unnamed)
            {
                mark(_object, "no name", _after_weak);
            }
        }
        else if(!ui::qt::detail::has_sanctioned_name(_object))
        {
            mark(_object, "explicit name", _after_weak);
        }
    }

    //------------------------------------------------------------------------------

    void actions(const QList<QAction*>& _actions, int _depth, bool _in_qt_dialog)
    {
        for(const QAction* const action : _actions)
        {
            this->action(*action, _depth, _in_qt_dialog);
        }
    }

    //------------------------------------------------------------------------------

    void action(const QAction& _action, int _depth, bool _in_qt_dialog)
    {
        // A widget in a toolbar has an action of its own, without text, and is listed as a widget.
        if(_action.isSeparator() || !_action.isVisible() || qobject_cast<const QWidgetAction*>(&_action) != nullptr)
        {
            return;
        }

        // The action of a menu is that menu: one element, one name.
        const QMenu* const menu = _action.menu();
        const bool is_menu      = menu != nullptr && !has_name(_action);
        const QObject& element  = is_menu ? static_cast<const QObject&>(*menu) : _action;
        const QString name      = has_name(element) ? element.objectName() : _action.text();
        const std::string indent(static_cast<std::size_t>(_depth) * 2, ' ');
        write(std::format("{}{} \"{}\"", indent, is_menu ? "menu" : "action", name.toStdString()));
        if(_action.isCheckable())
        {
            write(std::format(" checkable checked={}", _action.isChecked()));
        }

        write(std::format(" enabled={}", _action.isEnabled()));
        check(element, false, _in_qt_dialog);
        write("\n");

        if(menu != nullptr)
        {
            actions(menu->actions(), _depth + 1, _in_qt_dialog);
        }
    }

    //------------------------------------------------------------------------------

    void widget(const QWidget& _widget, int _depth, bool _in_qt_dialog)
    {
        const bool is_label = qobject_cast<const QLabel*>(&_widget) != nullptr;
        const bool named    = has_name(_widget);
        int depth           = _depth;
        if(named || is_interactive(_widget) || is_label)
        {
            // The locator is costly: only the dump needs it.
            bool is_weak             = false;
            const bool is_scene      = _widget.inherits("QOpenGLWidget");
            const std::string target = named && m_with_text ? locator_of(_widget, is_weak) : std::string();
            const std::string indent(static_cast<std::size_t>(_depth) * 2, ' ');
            if(named)
            {
                const std::string kind = is_scene ? "scene" : _widget.metaObject()->className();
                write(std::format("{}{} \"{}\"{}", indent, kind, target, describe_widget(_widget)));
            }
            else
            {
                write(std::format("{}{}{}", indent, _widget.metaObject()->className(), describe_widget(_widget)));
            }

            if(named && is_scene)
            {
                write(std::format(" {}x{}", _widget.width(), _widget.height()));
            }

            if(is_weak)
            {
                write("   <- weak name: add a part if a test needs it");
            }

            // A label needs no name, but a name it has must be a sanctioned one.
            check(_widget, is_label, _in_qt_dialog, is_weak);
            write("\n");
            depth = _depth + 1;
        }

        if(const auto* const tool_bar = qobject_cast<const QToolBar*>(&_widget); tool_bar != nullptr)
        {
            actions(tool_bar->actions(), depth, _in_qt_dialog);
        }
        else if(const auto* const menu_bar = qobject_cast<const QMenuBar*>(&_widget); menu_bar != nullptr)
        {
            actions(menu_bar->actions(), depth, _in_qt_dialog);
        }

        // The inner widgets of a control are reached through it.
        if(!is_control(_widget))
        {
            children(_widget, depth, _in_qt_dialog);
        }
    }

    //------------------------------------------------------------------------------

    void children(const QWidget& _parent, int _depth, bool _in_qt_dialog)
    {
        for(const QObject* const child : _parent.children())
        {
            if(const auto* const widget = qobject_cast<const QWidget*>(child);
               widget != nullptr && !widget->isWindow() && is_visible(*widget))
            {
                this->widget(*widget, _depth, _in_qt_dialog);
            }
        }
    }

    //------------------------------------------------------------------------------

    static bool has_name(const QObject& _object)
    {
        return !_object.objectName().isEmpty() && !is_qt_name(_object.objectName());
    }

    //------------------------------------------------------------------------------

    /// The class of an object, its name and what a test reads of it.
    static std::string describe_object(const QObject& _object)
    {
        std::string description = _object.metaObject()->className();
        if(!_object.objectName().isEmpty())
        {
            description += std::format(" \"{}\"", _object.objectName().toStdString());
        }

        if(const auto* const widget = qobject_cast<const QWidget*>(&_object); widget != nullptr)
        {
            description += describe_widget(*widget);
        }
        else if(const auto* const action = qobject_cast<const QAction*>(&_object); action != nullptr)
        {
            description += std::format(" text=\"{}\"", action->text().toStdString());
        }

        return description;
    }

    //------------------------------------------------------------------------------

    /// Where an object is, outermost first: the names of its parents, or their class when they have none.
    static std::string ancestry(const QObject& _object)
    {
        std::vector<std::string> parents;
        for(const QObject* parent = _object.parent() ; parent != nullptr ; parent = parent->parent())
        {
            parents.push_back(
                has_name(*parent) ? parent->objectName().toStdString() : parent->metaObject()->className()
            );
        }

        std::string path;
        for(const std::string& parent : parents | std::views::reverse)
        {
            path += path.empty() ? parent : " > " + parent;
        }

        return path.empty() ? "no parent" : path;
    }

    //------------------------------------------------------------------------------

    /// The heading of a window: its kind, title, name and modality.
    static std::string window_line(const QWidget& _window)
    {
        std::string kind = "window";
        if(qobject_cast<const QMainWindow*>(&_window) != nullptr)
        {
            kind = "main window";
        }
        else if(qobject_cast<const QDialog*>(&_window) != nullptr)
        {
            kind = _window.isModal() ? "modal dialog" : "dialog";
        }
        else if(qobject_cast<const QMenu*>(&_window) != nullptr)
        {
            kind = "menu";
        }

        std::string line = std::format("{} \"{}\"", kind, _window.windowTitle().toStdString());
        line += _window.objectName().isEmpty() ? "" : std::format(" \"{}\"", _window.objectName().toStdString());
        if(_window.windowModality() != Qt::NonModal)
        {
            line += _window.windowModality() == Qt::ApplicationModal ? " application modal" : " window modal";
            if(const QWindow* const handle = _window.windowHandle();
               handle != nullptr && handle->modality() != _window.windowModality())
            {
                line += std::format(" (its window has modality {})", static_cast<int>(handle->modality()));
            }
        }

        return line + (&_window == QApplication::activeModalWidget() ? " (active modal)" : "");
    }

    bool m_with_text {false};
    std::string m_tree;
    std::vector<std::string> m_violations;
};

} // namespace

//------------------------------------------------------------------------------

/// Walks every visible window.
static tree_walk walk_windows(bool _with_text)
{
    tree_walk walk(_with_text);
    for(const QWidget* const window : QApplication::topLevelWidgets())
    {
        if(is_visible(*window))
        {
            walk.window(*window);
        }
    }

    return walk;
}

//------------------------------------------------------------------------------

std::string ui_tree()
{
    std::string tree = walk_windows(true).tree();
    if(const QWindow* const modal_window = QGuiApplication::modalWindow(); modal_window != nullptr)
    {
        tree += std::format("modal window \"{}\"\n", modal_window->title().toStdString());
    }

    // The modal widget Qt blocks input for, when it is not a window listed above.
    if(const QWidget* const modal = QApplication::activeModalWidget(); modal != nullptr && !is_visible(*modal))
    {
        tree += std::format(
            "active modal {} \"{}\" \"{}\", {}\n",
            modal->metaObject()->className(),
            modal->windowTitle().toStdString(),
            modal->objectName().toStdString(),
            modal->isVisible() ? "not exposed" : "hidden"
        );
    }

    return tree;
}

//------------------------------------------------------------------------------

std::vector<std::string> naming_violations()
{
    return walk_windows(false).violations();
}

//------------------------------------------------------------------------------

/// The number of single-character edits between two strings.
static std::size_t edit_distance(std::string_view _first, std::string_view _second)
{
    std::vector<std::size_t> distances(_second.size() + 1);
    std::iota(distances.begin(), distances.end(), std::size_t {0});
    for(const char first : _first)
    {
        std::size_t diagonal = distances.front();
        ++distances.front();
        for(std::size_t index = 0 ; index < _second.size() ; ++index)
        {
            const std::size_t above = distances[index + 1];
            distances[index + 1] = std::min(
                {above + 1, distances[index] + 1, diagonal + (first != _second[index] ? 1U : 0U)
                });
            diagonal = above;
        }
    }

    return distances.back();
}

//------------------------------------------------------------------------------

std::vector<std::string> closest_visible_names(std::string_view _requested, std::size_t _count)
{
    std::set<std::string> names;
    const auto add = [&names](const QString& _name)
                     {
                         if(!_name.isEmpty() && !is_qt_name(_name))
                         {
                             names.insert(_name.toStdString());
                         }
                     };

    for(const QWidget* const widget : QApplication::allWidgets())
    {
        if(is_visible(*widget))
        {
            add(widget->objectName());
            for(const QAction* const action : widget->actions())
            {
                add(action->objectName());
            }
        }
    }

    // Far names say nothing: half of the requested name may differ at most.
    std::multimap<std::size_t, std::string> ranked;
    for(const std::string& name : names)
    {
        // Its own name helps nobody: it is there, as something the operation does not apply to.
        if(const std::size_t distance = edit_distance(_requested, name);
           distance > 0 && distance <= (_requested.size() + 1) / 2)
        {
            ranked.emplace(distance, name);
        }
    }

    std::vector<std::string> closest;
    for(const auto& [distance, name] : ranked)
    {
        if(closest.size() == _count)
        {
            break;
        }

        closest.push_back(name);
    }

    return closest;
}

} // namespace sight::ui::test::detail
