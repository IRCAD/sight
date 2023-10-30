/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "modules/viz/scene2d/adaptor/viewport_interactor.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/graphics_view.hpp>

#include <cmath>

namespace sight::module::viz::scene2d::adaptor
{

viewport_interactor::viewport_interactor() noexcept =
    default;

//-----------------------------------------------------------------------------

viewport_interactor::~viewport_interactor() noexcept =
    default;

//-----------------------------------------------------------------------------

void viewport_interactor::configuring()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::starting()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::stopping()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::updating()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::process_interaction(sight::viz::scene2d::data::event& _event)
{
    if(_event.type() == sight::viz::scene2d::data::event::mouse_wheel_up
       && _event.get_modifier() == sight::viz::scene2d::data::event::shift_modifier)
    {
        this->zoom(true);
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_wheel_down
            && _event.get_modifier() == sight::viz::scene2d::data::event::shift_modifier)
    {
        this->zoom(false);
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_button_press
            && _event.get_button() == sight::viz::scene2d::data::event::left_button
            && _event.get_modifier() == sight::viz::scene2d::data::event::shift_modifier)
    {
        m_viewport_is_translated = true;
        m_last_coord_event       = _event.get_coord();
    }
    else if(m_viewport_is_translated)
    {
        if(_event.type() == sight::viz::scene2d::data::event::mouse_move)
        {
            sight::viz::scene2d::vec2d_t coord = _event.get_coord();
            const auto viewport                = m_viewport.lock();

            const double dx      = coord.x - m_last_coord_event.x;
            const double x_trans = dx * viewport->width()
                                   / static_cast<double>(this->get_scene_2d_render()->get_view()->width());

            const double dy      = coord.y - m_last_coord_event.y;
            const double y_trans = dy * viewport->height()
                                   / static_cast<double>(this->get_scene_2d_render()->get_view()->height());

            viewport->set_x(viewport->x() - x_trans);
            viewport->set_y(viewport->y() - y_trans);

            this->get_scene_2d_render()->get_view()->update_from_viewport(*viewport);

            m_last_coord_event = coord;
        }
        else if(_event.type() == sight::viz::scene2d::data::event::mouse_button_release)
        {
            m_viewport_is_translated = false;
        }
    }
}

//-----------------------------------------------------------------------------

void viewport_interactor::zoom(bool _zoom_in)
{
    const auto scene_viewport = m_viewport.lock();

    double y = scene_viewport->y();
    double x = scene_viewport->x();

    double width  = scene_viewport->width();
    double height = scene_viewport->height();

    const double zoom_percent = 10.F / 100.0F;
    const double center_x     = x + width / 2.0F;
    const double center_y     = y + height / 2.0F;

    double new_width  = NAN;
    double new_height = NAN;
    if(_zoom_in)
    {
        new_width  = width * zoom_percent;
        new_height = height * zoom_percent;
    }
    else
    {
        new_width  = -width * zoom_percent;
        new_height = -height * zoom_percent;
    }

    new_width  += width;
    new_height += height;

    x = center_x - new_width / 2;
    y = center_y - new_height / 2;

    width  = new_width;
    height = new_height;

    // Set viewport
    scene_viewport->set_x(x);
    scene_viewport->set_y(y);
    scene_viewport->set_width(width);
    scene_viewport->set_height(height);
    auto viewport_object = m_viewport.lock();
    this->get_scene_2d_render()->get_view()->update_from_viewport(*viewport_object);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
