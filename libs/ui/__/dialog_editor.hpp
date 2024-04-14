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

#include <sight/ui/__/config.hpp>

#include "ui/__/service.hpp"

#include <core/tools/failed.hpp>

#include <service/base.hpp>

namespace sight::ui
{

/**
 * @brief Defines the service interface managing the editor service which create their own container.
 */
class SIGHT_UI_CLASS_API dialog_editor : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(dialog_editor, service::base);

protected:

    dialog_editor() noexcept =
        default;

    ~dialog_editor() noexcept override =
        default;
};

} // namespace sight::ui
