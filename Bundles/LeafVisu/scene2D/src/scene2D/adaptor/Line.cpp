/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/Line.hpp"

#include <fwData/Composite.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::Line, ::fwData::Composite );

namespace scene2D
{
namespace adaptor
{

Line::Line() noexcept :
    m_x1(0.f),
    m_x2(0.f),
    m_y1(0.f),
    m_y2(0.f),
    m_lineType(PLAIN),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

Line::~Line() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void Line::configuring()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    // Set the beginning and ending coordinates values
    m_x1 = std::stof( m_configuration->getAttributeValue("x1") );
    m_x2 = std::stof( m_configuration->getAttributeValue("x2") );
    m_y1 = std::stof( m_configuration->getAttributeValue("y1") );
    m_y2 = std::stof( m_configuration->getAttributeValue("y2") );

    // If the corresponding attributes are present in the config, set the color of the line
    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"));
    }
}

//---------------------------------------------------------------------------------------------------------------

void Line::draw()
{
    const Point2DType pt1 = this->mapAdaptorToScene(Point2DType( m_x1, m_y1), *m_xAxis, *m_yAxis);
    const Point2DType pt2 = this->mapAdaptorToScene(Point2DType( m_x2, m_y2), *m_xAxis, *m_yAxis);

    // Draw the line
    QGraphicsLineItem* line = new QGraphicsLineItem(pt1.first, pt1.second,
                                                    pt2.first, pt2.second);
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

//---------------------------------------------------------------------------------------------------------------

void Line::doStart()
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    m_pen.setCosmetic(true);

    this->draw();
}

//---------------------------------------------------------------------------------------------------------------

void Line::doUpdate()
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void Line::doSwap()
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void Line::doStop()
{
    SLM_TRACE_FUNC();
    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D

