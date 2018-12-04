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

#include "fwRenderOgre/interactor/IMovementInteractor.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>

namespace fwRenderOgre
{
namespace interactor
{

const ::fwCom::Signals::SignalKeyType IMovementInteractor::s_RESET_CAMERA_SIG = "resetCamera";

// ----------------------------------------------------------------------------

IMovementInteractor::IMovementInteractor() :
    m_lookAtZ(0.f),
    m_mouseScale(33.f),
    m_fZoom(1.f)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    newSignal<ResetCameraSignalType>( s_RESET_CAMERA_SIG );

    m_sigRenderRequested = newSignal< RenderRequestedSigType >( s_RENDER_REQUESTED_SIG );
}

// ----------------------------------------------------------------------------

IMovementInteractor::~IMovementInteractor()
{
}

// ----------------------------------------------------------------------------

void IMovementInteractor::setMouseScale(float mouseScale)
{
    m_mouseScale = static_cast<float>(MOUSE_SCALE_COEFFICIENT) / mouseScale;
    m_lookAtZ    = mouseScale;
    m_fZoom      = 1.f;
}

// ----------------------------------------------------------------------------

float IMovementInteractor::getLookAtZ() const
{
    return m_lookAtZ;
}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre
