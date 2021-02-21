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

#include "modules/viz/scene2d/adaptor/SHistogram.hpp"

#include <data/Histogram.hpp>
#include <data/Point.hpp>

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

fwServicesRegisterMacro( ::sight::viz::scene2d::IAdaptor, ::sight::module::viz::scene2d::adaptor::SHistogram)

namespace sight::module::viz::scene2d
{

namespace adaptor
{
static const service::IService::KeyType s_POINT_INOUT     = "point";
static const service::IService::KeyType s_HISTOGRAM_INPUT = "histogram";

const float SHistogram::SCALE = 1.1f; // vertical scaling factor applied at each mouse scroll

//---------------------------------------------------------------------------------------------------------

SHistogram::SHistogram() noexcept
{
}

//---------------------------------------------------------------------------------------------------------

SHistogram::~SHistogram() noexcept
{
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::configuring()
{
    this->configureParams();  // Looks for 'xAxis', 'yAxis' and 'zValue'

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_scale = m_yAxis->getScale();

    if (config.count("color"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    if (config.count("opacity"))
    {
        m_opacity = config.get<float>("opacity");
    }
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::starting()
{
    updating();
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::updating()
{
    this->stopping();

    data::Histogram::csptr histogram          = this->getInput< data::Histogram>(s_HISTOGRAM_INPUT);
    data::Histogram::fwHistogramValues values = histogram->getValues();

    m_layer = new QGraphicsItemGroup();

    if (!values.empty())
    {
        // Update color with opacity
        QColor color = m_color.color();
        color.setAlphaF( m_opacity );
        m_color.setColor( color );

        const float min       = histogram->getMinValue();
        const float binsWidth = histogram->getBinsWidth();

        // Initialize the path with a start point:
        // The value preceding the current value that we'll use to build the arcs of the path
        Point2DType startPoint = this->mapAdaptorToScene(Point2DType(min, values[0]), m_xAxis, m_yAxis);

        Point2DType pair;

        QBrush brush = QBrush(m_color.color());

        // Build the graphic items:
        const int nbValues = (int)values.size();
        for(int i = 1; i < nbValues; ++i)
        {
            pair = this->mapAdaptorToScene(Point2DType(min + i * binsWidth, values[i]), m_xAxis, m_yAxis);

            QPainterPath painter( QPointF(startPoint.first, 0) );
            painter.lineTo( startPoint.first, startPoint.second );
            painter.lineTo( pair.first, pair.second );
            painter.lineTo( pair.first, 0 );

            QGraphicsPathItem* item = new QGraphicsPathItem( painter );
            item->setPath( painter );
            item->setBrush( brush );
            item->setPen( Qt::NoPen );
            item->setCacheMode( QGraphicsItem::DeviceCoordinateCache );

            m_layer->addToGroup( item );

            startPoint = pair;
        }

        // Adjust the layer's position and zValue depending on the associated axis
        m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
        m_layer->setZValue(m_zValue);

        // Add to the scene the unique item which gather the whole set of rectangle graphic items:
        this->getScene2DRender()->getScene()->addItem( m_layer );
    }
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::updateCurrentPoint(sight::viz::scene2d::data::Event& _event, const data::Point::sptr& point)
{
    data::Histogram::csptr histogram          = this->getInput< data::Histogram >(s_HISTOGRAM_INPUT);
    data::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue             = histogram->getMinValue();
    const float histogramBinsWidth            = histogram->getBinsWidth();

    // Event coordinates in scene
    sight::viz::scene2d::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( _event.getCoord() );

    const int histIndex = (int) sceneCoord.getX();
    const int index     = histIndex - histogramMinValue;
    const int nbValues  = (int)values.size() * histogramBinsWidth;

    if(index >= 0 && index < nbValues)
    {
        point->getCoord()[0] = sceneCoord.getX();
        point->getCoord()[1] = values.at( index / histogramBinsWidth ) * m_scale;
    }
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::stopping()
{
    if (m_layer)
    {
        this->getScene2DRender()->getScene()->removeItem(m_layer);
        delete m_layer;
        m_layer = nullptr;
    }
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::processInteraction( sight::viz::scene2d::data::Event& _event)
{
    bool updatePointedPos = false;

    // Vertical scaling
    if( _event.getType() == sight::viz::scene2d::data::Event::MouseWheelUp )
    {
        m_scale *= SCALE;
        m_layer->setTransform(QTransform::fromScale(1, SCALE), true);

        _event.setAccepted( true );
        m_yAxis->setScale( m_scale );

        updatePointedPos = true;
    }
    else if( _event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown )
    {
        m_scale /= SCALE;
        m_layer->setTransform(QTransform::fromScale(1, 1 / SCALE), true);

        _event.setAccepted( true );
        m_yAxis->setScale( m_scale );

        updatePointedPos = true;
    }
    else if( _event.getType() == sight::viz::scene2d::data::Event::MouseMove )
    {
        updatePointedPos = true;
    }

    data::Point::sptr point = this->getInOut< data::Point>(s_POINT_INOUT);
    if( point && updatePointedPos )
    {
        this->updateCurrentPoint(_event, point);
    }
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SHistogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_HISTOGRAM_INPUT, data::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

}   // namespace adaptor

}   // namespace sight::module::viz::scene2d
