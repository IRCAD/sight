/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/__/application.hpp"

namespace sight::ui
{

const application::factory_registry_key_t application::REGISTRY_KEY = "sight::ui::application";

//-----------------------------------------------------------------------------

application::sptr application::get()
{
    ui::object::sptr gui_obj     = ui::factory::make(application::REGISTRY_KEY);
    static application::sptr app = std::dynamic_pointer_cast<ui::application>(gui_obj);
    SIGHT_ASSERT("no implementation of " << ui::application::REGISTRY_KEY, app);
    return app;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
