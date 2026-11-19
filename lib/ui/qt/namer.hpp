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

#include <QWidget>

#include <string_view>

namespace sight::ui::qt
{

/**
 * @brief Names the unnamed widgets below this one, a service or a sub-view, after it when they are polished or shown.
 *
 * The name is `<its name>/<role>`, the role coming from the type (`button`, `slider`, `line_edit`...). The nearest such
 * widget wins, a widget that has a name keeps it, and the inner widgets of a control (the line edit of a spin box, the
 * scroll bars of a list) stay unnamed. A dialog opened during the start, update or stop of a service is `<uid>/dialog`
 * and contains its own widgets; Qt's file, color and font dialogs keep Qt's names.
 *
 * @param _container the widget of the service or sub-view.
 */
SIGHT_UI_QT_API void name_widgets_below(QWidget& _container);

/**
 * @brief Gives a widget the stable name `<service>/<part>` instead of its role, only where a test needs one.
 *
 * For a role that repeats, or a dialog among several of its service. Throws std::invalid_argument unless the part is
 * snake_case.
 *
 * @code{.cpp}
 * sight::ui::qt::name_part(*ok_button, "ok");
 * @endcode
 *
 * @param _widget the widget to name.
 * @param _part   the snake_case part.
 */
SIGHT_UI_QT_API void name_part(QWidget& _widget, std::string_view _part);

} // namespace sight::ui::qt
