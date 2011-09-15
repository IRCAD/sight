/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Composite.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/adaptor/Line.hpp"
#include "scene2D/data/InitQtPen.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Line  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

Line::Line() throw()
{
}

Line::~Line() throw()
{
}

void Line::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    // Set the beginning and ending coordinates values
    m_x1 = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("x1") );
    m_x2 = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("x2") );
    m_y1 = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("y1") );
    m_y2 = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("y2") );
    
    // If the corresponding attributes are present in the config, set the color of the line
    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"));
    }
}

void Line::draw()
{
    SLM_TRACE_FUNC();

    // Draw the line
    QGraphicsLineItem* line = new QGraphicsLineItem(
        this->mapAdaptorToScene(std::pair< double , double >( m_x1, m_y1) , m_xAxis, m_yAxis).first,
        this->mapAdaptorToScene(std::pair< double , double >( m_x1, m_y1) , m_xAxis, m_yAxis).second,
        this->mapAdaptorToScene(std::pair< double , double >( m_x2, m_y2) , m_xAxis, m_yAxis).first,
        this->mapAdaptorToScene(std::pair< double , double >( m_x2, m_y2) , m_xAxis, m_yAxis).second
    );
    // Set the line the pen
    line->setPen(m_pen);

    // Add the line to the layer
    m_layer->addToGroup(line);

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);
    // Add the layer to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

void Line::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    this->draw();
}

void Line::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Line::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Line::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Line::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D



