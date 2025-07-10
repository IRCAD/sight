/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "module/viz/scene2d/adaptor/histogram.hpp"

#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <viz/scene2d/data/init_qt_pen.hpp>
#include <viz/scene2d/graphics_view.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

namespace sight::module::viz::scene2d::adaptor
{

const core::com::slots::key_t IMAGE_CHANGE_SLOT = "imageChange";

//---------------------------------------------------------------------------------------------------------

histogram::histogram() noexcept
{
    new_slot(IMAGE_CHANGE_SLOT, &histogram::on_image_change, this);
}

//---------------------------------------------------------------------------------------------------------

histogram::~histogram() noexcept =
    default;

//---------------------------------------------------------------------------------------------------------

void histogram::configuring()
{
    this->configure_params(); // Looks for 'xAxis', 'yAxis' and 'zValue'

    const config_t srv_config = this->get_config();
    const config_t config     = srv_config.get_child("config.<xmlattr>");

    m_scale = m_y_axis->get_scale();

    if(config.count("color") != 0U)
    {
        sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_color, config.get<std::string>("color"));
    }

    const auto cursor_config = srv_config.get_child_optional("config.cursor");
    if(cursor_config.has_value())
    {
        std::string color;
        m_cursor_enabled = true;

        color = cursor_config->get<std::string>("<xmlattr>.color", "");
        sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_cursor_color, color);

        color = cursor_config->get<std::string>("<xmlattr>.borderColor", "");
        sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_cursor_border_color, color);

        m_cursor_size = cursor_config->get<double>("<xmlattr>.size", m_cursor_size);

        color = cursor_config->get<std::string>("<xmlattr>.textColor", "#FFFFFF");
        sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_cursor_label_color, color);

        m_font_size = cursor_config->get<int>("<xmlattr>.fontSize", m_font_size);
    }
}

//---------------------------------------------------------------------------------------------------------

void histogram::starting()
{
    if(m_cursor_enabled)
    {
        m_cursor_layer = new QGraphicsItemGroup();
        // Adjust the layer's position and zValue depending on the associated axis
        m_cursor_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
        m_cursor_layer->setZValue(m_z_value);
        this->get_scene_2d_render()->get_scene()->addItem(m_cursor_layer);

        m_cursor_item = new QGraphicsEllipseItem();
        m_cursor_item->setBrush(m_cursor_color.color());
        m_cursor_item->setPen(m_cursor_border_color);
        m_cursor_item->setZValue(m_z_value);
        m_color.setCosmetic(true);
        m_cursor_layer->addToGroup(m_cursor_item);

        // We set the point size to 1.f but we will scale the text later according to m_fontSize
        QFont font;
        font.setPointSize(1);
        font.setKerning(true);
        font.setFixedPitch(true);

        m_cursor_label = new QGraphicsSimpleTextItem();
        m_cursor_label->setBrush(QBrush(m_cursor_label_color.color()));
        m_cursor_label->setFont(font);
        m_cursor_label->setVisible(false);
        m_cursor_layer->addToGroup(m_cursor_label);
    }

    const auto image = m_image.lock();
    m_histogram = std::make_unique<data::helper::histogram>(image.get_shared());

    on_image_change();
}

//---------------------------------------------------------------------------------------------------------

void histogram::updating()
{
    if(m_layer != nullptr)
    {
        this->get_scene_2d_render()->get_scene()->removeItem(m_layer);
        delete m_layer;
    }

    m_layer = new QGraphicsItemGroup();

    const auto image = m_image.lock();

    if(data::helper::medical_image::check_image_validity(image.get_shared()))
    {
        const auto values = m_histogram->sample(m_histogram_bins_width);

        if(!values.empty())
        {
            // Update color with opacity
            QColor color = m_color.color();
            color.setAlphaF(static_cast<float>(m_opacity));
            m_color.setColor(color);

            const double min      = m_histogram->min();
            const auto bins_width = static_cast<double>(m_histogram_bins_width);

            QBrush brush = QBrush(m_color.color());

            // Build the graphic items:
            for(std::size_t i = 0 ; i < values.size() ; ++i)
            {
                glm::dvec2 pt1 = this->map_adaptor_to_scene({min + static_cast<double>(i) * bins_width, values[i]});
                glm::dvec2 pt2 = this->map_adaptor_to_scene({min + static_cast<double>(i + 1) * bins_width, values[i]});

                QPainterPath painter(QPointF(pt1.x, 0));
                painter.lineTo(pt1.x, pt1.y);
                painter.lineTo(pt2.x, pt1.y);
                painter.lineTo(pt2.x, 0);

                auto* item = new QGraphicsPathItem(painter);
                item->setPath(painter);
                item->setBrush(brush);
                item->setPen(Qt::NoPen);

                m_layer->addToGroup(item);
            }

            // Adjust the layer's position and zValue depending on the associated axis
            m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
            m_layer->setZValue(m_z_value);

            // Add to the scene the unique item which gather the whole set of rectangle graphic items:
            this->get_scene_2d_render()->get_scene()->addItem(m_layer);
        }
    }
}

//---------------------------------------------------------------------------------------------------------

void histogram::stopping()
{
    m_histogram.reset();

    if(m_layer != nullptr)
    {
        this->get_scene_2d_render()->get_scene()->removeItem(m_layer);
        delete m_layer;
        m_layer = nullptr;
    }

    if(m_cursor_layer != nullptr)
    {
        this->get_scene_2d_render()->get_scene()->removeItem(m_cursor_layer);
        delete m_cursor_layer;
        m_cursor_layer = nullptr;
    }
}

