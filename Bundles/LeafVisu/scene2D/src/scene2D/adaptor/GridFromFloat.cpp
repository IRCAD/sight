/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Float.hpp>
#include <fwComEd/FloatMsg.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/adaptor/GridFromFloat.hpp"
#include "scene2D/data/InitQtPen.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::GridFromFloat  , ::fwData::Float ) ;


namespace scene2D
{
namespace adaptor
{

GridFromFloat::GridFromFloat() throw()
: m_xSpacing (10),
  m_ySpacing (10)
{
    addNewHandledEvent( ::fwComEd::FloatMsg::VALUE_IS_MODIFIED );
}

GridFromFloat::~GridFromFloat() throw()
{
}

void GridFromFloat::configuring() throw ( ::fwTools::Failed )
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
   
    // If the corresponding attributes are present in the config, set the xSpacing, ySpacing between the lines and color of the lines
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
        ::scene2D::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"));
    }
}

void GridFromFloat::draw()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("m_xSpacing can not equal 0", m_xSpacing != 0)
    SLM_ASSERT("m_ySpacing can not equal 0", m_ySpacing != 0)

    // Remove all lines from the scene
    for (std::vector<QGraphicsItem*>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
    {
        this->getScene2DRender()->getScene()->removeItem(*it);
    }
    // Clear the lines vector
    m_lines.clear();

    // Calculate the start, end and step on x for the lines
    float xStartVal = (int)( m_xMin / m_xSpacing ) * m_xSpacing;
    float xEndVal = (int)( m_xMax / m_xSpacing ) * m_xSpacing;
    float xStep = m_xSpacing;

    // Calculate the start, end and step on y for the lines
    float yStartVal = (int)( m_yMin / m_ySpacing ) * m_ySpacing;
    float yEndVal = (int)( m_yMax / m_ySpacing ) * m_ySpacing;
    float yStep = m_ySpacing;

    // Draw the horizontal lines
    for ( float yVal = yStartVal ; yVal <= yEndVal ; yVal += yStep )
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(
            this->mapAdaptorToScene(std::pair< double , double >( xStartVal, yVal) , m_xAxis, m_yAxis).first,
            this->mapAdaptorToScene(std::pair< double , double >( xStartVal, yVal) , m_xAxis, m_yAxis).second,
            this->mapAdaptorToScene(std::pair< double , double >( xEndVal, yVal) , m_xAxis, m_yAxis).first,
            this->mapAdaptorToScene(std::pair< double , double >( xEndVal, yVal) , m_xAxis, m_yAxis).second
        );
        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);
    }

    // Draw the vertical lines
    for ( float xVal = xStartVal ; xVal <= xEndVal ; xVal += xStep )
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(
            this->mapAdaptorToScene(std::pair< double , double >( xVal, yStartVal) , m_xAxis, m_yAxis).first,
            this->mapAdaptorToScene(std::pair< double , double >( xVal, yStartVal) , m_xAxis, m_yAxis).second,
            this->mapAdaptorToScene(std::pair< double , double >( xVal, yEndVal) , m_xAxis, m_yAxis).first,
            this->mapAdaptorToScene(std::pair< double , double >( xVal, yEndVal) , m_xAxis, m_yAxis).second
        );
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
    // Add the layer to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

void GridFromFloat::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    // Set the pen a style
    m_pen.setStyle(Qt::DashLine);
    this->draw();
}

void GridFromFloat::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void GridFromFloat::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Get and cast the ObjectMsg to FloatMsg
    ::fwComEd::FloatMsg::csptr floatMsg = ::fwComEd::FloatMsg::dynamicConstCast(_msg);

    // If the message is VALUE_IS_MODIFIED
    if(floatMsg && floatMsg->hasEvent( ::fwComEd::FloatMsg::VALUE_IS_MODIFIED ) )
    {
        // Check if the float object isn't negative
        if (this->getObject< ::fwData::Float >()->getValue() > 0)
        {
            // Set the xSpacing the float object value
            m_xSpacing = this->getObject< ::fwData::Float >()->getValue();
        }
        
        this->draw();
    }
}

void GridFromFloat::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void GridFromFloat::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Clear the lines vector
    m_lines.clear();
    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D



