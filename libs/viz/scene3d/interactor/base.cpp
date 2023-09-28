/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/interactor/base.hpp"

#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/Utils.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::viz::scene3d::interactor
{

// ----------------------------------------------------------------------------

base::base(Layer::sptr _layer, bool _layerOrderDependant) :
    m_layer(_layer),
    m_layerOrderDependant(_layerOrderDependant)
{
}

// ----------------------------------------------------------------------------

base::~base()
= default;

// ----------------------------------------------------------------------------

void base::setSceneLength(float /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::mouseMoveEvent(
    MouseButton /*unused*/,
    Modifier /*unused*/,
    int /*unused*/,
    int /*unused*/,
    int /*unused*/,
    int /*unused*/
)
{
}

//------------------------------------------------------------------------------

void base::wheelEvent(Modifier /*unused*/, double /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::buttonReleaseEvent(MouseButton /*unused*/, Modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::buttonPressEvent(MouseButton /*unused*/, Modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::buttonDoublePressEvent(MouseButton /*unused*/, Modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::keyPressEvent(int /*unused*/, Modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::keyReleaseEvent(int /*unused*/, Modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::resizeEvent(int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::pinchGestureEvent(double /*_scaleFactor*/, int /*_centerX*/, int /*_centerY*/)
{
}

//------------------------------------------------------------------------------

void base::panGestureMoveEvent(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
}

//------------------------------------------------------------------------------

void base::panGestureReleaseEvent(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
}

//------------------------------------------------------------------------------

void base::longTapGestureEvent(int /*_x*/, int /*_y*/)
{
}

//------------------------------------------------------------------------------

void base::leaveEvent()
{
}

//------------------------------------------------------------------------------

void base::enterEvent()
{
}

// ----------------------------------------------------------------------------

bool base::isInLayer(int _mouseX, int _mouseY, Layer::sptr _layer, bool _layerOrderDependant)
{
    const auto* const layerVp = _layer->getViewport();
    bool isInLayer            = isInViewport(_mouseX, _mouseY, layerVp);

    // Check if there's no layer above.
    if(_layerOrderDependant)
    {
        auto* const renderWindow      = layerVp->getTarget();
        const std::uint16_t numLayers = renderWindow->getNumViewports();
        for(std::uint16_t i = 0 ; i < numLayers && isInLayer ; ++i)
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

bool base::isInViewport(int _mouseX, int _mouseY, const Ogre::Viewport* const _vp)
{
    const int top    = _vp->getActualTop();
    const int left   = _vp->getActualLeft();
    const int bottom = top + _vp->getActualHeight();
    const int right  = left + _vp->getActualWidth();

    return _mouseX >= left && _mouseX <= right && _mouseY >= top && _mouseY <= bottom;
}

} // namespace sight::viz::scene3d::interactor
