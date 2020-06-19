/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "ExTimeLine/SMain.hpp"

#include "fwData/Composite.hpp"

#include <fwServices/macros.hpp>

#include <fwThread/Timer.hpp>

#include <fwTools/Object.hpp>

fwServicesRegisterMacro( ::fwServices::IService, ::ExTimeLine::SMain, ::fwData::Composite );

namespace ExTimeLine
{

//------------------------------------------------------------------------------

SMain::SMain() noexcept
{
}

//------------------------------------------------------------------------------

SMain::~SMain() noexcept
{
}

//------------------------------------------------------------------------------

void SMain::starting()
{
}

//------------------------------------------------------------------------------

void SMain::stopping()
{
}

//------------------------------------------------------------------------------

void SMain::updating()
{
    // This allows us to block the main thread...
    getchar();
}

//------------------------------------------------------------------------------

void SMain::configuring()
{
}

//------------------------------------------------------------------------------

void SMain::swapping( )
{

}

//------------------------------------------------------------------------------

} // namespace ExTimeLine
