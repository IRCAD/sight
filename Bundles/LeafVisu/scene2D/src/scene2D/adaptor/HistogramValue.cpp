/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

#include <QGraphicsEllipseItem>
#include <QFont>

#include "scene2D/Scene2DGraphicsView.hpp"
#include "scene2D/adaptor/HistogramValue.hpp"
#include "scene2D/data/InitQtPen.hpp"
#include "scene2D/data/ViewportMsg.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::HistogramValue, ::fwData::Histogram);

namespace scene2D
{
namespace adaptor
{

HistogramValue::HistogramValue() throw()
: m_color("white"), m_isInteracting(false), m_fontSize(8)
{
    addNewHandledEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED);
}

//---------------------------------------------------------------------------------------------------------------

HistogramValue::~HistogramValue() throw()
{
}

//---------------------------------------------------------------------------------------------------------------

void HistogramValue::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->IAdaptor::configuring();

    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_color, m_configuration->getAttributeValue("color"));
    }

    if (!m_configuration->getAttributeValue("fontSize").empty())
    {
        m_fontSize = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("fontSize") );
    }

    SLM_ASSERT("A viewport attribute must be specified with 'viewportUID'.",
            !m_configuration->getAttributeValue("viewportUID").empty());

    if( !m_configuration->getAttributeValue("viewportUID").empty() )
    {
        m_viewport = ::scene2D::data::Viewport::dynamicCast(
                ::fwTools::fwID::getObject( m_configuration->getAttributeValue("viewportUID") ) );
        m_comChannel = ::fwServices::registerCommunicationChannel( m_viewport , this->getSptr());
        m_comChannel->start();
    }

    SLM_ASSERT("'histogramPointUID' attribute is missing.", !m_configuration->getAttributeValue("histogramPointUID").empty());
    m_histogramPointUID = m_configuration->getAttributeValue("histogramPointUID");
}

//---------------------------------------------------------------------------------------------------------------

void HistogramValue::doStart() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_font.setPointSize(m_fontSize);
    m_font.setLetterSpacing( QFont::AbsoluteSpacing, 0.2 );
    m_font.setKerning( true );
    m_font.setFixedPitch( true );

    m_text = new QGraphicsSimpleTextItem(0, this->getScene2DRender()->getScene());
    m_text->setBrush( QBrush(m_color.color()) );
    m_text->setFont( m_font );
    m_text->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    m_text->setVisible( false );

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    m_layer->addToGroup( m_text );

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void HistogramValue::doStop() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_comChannel->stop();
    ::fwServices::registry::ObjectService::unregisterService( m_comChannel );
}

//---------------------------------------------------------------------------------------------------------------

void HistogramValue::doUpdate() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Histogram::sptr histogram = this->getObject< ::fwData::Histogram>();
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    ::scene2D::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( m_coord );

    int histIndex = (int) sceneCoord.getX();
    int index = (histIndex - histogramMinValue) / histogramBinsWidth;

    if(index >= 0 && index < (int)values.size() && m_isInteracting) // avoid std out_of_range on Windows
    {
        ::scene2D::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
        const double viewportHeight = viewport->getHeight();
        const double viewportWidth = viewport->getWidth();

        const double viewportSizeRatio = viewportHeight / viewportWidth;
        const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

        Scene2DRatio ratio = this->getRatio();  // Total ratio
        double viewportWidthRatio = this->getViewportSizeRatio().first;

        double diameterH  = m_fontSize;
        double diameterV  = m_fontSize * viewportSizeRatio;

        diameterV /= viewportWidthRatio;
        diameterV *= viewInitialSizeRatio;

        diameterH *= ratio.first;
        diameterV *= ratio.second;

        m_text->setText( QString::number( histIndex ) );

        double scaleX = m_fontSize;
        double scaleY = m_fontSize * viewportSizeRatio;

        scaleY /= viewportWidthRatio;
        scaleY *= viewInitialSizeRatio;

        scaleX = scaleX * ratio.first;
        scaleY = scaleY * ratio.second;

        QTransform transform;
        transform.scale(scaleX, scaleY);

        ::fwData::Point::sptr point = ::fwData::Point::dynamicCast( ::fwTools::fwID::getObject( m_histogramPointUID ) );

        m_text->setTransform( transform );
        m_text->setPos( point->getRefCoord()[0] + diameterH * 2, point->getRefCoord()[1] - diameterV * 2 );
        m_text->setVisible( true );
    }
    else
    {
        m_text->setVisible( false );
    }
}
//---------------------------------------------------------------------------------------------------------------

void HistogramValue::doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( _msg->hasEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED) )
    {
        this->initializeViewSize();
        this->initializeViewportSize();

        doUpdate();
    }
}

//---------------------------------------------------------------------------------------------------------------

void HistogramValue::doSwap() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void HistogramValue::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    this->initializeViewSize();
    this->initializeViewportSize();

    if( _event->getType() == ::scene2D::data::Event::MouseMove )
    {
        m_coord = _event->getCoord();
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseButtonPress )
    {
        m_isInteracting = true;
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
    {
        m_isInteracting = false;
    }

    doUpdate();
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D
