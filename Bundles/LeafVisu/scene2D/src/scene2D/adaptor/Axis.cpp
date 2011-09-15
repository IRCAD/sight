/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwServices/Base.hpp>

#include <fwData/Composite.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/data/InitQtPen.hpp"
#include "scene2D/data/ViewportMsg.hpp"

#include "scene2D/adaptor/Axis.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::Axis, ::fwData::Composite );

namespace scene2D
{

namespace adaptor
{

Axis::Axis() throw() : m_showLine(true), m_tickSize(0.02), m_color("white")
{
    addNewHandledEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED);
}

//--------------------------------------------------------------------------------------------------

Axis::~Axis() throw()
{

}

//--------------------------------------------------------------------------------------------------

void Axis::doStart() throw( ::fwTools::Failed)
{
    this->buildAxis();
    this->doUpdate();
}

//--------------------------------------------------------------------------------------------------

void Axis::doStop() throw( ::fwTools::Failed)
{
    m_comChannel->stop();
    ::fwServices::registry::ObjectService::unregisterService( m_comChannel );

    delete m_layer;
}

//--------------------------------------------------------------------------------------------------

void Axis::doSwap() throw( ::fwTools::Failed)
{

}

//--------------------------------------------------------------------------------------------------

void Axis::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag is missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();  // Looks for 'xAxis', 'yAxis' and 'zValue'


    // 'color'
    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_color, m_configuration->getAttributeValue("color"));
    }


    // 'align' attribute configuration
    m_align = m_configuration->getAttributeValue("align");
    SLM_ASSERT("'align' attribute is missing. Please add an 'align' attribute with value 'left', 'right', 'top' or 'bottom'",
            !m_align.empty());
    SLM_ASSERT("Unsupported value for 'align' attribute.",
           m_align == "left" || m_align == "right" || m_align == "top" || m_align == "bottom");


    // Axis bounds
    const std::string min = m_configuration->getAttributeValue("min");
    const std::string max = m_configuration->getAttributeValue("max");

    SLM_ASSERT("'min' attribute is missing.", !min.empty());
    SLM_ASSERT("'max' attribute is missing.", !max.empty());

    m_min = ::boost::lexical_cast< float >( min );
    m_max = ::boost::lexical_cast< float >( max );


    // Ticks size
    const std::string tickSize = m_configuration->getAttributeValue("tickSize");
    m_tickSize = ( tickSize.empty() ) ? 1.0 : ::boost::lexical_cast< float >( tickSize );


    // Viewport
    SLM_ASSERT("A viewport attribute must be specified with 'viewportUID'.",
            !m_configuration->getAttributeValue("viewportUID").empty());

    if( !m_configuration->getAttributeValue("viewportUID").empty() )
    {
        m_viewport = ::scene2D::data::Viewport::dynamicCast(
                ::fwTools::fwID::getObject( m_configuration->getAttributeValue("viewportUID") ) );

        m_comChannel = ::fwServices::registerCommunicationChannel( m_viewport , this->getSptr());
        m_comChannel->start();
    }


    // Step
    const std::string interval = m_configuration->getAttributeValue("interval");
    m_interval = ( interval.empty() ) ? 1.0f : ::boost::lexical_cast< float >( interval );
}

//---------------------------------------------------------------------------------------------------------------

void Axis::buildAxis()
{
    const int nbValues = (m_max - m_min) / m_interval + 1;
    m_layer = new QGraphicsItemGroup();

    for(int i = 0; i < nbValues; ++i)
    {
        QGraphicsLineItem* tick = new QGraphicsLineItem(0, 0, 0, 0);
        tick->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
        tick->setPen( m_color );

        m_ticks.push_back( tick );
        m_layer->addToGroup( tick );
    }

    m_line = new QGraphicsLineItem();
    m_line->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    m_line->setPen( m_color );

    if( m_showLine )
    {
        m_layer->addToGroup( m_line );
    }

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );

    // Add to the scene the unique item which gather the whole set of rectangle graphic items:
    this->getScene2DRender()->getScene()->addItem( m_layer );
}

//---------------------------------------------------------------------------------------

double Axis::getStartVal()
{
    return (int)( m_min / m_interval ) * m_interval;
}

//---------------------------------------------------------------------------------------

double Axis::getEndVal()
{
    return (int)( m_max / m_interval ) * m_interval;
}

//---------------------------------------------------------------------------------------------------------------

