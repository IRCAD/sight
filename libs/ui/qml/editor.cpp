/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "editor.hpp"

namespace sight::ui::qml
{

//-----------------------------------------------------------------------------

editor::editor() noexcept =
    default;

//-----------------------------------------------------------------------------

editor::~editor() noexcept =
    default;

//-----------------------------------------------------------------------------

void editor::starting()
{
    Q_EMIT started();
    this->updating();
}

//-----------------------------------------------------------------------------

void editor::stopping()
{
    Q_EMIT stopped();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qml
