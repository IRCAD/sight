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

#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/utils.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::viz::scene3d::interactor
{

// ----------------------------------------------------------------------------

base::base(layer::sptr _layer, bool _layer_order_dependant) :
    m_layer(_layer),
    m_layerOrderDependant(_layer_order_dependant)
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

bool base::isInLayer(int _mouse_x, int _mouse_y, layer::sptr _layer, bool _layer_order_dependant)
{
    const auto* const layer_vp = _layer->getViewport();
    bool is_in_layer           = isInViewport(_mouse_x, _mouse_y, layer_vp);

    // Check if there's no layer above.
    if(_layer_order_dependant)
    {
        auto* const render_window      = layer_vp->getTarget();
        const std::uint16_t num_layers = render_window->getNumViewports();
        for(std::uint16_t i = 0 ; i < num_layers && is_in_layer ; ++i)
        {
            const auto* const vp = render_window->getViewport(i);
            if(vp->getZOrder() > layer_vp->getZOrder())
            {
                is_in_layer = !isInViewport(_mouse_x, _mouse_y, vp);
            }
        }
    }

    return is_in_layer;
}

// ----------------------------------------------------------------------------

bool base::isInViewport(int _mouse_x, int _mouse_y, const Ogre::Viewport* const _vp)
{
    const int top    = _vp->getActualTop();
    const int left   = _vp->getActualLeft();
    const int bottom = top + _vp->getActualHeight();
    const int right  = left + _vp->getActualWidth();

    return _mouse_x >= left && _mouse_x <= right && _mouse_y >= top && _mouse_y <= bottom;
}

} // namespace sight::viz::scene3d::interactor
