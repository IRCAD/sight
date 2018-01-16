/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/SHistogramCursor.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QFont>
#include <QGraphicsEllipseItem>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::SHistogramCursor);

namespace scene2D
{
namespace adaptor
{

static const ::fwServices::IService::KeyType s_POINT_INPUT     = "point";
static const ::fwServices::IService::KeyType s_HISTOGRAM_INPUT = "histogram";
static const ::fwServices::IService::KeyType s_VIEWPORT_INPUT  = "viewport";

SHistogramCursor::SHistogramCursor() noexcept :
    m_color(Qt::red),
    m_borderColor(Qt::gray),
    m_index(nullptr),
    m_pointSize(6.f),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

SHistogramCursor::~SHistogramCursor() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if (config.count("color"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    if (config.count("borderColor"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_borderColor, config.get<std::string>("borderColor"));
    }

    if (config.count("pointSize"))
    {
        m_pointSize = config.get<float>("pointSize");
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::starting()
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

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::updating()
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

        ::fwData::Point::csptr point = this->getInput< ::fwData::Point>(s_POINT_INPUT);

        const double x = point->getCoord()[0] - diameterH / 2;
        const double y = point->getCoord()[1] - diameterV / 2;

        m_index->setRect( x, y, diameterH, diameterV );
    }
}

//---------------------------------------------------------------------------------------------------------------

void SHistogramCursor::processInteraction( ::fwRenderQt::data::Event& _event )
{
    this->initializeViewSize();
    this->initializeViewportSize();

    if( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
    {
        m_coord = _event.getCoord();
    }

    updating();
}

//----------------------------------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SHistogramCursor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_HISTOGRAM_INPUT, ::fwData::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_VIEWPORT_INPUT, ::fwRenderQt::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D
