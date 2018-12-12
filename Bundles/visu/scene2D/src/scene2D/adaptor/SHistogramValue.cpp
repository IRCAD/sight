/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "scene2D/adaptor/SHistogramValue.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QFont>
#include <QGraphicsEllipseItem>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::SHistogramValue);

namespace scene2D
{
namespace adaptor
{

static const ::fwServices::IService::KeyType s_POINT_INPUT     = "point";
static const ::fwServices::IService::KeyType s_HISTOGRAM_INPUT = "histogram";
static const ::fwServices::IService::KeyType s_VIEWPORT_INPUT  = "viewport";

SHistogramValue::SHistogramValue() noexcept :
    m_color(Qt::white),
    m_text(nullptr),
    m_isInteracting(false),
    m_fontSize(8.f),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

SHistogramValue::~SHistogramValue() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if (config.count("color"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    if (config.count("fontSize"))
    {
        m_fontSize = config.get<float>("fontSize");
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::starting()
{
    m_font.setPointSize(m_fontSize);
    m_font.setLetterSpacing( QFont::AbsoluteSpacing, 0.2 );
    m_font.setKerning( true );
    m_font.setFixedPitch( true );

    m_text = new QGraphicsSimpleTextItem();
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

void SHistogramValue::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::updating()
{
    this->initializeViewSize();
    this->initializeViewportSize();

    ::fwData::Histogram::csptr histogram          = this->getInput< ::fwData::Histogram>(s_HISTOGRAM_INPUT);
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue  = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    ::fwRenderQt::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( m_coord );

    int histIndex = (int) sceneCoord.getX();
    int index     = (histIndex - histogramMinValue) / histogramBinsWidth;

    if(index >= 0 && index < (int)values.size() && m_isInteracting) // avoid std out_of_range on Windows
    {
        ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
        const double viewportHeight = viewport->getHeight();
        const double viewportWidth  = viewport->getWidth();

        const double viewportSizeRatio    = viewportHeight / viewportWidth;
        const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

        Scene2DRatio ratio        = this->getRatio(); // Total ratio
        double viewportWidthRatio = this->getViewportSizeRatio().first;

        double diameterH = m_fontSize;
        double diameterV = m_fontSize * viewportSizeRatio;

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

        ::fwData::Point::csptr point = this->getInput< ::fwData::Point>(s_POINT_INPUT);

        m_text->setTransform( transform );
        m_text->setPos( point->getCoord()[0] + diameterH * 2, point->getCoord()[1] - diameterV * 2 );
        m_text->setVisible( true );
    }
    else
    {
        m_text->setVisible( false );
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramValue::processInteraction( ::fwRenderQt::data::Event& _event )
{
    this->initializeViewSize();
    this->initializeViewportSize();

    if( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
    {
        m_coord = _event.getCoord();
    }
    else if( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress )
    {
        m_isInteracting = true;
    }
    else if( _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease )
    {
        m_isInteracting = false;
    }

    updating();
}

//----------------------------------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SHistogramValue::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_HISTOGRAM_INPUT, ::fwData::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_VIEWPORT_INPUT, ::fwRenderQt::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D
