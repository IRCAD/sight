/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base
{

/**
 * @brief   Defines the application manipulation API.
 */
class UI_BASE_CLASS_API IApplication : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IApplication, ui::base::GuiBaseObject);

    /// Constructor. Do nothing.
    UI_BASE_API IApplication();
    /// Destructor. Do nothing.
    UI_BASE_API ~IApplication() override;

    /// Tells the application to exit with a returncode
    UI_BASE_API virtual void exit(int returncode) = 0;

    /// Sets if confirmation is required when exiting application
    void setConfirm(bool confirm)
    {
        m_confirm = confirm;
    }

    typedef std::string FactoryRegistryKeyType;
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

protected:

    /// Asks confirmation before exiting application.
    bool m_confirm {false};
};

} // namespace sight::ui::base
