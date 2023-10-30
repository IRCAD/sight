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

#include "modules/viz/scene2d/adaptor/line.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/data/init_qt_pen.hpp>

#include <QGraphicsItemGroup>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

line::line() noexcept =
    default;

//---------------------------------------------------------------------------------------------------------------

line::~line() noexcept =
    default;

//---------------------------------------------------------------------------------------------------------------

void line::configuring()
{
    this->configure_params();

    const config_t config = this->get_config().get_child("config.<xmlattr>");

    SIGHT_ASSERT("Attribute 'x1' is missing", config.count("x1"));
    SIGHT_ASSERT("Attribute 'x2' is missing", config.count("x2"));
    SIGHT_ASSERT("Attribute 'y1' is missing", config.count("y1"));
    SIGHT_ASSERT("Attribute 'y2' is missing", config.count("y2"));

    // Set the beginning and ending coordinates values
    m_x1 = config.get<float>("x1");
    m_x2 = config.get<float>("x2");
    m_y1 = config.get<float>("y1");
    m_y2 = config.get<float>("y2");

    // If the corresponding attributes are present in the config, set the color of the line
    if(config.count("color") != 0U)
    {
        sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_pen, config.get<std::string>("color"));
    }
}

//---------------------------------------------------------------------------------------------------------------

void line::draw()
{
    const vec2d_t pt1 = this->map_adaptor_to_scene((vec2d_t(m_x1, m_y1)));
    const vec2d_t pt2 = this->map_adaptor_to_scene((vec2d_t(m_x2, m_y2)));

    // Draw the line
    auto* line = new QGraphicsLineItem(pt1.x, pt1.y, pt2.x, pt2.y);
    // Set the line the pen
    line->setPen(m_pen);

    // Add the line to the layer
    m_layer->addToGroup(line);

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
    m_layer->setZValue(m_z_value);
    // Add the layer to the scene
    this->get_scene_2d_render()->get_scene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void line::starting()
{
    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    m_pen.setCosmetic(true);

    this->draw();
}

//---------------------------------------------------------------------------------------------------------------

void line::updating()
{
    this->draw();
}

//---------------------------------------------------------------------------------------------------------------

void line::stopping()
{
    // Remove the layer (and therefore its related line item) from the scene
    this->get_scene_2d_render()->get_scene()->removeItem(m_layer);
}

} // namespace sight::module::viz::scene2d::adaptor
