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

#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QString>
#include <QTextEdit>
#include <QWidget>

#include <optional>

namespace sight::ui::test::detail
{

/// What a user reads on a widget, for the kinds of widget that show a text.
[[nodiscard]] inline std::optional<QString> displayed_text(const QWidget& _widget)
{
    if(const auto* const label = qobject_cast<const QLabel*>(&_widget); label != nullptr)
    {
        return label->text();
    }

    if(const auto* const button = qobject_cast<const QAbstractButton*>(&_widget); button != nullptr)
    {
        return button->text();
    }

    if(const auto* const line_edit = qobject_cast<const QLineEdit*>(&_widget); line_edit != nullptr)
    {
        return line_edit->text();
    }

    if(const auto* const combo = qobject_cast<const QComboBox*>(&_widget); combo != nullptr)
    {
        return combo->currentText();
    }

    if(const auto* const spin_box = qobject_cast<const QAbstractSpinBox*>(&_widget); spin_box != nullptr)
    {
        return spin_box->text();
    }

    if(const auto* const text_edit = qobject_cast<const QTextEdit*>(&_widget); text_edit != nullptr)
    {
        return text_edit->toPlainText();
    }

    if(const auto* const text_edit = qobject_cast<const QPlainTextEdit*>(&_widget); text_edit != nullptr)
    {
        return text_edit->toPlainText();
    }

    return std::nullopt;
}

} // namespace sight::ui::test::detail
