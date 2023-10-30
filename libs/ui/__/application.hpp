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

#pragma once

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

namespace sight::ui
{

/**
 * @brief   Defines the application manipulation API.
 */
class UI_CLASS_API application : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(application, ui::object);

    /// Destructor. Do nothing.
    UI_API ~application() override = default;

    /// Tells the application to exit with a returncode
    UI_API virtual void exit(int _return_code = 0, bool _async = true) = 0;

    /// Sets if confirmation is required when exiting application
    void set_confirm(bool _confirm)
    {
        m_confirm = _confirm;
    }

    /// application factory, returning the registered instance of application. Manage a Singleton.
    UI_API static application::sptr get();

    using factory_registry_key_t = std::string;
    UI_API static const factory_registry_key_t REGISTRY_KEY;

protected:

    /// Asks confirmation before exiting application.
    bool m_confirm {false};
};

} // namespace sight::ui
