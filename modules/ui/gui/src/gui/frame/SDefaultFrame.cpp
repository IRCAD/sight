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

#include "gui/frame/SDefaultFrame.hpp"

#include <core/tools/fwID.hpp>

#include <services/macros.hpp>

namespace gui
{
namespace frame
{

fwServicesRegisterMacro( ::fwGui::IFrameSrv, ::gui::frame::SDefaultFrame )

SDefaultFrame::SDefaultFrame() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultFrame::~SDefaultFrame() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultFrame::configuring()
{
    SLM_ASSERT( "<service> tag is required.", m_configuration->getName() == "service" );
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::starting()
{
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::info(std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SDefaultFrame::updating()
{
}

//-----------------------------------------------------------------------------

}
}
