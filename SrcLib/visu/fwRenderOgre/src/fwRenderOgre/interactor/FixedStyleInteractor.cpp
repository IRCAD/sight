/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/FixedStyleInteractor.hpp"

#include <fwRenderOgre/registry/macros.hpp>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::FixedStyleInteractor );

namespace fwRenderOgre
{
namespace interactor
{

// ----------------------------------------------------------------------------

FixedStyleInteractor::FixedStyleInteractor() :
    ::fwRenderOgre::interactor::IMovementInteractor()
{
}

// ----------------------------------------------------------------------------

void FixedStyleInteractor::mouseMoveEvent(MouseButton, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::buttonReleaseEvent(MouseButton, int, int)
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::buttonPressEvent(MouseButton, int, int)
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::resizeEvent(int, int)
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::keyPressEvent(int)
{
}

} // namespace interactor
} // namespace fwRenderOgre
