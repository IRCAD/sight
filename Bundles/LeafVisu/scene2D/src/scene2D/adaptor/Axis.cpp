/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/Axis.hpp"

#include <fwData/Composite.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::Axis);

namespace scene2D
{

namespace adaptor
{

Axis::Axis() noexcept :
    m_interval(1.f),
    m_min(0.f),
    m_max(0.f),
    m_tickSize(1.0f),
    m_showLine(true),
    m_layer(nullptr),
    m_line(nullptr),
    m_color(Qt::white)
{
}

//--------------------------------------------------------------------------------------------------

Axis::~Axis() noexcept
{
}

//--------------------------------------------------------------------------------------------------

void Axis::doStart()
{
    this->buildAxis();
    this->doUpdate();
}

//--------------------------------------------------------------------------------------------------

void Axis::doStop()
{
    delete m_layer;
}

//--------------------------------------------------------------------------------------------------

void Axis::doSwap()
{
}

//--------------------------------------------------------------------------------------------------

void Axis::configuring()
{
    this->IAdaptor::configuring();  // Looks for 'xAxis', 'yAxis' and 'zValue'

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    // 'color'
    if (config.count("color"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    // 'align' attribute configuration
    m_align = config.get<std::string>("align");
    SLM_ASSERT("'align' attribute is missing. Please add an 'align' attribute "
               "with value 'left', 'right', 'top' or 'bottom'", !m_align.empty());
    SLM_ASSERT("Unsupported value for 'align' attribute.",
               m_align == "left" || m_align == "right" || m_align == "top" || m_align == "bottom");

    // Axis bounds
    SLM_ASSERT("'min' attribute is missing.", config.count("min"));
    SLM_ASSERT("'max' attribute is missing.", config.count("max"));
    m_min = config.get<float>("min");
    m_max = config.get<float>("max");

    // Ticks size
    m_tickSize = config.get<float>("tickSize", 1.0f);

    // Step
    m_interval = config.get<float>("interval", 1.0f);
}

//---------------------------------------------------------------------------------------------------------------

void Axis::buildAxis()
{
    m_color.setCosmetic(true);
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

void Axis::doUpdate()
{
    this->initializeViewSize();
    this->initializeViewportSize();

    const Scene2DRatio ratio = this->getRatio();

    ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
    const double viewportHeight = viewport->getHeight();
    const double viewportWidth  = viewport->getWidth();

    const double viewportSizeRatio    = viewportHeight / viewportWidth;
    const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

    const double viewportWidthRatio = this->getViewportSizeRatio().first;

    double scaleX = m_tickSize;
    double scaleY = m_tickSize * viewportSizeRatio;
    scaleY /= viewportWidthRatio;
    scaleY *= viewInitialSizeRatio;

    scaleX = scaleX * ratio.first;
    scaleY = scaleY * ratio.second;

    const size_t nbValues = m_ticks.size();

    const double min = this->getStartVal();
    const double max = this->getEndVal();

    float pos;
    Point2DType tickSize;
    Point2DType tickPos;

    const Point2DType viewportSize =
        this->mapAdaptorToScene(Point2DType(viewportWidth, viewportHeight), m_xAxis, m_yAxis);

    if(m_align == "bottom")
    {
        tickSize = this->mapAdaptorToScene(Point2DType(0, m_tickSize), m_xAxis, m_yAxis);

        const double tickPosY = viewport->getY();

        for(int i = 0; i < nbValues; ++i)
        {
            pos     = min + i * m_interval;
            tickPos = this->mapAdaptorToScene(Point2DType(pos, tickPosY), m_xAxis, m_yAxis);
            m_ticks.at(i)->setLine(
                tickPos.first, tickPos.second,
                tickPos.first, tickPos.second - tickSize.second * scaleY);
        }

        m_line->setLine(min, tickPos.second, max, tickPos.second);
    }
    else if(m_align == "top")
    {
        tickSize = this->mapAdaptorToScene(Point2DType(0, m_tickSize), m_xAxis, m_yAxis);

        const double tickPosY = viewport->getHeight() * 0.9;

        for(int i = 0; i < nbValues; ++i)
        {
            pos     = min + i * m_interval;
            tickPos = this->mapAdaptorToScene(Point2DType(pos, tickPosY), m_xAxis, m_yAxis);

            m_ticks.at(i)->setLine(
                tickPos.first, tickPos.second,
                tickPos.first, tickPos.second - tickSize.second * scaleY);
        }

        m_line->setLine(min, tickPos.second, max, tickPos.second);
    }
    else if(m_align == "left")
    {
        tickSize = this->mapAdaptorToScene(Point2DType(m_tickSize, 0), m_xAxis, m_yAxis);

        const double tickPosX = viewport->getX();

        for(int i = 0; i < nbValues; ++i)
        {
            pos     = min + i * m_interval;
            tickPos = this->mapAdaptorToScene(Point2DType(tickPosX, pos), m_xAxis, m_yAxis);
            m_ticks.at(i)->setLine(
                tickPos.first, tickPos.second,
                tickPos.first + tickSize.first * scaleX, tickPos.second);
        }

        m_line->setLine( tickPos.first, min, tickPos.first, tickPos.second);
    }
    else if(m_align == "right")
    {
        tickSize = this->mapAdaptorToScene(Point2DType(m_tickSize, 0), m_xAxis, m_yAxis);

        const double tickPosX = viewport->getX() + viewport->getWidth();

        for(int i = 0; i < nbValues; ++i)
        {
            pos = min + i * m_interval;

            tickPos = this->mapAdaptorToScene(Point2DType(tickPosX, pos), m_xAxis, m_yAxis);

            m_ticks.at(i)->setLine(
                tickPos.first - tickSize.first * scaleX, tickPos.second,
                tickPos.first, tickPos.second);
        }

        m_line->setLine(tickPos.first, min, tickPos.first, tickPos.second);
    }
}

//---------------------------------------------------------------------------------------

void Axis::processInteraction( ::fwRenderQt::data::Event& _event)
{
    if( _event.getType() == ::fwRenderQt::data::Event::Resize)
    {
        doUpdate();
    }
}

//--------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D

