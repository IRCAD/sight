/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
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

void FixedStyleInteractor::mouseMoveEvent(MouseButton /*button*/, int /*x*/, int /*y*/, int /*dx*/, int /*dy*/)
{
}

// ----------------------------------------------------------------------------

void FixedStyleInteractor::wheelEvent(int /*delta*/, int /*x*/, int /*y*/)
{
}

// ----------------------------------------------------------------------------

void FixedStyleInteractor::buttonReleaseEvent(IInteractor::MouseButton, int, int)
{

}

// ----------------------------------------------------------------------------

void FixedStyleInteractor::buttonPressEvent(IInteractor::MouseButton /*button*/, int /*x*/, int /*y*/)
{

}

// ----------------------------------------------------------------------------

void FixedStyleInteractor::resizeEvent(int /*x*/, int /*y*/)
{
}

// ----------------------------------------------------------------------------

void FixedStyleInteractor::keyPressEvent(int /*key*/)
{
}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre
