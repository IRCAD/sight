/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "modules/viz/scene2d/adaptor/grid2d.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsItemGroup>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

//---------------------------------------------------------------------------------------------------------------

const core::com::slots::key_t grid2d::SET_GRID_SPACING_SLOT = "setGridSpacing";

//---------------------------------------------------------------------------------------------------------------

grid2d::grid2d() noexcept
{
    new_slot(SET_GRID_SPACING_SLOT, &sight::module::viz::scene2d::adaptor::grid2d::setGridSpacing, this);
}

//---------------------------------------------------------------------------------------------------------------

grid2d::~grid2d() noexcept =
    default;

//---------------------------------------------------------------------------------------------------------------

void grid2d::configuring()
{
    this->configureParams();

    const config_t config = this->get_config().get_child("config.<xmlattr>");

    SIGHT_ASSERT("Attribute 'xMin' is missing", config.count("xMin"));
    SIGHT_ASSERT("Attribute 'xMax' is missing", config.count("xMax"));
    SIGHT_ASSERT("Attribute 'yMin' is missing", config.count("yMin"));
    SIGHT_ASSERT("Attribute 'yMax' is missing", config.count("yMax"));

    // Set the x/y min/max values
    m_xMin = config.get<float>("xMin");
    m_xMax = config.get<float>("xMax");
    m_yMin = config.get<float>("yMin");
    m_yMax = config.get<float>("yMax");

    // If the corresponding attributes are present in the config, set the xSpacing, ySpacing between
    // the lines and color of the lines:
    if(config.count("xSpacing") != 0U)
    {
        m_xSpacing = config.get<float>("xSpacing");
    }

    if(config.count("ySpacing") != 0U)
    {
        m_ySpacing = config.get<float>("ySpacing");
    }

    if(config.count("color") != 0U)
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_pen, config.get<std::string>("color"), m_opacity);
    }
}

//---------------------------------------------------------------------------------------------------------------

void grid2d::draw()
{
    SIGHT_ASSERT("m_xSpacing can not be equal to 0", m_xSpacing != 0.F);
    SIGHT_ASSERT("m_ySpacing can not be equal to 0", m_ySpacing != 0.F);

    // Remove all lines from the scene
    for(const auto& line : m_lines)
    {
        this->getScene2DRender()->getScene()->removeItem(line);
    }

    // Clear the lines vector
    m_lines.clear();

    this->getScene2DRender()->getScene()->removeItem(m_layer);
    m_layer = new QGraphicsItemGroup();

    // Calculate the start, end and step on x for the lines
    const float x_start_val = getXStartVal(); // Allows to start drawing the grid from 0 with the correct step
    const float x_end_val   = getXEndVal();   // Allows to start drawing the grid from 0 with the correct step

    // Calculate the start, end and step on y for the lines
    const float y_start_val = getYStartVal(); // Allows to start drawing the grid from 0 with the correct step
    const float y_end_val   = getYEndVal();   // Allows to start drawing the grid from 0 with the correct step

    // Holds the current computed coordinates:
    vec2d_t coord1;
    vec2d_t coord2;

    // Draw the horizontal lines
    float y_val = y_start_val;
    for(std::size_t i = 0 ; i < static_cast<std::size_t>((y_end_val - y_start_val) / m_ySpacing) ; i++)
    {
        coord1 = this->mapAdaptorToScene((vec2d_t(x_start_val, y_val)));
        coord2 = this->mapAdaptorToScene((vec2d_t(x_end_val, y_val)));

        auto* line = new QGraphicsLineItem(coord1.x, coord1.y, coord2.x, coord2.y);

        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);

        y_val += m_ySpacing;
    }

    // Draw the vertical lines
    float x_val = x_start_val;
    for(std::size_t i = 0 ; i < static_cast<std::size_t>((x_end_val - x_start_val) / m_xSpacing) ; i++)
    {
        coord1 = this->mapAdaptorToScene((vec2d_t(x_val, y_start_val)));
        coord2 = this->mapAdaptorToScene((vec2d_t(x_val, y_end_val)));

        auto* line = new QGraphicsLineItem(coord1.x, coord1.y, coord2.x, coord2.y);

        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);

        x_val += m_xSpacing;
    }

    // Add the lines contained in the lines vector to the layer
    for(auto& m_line : m_lines)
    {
        m_layer->addToGroup(m_line);
    }

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void grid2d::starting()
{
    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    // Set the pen a style
    m_pen.setStyle(Qt::DashLine);
    m_pen.setCosmetic(true);

    this->updating();
}

//---------------------------------------------------------------------------------------------------------------

float grid2d::getXStartVal() const
{
    return std::floor(m_xMin / m_xSpacing) * m_xSpacing;
}

//---------------------------------------------------------------------------------------------------------------

float grid2d::getXEndVal() const
{
    return std::floor(m_xMax / m_xSpacing) * m_xSpacing;
}

//---------------------------------------------------------------------------------------------------------------

float grid2d::getYStartVal() const
{
    return std::floor(m_yMin / m_ySpacing) * m_ySpacing;
}

//---------------------------------------------------------------------------------------------------------------

float grid2d::getYEndVal() const
{
    return std::floor(m_yMax / m_ySpacing) * m_ySpacing;
}

//---------------------------------------------------------------------------------------------------------------

void grid2d::setGridSpacing(double _x, double _y, std::string _key)
{
    if(_key == "spacing")
    {
        m_xSpacing = static_cast<float>(_x);
        m_ySpacing = static_cast<float>(_y);
        this->draw();
    }
}

//---------------------------------------------------------------------------------------------------------------

void grid2d::updating()
{
    this->draw();
}

//---------------------------------------------------------------------------------------------------------------

void grid2d::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(_event.getType() == sight::viz::scene2d::data::Event::Resize)
    {
        this->updating();
    }
}

//---------------------------------------------------------------------------------------------------------------

void grid2d::stopping()
{
    // Clear the lines vector
    m_lines.clear();

    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace sight::module::viz::scene2d::adaptor
