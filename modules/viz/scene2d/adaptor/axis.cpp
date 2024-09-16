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

#include "modules/viz/scene2d/adaptor/axis.hpp"

#include <viz/scene2d/data/init_qt_pen.hpp>
#include <viz/scene2d/graphics_view.hpp>

#include <glm/common.hpp>

#include <QGraphicsItemGroup>

#include <cmath>

namespace sight::module::viz::scene2d::adaptor
{

axis::axis() noexcept =
    default;

//--------------------------------------------------------------------------------------------------

axis::~axis() noexcept =
    default;

//--------------------------------------------------------------------------------------------------

void axis::starting()
{
    m_layer = new QGraphicsItemGroup();

    this->build_axis();
    this->build_labels();

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
    m_layer->setZValue(m_z_value);

    // Add to the scene the unique item which gather the whole set of rectangle graphic items:
    this->get_scene_2d_render()->get_scene()->addItem(m_layer);

    this->updating();
}

//--------------------------------------------------------------------------------------------------

void axis::stopping()
{
    delete m_layer;
}

//--------------------------------------------------------------------------------------------------

void axis::configuring()
{
    this->configure_params(); // Looks for 'xAxis', 'yAxis' and 'zValue'

    const config_t srv_config = this->get_config();
    const config_t config     = srv_config.get_child("config.<xmlattr>");

    // 'color'
    if(config.count("color") != 0U)
    {
        sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_line.color, config.get<std::string>("color"));
    }

    // 'align' attribute configuration
    m_align = config.get<std::string>("align");
    SIGHT_ASSERT(
        "'align' attribute is missing. Please add an 'align' attribute "
        "with value 'left', 'right', 'top' or 'bottom'",
        !m_align.empty()
    );
    SIGHT_ASSERT(
        "Unsupported value for 'align' attribute.",
        m_align == "left" || m_align == "right" || m_align == "top" || m_align == "bottom"
    );

    // axis bounds
    SIGHT_ASSERT("'min' attribute is missing.", config.count("min"));
    SIGHT_ASSERT("'max' attribute is missing.", config.count("max"));
    m_min = config.get<double>("min");
    m_max = config.get<double>("max");
    SIGHT_FATAL_IF("'max' attribute should be greater than 'min'.", m_max <= m_min);

    // Ticks size
    m_line.tick_size = config.get<double>("tickSize", m_line.tick_size);

    // Step
    m_interval = config.get<double>("interval", m_interval);

