/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/GridFromFloat.hpp"

#include <fwData/Float.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::GridFromFloat, ::fwData::Float );

namespace scene2D
{
namespace adaptor
{

//------------------------------------------------------------------------------

GridFromFloat::GridFromFloat() noexcept :
    m_xMin(0.f),
    m_xMax(10.f),
    m_yMin(0.f),
    m_yMax(10.f),
    m_xSpacing(10.f),
    m_ySpacing(10.f),
    m_lineType(PLAIN),
    m_layer(nullptr)
{
}

//------------------------------------------------------------------------------

GridFromFloat::~GridFromFloat() noexcept
{
}

//------------------------------------------------------------------------------

void GridFromFloat::configuring()
{
    this->IAdaptor::configuring();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    SLM_ASSERT("Attribute 'xMin' is missing", config.count("xMin"));
    SLM_ASSERT("Attribute 'xMax' is missing", config.count("xMax"));
    SLM_ASSERT("Attribute 'yMin' is missing", config.count("yMin"));
    SLM_ASSERT("Attribute 'yMax' is missing", config.count("yMax"));

    // Set the x/y min/max values
    m_xMin = config.get<float>("xMin");
    m_xMax = config.get<float>("xMax");
    m_yMin = config.get<float>("yMin");
    m_yMax = config.get<float>("yMax");

    // If the corresponding attributes are present in the config, set the xSpacing, ySpacing between the lines and color
    // of the lines
    m_xSpacing = config.get<float>("xSpacing", 10.f);
    m_ySpacing = config.get<float>("ySpacing", 10.f);

    if (config.count("color"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_pen, config.get<std::string>("color"));
    }
}

//------------------------------------------------------------------------------

void GridFromFloat::draw()
{
    SLM_ASSERT("m_xSpacing can not equal 0", m_xSpacing != 0);
    SLM_ASSERT("m_ySpacing can not equal 0", m_ySpacing != 0);

    // Remove all lines from the scene
    for (std::vector<QGraphicsItem*>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
    {
        this->getScene2DRender()->getScene()->removeItem(*it);
    }
    // Clear the lines vector
    m_lines.clear();

    // Calculate the start, end and step on x for the lines
    float xStartVal = (int)( m_xMin / m_xSpacing ) * m_xSpacing;
    float xEndVal   = (int)( m_xMax / m_xSpacing ) * m_xSpacing;
    float xStep     = m_xSpacing;

    // Calculate the start, end and step on y for the lines
    float yStartVal = (int)( m_yMin / m_ySpacing ) * m_ySpacing;
    float yEndVal   = (int)( m_yMax / m_ySpacing ) * m_ySpacing;
    float yStep     = m_ySpacing;

    // Draw the horizontal lines
    for ( float yVal = yStartVal; yVal <= yEndVal; yVal += yStep )
    {
        const Point2DType pt1 = this->mapAdaptorToScene(Point2DType( xStartVal, yVal),
                                                        m_xAxis, m_yAxis);
        const Point2DType pt2 = this->mapAdaptorToScene(Point2DType( xEndVal, yVal),
                                                        m_xAxis, m_yAxis);
        QGraphicsLineItem* line = new QGraphicsLineItem(pt1.first, pt1.second,
                                                        pt2.first, pt2.second);
        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);
    }

    // Draw the vertical lines
    for ( float xVal = xStartVal; xVal <= xEndVal; xVal += xStep )
    {
        const Point2DType pt1   = this->mapAdaptorToScene(Point2DType( xVal, yStartVal), m_xAxis, m_yAxis);
        const Point2DType pt2   = this->mapAdaptorToScene(Point2DType( xVal, yEndVal), m_xAxis, m_yAxis);
        QGraphicsLineItem* line = new QGraphicsLineItem(pt1.first, pt1.second,
                                                        pt2.first, pt2.second);
        // Set the line the pen and push it back in to the lines vector
        line->setPen(m_pen);
        m_lines.push_back(line);
    }

    // Add the lines contained in the lines vector to the layer
    for ( unsigned int i = 0; i < m_lines.size(); i++)
    {
        m_layer->addToGroup(m_lines.at(i));
    }

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);
    // Add the layer to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//------------------------------------------------------------------------------

void GridFromFloat::doStart()
{
    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    // Set the pen a style
    m_pen.setStyle(Qt::DashLine);
    m_pen.setCosmetic(true);
    this->draw();
}

//------------------------------------------------------------------------------

void GridFromFloat::doUpdate()
{
    const float value = this->getObject< ::fwData::Float >()->getValue();
    // Check if the float object isn't negative
    if ( value > 0)
    {
        // Set the xSpacing the float object value
        m_xSpacing = value;
    }

    this->draw();
}

//------------------------------------------------------------------------------

void GridFromFloat::doSwap()
{
}

//------------------------------------------------------------------------------

void GridFromFloat::doStop()
{
    // Clear the lines vector
    m_lines.clear();
    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType GridFromFloat::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D

