/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/IApplication.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{

/**
 * @brief   Gives access to the underlayed application part
 */
class UI_BASE_CLASS_API Application : public ui::base::IApplication
{

public:

    fwCoreClassMacro(Application, ui::base::IApplication, Application::factory)

    UI_BASE_API static Application::sptr getDefault();

protected:

    /// Application factory, returning the registered instance of Application. Manage a Singleton.
    UI_BASE_API static Application::sptr factory();

    Application()
    {
    }
    virtual ~Application()
    {
    }

};

} // namespace sight::ui::base
