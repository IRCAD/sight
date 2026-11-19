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

#include <ui/test/locator.hpp>

#include <QAbstractItemView>
#include <QColor>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QWidget>

#include <string_view>

/// How a user sets each kind of widget: input only, on the GUI thread, inside one dispatched call. Each one checks the
/// value it reached and throws a non-retryable input_failure otherwise, since its input already reached the widget.
namespace sight::ui::test::detail::adaptors
{

/// Where a set(int) presses: the slider handle when it is dragged, else the widget's centre.
[[nodiscard]] QPoint slider_point(const QWidget& _widget, bool _by_drag);

/// Where a set(bool) clicks: the indicator and label of a check box or a radio button, which alone take a click
/// (QCheckBox::hitButton()), else the widget's centre.
[[nodiscard]] QPoint checkable_point(const QWidget& _widget);

/// Clicks a checkable button only when its state differs.
/// Types _text into _widget itself, without committing it, once a user's click there would land.
void type_text(QWidget& _widget, const QString& _text);

/// Clicks a button or activates an action through the Qt API, once a user's click at _position on _widget would land.
/// Returns false for a target the Qt API does not cover, which the caller clicks with input.
[[nodiscard]] bool click_through_qt(QObject& _target, QWidget& _widget, QPoint _position, bool _reaches_target);

void set_checked(QObject& _target, QWidget& _widget, QPoint _position, bool _checked, std::string_view _expression);

/// Moves a slider with the keyboard (or a drag), or types into a spin box.
void set_int(
    QObject& _target,
    QWidget& _widget,
    QPoint _position,
    int _value,
    bool _by_drag,
    std::string_view _expression
);

/// Types into a double spin box.
void set_double(QObject& _target, double _value, std::string_view _expression);

/// Picks a combo box item through its Qt API, or replaces a line edit's text.
void set_text(QObject& _target, const QString& _text, std::string_view _expression);

/// Selects a combo box item through its Qt API when it differs from the current item.
void select_text(QObject& _target, const QString& _text, std::string_view _expression);

/// Clicks the widget embedded at an item of the view.
void click_item_widget(QObject& _target, const item_locator& _item);

/// Expands tree ancestors through their branch indicators and selects the item with a click, unless it is already
/// the selected current item.
void select_item(QObject& _target, const item_locator& _item);

/// Sets the color of the active color dialog and accepts it with its OK button.
void choose_color(QWidget& _modal, const QColor& _color);

} // namespace sight::ui::test::detail::adaptors
