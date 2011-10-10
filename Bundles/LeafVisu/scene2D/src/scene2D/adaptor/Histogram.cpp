/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/Histogram.hpp>
#include <fwData/Composite.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

#include "scene2D/adaptor/Histogram.hpp"
#include "scene2D/data/InitQtPen.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::Histogram, ::fwData::Histogram);

namespace scene2D
{

namespace adaptor
{

const float Histogram::SCALE = 1.1f; // vertical scaling factor applied at each mouse scroll

//---------------------------------------------------------------------------------------------------------

Histogram::Histogram() throw() : m_color("green"), m_opacity( 0.80f )
{
}

//---------------------------------------------------------------------------------------------------------

Histogram::~Histogram() throw()
{
}

//---------------------------------------------------------------------------------------------------------

void Histogram::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag is missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();  // Looks for 'xAxis', 'yAxis' and 'zValue'

    m_scale = m_yAxis->getScale();

    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_color, m_configuration->getAttributeValue("color"));
    }

    if (!m_configuration->getAttributeValue("opacity").empty())
    {
        m_opacity = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("opacity") );
    }

    m_histogramPointUID = m_configuration->getAttributeValue("histogramPointUID");
    
    OSLM_WARN_IF("If an histogram cursor is used with this histogram, m_histogramPointUID must be set in order to "
            << "inform about the position that the cursor should use.", m_histogramPointUID.empty());
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doStart() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    doUpdate();
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doUpdate() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_layer = new QGraphicsItemGroup();
    ::fwData::Histogram::sptr histogram = this->getObject< ::fwData::Histogram>();

    // Update color with opacity
    QColor color = m_color.color();
    color.setAlphaF( m_opacity );
    m_color.setColor( color );

    const float min = histogram->getMinValue();
    const float binsWidth = histogram->getBinsWidth();
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();

    // Initialize the path with a start point:
    // The value preceding the current value that we'll use to build the arcs of the path
    std::pair< double, double > startPoint = this->mapAdaptorToScene(
                std::pair<double, double>(min, values[0]), m_xAxis, m_yAxis);

    std::pair<double, double> pair;

    QBrush brush = QBrush(m_color.color());

    // Build the graphic items:
    const int nbValues = (int)values.size();
    for(int i = 1; i < nbValues; ++i)
    {
        pair = this->mapAdaptorToScene(
                std::pair<double, double>(min + i * binsWidth, values[i]), m_xAxis, m_yAxis);

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
        /*
        QGraphicsRectItem* rect = new QGraphicsRectItem(startPoint.first, 0, pair.first - startPoint.first, pair.second);
        rect->setBrush( brush );
        rect->setPen( Qt::NoPen );
        m_layer->addToGroup( rect );
        */

        startPoint = pair;
    }

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add to the scene the unique item which gather the whole set of rectangle graphic items:
    this->getScene2DRender()->getScene()->addItem( m_layer );
}

//---------------------------------------------------------------------------------------------------------

void Histogram::updateCurrentPoint( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();
    
    SLM_ASSERT("m_histogramPointUID must be defined in order to update the related ::fwData::Point data.",
            !m_histogramPointUID.empty());
    
    ::fwData::Histogram::sptr histogram = this->getObject< ::fwData::Histogram>();
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    ::scene2D::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( _event->getCoord() );

    const int histIndex = (int) sceneCoord.getX();
    const int index = histIndex - histogramMinValue;
    const int nbValues = (int)values.size() * histogramBinsWidth;

    if(index >= 0 && index < nbValues)
    {
        ::fwData::Point::sptr point =
            ::fwData::Point::dynamicCast( ::fwTools::fwID::getObject( m_histogramPointUID ) );

        SLM_ASSERT("m_histogramPointUID can't be null here.", point);

        point->getRefCoord()[0] = sceneCoord.getX();
        point->getRefCoord()[1] = values.at( index / histogramBinsWidth ) * m_scale;
    }
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doSwap() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------

void Histogram::doStop() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------

void Histogram::processInteraction( ::scene2D::data::Event::sptr _event)
{
    SLM_TRACE_FUNC();

    bool updatePointedPos = false;

    // Vertical scaling
    if( _event->getType() == ::scene2D::data::Event::MouseWheelUp )
    {
        m_scale *= SCALE;
        m_layer->scale(1, SCALE);

        //_event->setAccepted( true );
        m_yAxis->setScale( m_scale );

        updatePointedPos = true;
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseWheelDown )
    {
        m_scale /= SCALE;
        m_layer->scale(1, 1 / SCALE);

        //_event->setAccepted( true );
        m_yAxis->setScale( m_scale );
        
        updatePointedPos = true;
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseMove )
    {
        updatePointedPos = true;
    }
    
    if( !m_histogramPointUID.empty() && updatePointedPos )
    {
        updateCurrentPoint( _event );
    }
}

}   // namespace adaptor

}   // namespace scene2D

