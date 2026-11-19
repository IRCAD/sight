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

#include "detail/adaptors.hpp"

#include "detail/widget_visibility.hpp"
#include "detail/window_input.hpp"
#include "locator.hpp"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAction>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLineEdit>
#include <QList>
#include <QLocale>
#include <QMenu>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPoint>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QRect>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionSlider>
#include <QToolButton>
#include <QTreeView>
#include <QVariant>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace sight::ui::test::detail::adaptors
{

//------------------------------------------------------------------------------

static QPoint slider_handle_position(const QSlider& _slider, int _value)
{
    QStyleOptionSlider option;
    option.initFrom(&_slider);
    option.orientation    = _slider.orientation();
    option.minimum        = _slider.minimum();
    option.maximum        = _slider.maximum();
    option.sliderPosition = _value;
    option.sliderValue    = _value;
    option.singleStep     = _slider.singleStep();
    option.pageStep       = _slider.pageStep();
    option.upsideDown     = _slider.orientation() == Qt::Horizontal
                            ? _slider.invertedAppearance() != (_slider.layoutDirection() == Qt::RightToLeft)
                            : !_slider.invertedAppearance();

    return _slider.style()->subControlRect(
        QStyle::CC_Slider,
        &option,
        QStyle::SC_SliderHandle,
        &_slider
    ).center();
}

//------------------------------------------------------------------------------

/// One key press and release handed to _widget itself: Qt's own handler runs as for a user's key, without depending on
/// the window system, the active window or the focus.
static void send_key(
    QWidget& _widget,
    Qt::Key _key,
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier,
    const QString& _text             = QString()
)
{
    QPointer<QWidget> widget(&_widget);
    QKeyEvent press(QEvent::KeyPress, _key, _modifiers, _text);
    QCoreApplication::sendEvent(widget, &press);
    if(widget.isNull())
    {
        throw input_failure("'" + describe_object(&_widget) + "' was removed during its input", false);
    }

    QKeyEvent release(QEvent::KeyRelease, _key, _modifiers, _text);
    QCoreApplication::sendEvent(widget, &release);
}

//------------------------------------------------------------------------------

/// A left click handed to _widget itself, once a user's click at _position would land: the widget's handler computes
/// what a user's click emits (an item view's selection, clicked, itemClicked), without the window system's timing.
static void send_click(QWidget& _widget, QPoint _position)
{
    static_cast<void>(check_mouse_target(_widget, _position));
    QPointer<QWidget> widget(&_widget);
    const QPointF local(_position);
    const QPointF global(_widget.mapToGlobal(_position));
    QMouseEvent press(QEvent::MouseButtonPress, local, global, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(widget, &press);
    if(widget.isNull())
    {
        throw input_failure("'" + describe_object(&_widget) + "' was removed during its click", false);
    }

    QMouseEvent release(QEvent::MouseButtonRelease, local, global, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(widget, &release);
}

//------------------------------------------------------------------------------

void type_text(QWidget& _widget, const QString& _text)
{
    static_cast<void>(check_mouse_target(_widget, _widget.rect().center()));
    QPointer<QWidget> widget(&_widget);
    for(qsizetype index = 0 ; index < _text.size() ; )
    {
        if(widget.isNull())
        {
            throw input_failure("'" + describe_object(&_widget) + "' was removed during its input", false);
        }

        const qsizetype length = _text.at(index).isHighSurrogate() && index + 1 < _text.size() ? 2 : 1;
        send_key(*widget, Qt::Key_unknown, Qt::NoModifier, _text.mid(index, length));
        index += length;
    }
}

//------------------------------------------------------------------------------

/// Selects the whole text, types the new one over it or erases it, and commits it with Return, as a user does. The keys
/// go to the field itself, once a user's click there would land: the field's handler computes what a user's edit emits
/// (textEdited, returnPressed, editingFinished, valueChanged) and applies its validator.
static void replace_and_commit(QWidget& _widget, const QString& _text)
{
    static_cast<void>(check_mouse_target(_widget, _widget.rect().center()));
    QPointer<QWidget> widget(&_widget);
    send_key(*widget, Qt::Key_A, Qt::ControlModifier);
    if(_text.isEmpty())
    {
        send_key(*widget, Qt::Key_Backspace);
    }
    else if(!widget.isNull())
    {
        type_text(*widget, _text);
    }

    if(widget.isNull())
    {
        throw input_failure("'" + describe_object(&_widget) + "' was removed during its input", false);
    }

    send_key(*widget, Qt::Key_Return);
}

//------------------------------------------------------------------------------

static bool same_double_value(double _first, double _second)
{
    const double tolerance = std::numeric_limits<double>::epsilon()
                             * std::max({1., std::abs(_first), std::abs(_second)});
    return std::abs(_first - _second) <= tolerance;
}

//------------------------------------------------------------------------------

QPoint slider_point(const QWidget& _widget, bool _by_drag)
{
    const auto* const slider = qobject_cast<const QSlider*>(&_widget);
    if(slider != nullptr && _by_drag)
    {
        return slider_handle_position(*slider, slider->sliderPosition());
    }

    return _widget.rect().center();
}

//------------------------------------------------------------------------------

QPoint checkable_point(const QWidget& _widget)
{
    const auto* const check_box = qobject_cast<const QCheckBox*>(&_widget);
    const auto* const radio     = qobject_cast<const QRadioButton*>(&_widget);
    if(check_box == nullptr && radio == nullptr)
    {
        return _widget.rect().center();
    }

    const auto& button = static_cast<const QAbstractButton&>(_widget);
    QStyleOptionButton option;
    option.initFrom(&button);
    option.text     = button.text();
    option.icon     = button.icon();
    option.iconSize = button.iconSize();
    const QStyle::SubElement element = check_box != nullptr ? QStyle::SE_CheckBoxClickRect
                                                            : QStyle::SE_RadioButtonClickRect;
    return button.style()->subElementRect(element, &option, &button).center();
}

//------------------------------------------------------------------------------

/// The checked state of a checkable button or action, none for anything else.
static std::optional<bool> checked_state(const QObject* _target)
{
    if(const auto* const button = qobject_cast<const QAbstractButton*>(_target);
       button != nullptr && button->isCheckable())
    {
        return button->isChecked();
    }

    if(const auto* const action = qobject_cast<const QAction*>(_target); action != nullptr && action->isCheckable())
    {
        return action->isChecked();
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

bool click_through_qt(QObject& _target, QWidget& _widget, QPoint _position, bool _reaches_target)
{
    auto* const action = qobject_cast<QAction*>(&_target);
    auto* const button = qobject_cast<QAbstractButton*>(&_widget);
    if(action == nullptr && (button == nullptr || &_widget != &_target))
    {
        return false;
    }

    // Opening a menu, or a submenu of a popup already open: the popup is opened as a user does.
    if(!_reaches_target || (action != nullptr && action->menu() != nullptr))
    {
        return false;
    }

    // Only where a user's click would land: shown, enabled, not covered and not blocked by a modal dialog.
    static_cast<void>(check_mouse_target(_widget, _position));
    if(action == nullptr)
    {
        auto* const tool_button = qobject_cast<QToolButton*>(button);
        if(tool_button != nullptr && tool_button->menu() != nullptr
           && tool_button->popupMode() == QToolButton::InstantPopup)
        {
            tool_button->showMenu();
        }
        else
        {
            button->click();
        }

        return true;
    }

    if(!action->isEnabled())
    {
        throw input_failure("action '" + describe_object(action) + "' is disabled");
    }

    // In an open menu, Qt's own keyboard activation: the menu closes and its exec() returns the action.
    if(auto* const menu = qobject_cast<QMenu*>(&_widget); menu != nullptr && menu->actions().contains(action))
    {
        menu->setActiveAction(action);
        QKeyEvent press(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QCoreApplication::sendEvent(menu, &press);
        return true;
    }

    // The button showing it emits what a click does, and triggers it.
    if(button != nullptr && button->actions().contains(action))
    {
        button->click();
        return true;
    }

    // In a closed menu or a folded toolbar, reached from what opens them.
    action->trigger();
    return true;
}

//------------------------------------------------------------------------------

void set_checked(QObject& _target, QWidget& _widget, QPoint _position, bool _checked, std::string_view _expression)
{
    const QPointer<QObject> target(&_target);
    const std::optional<bool> before = checked_state(target);
    if(!before.has_value())
    {
        throw std::invalid_argument("set(bool) requires a checkable button or action target");
    }

    if(*before != _checked && !click_through_qt(_target, _widget, _position, true))
    {
        inject_mouse_click(_widget, _position);
    }

    if(checked_state(target) != _checked)
    {
        throw input_failure(
                  "set on '" + std::string(_expression) + "' did not reach the requested checked state",
                  false
        );
    }
}

//------------------------------------------------------------------------------

void set_int(
    QObject& _target,
    QWidget& _widget,
    QPoint _position,
    int _value,
    bool _by_drag,
    std::string_view _expression
)
{
    const std::string expression(_expression);
    if(QPointer<QSpinBox> spin_box = qobject_cast<QSpinBox*>(&_target); !spin_box.isNull())
    {
        if(_by_drag)
        {
            throw std::invalid_argument("set(int, by_drag) only applies to sliders");
        }

        if(_value < spin_box->minimum() || _value > spin_box->maximum())
        {
            throw std::invalid_argument("spin box value is outside its range");
        }

        if(spin_box->value() != _value)
        {
            replace_and_commit(*spin_box, QString::number(_value));
        }

        if(spin_box.isNull() || spin_box->value() != _value)
        {
            throw input_failure(
                      "set on '" + expression + "' did not reach spin box value " + std::to_string(_value),
                      false
            );
        }

        return;
    }

    if(qobject_cast<QDoubleSpinBox*>(&_target) != nullptr && !_by_drag)
    {
        set_double(_target, _value, _expression);
        return;
    }

    QPointer<QSlider> slider = qobject_cast<QSlider*>(&_target);

    // A slider over a list of values (ui::qt::widget::non_linear_slider) wraps a QSlider over their indices, which
    // the user moves to the index of the value.
    if(const QVariant values = _target.property("values"); slider.isNull() && values.isValid())
    {
        auto* const indices = _target.findChild<QSlider*>(QString(), Qt::FindDirectChildrenOnly);
        const auto index    = values.value<QList<int> >().indexOf(_value);
        if(indices == nullptr || index < 0)
        {
            throw std::invalid_argument("value is not one of the slider's values");
        }

        set_int(*indices, *indices, slider_point(*indices, _by_drag), static_cast<int>(index), _by_drag, _expression);
        return;
    }

    if(slider.isNull())
    {
        throw std::invalid_argument("set(int) requires a slider, a QSpinBox or a QDoubleSpinBox target");
    }

    if(_value < slider->minimum() || _value > slider->maximum())
    {
        throw std::invalid_argument("slider value is outside its range");
    }

    if(slider->value() != _value)
    {
        if(_by_drag)
        {
            inject_mouse_drag(_widget, _position, slider_handle_position(*slider, _value));
        }
        else
        {
            // Through the Qt API, once a user's click there would land: valueChanged, as for a user's keys, without
            // depending on the focus. A service waiting for sliderReleased needs by_drag.
            static_cast<void>(check_mouse_target(_widget, _position));
            slider->setValue(_value);
        }
    }

    if(slider.isNull() || slider->value() != _value)
    {
        throw input_failure(
                  "set on '" + expression + "' did not reach slider value " + std::to_string(_value),
                  false
        );
    }
}

//------------------------------------------------------------------------------

void set_double(QObject& _target, double _value, std::string_view _expression)
{
    QPointer<QDoubleSpinBox> spin_box = qobject_cast<QDoubleSpinBox*>(&_target);
    if(spin_box.isNull())
    {
        throw std::invalid_argument("set(double) requires a QDoubleSpinBox target");
    }

    if(_value < spin_box->minimum() || _value > spin_box->maximum())
    {
        throw std::invalid_argument("double spin box value is outside its range");
    }

    const QString text       = spin_box->locale().toString(_value, 'f', spin_box->decimals());
    bool parsed              = false;
    const double typed_value = spin_box->locale().toDouble(text, &parsed);
    if(!parsed || !same_double_value(typed_value, _value))
    {
        throw std::invalid_argument("double spin box precision cannot represent the requested value");
    }

    if(!same_double_value(spin_box->value(), _value))
    {
        replace_and_commit(*spin_box, text);
    }

    if(spin_box.isNull() || !same_double_value(spin_box->value(), _value))
    {
        throw input_failure(
                  "set on '" + std::string(_expression) + "' did not reach double spin box value "
                  + text.toStdString(),
                  false
        );
    }
}

//------------------------------------------------------------------------------

/// A combo item is chosen through the Qt API, as Qt itself does once a user picked an item in the popup
/// (QComboBoxPrivate::itemSelected): a scenario tests the application, not the timing of Qt's popup.
static void select_combo_item(QComboBox& _combo, const QString& _text, std::string_view _expression)
{
    int row = -1;
    for(int index = 0 ; index < _combo.count() ; ++index)
    {
        if(_combo.itemText(index) == _text)
        {
            if(row >= 0)
            {
                throw std::invalid_argument("combo item text is ambiguous");
            }

            row = index;
        }
    }

    if(row < 0)
    {
        throw std::invalid_argument("combo has no item with the requested text");
    }

    if(_combo.currentIndex() == row)
    {
        return;
    }

    const QModelIndex index = _combo.model()->index(row, _combo.modelColumn(), _combo.rootModelIndex());
    if(!_combo.model()->flags(index).testFlag(Qt::ItemIsEnabled))
    {
        throw input_failure("combo item '" + _text.toStdString() + "' is disabled");
    }

    QPointer<QComboBox> combo(&_combo);
    combo->setCurrentIndex(row);
    if(combo.isNull() || combo->currentIndex() != row)
    {
        throw input_failure(
                  "'" + std::string(_expression) + "' did not select combo item '" + _text.toStdString() + "'",
                  false
        );
    }

    Q_EMIT combo->activated(row);
    if(!combo.isNull())
    {
        Q_EMIT combo->textActivated(combo->itemText(row));
    }
}

//------------------------------------------------------------------------------

void set_text(QObject& _target, const QString& _text, std::string_view _expression)
{
    if(auto* const combo = qobject_cast<QComboBox*>(&_target); combo != nullptr)
    {
        select_combo_item(*combo, _text, _expression);
        return;
    }

    QPointer<QLineEdit> line_edit = qobject_cast<QLineEdit*>(&_target);
    if(line_edit.isNull())
    {
        throw std::invalid_argument("set(text) requires a QLineEdit or QComboBox target");
    }

    if(line_edit->text() != _text)
    {
        replace_and_commit(*line_edit, _text);
    }

    if(line_edit.isNull() || line_edit->text() != _text)
    {
        throw input_failure("set on '" + std::string(_expression) + "' did not reach the requested text", false);
    }

    // Typed but refused by the validator at Return: the application never received it.
    if(!line_edit->hasAcceptableInput())
    {
        throw input_failure(
                  "set on '" + std::string(_expression) + "': the line edit does not accept '" + _text.toStdString()
                  + "'",
                  false
        );
    }
}

//------------------------------------------------------------------------------

void select_text(QObject& _target, const QString& _text, std::string_view _expression)
{
    auto* const combo = qobject_cast<QComboBox*>(&_target);
    if(combo == nullptr)
    {
        throw std::invalid_argument("select(text) requires a QComboBox target");
    }

    select_combo_item(*combo, _text, _expression);
}

//------------------------------------------------------------------------------

void click_item_widget(QObject& _target, const item_locator& _item)
{
    auto* const view = qobject_cast<QAbstractItemView*>(&_target);
    if(view == nullptr)
    {
        throw std::invalid_argument("an item locator requires an item view");
    }

    const std::vector<QModelIndex> matches = _item.matching_indexes(*view);
    if(matches.empty())
    {
        throw input_failure("the item is not in the view");
    }

    if(matches.size() != 1)
    {
        throw std::invalid_argument("the item text path is ambiguous");
    }

    // scrollTo() only moves the viewport, as a user's scrolling would.
    view->scrollTo(matches.front());
    QWidget* const widget = view->indexWidget(matches.front());
    if(widget == nullptr)
    {
        throw std::invalid_argument("the item has no embedded widget");
    }

    if(!is_visible(*widget))
    {
        throw input_failure("the item's embedded widget is not visible");
    }

    if(!click_through_qt(*widget, *widget, widget->rect().center(), true))
    {
        inject_mouse_click(*widget, widget->rect().center());
    }
}

//------------------------------------------------------------------------------

/// Where a user clicks to expand an item: its branch indicator, located as QTreeView hit-tests it.
static QPoint branch_indicator_point(const QTreeView& _tree, const QModelIndex& _index)
{
    if(!_tree.itemsExpandable())
    {
        throw std::invalid_argument("the tree does not let a user expand its items");
    }

    if(!_tree.rootIsDecorated() && _index.parent() == _tree.rootIndex())
    {
        throw std::invalid_argument("the tree shows no branch indicator for its top-level items");
    }

    const int tree_column = _tree.treePosition() >= 0 ? _tree.treePosition() : _tree.header()->logicalIndex(0);
    const QRect item      = _tree.visualRect(_index.siblingAtColumn(tree_column));
    const int indentation = _tree.indentation();

    // visualRect() leaves out the indentation, whose last step holds the indicator.
    QStyleOption option;
    option.initFrom(&_tree);
    option.rect = _tree.isRightToLeft()
                  ? QRect(item.right() + 1, item.top(), indentation, item.height())
                  : QRect(item.left() - indentation, item.top(), indentation, item.height());
    return _tree.style()->subElementRect(QStyle::SE_TreeViewDisclosureItem, &option, &_tree).center();
}

//------------------------------------------------------------------------------

/// Expands a tree item as a click on its branch indicator does, which neither selects it nor needs the focus.
static void expand_tree_item(QTreeView& _tree, const QModelIndex& _index)
{
    if(_tree.isExpanded(_index))
    {
        return;
    }

    _tree.scrollTo(_index);
    const QPoint indicator = branch_indicator_point(_tree, _index);
    if(!_tree.viewport()->rect().contains(indicator))
    {
        throw input_failure("tree parent is not visible");
    }

    // Through the Qt API, once a user's click on the indicator would land: expanded, and lazy children fetched.
    static_cast<void>(check_mouse_target(*_tree.viewport(), indicator));
    _tree.expand(_index);
    if(!_tree.isExpanded(_index))
    {
        throw input_failure("tree parent did not expand after a click on its branch indicator", false);
    }
}

//------------------------------------------------------------------------------

void select_item(QObject& _target, const item_locator& _item)
{
    auto* const view = qobject_cast<QAbstractItemView*>(&_target);
    if(view == nullptr)
    {
        throw std::invalid_argument("an item locator requires an item view");
    }

    auto* const tree = qobject_cast<QTreeView*>(view);
    QModelIndex selected;
    for(std::size_t depth = 1 ; depth <= _item.depth() ; ++depth)
    {
        // Resolve prefixes separately so expanding a branch can populate lazy children.
        const std::vector<QModelIndex> matches = _item.matching_prefix(*view, depth);
        if(matches.empty())
        {
            throw input_failure("the item path is not in the view");
        }

        if(depth == _item.depth())
        {
            // Repeated parent labels may lead to different children; only the full path must be unique.
            if(matches.size() != 1)
            {
                throw std::invalid_argument("the item text path is ambiguous");
            }

            selected = matches.front();
            break;
        }

        if(tree == nullptr)
        {
            throw std::invalid_argument("a nested item path requires a tree view");
        }

        // Repeated parent labels are all expanded, leaves among them skipped.
        bool expandable = false;
        for(const QModelIndex& parent : matches)
        {
            if(parent.model()->hasChildren(parent))
            {
                expandable = true;
                expand_tree_item(*tree, parent);
            }
        }

        if(!expandable)
        {
            throw std::invalid_argument("item path continues below a leaf");
        }
    }

    // Clicking the selected item again would make a double click of two consecutive selections.
    const auto is_selected = [view, &selected]
                             {
                                 return view->currentIndex() == selected
                                        && view->selectionModel()->isSelected(selected);
                             };
    if(is_selected())
    {
        return;
    }

    // scrollTo() only moves the viewport, as a user's scrolling would.
    view->scrollTo(selected);
    const QRect rect = view->visualRect(selected).intersected(view->viewport()->rect());
    if(rect.isEmpty())
    {
        throw input_failure("the item is not visible in its view");
    }

    send_click(*view->viewport(), rect.center());
    // A delivered click that did not select the row cannot safely be retried.
    if(!is_selected())
    {
        throw input_failure("the item was clicked but not selected", false);
    }
}

//------------------------------------------------------------------------------

void choose_color(QWidget& _modal, const QColor& _color)
{
    QPointer<QColorDialog> dialog = qobject_cast<QColorDialog*>(&_modal);
    if(dialog.isNull())
    {
        throw input_failure("the active modal dialog is not a color dialog");
    }

    // The dialog's own API: it emits currentColorChanged as a user's pick does.
    dialog->setCurrentColor(_color);
    if(dialog->currentColor() != _color)
    {
        throw std::invalid_argument("the color dialog cannot represent the requested color");
    }

    auto* const buttons   = dialog->findChild<QDialogButtonBox*>();
    QPushButton* const ok = buttons == nullptr ? nullptr : buttons->button(QDialogButtonBox::Ok);
    if(ok == nullptr)
    {
        throw std::invalid_argument("the color dialog has no OK button");
    }

    // Qt's dialog, not the application: its button is clicked through Qt, never missed on Qt's timing.
    ok->click();
    if(!dialog.isNull() && dialog->isVisible())
    {
        throw input_failure("the color dialog did not close after its OK button was clicked", false);
    }
}

} // namespace sight::ui::test::detail::adaptors
