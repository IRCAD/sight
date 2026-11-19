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

#include <sight/ui/qt/config.hpp>

#include <QObject>
#include <QString>
#include <QStringView>

// The sanctioned naming paths of the naming contract. Each records the name it gives: a GUI test then tells such a
// name from one set by hand, and a later setObjectName() voids the record. Only the paths of the contract call them.
namespace sight::ui::qt::detail
{

/// Names an object after its XML configuration (a uid, a sid or wid, a menu item or a settings key), or after the
/// role the framework gives it.
SIGHT_UI_QT_API void set_name(QObject& _object, const QString& _name);

/// Names an object `<owner>/<part>`. Throws std::invalid_argument unless the part is snake_case.
SIGHT_UI_QT_API void set_part_name(QObject& _object, const QString& _owner, QStringView _part);

/**
 * @brief Binds the name of an object to its owner's, `<owner>/<part>`, none while the owner has none.
 *
 * Throws std::invalid_argument unless the part is snake_case.
 *
 * @param _object the object to name.
 * @param _owner  the object whose name it follows.
 * @param _part   the snake_case part.
 */
SIGHT_UI_QT_API void bind_part_name(QObject& _object, const QObject& _owner, QStringView _part);

/// Whether a part is a stable snake_case name: `[a-z][a-z0-9]*(_[a-z0-9]+)*`.
[[nodiscard]] SIGHT_UI_QT_API bool is_part(QStringView _part) noexcept;

/// Whether the current name of an object is the one a sanctioned path gave it.
[[nodiscard]] SIGHT_UI_QT_API bool has_sanctioned_name(const QObject& _object);

} // namespace sight::ui::qt::detail
