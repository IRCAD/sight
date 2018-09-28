/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
