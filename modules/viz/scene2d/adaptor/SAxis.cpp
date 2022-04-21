/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/viz/scene2d/adaptor/SAxis.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsItemGroup>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

SAxis::SAxis() noexcept :
    m_interval(1.f),
    m_min(0.f),
    m_max(0.f),
    m_tickSize(1.0f),
    m_color(Qt::white),
    m_showLine(true),
    m_layer(nullptr),
    m_line(nullptr)
{
}

//--------------------------------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
}

//--------------------------------------------------------------------------------------------------

void SAxis::starting()
{
    this->buildAxis();
    this->updating();
}

//--------------------------------------------------------------------------------------------------

void SAxis::stopping()
{
    delete m_layer;
}

//--------------------------------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams(); // Looks for 'xAxis', 'yAxis' and 'zValue'

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    // 'color'
    if(config.count("color"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
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

    // SAxis bounds
    SIGHT_ASSERT("'min' attribute is missing.", config.count("min"));
    SIGHT_ASSERT("'max' attribute is missing.", config.count("max"));
    m_min = config.get<float>("min");
    m_max = config.get<float>("max");

    // Ticks size
    m_tickSize = config.get<float>("tickSize", 1.0f);

    // Step
    m_interval = config.get<float>("interval", 1.0f);
}

//---------------------------------------------------------------------------------------------------------------

void SAxis::buildAxis()
{
    m_color.setCosmetic(true);
    const int nbValues = static_cast<int>(std::ceil((m_max - m_min) / m_interval)) + 1;

    m_layer = new QGraphicsItemGroup();

    for(int i = 0 ; i < nbValues ; ++i)
    {
        QGraphicsLineItem* tick = new QGraphicsLineItem(0, 0, 0, 0);
        tick->setPen(m_color);

        m_ticks.push_back(tick);
        m_layer->addToGroup(tick);
    }

    m_line = new QGraphicsLineItem();
    m_line->setPen(m_color);

    if(m_showLine)
    {
        m_layer->addToGroup(m_line);
    }

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add to the scene the unique item which gather the whole set of rectangle graphic items:
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------

double SAxis::getStartVal() const
{
    return std::floor(m_min / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------

double SAxis::getEndVal() const
{
    return std::ceil(m_max / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------------------------------

void SAxis::updating()
{
    const auto viewport = m_viewport.lock();

    auto view = this->getScene2DRender()->getView();

    const double viewportHeight = viewport->getHeight();
    const double viewportWidth  = viewport->getWidth();

    const double viewportViewRatio = viewport->getWidth() / view->width();
    const double viewportSizeRatio = viewportHeight / viewportWidth;

    double scaleX = viewportViewRatio;
    double scaleY = viewportSizeRatio * this->getViewSizeRatio() * viewportViewRatio;

    const std::size_t nbValues = m_ticks.size();

    const double min = this->getStartVal();
    const double max = this->getEndVal();

    double pos;
    Point2DType tickSize;
    Point2DType tickPos;

    if(m_align == "bottom")
    {
        tickSize = this->mapAdaptorToScene((Point2DType(0, m_tickSize)));

        const double tickPosY = viewport->getY();

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos     = min + static_cast<double>(i) * m_interval;
            tickPos = this->mapAdaptorToScene((Point2DType(pos, tickPosY)));
            m_ticks.at(i)->setLine(
                tickPos.first,
                tickPos.second,
                tickPos.first,
                tickPos.second - tickSize.second * scaleY
            );
        }

        m_line->setLine(min, tickPos.second, max, tickPos.second);
    }
    else if(m_align == "top")
    {
        tickSize = this->mapAdaptorToScene((Point2DType(0, m_tickSize)));

        const double tickPosY = viewport->getHeight() * 0.9;

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos     = min + static_cast<double>(i) * m_interval;
            tickPos = this->mapAdaptorToScene((Point2DType(pos, tickPosY)));
            m_ticks.at(i)->setLine(
                tickPos.first,
                tickPos.second,
                tickPos.first,
                tickPos.second - tickSize.second * scaleY
            );
        }

        m_line->setLine(min, tickPos.second, max, tickPos.second);
    }
    else if(m_align == "left")
    {
        tickSize = this->mapAdaptorToScene((Point2DType(m_tickSize, 0)));

        const double tickPosX = viewport->getX();

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos     = min + static_cast<double>(i) * m_interval;
            tickPos = this->mapAdaptorToScene((Point2DType(tickPosX, pos)));
            m_ticks.at(i)->setLine(
                tickPos.first,
                tickPos.second,
                tickPos.first + tickSize.first * scaleX,
                tickPos.second
            );
        }

        m_line->setLine(tickPos.first, min, tickPos.first, tickPos.second);
    }
    else if(m_align == "right")
    {
        tickSize = this->mapAdaptorToScene((Point2DType(m_tickSize, 0)));

        const double tickPosX = viewport->getX() + viewport->getWidth();

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos = min + static_cast<double>(i) * m_interval;

            tickPos = this->mapAdaptorToScene((Point2DType(tickPosX, pos)));
            m_ticks.at(i)->setLine(
                tickPos.first - tickSize.first * scaleX,
                tickPos.second,
                tickPos.first,
                tickPos.second
            );
        }

        m_line->setLine(tickPos.first, min, tickPos.first, tickPos.second);
    }
}

//---------------------------------------------------------------------------------------

void SAxis::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(_event.getType() == sight::viz::scene2d::data::Event::Resize)
    {
        this->updating();
    }
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SAxis::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//--------------------------------------------------------------------------------------------------

} // namespace adaptor

} // namespace sight::module::viz::scene2d
