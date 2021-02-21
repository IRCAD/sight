/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/viz/scene2d/adaptor/SHistogramCursor.hpp"

#include <data/Histogram.hpp>
#include <data/Point.hpp>

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QFont>
#include <QGraphicsEllipseItem>

fwServicesRegisterMacro( ::sight::viz::scene2d::IAdaptor, ::sight::module::viz::scene2d::adaptor::SHistogramCursor)

namespace sight::module::viz::scene2d
{
namespace adaptor
{

static const service::IService::KeyType s_POINT_INPUT     = "point";
static const service::IService::KeyType s_HISTOGRAM_INPUT = "histogram";
static const service::IService::KeyType s_VIEWPORT_INPUT  = "viewport";

SHistogramCursor::SHistogramCursor() noexcept :
    m_color(Qt::red),
    m_borderColor(Qt::gray),
    m_index(nullptr),
    m_pointSize(6.f),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

SHistogramCursor::~SHistogramCursor() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if (config.count("color"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    if (config.count("borderColor"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_borderColor, config.get<std::string>("borderColor"));
    }

    if (config.count("pointSize"))
    {
        m_pointSize = config.get<float>("pointSize");
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::starting()
{
    m_index = new QGraphicsEllipseItem();
    m_index->setBrush( m_color.color() );
    m_index->setPen( m_borderColor );
    m_index->setZValue(m_zValue);
    m_index->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    m_color.setCosmetic(true);

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    m_layer->addToGroup( m_index );

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::updating()
{
    this->initializeViewSize();
    this->initializeViewportSize();

    data::Histogram::csptr histogram          = this->getInput< data::Histogram>(s_HISTOGRAM_INPUT);
    data::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue             = histogram->getMinValue();
    const float histogramBinsWidth            = histogram->getBinsWidth();

    // Event coordinates in scene
    sight::viz::scene2d::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( m_coord );

    int histIndex = (int) sceneCoord.getX();
    int index     = (histIndex - histogramMinValue) / histogramBinsWidth;

    if(index >= 0 && index < (int)values.size()) // avoid std out_of_range on MS Windows
    {
        sight::viz::scene2d::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
        const double viewportHeight                        = viewport->getHeight();
        const double viewportWidth                         = viewport->getWidth();

        const double viewportSizeRatio    = viewportHeight / viewportWidth;
        const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

        const Scene2DRatio ratio   = this->getRatio(); // Total ratio
        const double viewportRatio = this->getViewportSizeRatio().first;

        double diameterH = m_pointSize;
        double diameterV = m_pointSize * viewportSizeRatio;

        diameterV /= viewportRatio;
        diameterV *= viewInitialSizeRatio;

        // Apply the ratio of the scene 2D in order to keep the same size for the circles if viewport's size or
        // view's size change:
        diameterH *= ratio.first;
        diameterV *= ratio.second;

        data::Point::csptr point = this->getInput< data::Point>(s_POINT_INPUT);

        const double x = point->getCoord()[0] - diameterH / 2;
        const double y = point->getCoord()[1] - diameterV / 2;

        m_index->setRect( x, y, diameterH, diameterV );
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::processInteraction( sight::viz::scene2d::data::Event& _event )
{
    this->initializeViewSize();
    this->initializeViewportSize();

    if( _event.getType() == sight::viz::scene2d::data::Event::MouseMove )
    {
        m_coord = _event.getCoord();
    }

    updating();
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SHistogramCursor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_HISTOGRAM_INPUT, data::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace sight::module::viz::scene2d
