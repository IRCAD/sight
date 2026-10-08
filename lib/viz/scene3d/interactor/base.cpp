/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

namespace sight::viz::scene3d::interactor
{

// ----------------------------------------------------------------------------

base::~base()
= default;

// ----------------------------------------------------------------------------

void base::set_scene_length(float /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::mouse_move_event(
    mouse_button /*unused*/,
    modifier /*unused*/,
    int /*unused*/,
    int /*unused*/,
    int /*unused*/,
    int /*unused*/
)
{
}

//------------------------------------------------------------------------------

void base::wheel_event(modifier /*unused*/, double /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::button_release_event(mouse_button /*unused*/, modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::button_press_event(mouse_button /*unused*/, modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::button_double_press_event(mouse_button /*unused*/, modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::key_press_event(int /*unused*/, modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::key_release_event(int /*unused*/, modifier /*unused*/, int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::resize_event(int /*unused*/, int /*unused*/)
{
}

//------------------------------------------------------------------------------

void base::pinch_gesture_event(double /*_scaleFactor*/, int /*_centerX*/, int /*_centerY*/)
{
}

//------------------------------------------------------------------------------

void base::pan_gesture_move_event(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
}

//------------------------------------------------------------------------------

void base::pan_gesture_release_event(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
}

//------------------------------------------------------------------------------

void base::long_tap_gesture_event(int /*_x*/, int /*_y*/)
{
}

//------------------------------------------------------------------------------

void base::leave_event()
{
}

//------------------------------------------------------------------------------

void base::enter_event()
{
}

// ----------------------------------------------------------------------------

bool base::is_in_layer(int _mouse_x, int _mouse_y, layer::sptr _layer)
{
    const auto* const layer_vp = _layer->get_viewport();
    return is_in_viewport(_mouse_x, _mouse_y, layer_vp);
}

// ----------------------------------------------------------------------------

bool base::is_in_viewport(int _mouse_x, int _mouse_y, const Ogre::Viewport* const _vp)
{
    const int top    = _vp->getActualTop();
    const int left   = _vp->getActualLeft();
    const int bottom = top + _vp->getActualHeight();
    const int right  = left + _vp->getActualWidth();

    return _mouse_x >= left && _mouse_x <= right && _mouse_y >= top && _mouse_y <= bottom;
}

} // namespace sight::viz::scene3d::interactor
