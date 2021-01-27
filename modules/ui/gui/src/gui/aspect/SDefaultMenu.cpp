/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "gui/aspect/SDefaultMenu.hpp"

#include <services/macros.hpp>

fwServicesRegisterMacro( ::sight::gui::IMenuSrv, ::sight::modules::gui::aspect::SDefaultMenu )

namespace sight::modules::gui
{

namespace aspect
{

//-----------------------------------------------------------------------------

SDefaultMenu::SDefaultMenu() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultMenu::~SDefaultMenu() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultMenu::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::starting()
{
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::updating()
{
}

//-----------------------------------------------------------------------------

}

}
