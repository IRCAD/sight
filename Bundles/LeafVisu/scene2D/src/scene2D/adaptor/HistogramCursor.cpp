/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/HistogramCursor.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QFont>
#include <QGraphicsEllipseItem>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::HistogramCursor, ::fwData::Histogram);

namespace scene2D
{
namespace adaptor
{

HistogramCursor::HistogramCursor() noexcept :
    m_color("red"),
    m_borderColor(Qt::gray),
    m_opacity(0.8f),
    m_index(nullptr),
    m_pointSize(6.f),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

HistogramCursor::~HistogramCursor() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void HistogramCursor::configuring()
{
    SLM_TRACE_FUNC();

    this->IAdaptor::configuring();

    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_color, m_configuration->getAttributeValue("color"));
    }

    if (!m_configuration->getAttributeValue("opacity").empty())
    {
        m_opacity = std::stof( m_configuration->getAttributeValue("opacity") );
    }

    if (!m_configuration->getAttributeValue("borderColor").empty())
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_borderColor, m_configuration->getAttributeValue("borderColor"));
    }

    if (!m_configuration->getAttributeValue("pointSize").empty())
    {
        m_pointSize = std::stof( m_configuration->getAttributeValue("pointSize") );
    }

    SLM_ASSERT("A viewport attribute must be specified with 'viewportUID'.",
               !m_configuration->getAttributeValue("viewportUID").empty());

    if( !m_configuration->getAttributeValue("viewportUID").empty() )
    {
        m_viewportID = m_configuration->getAttributeValue("viewportUID");
    }

    SLM_ASSERT("'histogramPointUID' attribute is missing.", !m_configuration->getAttributeValue(
                   "histogramPointUID").empty());
    m_histogramPointUID = m_configuration->getAttributeValue("histogramPointUID");
}

//---------------------------------------------------------------------------------------------------------------

void HistogramCursor::doStart()
{
    m_index = new QGraphicsEllipseItem();
    m_index->setBrush( m_color.color() );
    m_index->setPen( m_borderColor );
    m_index->setZValue(m_zValue);
    m_index->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    m_color.setCosmetic(true);

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    m_layer->addToGroup( m_index );

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    m_viewport = this->getSafeInOut< ::fwRenderQt::data::Viewport>( m_viewportID );

    m_connection = m_viewport->signal(::fwData::Object::s_MODIFIED_SIG)->connect(
        this->slot(::fwServices::IService::s_UPDATE_SLOT));

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void HistogramCursor::doStop()
{
    m_connection.disconnect();
}

//---------------------------------------------------------------------------------------------------------------

void HistogramCursor::doUpdate()
{
    this->initializeViewSize();
    this->initializeViewportSize();

    ::fwData::Histogram::sptr histogram           = this->getObject< ::fwData::Histogram>();
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue  = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    ::fwRenderQt::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( m_coord );

    int histIndex = (int) sceneCoord.getX();
    int index     = (histIndex - histogramMinValue) / histogramBinsWidth;

    if(index >= 0 && index < (int)values.size()) // avoid std out_of_range on MS Windows
    {
        ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
        const double viewportHeight = viewport->getHeight();
        const double viewportWidth  = viewport->getWidth();

        const double viewportSizeRatio    = viewportHeight / viewportWidth;
        const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

        const Scene2DRatio ratio   = this->getRatio(); // Total ratio
        const double viewportRatio = this->getViewportSizeRatio().first;

        double diameterH = m_pointSize;
        double diameterV = m_pointSize * viewportSizeRatio;

        diameterV /= viewportRatio;
        diameterV *= viewInitialSizeRatio;

        // Apply the ratio of the scene 2D in order to keep the same size for the circles if viewport's size or
        // view's size change:
        diameterH *= ratio.first;
        diameterV *= ratio.second;

        ::fwData::Point::sptr point = this->getSafeInOut< ::fwData::Point>( m_histogramPointUID );
        SLM_ASSERT("[inout] Point '" + m_histogramPointUID + "' is not found", point);

        double x = point->getRefCoord()[0] - diameterH / 2;
        double y = point->getRefCoord()[1] - diameterV / 2;

        m_index->setRect( x, y, diameterH, diameterV );
    }
}

//---------------------------------------------------------------------------------------------------------------

void HistogramCursor::doSwap()
{
}

//---------------------------------------------------------------------------------------------------------------

void HistogramCursor::processInteraction( ::fwRenderQt::data::Event& _event )
{
    this->initializeViewSize();
    this->initializeViewportSize();

    if( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
    {
        m_coord = _event.getCoord();
    }

    doUpdate();
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D
