/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene2d/adaptor/viewport_range_selector.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <viz/scene2d/data/init_qt_pen.hpp>
#include <viz/scene2d/data/viewport.hpp>
#include <viz/scene2d/graphics_view.hpp>

#include <QGraphicsRectItem>

namespace sight::module::viz::scene2d::adaptor
{

static const std::string INITIAL_WIDTH_CONFIG = "initialWidth";
static const std::string INITIAL_POS_CONFIG   = "initialPos";
static const std::string COLOR_CONFIG         = "color";

static const core::com::slots::key_t UPDATE_VIEWPORT_SLOT = "updateViewport";

//---------------------------------------------------------------------------------------------------------------

viewport_range_selector::viewport_range_selector()
{
    new_slot(UPDATE_VIEWPORT_SLOT, [this]{this->update_viewport(false);});
}

//------------------------------------------------------------------------------

void viewport_range_selector::configuring()
{
    this->configure_params();

    const config_t config = this->get_config().get_child("config.<xmlattr>");
    m_initial_width = config.get<double>(INITIAL_WIDTH_CONFIG, m_initial_width);

    m_initial_x = config.get<double>(INITIAL_POS_CONFIG, m_initial_x);

    const std::string color = config.get(COLOR_CONFIG, "#FFFFFF");
    sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_color, color, m_opacity);
}

//----------------------------------------------------------------------------------------------------------

