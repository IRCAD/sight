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

//------------------------------------------------------------------------------

void FixedStyleInteractor::keyReleaseEvent(int)
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::focusInEvent()
{
}

//------------------------------------------------------------------------------

void FixedStyleInteractor::focusOutEvent()
{
}

//------------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre
