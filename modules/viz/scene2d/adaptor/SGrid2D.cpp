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

#include "modules/viz/scene2d/adaptor/SGrid2D.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsItemGroup>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

//---------------------------------------------------------------------------------------------------------------

const core::com::slots::key_t SGrid2D::SET_GRID_SPACING_SLOT = "setGridSpacing";

//---------------------------------------------------------------------------------------------------------------

SGrid2D::SGrid2D() noexcept
{
    new_slot(SET_GRID_SPACING_SLOT, &sight::module::viz::scene2d::adaptor::SGrid2D::setGridSpacing, this);
}

//---------------------------------------------------------------------------------------------------------------

SGrid2D::~SGrid2D() noexcept =
    default;

//---------------------------------------------------------------------------------------------------------------

void SGrid2D::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration().get_child("config.<xmlattr>");

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

void SGrid2D::draw()
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
    const float xStartVal = getXStartVal(); // Allows to start drawing the grid from 0 with the correct step
    const float xEndVal   = getXEndVal();   // Allows to start drawing the grid from 0 with the correct step

    // Calculate the start, end and step on y for the lines
    const float yStartVal = getYStartVal(); // Allows to start drawing the grid from 0 with the correct step
    const float yEndVal   = getYEndVal();   // Allows to start drawing the grid from 0 with the correct step

    // Holds the current computed coordinates:
    vec2d_t coord1;
    vec2d_t coord2;

    // Draw the horizontal lines
    float yVal = yStartVal;
    for(std::size_t i = 0 ; i < static_cast<std::size_t>((yEndVal - yStartVal) / m_ySpacing) ; i++)
    {
        coord1 = this->mapAdaptorToScene((vec2d_t(xStartVal, yVal)));
        coord2 = this->mapAdaptorToScene((vec2d_t(xEndVal, yVal)));

        auto* line = new QGraphicsLineItem(coord1.x, coord1.y, coord2.x, coord2.y);

        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);

        yVal += m_ySpacing;
    }

    // Draw the vertical lines
    float xVal = xStartVal;
    for(std::size_t i = 0 ; i < static_cast<std::size_t>((xEndVal - xStartVal) / m_xSpacing) ; i++)
    {
        coord1 = this->mapAdaptorToScene((vec2d_t(xVal, yStartVal)));
        coord2 = this->mapAdaptorToScene((vec2d_t(xVal, yEndVal)));

        auto* line = new QGraphicsLineItem(coord1.x, coord1.y, coord2.x, coord2.y);

        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);

        xVal += m_xSpacing;
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

void SGrid2D::starting()
{
    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    // Set the pen a style
    m_pen.setStyle(Qt::DashLine);
    m_pen.setCosmetic(true);

    this->updating();
}

//---------------------------------------------------------------------------------------------------------------

float SGrid2D::getXStartVal() const
{
    return std::floor(m_xMin / m_xSpacing) * m_xSpacing;
}

//---------------------------------------------------------------------------------------------------------------

float SGrid2D::getXEndVal() const
{
    return std::floor(m_xMax / m_xSpacing) * m_xSpacing;
}

//---------------------------------------------------------------------------------------------------------------

float SGrid2D::getYStartVal() const
{
    return std::floor(m_yMin / m_ySpacing) * m_ySpacing;
}

//---------------------------------------------------------------------------------------------------------------

float SGrid2D::getYEndVal() const
{
    return std::floor(m_yMax / m_ySpacing) * m_ySpacing;
}

//---------------------------------------------------------------------------------------------------------------

void SGrid2D::setGridSpacing(double _x, double _y, std::string _key)
{
    if(_key == "spacing")
    {
        m_xSpacing = static_cast<float>(_x);
        m_ySpacing = static_cast<float>(_y);
        this->draw();
    }
}

//---------------------------------------------------------------------------------------------------------------

void SGrid2D::updating()
{
    this->draw();
}

//---------------------------------------------------------------------------------------------------------------

void SGrid2D::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(_event.getType() == sight::viz::scene2d::data::Event::Resize)
    {
        this->updating();
    }
}

//---------------------------------------------------------------------------------------------------------------

void SGrid2D::stopping()
{
    // Clear the lines vector
    m_lines.clear();

    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace sight::module::viz::scene2d::adaptor
