/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "gui/aspect/SDefaultToolBar.hpp"

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwGui::IToolBarSrv, ::gui::aspect::SDefaultToolBar );

namespace gui
{

namespace aspect
{

SDefaultToolBar::SDefaultToolBar() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultToolBar::~SDefaultToolBar() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::starting()
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::stopping()
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::updating()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::info( std::ostream& _sstream )
{
    SLM_TRACE_FUNC();
}

}

}