    const auto label_config = srv_config.get_child_optional("config.labels.<xmlattr>");
    if(label_config.has_value())
    {
        // Font size configuration
        m_labels.font_size = label_config->get<int>("fontSize", m_labels.font_size);

        // Show unit
        m_labels.show_unit = label_config->get<bool>("showUnit", m_labels.show_unit);

        // Unit text configuration
        m_labels.displayed_unit = label_config->get<std::string>("unit", "");

        // Color configuration
        if(label_config->count("color") != 0U)
        {
            sight::viz::scene2d::data::init_qt_pen::set_pen_color(
                m_labels.pen,
                label_config->get<std::string>("color"),
                m_opacity
            );
        }
        else
        {
            sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_labels.pen, "white", m_opacity);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void axis::build_axis()
{
    m_line.color.setCosmetic(true);
    const int nb_values = static_cast<int>(std::ceil((m_max - m_min) / m_interval)) + 1;

    for(int i = 0 ; i < nb_values ; ++i)
    {
        auto* tick = new QGraphicsLineItem(0, 0, 0, 0);
        tick->setPen(m_line.color);

        m_line.ticks.push_back(tick);
        m_layer->addToGroup(tick);
    }

    m_line.line = new QGraphicsLineItem();
    m_line.line->setPen(m_line.color);
    m_layer->addToGroup(m_line.line);
}

//---------------------------------------------------------------------------------------------------------------

void axis::build_labels()
{
    SIGHT_ASSERT("m_interval can not be equal to 0", m_interval != 0);

    m_labels.brush = QBrush(m_labels.pen.color());
    m_labels.pen.setCosmetic(true);

    // We may not have a viewport with the same range magnitude on both axis, thus specifying the font size with only
    // 1 dimension does not work. The font would be deformed.
    // Since we can't specify a width and height for the font size, we are forced to use a QTransform to scale the font
    // and keep a correct display ratio.
    // We would like to set the point size to 1.f and scale the text later with a QTransform according to m_"fontSize",
    // but under Windows, rendering a font with a tiny size like 1,2, etc... gives incorrect results in terms of
    // letter spacing and weight. Thus we "pretend" to use a font of a more common size like 10 and take that extra
    // scale into account in our QTransform (in updateLabels())
    m_labels.font.setPixelSize(static_cast<int>(m_labels.extra_scale));
    m_labels.font.setKerning(true);
    m_labels.font.setFixedPitch(true);

    m_labels.values.clear();

    double val          = this->get_start_val();
    const int range     = static_cast<int>(std::ceil(this->get_end_val() - val));
    const int nb_values = static_cast<int>((std::ceil(range / m_interval))) + 1;

    std::string format;

    if(m_align == "left")
    {
        format = "  %1";
    }
    else if(m_align == "right")
    {
        format = "%1  ";
    }
    else
    {
        format = "%1";
    }

    // Build the values as graphic items
    for(int i = 0 ; i < nb_values ; ++i)
    {
        auto* text = new QGraphicsSimpleTextItem();
        text->setText(QString::fromStdString(format).arg(val));
        text->setFont(m_labels.font);
        text->setBrush(m_labels.brush);

        m_labels.values.push_back(text);

        val += m_interval;
    }

    // Add the values to the item group
    for(QGraphicsItem* item : m_labels.values)
    {
        m_layer->addToGroup(item);
    }

    // Build the unit graphic item
    m_labels.unit = new QGraphicsSimpleTextItem();
    m_labels.unit->setText(QString::fromStdString(m_labels.displayed_unit));
    m_labels.unit->setFont(m_labels.font);
    m_labels.unit->setBrush(m_labels.brush);

    // Add it to the items to be displayed if required
    if(m_labels.show_unit)
    {
        m_layer->addToGroup(m_labels.unit);
    }
}

//---------------------------------------------------------------------------------------

double axis::get_start_val() const
{
    return std::floor(m_min / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------

double axis::get_end_val() const
{
    return std::ceil(m_max / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------------------------------

void axis::updating()
{
    update_axis();
    update_labels();
}

//---------------------------------------------------------------------------------------

void axis::update_axis()
{
    const auto viewport         = m_viewport.lock();
    const auto view_to_viewport = this->view_to_viewport(*viewport);

    const std::size_t nb_values = m_line.ticks.size();

    const double min = this->get_start_val();
    const double max = this->get_end_val();

    using sight::viz::scene2d::vec2d_t;

    double pos = NAN;
    vec2d_t tick_size;
    vec2d_t tick_pos {0., 0.};

    if(m_align == "bottom")
    {
        tick_size = this->map_adaptor_to_scene((vec2d_t(0, m_line.tick_size)));

        const double tick_pos_y = 0.;

        for(std::size_t i = 0 ; i < nb_values ; ++i)
        {
            pos      = min + static_cast<double>(i) * m_interval;
            tick_pos = this->map_adaptor_to_scene((vec2d_t(pos, tick_pos_y)));
            m_line.ticks.at(i)->setLine(
                tick_pos.x,
                tick_pos.y,
                tick_pos.x,
                tick_pos.y - tick_size.y * view_to_viewport.y
            );
        }

        m_line.line->setLine(min, tick_pos.y, max, tick_pos.y);
    }
    else if(m_align == "top")
    {
        tick_size = this->map_adaptor_to_scene((vec2d_t(0, m_line.tick_size)));

        const double tick_pos_y = 1.0;

        for(std::size_t i = 0 ; i < nb_values ; ++i)
        {
            pos      = min + static_cast<double>(i) * m_interval;
            tick_pos = this->map_adaptor_to_scene((vec2d_t(pos, tick_pos_y)));
            m_line.ticks.at(i)->setLine(
                tick_pos.x,
                tick_pos.y,
                tick_pos.x,
                tick_pos.y - tick_size.y * view_to_viewport.y
            );
        }

        m_line.line->setLine(min, tick_pos.y, max, tick_pos.y);
    }
    else if(m_align == "left")
    {
        tick_size = this->map_adaptor_to_scene((vec2d_t(m_line.tick_size, 0)));

        const double tick_pos_x = viewport->x();

        for(std::size_t i = 0 ; i < nb_values ; ++i)
        {
            pos      = min + static_cast<double>(i) * m_interval;
            tick_pos = this->map_adaptor_to_scene((vec2d_t(tick_pos_x, pos)));
            m_line.ticks.at(i)->setLine(
                tick_pos.x,
                tick_pos.y,
                tick_pos.x + tick_size.x * view_to_viewport.x,
                tick_pos.y
            );
        }

        m_line.line->setLine(tick_pos.x, min, tick_pos.x, tick_pos.y);
    }
    else if(m_align == "right")
    {
        tick_size = this->map_adaptor_to_scene((vec2d_t(m_line.tick_size, 0)));

        const double tick_pos_x = viewport->x() + viewport->width();

        for(std::size_t i = 0 ; i < nb_values ; ++i)
        {
            pos = min + static_cast<double>(i) * m_interval;

            tick_pos = this->map_adaptor_to_scene((vec2d_t(tick_pos_x, pos)));
            m_line.ticks.at(i)->setLine(
                tick_pos.x - tick_size.x * view_to_viewport.x,
                tick_pos.y,
                tick_pos.x,
                tick_pos.y
            );
        }

        m_line.line->setLine(tick_pos.x, min, tick_pos.x, tick_pos.y);
    }
}

//---------------------------------------------------------------------------------------

void axis::update_labels()
{
    using sight::viz::scene2d::vec2d_t;

    const auto viewport = m_viewport.lock();

    const auto view_to_viewport = this->view_to_viewport(*viewport);
    const auto size_vp          =
        vec2d_t(m_labels.font_size, m_labels.font_size) * view_to_viewport / m_labels.extra_scale;
    const vec2d_t interval_sc = glm::abs(this->map_adaptor_to_scene(vec2d_t(m_interval, m_interval)));

    QTransform transform;
    transform.scale(size_vp.x, size_vp.y);

    const std::size_t values_size = m_labels.values.size();
    double val                    = get_start_val();

    const double viewport_x      = viewport->x();
    const double viewport_width  = viewport->width();
    const double viewport_height = viewport->height();

    if(m_align == "left" || m_align == "right")
    {
        double coeff = 0.F;

        double text_pos_x = NAN;

        if(m_align == "left")
        {
            text_pos_x = viewport_x;
        }
        else
        {
            coeff      = -1.F;
            text_pos_x = viewport_x + viewport_width;
        }

        for(std::size_t i = 0 ; i < values_size ; ++i)
        {
            const vec2d_t coord = this->map_adaptor_to_scene((vec2d_t(text_pos_x, val)));

            const vec2d_t size =
                this->map_adaptor_to_scene(
                    vec2d_t(
                        m_labels.values[i]->boundingRect().width(),
                        m_labels.values[i]->boundingRect().height()
                    )
                );

            m_labels.values[i]->setTransform(transform);
            m_labels.values[i]->setPos(coord.x + coeff * size.x * size_vp.x, coord.y);

            val += m_interval;
        }

        // Always displays the maximum value but we may hide other values depending on available size
        double prev_available_size = 0.;
        for(std::size_t i = 1 ; i < values_size ; ++i)
        {
            const std::size_t idx = values_size - i - 1;
            const vec2d_t size    =
                vec2d_t(
                    m_labels.values[idx]->boundingRect().width(),
                    m_labels.values[idx]->boundingRect().height()
                );

            const double margin = 0.8;
            const auto size_y   = std::abs(size.y * margin * size_vp.y);

            if(size_y < interval_sc.y + prev_available_size)
            {
                m_labels.values[idx]->setVisible(true);
                prev_available_size = 0.;
            }
            else
            {
                m_labels.values[idx]->setVisible(false);
                prev_available_size += interval_sc.y;
            }
        }

        m_labels.unit->setTransform(transform);

        val = viewport_height * 0.8F;

        const vec2d_t size =
            this->map_adaptor_to_scene(
                vec2d_t(
                    m_labels.unit->boundingRect().width(),
                    m_labels.unit->boundingRect().height()
                )
            );

        const vec2d_t coord = this->map_adaptor_to_scene((vec2d_t(text_pos_x, val)));
        coeff = (m_align == "left") ? 1 : -1.5;
        m_labels.unit->setPos(coord.x + coeff * 2 * size.x * size_vp.x, coord.y);
    }
    else // axis centered on top or bottom
    {
        float coeff = 0.5F;

        const double text_pos_y = (m_align == "bottom") ? 0 : 1.1;

        for(std::size_t i = 0 ; i < values_size ; ++i)
        {
            const vec2d_t coord = this->map_adaptor_to_scene(vec2d_t(val, text_pos_y));

            const vec2d_t size =
                this->map_adaptor_to_scene(
                    vec2d_t(
                        m_labels.values[i]->boundingRect().width(),
                        m_labels.values[i]->boundingRect().height()
                    )
                );

            m_labels.values[i]->setTransform(transform);
            m_labels.values[i]->setPos(coord.x - size.x / 2 * size_vp.x, coord.y - coeff * size.y / 2 * size_vp.y);

            val += m_interval;
        }

        // Always displays the maximum value but we may hide other values depending on available size
        double prev_available_size = 0.;
        for(std::size_t i = 1 ; i < values_size ; ++i)
        {
            const vec2d_t size =
                vec2d_t(
                    m_labels.values[i]->boundingRect().width(),
                    m_labels.values[i]->boundingRect().height()
                );

            const double margin = 1.25;
            const auto size_x   = size.x * margin * size_vp.x;
            if(size_x < interval_sc.x + prev_available_size)
            {
                m_labels.values[i]->setVisible(true);
                prev_available_size = 0.;
            }
            else
            {
                m_labels.values[i]->setVisible(false);
                prev_available_size += interval_sc.x;
            }
        }

        m_labels.unit->setTransform(transform);

        const vec2d_t size =
            this->map_adaptor_to_scene(
                vec2d_t(
                    m_labels.unit->boundingRect().width(),
                    m_labels.unit->boundingRect().height()
                )
            );
        const vec2d_t coord = this->map_adaptor_to_scene(vec2d_t(viewport_x + viewport_width * .5, text_pos_y));
        coeff = (m_align == "top") ? 1 : -1.5;

        m_labels.unit->setPos(coord.x - size.x * size_vp.x, coord.y + coeff * size.y * size_vp.y);
    }
}

//---------------------------------------------------------------------------------------

void axis::process_interaction(sight::viz::scene2d::data::event& _event)
{
    if(_event.type() == sight::viz::scene2d::data::event::resize)
    {
        this->updating();
    }
}

//----------------------------------------------------------------------------------------------------------

service::connections_t axis::auto_connections() const
{
    connections_t connections;
    connections.push(VIEWPORT_INPUT, sight::viz::scene2d::data::viewport::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//--------------------------------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