service::connections_t viewport_range_selector::auto_connections() const
{
    return {
        {SELECTED_VIEWPORT_INOUT, sight::viz::scene2d::data::viewport::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_INPUT, sight::data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_INPUT, sight::data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {TF_INPUT, sight::data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_VIEWPORT_SLOT}
    };
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::starting()
{
    {
        auto* const scene         = this->get_scene_2d_render()->get_scene();
        const auto scene_viewport = m_viewport.lock();

        const double viewport_width = scene_viewport->width_or(scene->sceneRect().width());
        const double default_width  = 2. * viewport_width / 4.;

        if(m_initial_width > viewport_width || m_initial_width < m_click_catch_range)
        {
            SIGHT_WARN("Set viewport width to a default value instead of the given one because it can't be accepted.");
            m_initial_width = default_width;
        }

        const double default_pos = (viewport_width - m_initial_width) / 2.;
        if(m_initial_x < scene_viewport->x_or(scene->sceneRect().x())
           || (m_initial_x + m_initial_width) > viewport_width)
        {
            SIGHT_WARN("Set viewport position to a default value since the given one is not correct.");
            m_initial_x = default_pos;
        }
    }

    {
        auto* const scene = this->get_scene_2d_render()->get_scene();
        auto viewport     = m_viewport.lock();

        // If the viewport Y and height are not set, scale the viewport to the height of the scene
        {
            viewport->set_y(viewport->y_or(scene->sceneRect().y()));
            viewport->set_height(viewport->height_or(scene->sceneRect().height()));
        }
    }

    this->updating();
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::updating()
{
    this->update_viewport(true);
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::update_viewport(bool _signal_selected_viewport)
{
    const auto tf    = m_tf.lock();
    const auto image = m_image.lock();

    m_min = (tf || image) ? std::numeric_limits<double>::max() : m_initial_x;
    m_max = (tf || image) ? std::numeric_limits<double>::lowest() : m_initial_x + m_initial_width;

    const glm::dvec2 pair = this->map_scene_to_adaptor(glm::dvec2(m_initial_x, 1.0));
    QRectF rect(pair.x, 0, m_initial_width * m_x_axis->get_scale(), pair.y);
    if(tf)
    {
        std::tie(m_min, m_max) = tf->window_min_max();
    }

    if(image)
    {
        std::tie(m_image_min, m_image_max) = data::helper::medical_image::get_min_max<double>(image.get_shared());

        m_min = std::min(m_min, m_image_min);
        m_max = std::max(m_max, m_image_max);
    }

    // Fit the shutter from the current range
    rect.setRect(m_min, rect.y(), m_max - m_min, rect.height());
    if(m_shutter == nullptr)
    {
        m_shutter = new QGraphicsRectItem(rect);
        m_shutter->setBrush(m_color.color());
        m_shutter->setPen(Qt::NoPen);

        m_layer = new QGraphicsItemGroup();
        m_layer->addToGroup(m_shutter);
        // Adjust the layer's position and zValue depending on the associated axis
        m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
        m_layer->setZValue(m_z_value);
        this->get_scene_2d_render()->get_scene()->addItem(m_layer);
    }
    else
    {
        m_shutter->setRect(rect);
        m_layer->removeFromGroup(m_shutter);
        m_layer->addToGroup(m_shutter);
    }

    // update the viewports
    update_viewport_from_shutter(rect.x(), rect.y(), rect.width(), rect.height());
    auto viewport = m_viewport.lock();

    viewport->set_x(m_min);
    viewport->set_width(m_max - m_min);
    this->get_scene_2d_render()->get_view()->update_from_viewport(*viewport);

    viewport->async_emit(this, data::object::MODIFIED_SIG);

    if(_signal_selected_viewport)
    {
        auto selected_viewport = m_selected_viewport.lock();
        selected_viewport->async_emit(this, data::object::MODIFIED_SIG);
    }

    m_click_catch_range = static_cast<int>(m_max - m_min) / 100;
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::process_interaction(sight::viz::scene2d::data::event& _event)
{
    // Event coordinates in scene
    glm::dvec2 coord;
    coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord());

    // Shutter coordinates in scene
    const glm::dvec2 shutter_coord_pair =
        this->map_adaptor_to_scene(glm::dvec2(m_shutter->rect().x(), m_shutter->rect().y()));
    const double shutter_width = m_shutter->rect().width() * m_x_axis->get_scale();

    const QRectF scene_rect = this->get_scene_2d_render()->get_scene()->sceneRect();

    const bool on_shutter_left   = mouse_on_shutter_left(coord);
    const bool on_shutter_right  = mouse_on_shutter_right(coord);
    const bool on_shutter_middle = mouse_on_shutter_middle(coord);

    QRectF rect = m_shutter->rect();

    if(_event.type() == sight::viz::scene2d::data::event::mouse_button_press)
    {
        if(on_shutter_left)
        {
            m_is_left_interacting = true;
        }
        else if(on_shutter_right)
        {
            m_is_right_interacting = true;
        }
        else if(on_shutter_middle)
        {
            this->get_scene_2d_render()->get_view()->setCursor(Qt::ClosedHandCursor);

            // Interaction when clicking on the center of the shutter
            m_is_interacting           = true;
            m_drag_start_point         = coord;
            m_drag_start_shutter_pos.x = shutter_coord_pair.x;
            m_drag_start_shutter_pos.y = shutter_coord_pair.y;
        }
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_button_release)
    {
        m_is_interacting       = false;
        m_is_left_interacting  = false;
        m_is_right_interacting = false;

        // Reset cursor
        if(on_shutter_middle)
        {
            this->get_scene_2d_render()->get_view()->setCursor(Qt::OpenHandCursor);
        }
        else
        {
            this->get_scene_2d_render()->get_view()->setCursor(Qt::ArrowCursor);
        }
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_move)
    {
        // If the mouse is moving onto the shutter, without interactions, the cursor is changed to an other cursor
        // that symbolize the available interactions
        if(!m_is_left_interacting && !m_is_right_interacting && !m_is_interacting)
        {
            if(on_shutter_left || on_shutter_right)
            {
                this->get_scene_2d_render()->get_view()->setCursor(Qt::SizeHorCursor); // horizontal double arrow
            }
            else if(on_shutter_middle)
            {
                this->get_scene_2d_render()->get_view()->setCursor(Qt::OpenHandCursor); // open hand, for moving the
                // whole shutter
            }
            else
            {
                this->get_scene_2d_render()->get_view()->setCursor(Qt::ArrowCursor); // reset the cursor to the
                // default cursor
            }
        }

        bool update = false; // if a viewport update will be requested

        const auto image                 = m_image.lock();
        const double left_side_boundary  = image ? m_min : scene_rect.x();
        const double right_side_boundary = image ? m_max : scene_rect.width() - scene_rect.x();
        if(m_is_left_interacting)
        {
            // Shutter right side position
            const double right_side = rect.x() + rect.width();

            if(coord.x < right_side - m_click_catch_range)
            {
                rect.setX(std::max(left_side_boundary, coord.x));
            }
            else
            {
                rect.setX(right_side - m_click_catch_range);
            }

            update = true;
        }
        else if(m_is_right_interacting)
        {
            const double new_width         = coord.x - shutter_coord_pair.x;
            const double shutter_right_pos = shutter_coord_pair.x + new_width;

            if(new_width > m_click_catch_range) // Shutter's width must be greater than the allowed picking range
            {
                if(shutter_right_pos <= right_side_boundary)
                {
                    rect.setWidth(new_width);
                }
                else
                {
                    rect.setWidth(right_side_boundary - shutter_coord_pair.x);
                }
            }
            else
            {
                rect.setWidth(m_click_catch_range);
            }

            update = true;
        }
        else if(m_is_interacting)
        {
            const double offset            = coord.x - m_drag_start_point.x;
            const double new_x             = m_drag_start_shutter_pos.x + offset;
            const double shutter_right_pos = new_x + shutter_width;

            if(new_x >= left_side_boundary && shutter_right_pos < right_side_boundary)
            {
                rect.setX(new_x);
            }
            else if(new_x < left_side_boundary)
            {
                rect.setX(left_side_boundary);
            }
            else if(shutter_right_pos >= right_side_boundary)
            {
                rect.setX(right_side_boundary - shutter_width);
            }

            rect.setWidth(shutter_width);
            update = true;
        }

        if(update)
        {
            // Update graphical shutter
            m_shutter->setRect(rect);
            m_layer->removeFromGroup(m_shutter);
            m_layer->addToGroup(m_shutter);

            // Update object
            this->update_viewport_from_shutter(rect.x(), rect.y(), rect.width(), rect.height());
            {
                auto selected_viewport = m_selected_viewport.lock();
                selected_viewport->async_emit(this, data::object::MODIFIED_SIG);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::update_viewport_from_shutter(double _x, double _y, double _width, double _height)
{
    auto selected_viewport = m_selected_viewport.lock();

    const glm::dvec2 from_scene_coord = this->map_scene_to_adaptor(glm::dvec2(_x, _y));
    const glm::dvec2 pair             = this->map_scene_to_adaptor(glm::dvec2(_width, _height));

    selected_viewport->set_x(from_scene_coord.x);
    selected_viewport->set_width(pair.x);
}

//---------------------------------------------------------------------------------------------------------------

bool viewport_range_selector::mouse_on_shutter_middle(glm::dvec2 _coord)
{
    glm::dvec2 shutter_coord_pair;
    shutter_coord_pair = this->map_adaptor_to_scene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.x > m_shutter->rect().x() + m_click_catch_range)
           && (_coord.x < m_shutter->rect().x() + m_shutter->rect().width() - m_click_catch_range);
}

//---------------------------------------------------------------------------------------------------------------

bool viewport_range_selector::mouse_on_shutter_left(glm::dvec2 _coord)
{
    glm::dvec2 shutter_coord_pair = this->map_adaptor_to_scene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.x >= shutter_coord_pair.x - m_click_catch_range)
           && (_coord.x <= shutter_coord_pair.x + m_click_catch_range);
}

//---------------------------------------------------------------------------------------------------------------

bool viewport_range_selector::mouse_on_shutter_right(glm::dvec2 _coord)
{
    const glm::dvec2 shutter_coord_pair = this->map_adaptor_to_scene({m_shutter->rect().x(), m_shutter->rect().y()});

    const double shutter_right_pos = shutter_coord_pair.x + m_shutter->rect().width() * m_x_axis->get_scale();

    return (_coord.x >= shutter_right_pos - m_click_catch_range)
           && (_coord.x <= shutter_right_pos + m_click_catch_range);
}

//---------------------------------------------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