void Axis::doUpdate() throw( ::fwTools::Failed)
{
    this->initializeViewSize();
    this->initializeViewportSize();

    Scene2DRatio ratio = this->getRatio();

    ::scene2D::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
    const double viewportX = viewport->getX();
    const double viewportHeight = viewport->getHeight();
    const double viewportWidth = viewport->getWidth();

    const double viewportSizeRatio = viewportHeight / viewportWidth;
    const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

    double viewportWidthRatio = this->getViewportSizeRatio().first;

    double scaleX = m_tickSize;
    double scaleY = m_tickSize * viewportSizeRatio;
    scaleY /= viewportWidthRatio;
    scaleY *= viewInitialSizeRatio;

    scaleX = scaleX * ratio.first;
    scaleY = scaleY * ratio.second;

    const int nbValues = m_ticks.size();

    const double min = this->getStartVal();
    const double max = this->getEndVal();

    float pos;
    std::pair<double, double> tickSize;
    std::pair<double, double> tickPos;

    const std::pair<double, double> viewportSize = this->mapAdaptorToScene(
            std::pair<double, double>(viewportWidth, viewportHeight), m_xAxis, m_yAxis);

    if(m_align == "bottom")
    {
        tickSize = this->mapAdaptorToScene(std::pair<double, double>(0, m_tickSize), m_xAxis, m_yAxis);

        const double tickPosY = m_viewport->getY();

        for(int i = 0; i < nbValues; ++i)
        {
            pos = min + i * m_interval;
            tickPos = this->mapAdaptorToScene(std::pair<double, double>(pos, tickPosY), m_xAxis, m_yAxis);
            m_ticks.at(i)->setLine(
                    tickPos.first, tickPos.second,
                    tickPos.first, tickPos.second - tickSize.second * scaleY);
        }

        m_line->setLine(min, tickPos.second, max, tickPos.second);
    }
    else if(m_align == "top")
    {
        tickSize = this->mapAdaptorToScene(std::pair<double, double>(0, m_tickSize), m_xAxis, m_yAxis);

        const double tickPosY = m_viewport->getHeight() * 0.9;

        for(int i = 0; i < nbValues; ++i)
        {
            pos = min + i * m_interval;
            tickPos = this->mapAdaptorToScene(std::pair<double, double>(pos, tickPosY), m_xAxis, m_yAxis);

            m_ticks.at(i)->setLine(
                    tickPos.first, tickPos.second,
                    tickPos.first, tickPos.second - tickSize.second * scaleY);
        }

        m_line->setLine(min, tickPos.second, max, tickPos.second);
    }
    else if(m_align == "left")
    {
        tickSize = this->mapAdaptorToScene(std::pair<double, double>(m_tickSize, 0), m_xAxis, m_yAxis);

        const double tickPosX = m_viewport->getX();

        for(int i = 0; i < nbValues; ++i)
        {
            pos = min + i * m_interval;
            tickPos = this->mapAdaptorToScene(std::pair<double, double>(tickPosX, pos), m_xAxis, m_yAxis);
            m_ticks.at(i)->setLine(
                    tickPos.first, tickPos.second,
                    tickPos.first + tickSize.first * scaleX, tickPos.second);
        }

        m_line->setLine( tickPos.first, min, tickPos.first, tickPos.second);
    }
    else if(m_align == "right")
    {
        tickSize = this->mapAdaptorToScene(
                std::pair<double, double>(m_tickSize, 0), m_xAxis, m_yAxis);

        const double tickPosX = m_viewport->getX() + m_viewport->getWidth();

        for(int i = 0; i < nbValues; ++i)
        {
            pos = min + i * m_interval;

            tickPos = this->mapAdaptorToScene(
                    std::pair<double, double>(tickPosX, pos), m_xAxis, m_yAxis);

            m_ticks.at(i)->setLine(
                  tickPos.first - tickSize.first * scaleX, tickPos.second,
                  tickPos.first, tickPos.second);
        }

        m_line->setLine(tickPos.first, min, tickPos.first, tickPos.second);
    }
}

//--------------------------------------------------------------------------------------------------

void Axis::doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed)
{
    if( _msg->hasEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED) )
    {
        doUpdate();
    }
}

//---------------------------------------------------------------------------------------

void Axis::processInteraction( ::scene2D::data::Event::sptr _event)
{
    if( _event->getType() == ::scene2D::data::Event::Resize)
    {
        doUpdate();
    }
}

//--------------------------------------------------------------------------------------------------

}   // namespace adaptor

}   // namespace scene2D

