/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/viz/scene2d/adaptor/SHistogramValue.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QFont>
#include <QGraphicsEllipseItem>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

static const std::string s_COLOR_CONFIG     = "color";
static const std::string s_FONT_SIZE_CONFIG = "fontSize";

//---------------------------------------------------------------------------------------------------------------
SHistogramValue::SHistogramValue() noexcept :
    m_color(Qt::white),
    m_text(nullptr),
    m_isInteracting(false),
    m_fontSize(8.f),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

SHistogramValue::~SHistogramValue() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, color);

    m_fontSize = config.get<float>(s_FONT_SIZE_CONFIG, m_fontSize);
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::starting()
{
    m_font.setPointSize(m_fontSize);
    m_font.setLetterSpacing(QFont::AbsoluteSpacing, 0.2);
    m_font.setKerning(true);
    m_font.setFixedPitch(true);

    m_text = new QGraphicsSimpleTextItem();
    m_text->setBrush(QBrush(m_color.color()));
    m_text->setFont(m_font);
    m_text->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    m_text->setVisible(false);

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    m_layer->addToGroup(m_text);

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SHistogramValue::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_HISTOGRAM_INPUT, data::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::updating()
{
    this->initializeViewSize();
    this->initializeViewportSize();

    const auto histogram                            = m_histogram.lock();
    const data::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue                   = histogram->getMinValue();
    const float histogramBinsWidth                  = histogram->getBinsWidth();

    // Event coordinates in scene
    const sight::viz::scene2d::data::Coord sceneCoord = this->getScene2DRender()->mapToScene(m_coord);

    const int histIndex = (int) sceneCoord.getX();
    const int index     = (histIndex - histogramMinValue) / histogramBinsWidth;

    if(index >= 0 && index < (int) values.size() && m_isInteracting) // avoid std out_of_range on Windows
    {
        sight::viz::scene2d::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
        const double viewportHeight                        = viewport->getHeight();
        const double viewportWidth                         = viewport->getWidth();

        const double viewportSizeRatio    = viewportHeight / viewportWidth;
        const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

        const Scene2DRatio ratio        = this->getRatio(); // Total ratio
        const double viewportWidthRatio = this->getViewportSizeRatio().first;

        double diameterH = m_fontSize;
        double diameterV = m_fontSize * viewportSizeRatio;

        diameterV /= viewportWidthRatio;
        diameterV *= viewInitialSizeRatio;

        diameterH *= ratio.first;
        diameterV *= ratio.second;

        m_text->setText(QString::number(histIndex));

        double scaleX = m_fontSize;
        double scaleY = m_fontSize * viewportSizeRatio;

        scaleY /= viewportWidthRatio;
        scaleY *= viewInitialSizeRatio;

        scaleX = scaleX * ratio.first;
        scaleY = scaleY * ratio.second;

        QTransform transform;
        transform.scale(scaleX, scaleY);

        const auto point = m_point.lock();

        m_text->setTransform(transform);
        m_text->setPos(point->getCoord()[0] + diameterH * 2, point->getCoord()[1] - diameterV * 2);
        m_text->setVisible(true);
    }
    else
    {
        m_text->setVisible(false);
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    this->initializeViewSize();
    this->initializeViewportSize();

    if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
    {
        m_coord = _event.getCoord();
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        m_isInteracting = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
    {
        m_isInteracting = false;
    }

    updating();
}

//---------------------------------------------------------------------------------------------------------------

} // namespace adaptor

} // namespace sight::module::viz::scene2d
