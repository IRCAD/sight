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

#include "gui/action/SQuit.hpp"

#include <fwCore/base.hpp>

#include <fwGui/Application.hpp>
#include <fwGui/Cursor.hpp>

#include <fwServices/macros.hpp>

namespace gui
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SQuit );

//-----------------------------------------------------------------------------

SQuit::SQuit() noexcept
{
}

//-----------------------------------------------------------------------------

SQuit::~SQuit() noexcept
{
}

//-----------------------------------------------------------------------------

void SQuit::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void SQuit::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SQuit::stopping()
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SQuit::info(std::ostream& _sstream )
{
    _sstream << "Quit Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SQuit::updating()
{
    ::fwGui::Cursor cursor;
    cursor.setCursor(::fwGui::ICursor::BUSY);
    ::fwGui::Application::New()->exit(0);
    cursor.setDefaultCursor();
}

//-----------------------------------------------------------------------------

}
}
