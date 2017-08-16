/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/SLine.hpp"

#include <fwRenderQt/data/InitQtPen.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::SLine);

namespace scene2D
{
namespace adaptor
{

SLine::SLine() noexcept :
    m_x1(0.f),
    m_x2(0.f),
    m_y1(0.f),
    m_y2(0.f),
    m_lineType(PLAIN),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

SLine::~SLine() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SLine::configuring()
{
    this->IAdaptor::configuring();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    SLM_ASSERT("Attribute 'x1' is missing", config.count("x1"));
    SLM_ASSERT("Attribute 'x2' is missing", config.count("x2"));
    SLM_ASSERT("Attribute 'y1' is missing", config.count("y1"));
    SLM_ASSERT("Attribute 'y2' is missing", config.count("y2"));

    // Set the beginning and ending coordinates values
    m_x1 = config.get<float>("x1");
    m_x2 = config.get<float>("x2");
    m_y1 = config.get<float>("y1");
    m_y2 = config.get<float>("y2");

    // If the corresponding attributes are present in the config, set the color of the line
    if (config.count("color"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_pen, config.get<std::string>("color"));
    }
}

//---------------------------------------------------------------------------------------------------------------

void SLine::draw()
{
    const Point2DType pt1 = this->mapAdaptorToScene(Point2DType( m_x1, m_y1), m_xAxis, m_yAxis);
    const Point2DType pt2 = this->mapAdaptorToScene(Point2DType( m_x2, m_y2), m_xAxis, m_yAxis);

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

void SLine::starting()
{
    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    m_pen.setCosmetic(true);

    this->draw();
}

//---------------------------------------------------------------------------------------------------------------

void SLine::updating()
{
}

//---------------------------------------------------------------------------------------------------------------

void SLine::stopping()
{
    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D

