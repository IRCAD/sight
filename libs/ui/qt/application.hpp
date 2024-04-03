/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/base.hpp>

#include <ui/__/application.hpp>

namespace sight::ui::qt
{

/**
 * @brief   Gives access to the qml application part.
 */
class SIGHT_UI_QT_CLASS_API application : public ui::application
{
public:

    ~application() override
    = default;

    /// Tells the application to exit with a returncode
    SIGHT_UI_QT_API void exit(int _return_code = 0, bool _async = true) override;
};

} // namespace sight::ui::qt
