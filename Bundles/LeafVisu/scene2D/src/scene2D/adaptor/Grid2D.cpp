/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Composite.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/adaptor/Grid2D.hpp"
#include "scene2D/data/InitQtPen.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"
#include "scene2D/data/ViewportMsg.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Grid2D  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

Grid2D::Grid2D() throw() : m_xSpacing(10), m_ySpacing(10)
{
    addNewHandledEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED);
}

//---------------------------------------------------------------------------------------------------------------

Grid2D::~Grid2D() throw()
{
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");


    // Set the x/y min/max values
    m_xMin = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("xMin") );
    m_xMax = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("xMax") );
    m_yMin = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("yMin") );
    m_yMax = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("yMax") );


    // If the corresponding attributes are present in the config, set the xSpacing, ySpacing between
    // the lines and color of the lines:
    if (!m_configuration->getAttributeValue("xSpacing").empty())
    {
        m_xSpacing = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("xSpacing") );
    }

    if (!m_configuration->getAttributeValue("ySpacing").empty())
    {
        m_ySpacing = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("ySpacing") );
    }

    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"), m_opacity);
    }
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::draw()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("m_xSpacing can not be equal to 0", m_xSpacing != 0);
    SLM_ASSERT("m_ySpacing can not be equal to 0", m_ySpacing != 0);

    this->getScene2DRender()->getScene()->removeItem( m_layer );
    m_layer = new QGraphicsItemGroup();

    // Calculate the start, end and step on x for the lines
    const float xStartVal = getXStartVal();    // Allows to start drawing the grid from 0 with the correct step
    const float xEndVal = getXEndVal();      // Allows to start drawing the grid from 0 with the correct step

    // Calculate the start, end and step on y for the lines
    const float yStartVal = getYStartVal();    // Allows to start drawing the grid from 0 with the correct step
    const float yEndVal = getYEndVal();        // Allows to start drawing the grid from 0 with the correct step

    // Holds the current computed coordinates:
    std::pair< double, double > coord1, coord2;

    // Draw the horizontal lines
    for ( float yVal = yStartVal ; yVal <= yEndVal ; yVal += m_ySpacing )
    {
        coord1 = this->mapAdaptorToScene( std::pair< double, double >( xStartVal, yVal) , m_xAxis, m_yAxis );
        coord2 = this->mapAdaptorToScene( std::pair< double, double >( xEndVal, yVal) , m_xAxis, m_yAxis );

        QGraphicsLineItem* line = new QGraphicsLineItem(coord1.first, coord1.second, coord2.first, coord2.second);

        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);
    }

    // Draw the vertical lines
    for ( float xVal = xStartVal ; xVal <= xEndVal ; xVal += m_xSpacing )
    {
        coord1 = this->mapAdaptorToScene(std::pair< double, double >( xVal, yStartVal) , m_xAxis, m_yAxis);
        coord2 = this->mapAdaptorToScene(std::pair< double, double >( xVal, yEndVal) , m_xAxis, m_yAxis);

        QGraphicsLineItem* line = new QGraphicsLineItem(coord1.first, coord1.second, coord2.first, coord2.second);

        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);
    }

    // Add the lines contained in the lines vector to the layer
    for ( unsigned int i = 0 ; i < m_lines.size() ; i++)
    {
        m_layer->addToGroup(m_lines.at(i));
    }

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    // Set the pen a style
    m_pen.setStyle(Qt::DashLine);

    this->draw();
    this->doUpdate();
}

//---------------------------------------------------------------------------------------------------------------

float Grid2D::getXStartVal()
{
    return (int)( m_xMin / m_xSpacing ) * m_xSpacing;
}

//---------------------------------------------------------------------------------------------------------------

float Grid2D::getXEndVal()
{
    return (int)( m_xMax / m_xSpacing ) * m_xSpacing;
}

//---------------------------------------------------------------------------------------------------------------

float Grid2D::getYStartVal()
{
    return (int)( m_yMin / m_ySpacing ) * m_ySpacing;
}

//---------------------------------------------------------------------------------------------------------------

float Grid2D::getYEndVal()
{
    return (int)( m_yMax / m_ySpacing ) * m_ySpacing;
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    if( _msg->hasEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED) )
    {
        doUpdate();
    }
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::processInteraction( ::scene2D::data::Event::sptr _event)
{
    if( _event->getType() == ::scene2D::data::Event::Resize)
    {
        doUpdate();
    }
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void Grid2D::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Clear the lines vector
    m_lines.clear();

    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D



