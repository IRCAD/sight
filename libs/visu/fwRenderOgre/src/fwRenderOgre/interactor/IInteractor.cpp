/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "fwRenderOgre/interactor/IInteractor.hpp"

#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <services/macros.hpp>

namespace fwRenderOgre
{
namespace interactor
{

// ----------------------------------------------------------------------------

IInteractor::IInteractor(Layer::sptr _layer, bool _layerOrderDependant) :
    m_layer(_layer),
    m_layerOrderDependant(_layerOrderDependant)
{
}

// ----------------------------------------------------------------------------

IInteractor::~IInteractor()
{
}

// ----------------------------------------------------------------------------

void IInteractor::setSceneLength(float)
{
}

//------------------------------------------------------------------------------

void IInteractor::mouseMoveEvent(MouseButton, Modifier, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::wheelEvent(Modifier, int, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::buttonReleaseEvent(MouseButton, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::buttonPressEvent(MouseButton, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::buttonDoublePressEvent(MouseButton, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::keyPressEvent(int, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::keyReleaseEvent(int, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::resizeEvent(int, int)
{

}

// ----------------------------------------------------------------------------

bool IInteractor::isInLayer(int _mouseX, int _mouseY, Layer::sptr _layer, bool _layerOrderDependant)
{
    const auto* const layerVp = _layer->getViewport();
    bool isInLayer            = isInViewport(_mouseX, _mouseY, layerVp);

    // Check if there's no layer above.
    if(_layerOrderDependant)
    {
        auto* const renderWindow       = layerVp->getTarget();
        const unsigned short numLayers = renderWindow->getNumViewports();
        for(unsigned short i = 0; i < numLayers && isInLayer; ++i)
        {
            const auto* const vp = renderWindow->getViewport(i);
            if(vp->getZOrder() > layerVp->getZOrder())
            {
                isInLayer = !isInViewport(_mouseX, _mouseY, vp);
            }
        }
    }

    return isInLayer;
}

// ----------------------------------------------------------------------------

bool IInteractor::isInViewport(int _mouseX, int _mouseY, const ::Ogre::Viewport* const _vp)
{
    const int top    = _vp->getActualTop();
    const int left   = _vp->getActualLeft();
    const int bottom = top + _vp->getActualHeight();
    const int right  = left + _vp->getActualWidth();

    return _mouseX >= left && _mouseX <= right && _mouseY >= top && _mouseY <= bottom;
}

} // namespace interactor
} // namespace fwRenderOgre