//---------------------------------------------------------------------------------------------------------

void histogram::process_interaction(sight::viz::scene2d::data::event& _event)
{
    if(!this->started())
    {
        return;
    }

    bool update_pointed_pos = false;

    // Vertical scaling
    if(_event.type() == sight::viz::scene2d::data::event::mouse_wheel_up)
    {
        if(_event.get_modifier() == sight::viz::scene2d::data::event::control_modifier)
        {
            m_histogram_bins_width = std::min(
                std::max(std::size_t(1), static_cast<std::size_t>(m_histogram->max() - m_histogram->min())),
                m_histogram_bins_width * 2
            );
            this->updating();
        }
        else
        {
            double scale = SCALE;
            if(_event.get_modifier() == sight::viz::scene2d::data::event::shift_modifier)
            {
                scale = FAST_SCALE;
            }

            m_scale *= scale;

            m_layer->setTransform(QTransform::fromScale(1., scale), true);

            _event.set_accepted(true);
            m_y_axis->set_scale(m_scale);

            auto viewport = m_viewport.lock();
            auto sig      = viewport->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();
        }

        update_pointed_pos = true;
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_wheel_down)
    {
        if(_event.get_modifier() == sight::viz::scene2d::data::event::control_modifier)
        {
            m_histogram_bins_width = std::max(std::size_t(1), m_histogram_bins_width / 2);
            this->updating();
        }
        else
        {
            double scale = SCALE;
            if(_event.get_modifier() == sight::viz::scene2d::data::event::shift_modifier)
            {
                scale = FAST_SCALE;
            }

            m_scale /= scale;

            m_layer->setTransform(QTransform::fromScale(1., 1. / scale), true);

            _event.set_accepted(true);
            m_y_axis->set_scale(m_scale);

            auto viewport = m_viewport.lock();
            auto sig      = viewport->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();
        }

        update_pointed_pos = true;
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_move)
    {
        update_pointed_pos = true;
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_button_press)
    {
        m_is_interacting   = true;
        update_pointed_pos = true;
    }
    else if(_event.type() == sight::viz::scene2d::data::event::mouse_button_release)
    {
        m_is_interacting   = false;
        update_pointed_pos = true;
    }
    else if(_event.type() == sight::viz::scene2d::data::event::enter_event)
    {
        m_entered          = true;
        update_pointed_pos = true;
    }
    else if(_event.type() == sight::viz::scene2d::data::event::leave_event)
    {
        m_entered          = false;
        update_pointed_pos = true;
    }

    if(update_pointed_pos)
    {
        this->update_current_point(_event);
    }
}

//----------------------------------------------------------------------------------------------------------

service::connections_t histogram::auto_connections() const
{
    return {
        {IMAGE_INPUT, data::image::MODIFIED_SIG, IMAGE_CHANGE_SLOT},
        {IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, IMAGE_CHANGE_SLOT}
    };
}

//---------------------------------------------------------------------------------------------------------

void histogram::update_current_point(sight::viz::scene2d::data::event& _event)
{
    if(m_cursor_enabled)
    {
        const auto values                = m_histogram->sample(m_histogram_bins_width);
        const double histogram_min_value = m_histogram->min();
        const auto histogram_bins_width  = static_cast<double>(m_histogram_bins_width);

        // Event coordinates in scene
        glm::dvec2 scene_coord      = this->get_scene_2d_render()->map_to_scene(_event.get_coord());
        const double hist_index     = scene_coord.x;
        const double index          = hist_index - histogram_min_value;
        const std::size_t nb_values = values.size() * m_histogram_bins_width;

        const auto viewport         = m_viewport.lock();
        const auto view_to_viewport = this->view_to_viewport(*viewport);

        if(index >= 0.F && index < static_cast<double>(nb_values) && m_entered)
        {
            glm::dvec2 coord;
            coord.x = scene_coord.x;
            coord.y = static_cast<double>(values.at(static_cast<std::size_t>(index / histogram_bins_width))) * m_scale;

            // Draw the cursor
            const glm::dvec2 diameter = glm::dvec2(m_cursor_size, m_cursor_size) * view_to_viewport;

            const double x = coord.x - diameter.x / 2;
            const double y = coord.y - diameter.y / 2;

            m_cursor_item->setRect(x, y, diameter.x, diameter.y);
            m_cursor_item->setVisible(true);

            if(m_is_interacting)
            {
                // Draw the cursor text
                const glm::dvec2 scale = glm::dvec2(m_font_size, m_font_size) * view_to_viewport;

                // Event coordinates in scene
                m_cursor_label->setText(QString::number(static_cast<int>((coord.x))));

                QTransform transform;
                transform.scale(scale.x, scale.y);

                m_cursor_label->setTransform(transform);
                m_cursor_label->setPos(coord.x + scale.x * 0.5, coord.y - scale.y * 0.5);
                m_cursor_label->setVisible(true);
            }
            else
            {
                m_cursor_label->setVisible(false);
            }
        }
        else
        {
            m_cursor_item->setVisible(false);
            m_cursor_label->setVisible(false);
        }
    }
}

//------------------------------------------------------------------------------

void histogram::on_image_change()
{
    m_histogram->compute();
    m_histogram_bins_width = std::max(
        static_cast<std::size_t>(1),
        static_cast<std::size_t>(m_histogram->max() - m_histogram->min()) / 50
    );
    this->updating();
}

} // namespace sight::module::viz::scene2d::adaptor
