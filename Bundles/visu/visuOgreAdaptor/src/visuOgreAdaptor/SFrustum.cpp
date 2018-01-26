/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SFrustum.hpp"

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFrustum);

//-----------------------------------------------------------------------------

SFrustum::SFrustum() noexcept
{
}

//-----------------------------------------------------------------------------

SFrustum::~SFrustum() noexcept
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrustum::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    return connections;
}

//------------------------------------------------------------------------------

void SFrustum::configuring()
{
}

//-----------------------------------------------------------------------------

void SFrustum::starting()
{
}

//-----------------------------------------------------------------------------

void SFrustum::updating()
{
}

//-----------------------------------------------------------------------------

void SFrustum::stopping()
{
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
