/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Ex02TimeLine/SMain.hpp"

#include "fwData/Composite.hpp"

#include <fwServices/macros.hpp>
#include <fwThread/Timer.hpp>
#include <fwTools/Object.hpp>

fwServicesRegisterMacro( ::fwServices::IService, ::Ex02TimeLine::SMain, ::fwData::Composite );

namespace Ex02TimeLine
{

//------------------------------------------------------------------------------

SMain::SMain() throw()
{
}

//------------------------------------------------------------------------------

SMain::~SMain() throw()
{
}

//------------------------------------------------------------------------------

void SMain::starting() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void SMain::stopping() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void SMain::updating() throw( ::fwTools::Failed )
{
    // This allows us to block the main thread...
    getchar();
}

//------------------------------------------------------------------------------

void SMain::configuring() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void SMain::swapping( ) throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

} // namespace Ex02TimeLine
