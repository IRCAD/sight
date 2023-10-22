/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

static const std::string s_INITIAL_WIDTH_CONFIG = "initialWidth";
static const std::string s_INITIAL_POS_CONFIG   = "initialPos";
static const std::string s_COLOR_CONFIG         = "color";

static const core::com::slots::key_t UPDATE_VIEWPORT_SLOT = "updateViewport";

//---------------------------------------------------------------------------------------------------------------

viewport_range_selector::viewport_range_selector()
{
    new_slot(UPDATE_VIEWPORT_SLOT, [this]{this->updateViewport(false);});
}

//------------------------------------------------------------------------------

void viewport_range_selector::configuring()
{
    this->configureParams();

    const config_t config = this->get_config().get_child("config.<xmlattr>");
    m_initialWidth = config.get<double>(s_INITIAL_WIDTH_CONFIG, m_initialWidth);

    m_initialX = config.get<double>(s_INITIAL_POS_CONFIG, m_initialX);

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, color, m_opacity);
}

//----------------------------------------------------------------------------------------------------------

service::connections_t viewport_range_selector::auto_connections() const
{
    return {
        {s_SELECTED_VIEWPORT_INOUT, sight::viz::scene2d::data::Viewport::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_INPUT, sight::data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_INPUT, sight::data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {s_TF_INPUT, sight::data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_VIEWPORT_SLOT}
    };
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::starting()
{
    {
        auto* const scene         = this->getScene2DRender()->getScene();
        const auto scene_viewport = m_viewport.lock();

        const double viewport_width = scene_viewport->width_or(scene->sceneRect().width());
        const double default_width  = 2. * viewport_width / 4.;

        if(m_initialWidth > viewport_width || m_initialWidth < m_clickCatchRange)
        {
            SIGHT_WARN("Set viewport width to a default value instead of the given one because it can't be accepted.");
            m_initialWidth = default_width;
        }

        const double default_pos = (viewport_width - m_initialWidth) / 2.;
        if(m_initialX < scene_viewport->x_or(scene->sceneRect().x()) || (m_initialX + m_initialWidth) > viewport_width)
        {
            SIGHT_WARN("Set viewport position to a default value since the given one is not correct.");
            m_initialX = default_pos;
        }
    }

    {
        auto* const scene = this->getScene2DRender()->getScene();
        auto viewport     = m_viewport.lock();

        // If the viewport Y and height are not set, scale the viewport to the height of the scene
        {
            viewport->setY(viewport->y_or(scene->sceneRect().y()));
            viewport->setHeight(viewport->height_or(scene->sceneRect().height()));
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
    this->updateViewport(true);
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::updateViewport(bool _signal_selected_viewport)
{
    const auto tf    = m_tf.lock();
    const auto image = m_image.lock();

    m_min = (tf || image) ? std::numeric_limits<double>::max() : m_initialX;
    m_max = (tf || image) ? std::numeric_limits<double>::lowest() : m_initialX + m_initialWidth;

    const vec2d_t pair = this->mapSceneToAdaptor(vec2d_t(m_initialX, 1.0));
    QRectF rect(pair.x, 0, m_initialWidth * m_xAxis->getScale(), pair.y);
    if(tf)
    {
        std::tie(m_min, m_max) = tf->windowMinMax();
    }

    if(image)
    {
        sight::data::helper::medical_image::get_min_max(image.get_shared(), m_imageMin, m_imageMax);

        m_min = std::min(m_min, m_imageMin);
        m_max = std::max(m_max, m_imageMax);
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
        m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
        m_layer->setZValue(m_zValue);
        this->getScene2DRender()->getScene()->addItem(m_layer);
    }
    else
    {
        m_shutter->setRect(rect);
        m_layer->removeFromGroup(m_shutter);
        m_layer->addToGroup(m_shutter);
    }

    // update the viewports
    updateViewportFromShutter(rect.x(), rect.y(), rect.width(), rect.height());
    auto viewport = m_viewport.lock();

    viewport->setX(m_min);
    viewport->setWidth(m_max - m_min);
    this->getScene2DRender()->getView()->updateFromViewport(*viewport);

    auto sig = viewport->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }

    if(_signal_selected_viewport)
    {
        auto selected_viewport = m_selectedViewport.lock();
        auto sig_selected      = selected_viewport->signal<data::object::modified_signal_t>(
            data::object::MODIFIED_SIG
        );
        {
            core::com::connection::blocker block(sig_selected->get_connection(slot(service::slots::UPDATE)));
            sig_selected->async_emit();
        }
    }

    m_clickCatchRange = static_cast<int>(m_max - m_min) / 100;
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    // Event coordinates in scene
    sight::viz::scene2d::vec2d_t coord;
    coord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // Shutter coordinates in scene
    const vec2d_t shutter_coord_pair =
        this->mapAdaptorToScene(vec2d_t(m_shutter->rect().x(), m_shutter->rect().y()));
    const double shutter_width = m_shutter->rect().width() * m_xAxis->getScale();

    const QRectF scene_rect = this->getScene2DRender()->getScene()->sceneRect();

    const bool on_shutter_left   = mouseOnShutterLeft(coord);
    const bool on_shutter_right  = mouseOnShutterRight(coord);
    const bool on_shutter_middle = mouseOnShutterMiddle(coord);

    QRectF rect = m_shutter->rect();

    if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        if(on_shutter_left)
        {
            m_isLeftInteracting = true;
        }
        else if(on_shutter_right)
        {
            m_isRightInteracting = true;
        }
        else if(on_shutter_middle)
        {
            this->getScene2DRender()->getView()->setCursor(Qt::ClosedHandCursor);

            // Interaction when clicking on the center of the shutter
            m_isInteracting         = true;
            m_dragStartPoint        = coord;
            m_dragStartShutterPos.x = shutter_coord_pair.x;
            m_dragStartShutterPos.y = shutter_coord_pair.y;
        }
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
    {
        m_isInteracting      = false;
        m_isLeftInteracting  = false;
        m_isRightInteracting = false;

        // Reset cursor
        if(on_shutter_middle)
        {
            this->getScene2DRender()->getView()->setCursor(Qt::OpenHandCursor);
        }
        else
        {
            this->getScene2DRender()->getView()->setCursor(Qt::ArrowCursor);
        }
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
    {
        // If the mouse is moving onto the shutter, without interactions, the cursor is changed to an other cursor
        // that symbolize the available interactions
        if(!m_isLeftInteracting && !m_isRightInteracting && !m_isInteracting)
        {
            if(on_shutter_left || on_shutter_right)
            {
                this->getScene2DRender()->getView()->setCursor(Qt::SizeHorCursor); // horizontal double arrow
            }
            else if(on_shutter_middle)
            {
                this->getScene2DRender()->getView()->setCursor(Qt::OpenHandCursor); // open hand, for moving the
                                                                                    // whole shutter
            }
            else
            {
                this->getScene2DRender()->getView()->setCursor(Qt::ArrowCursor); // reset the cursor to the
                                                                                 // default cursor
            }
        }

        bool update = false; // if a viewport update will be requested

        const auto image                 = m_image.lock();
        const double left_side_boundary  = image ? m_min : scene_rect.x();
        const double right_side_boundary = image ? m_max : scene_rect.width() - scene_rect.x();
        if(m_isLeftInteracting)
        {
            // Shutter right side position
            const double right_side = rect.x() + rect.width();

            if(coord.x < right_side - m_clickCatchRange)
            {
                rect.setX(std::max(left_side_boundary, coord.x));
            }
            else
            {
                rect.setX(right_side - m_clickCatchRange);
            }

            update = true;
        }
        else if(m_isRightInteracting)
        {
            const double new_width         = coord.x - shutter_coord_pair.x;
            const double shutter_right_pos = shutter_coord_pair.x + new_width;

            if(new_width > m_clickCatchRange) // Shutter's width must be greater than the allowed picking range
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
                rect.setWidth(m_clickCatchRange);
            }

            update = true;
        }
        else if(m_isInteracting)
        {
            const double offset            = coord.x - m_dragStartPoint.x;
            const double new_x             = m_dragStartShutterPos.x + offset;
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
            this->updateViewportFromShutter(rect.x(), rect.y(), rect.width(), rect.height());
            {
                auto selected_viewport = m_selectedViewport.lock();
                auto sig               = selected_viewport->signal<data::object::modified_signal_t>(
                    data::object::MODIFIED_SIG
                );
                {
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void viewport_range_selector::updateViewportFromShutter(double _x, double _y, double _width, double _height)
{
    auto selected_viewport = m_selectedViewport.lock();

    const vec2d_t from_scene_coord = this->mapSceneToAdaptor(vec2d_t(_x, _y));
    const vec2d_t pair             = this->mapSceneToAdaptor(vec2d_t(_width, _height));

    selected_viewport->setX(from_scene_coord.x);
    selected_viewport->setWidth(pair.x);
}

//---------------------------------------------------------------------------------------------------------------

bool viewport_range_selector::mouseOnShutterMiddle(sight::viz::scene2d::vec2d_t _coord)
{
    vec2d_t shutter_coord_pair;
    shutter_coord_pair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.x > m_shutter->rect().x() + m_clickCatchRange)
           && (_coord.x < m_shutter->rect().x() + m_shutter->rect().width() - m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool viewport_range_selector::mouseOnShutterLeft(sight::viz::scene2d::vec2d_t _coord)
{
    vec2d_t shutter_coord_pair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.x >= shutter_coord_pair.x - m_clickCatchRange)
           && (_coord.x <= shutter_coord_pair.x + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool viewport_range_selector::mouseOnShutterRight(sight::viz::scene2d::vec2d_t _coord)
{
    const vec2d_t shutter_coord_pair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    const double shutter_right_pos = shutter_coord_pair.x + m_shutter->rect().width() * m_xAxis->getScale();

    return (_coord.x >= shutter_right_pos - m_clickCatchRange)
           && (_coord.x <= shutter_right_pos + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
