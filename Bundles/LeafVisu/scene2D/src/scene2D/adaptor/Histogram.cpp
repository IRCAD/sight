/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/Histogram.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::Histogram, ::fwData::Histogram);

namespace scene2D
{

namespace adaptor
{
static const ::fwServices::IService::KeyType s_POINT_INOUT     = "point";
static const ::fwServices::IService::KeyType s_HISTOGRAM_INPUT = "histogram";

const float Histogram::SCALE = 1.1f; // vertical scaling factor applied at each mouse scroll

//---------------------------------------------------------------------------------------------------------

Histogram::Histogram() noexcept :
    m_opacity( 0.80f ),
    m_scale(1.0f),
    m_layer(nullptr),
    m_color(Qt::green)
{
}

//---------------------------------------------------------------------------------------------------------

Histogram::~Histogram() noexcept
{
}

//---------------------------------------------------------------------------------------------------------

void Histogram::configuring()
{
    this->IAdaptor::configuring();  // Looks for 'xAxis', 'yAxis' and 'zValue'

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    m_scale = m_yAxis->getScale();

    if (config.count("color"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    if (config.count("opacity"))
    {
        m_opacity = config.get<float>("opacity");
    }
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doStart()
{
    doUpdate();
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doUpdate()
{
    this->doStop();

    ::fwData::Histogram::csptr histogram          = this->getInput< ::fwData::Histogram>(s_HISTOGRAM_INPUT);
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();

    if (!values.empty())
    {
        m_layer = new QGraphicsItemGroup();
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

void Histogram::updateCurrentPoint(::fwRenderQt::data::Event& _event, const ::fwData::Point::sptr& point)
{
    ::fwData::Histogram::csptr histogram          = this->getInput< ::fwData::Histogram >(s_HISTOGRAM_INPUT);
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue  = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    ::fwRenderQt::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( _event.getCoord() );

    const int histIndex = (int) sceneCoord.getX();
    const int index     = histIndex - histogramMinValue;
    const int nbValues  = (int)values.size() * histogramBinsWidth;

    if(index >= 0 && index < nbValues)
    {
        point->getRefCoord()[0] = sceneCoord.getX();
        point->getRefCoord()[1] = values.at( index / histogramBinsWidth ) * m_scale;
    }
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doSwap()
{
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doStop()
{
    if (m_layer)
    {
        this->getScene2DRender()->getScene()->removeItem(m_layer);
        delete m_layer;
        m_layer = nullptr;
    }
}

//---------------------------------------------------------------------------------------------------------

void Histogram::processInteraction( ::fwRenderQt::data::Event& _event)
{
    bool updatePointedPos = false;

    // Vertical scaling
    if( _event.getType() == ::fwRenderQt::data::Event::MouseWheelUp )
    {
        m_scale *= SCALE;
        m_layer->setTransform(QTransform::fromScale(1, SCALE), true);

        //_event.setAccepted( true );
        m_yAxis->setScale( m_scale );

        updatePointedPos = true;
    }
    else if( _event.getType() == ::fwRenderQt::data::Event::MouseWheelDown )
    {
        m_scale /= SCALE;
        m_layer->setTransform(QTransform::fromScale(1, 1 / SCALE), true);

        //_event.setAccepted( true );
        m_yAxis->setScale( m_scale );

        updatePointedPos = true;
    }
    else if( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
    {
        updatePointedPos = true;
    }

    ::fwData::Point::sptr point = this->getInOut< ::fwData::Point>(s_POINT_INOUT);
    if( point && updatePointedPos )
    {
        this->updateCurrentPoint(_event, point);
    }
}

//----------------------------------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap Histogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_HISTOGRAM_INPUT, ::fwData::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

}   // namespace adaptor

}   // namespace scene2D

