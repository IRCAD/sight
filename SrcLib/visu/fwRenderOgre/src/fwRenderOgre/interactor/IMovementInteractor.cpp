/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
