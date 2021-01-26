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

#include "gui/view/SDefaultView.hpp"

#include <core/tools/fwID.hpp>

#include <services/macros.hpp>

namespace gui
{
namespace view
{

fwServicesRegisterMacro( ::fwGui::view::IView, ::gui::view::SDefaultView)

SDefaultView::SDefaultView() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultView::~SDefaultView() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultView::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultView::starting()
{
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultView::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultView::info(std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SDefaultView::updating()
{
}

//-----------------------------------------------------------------------------

}
}
