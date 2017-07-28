/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/SViewportRangeSelector.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwRenderQt/data/Viewport.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsRectItem>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::SViewportRangeSelector);

namespace scene2D
{
namespace adaptor
{

static const ::fwServices::IService::KeyType s_VIEWPORT_INOUT = "viewport";

//---------------------------------------------------------------------------------------------------------------

SViewportRangeSelector::SViewportRangeSelector() noexcept :
    m_shutter(nullptr),
    m_isLeftInteracting( false ),
    m_isRightInteracting( false ),
    m_isInteracting( false ),
    m_clickCatchRange( 15 ),
    m_layer(nullptr),
    m_initialX(0.f),
    m_initialWidth(0.f)
{
}

//---------------------------------------------------------------------------------------------------------------

SViewportRangeSelector::~SViewportRangeSelector() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::configuring()
{
    this->IAdaptor::configuring();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");
    if (config.count("initialWidth"))
    {
        m_initialWidth = config.get<float>("initialWidth");
    }

    if (config.count("initialPos"))
    {
        m_initialX = config.get<float>("initialPos");
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::doStart()
{
    {
        ::fwRenderQt::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

        const double viewportWidth = sceneViewport->getWidth();
        const double defaultWidth  = 2. * viewportWidth / 4.;

        if( m_initialWidth > viewportWidth || m_initialWidth < m_clickCatchRange )
        {
            SLM_WARN("Set viewport width to a default value instead of the given one because it can't be accepted.");
            m_initialWidth = defaultWidth;
        }

        const double defaultPos = (viewportWidth - m_initialWidth) / 2.;
        if( m_initialX < sceneViewport->getX() || (m_initialX + m_initialWidth) > viewportWidth)
        {
            SLM_WARN("Set viewport position to a default value since the given one is not correct.");
            m_initialX = defaultPos;
        }
    }

    ::fwRenderQt::data::Viewport::sptr viewport =
        this->getInOut< ::fwRenderQt::data::Viewport>(s_VIEWPORT_INOUT);
    QRectF sceneRect = this->getScene2DRender()->getScene()->sceneRect();

    Point2DType pair = this->mapAdaptorToScene(
        Point2DType( m_initialX, viewport->getHeight() ), m_xAxis, m_yAxis );
    m_shutter = new QGraphicsRectItem(
        pair.first, 0, m_initialWidth * m_xAxis->getScale(), pair.second );
    m_shutter->setBrush( QBrush(QColor(127, 127, 127, 127)) );
    m_shutter->setPen( Qt::NoPen );

    m_layer = new QGraphicsItemGroup();
    m_layer->addToGroup( m_shutter );

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    this->getScene2DRender()->getScene()->addItem( m_layer );

    QRectF rect = m_shutter->rect();
    updateViewportFromShutter( rect.x(), rect.y(), rect.width(), rect.height() );

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = viewport->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::doStop()
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::doUpdate()
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::doSwap()
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::processInteraction( ::fwRenderQt::data::Event& _event )
{
    // Event coordinates in scene
    ::fwRenderQt::data::Coord coord;
    coord = this->getScene2DRender()->mapToScene( _event.getCoord() );

    // Shutter coordinates in scene
    Point2DType shutterCoordPair = this->mapAdaptorToScene(
        Point2DType( m_shutter->rect().x(), m_shutter->rect().y()),
        m_xAxis, m_yAxis);
    const double shutterWidth = m_shutter->rect().width() * m_xAxis->getScale();

    QRectF sceneRect = this->getScene2DRender()->getScene()->sceneRect();

    const bool onShutterLeft   = mouseOnShutterLeft( coord );
    const bool onShutterRight  = mouseOnShutterRight( coord );
    const bool onShutterMiddle = mouseOnShutterMiddle( coord );

    QRectF rect = m_shutter->rect();

    if( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress )
    {
        if( onShutterLeft )
        {
            m_isLeftInteracting = true;
        }
        else if( onShutterRight )
        {
            m_isRightInteracting = true;
        }
        else if( onShutterMiddle )
        {
            this->getScene2DRender()->getView()->setCursor( Qt::ClosedHandCursor );

            // Interaction when clicking on the center of the shutter
            m_isInteracting  = true;
            m_dragStartPoint = coord;
            m_dragStartShutterPos.setX( shutterCoordPair.first );
            m_dragStartShutterPos.setY( shutterCoordPair.second );
        }
    }
    else if( _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease )
    {
        m_isInteracting      = false;
        m_isLeftInteracting  = false;
        m_isRightInteracting = false;

        // Reset cursor
        if( onShutterMiddle )
        {
            this->getScene2DRender()->getView()->setCursor( Qt::OpenHandCursor );
        }
        else
        {
            this->getScene2DRender()->getView()->setCursor( Qt::ArrowCursor );
        }
    }
    else if( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
    {
        // If the mouse is moving onto the shutter, without interactions, the cursor is changed to an other cursor
        // that symbolize the available interactions
        if( !m_isLeftInteracting && !m_isRightInteracting && !m_isInteracting )
        {
            if( onShutterLeft || onShutterRight )
            {
                this->getScene2DRender()->getView()->setCursor( Qt::SizeHorCursor );    // horizontal double arrow
            }
            else if( onShutterMiddle )
            {
                this->getScene2DRender()->getView()->setCursor( Qt::OpenHandCursor );   // open hand, for moving the
                                                                                        // whole shutter
            }
            else
            {
                this->getScene2DRender()->getView()->setCursor( Qt::ArrowCursor );      // reset the cursor to the
                                                                                        // default cursor
            }
        }

        bool update = false;    // if a viewport update will be requested

        if( m_isLeftInteracting )
        {
            // Shutter right side position
            double rightSide = rect.x() + rect.width();

            if( coord.getX() < rightSide - m_clickCatchRange)
            {
                if( coord.getX() >= sceneRect.x() )
                {
                    rect.setX( coord.getX() );
                }
                else
                {
                    rect.setX( sceneRect.x() );
                }
            }
            else
            {
                rect.setX( rightSide - m_clickCatchRange );
            }

            update = true;
        }
        else if( m_isRightInteracting )
        {
            double newWidth        = coord.getX() - shutterCoordPair.first;
            double shutterRightPos = abs(sceneRect.x()) + shutterCoordPair.first + newWidth;

            if( newWidth > m_clickCatchRange ) // Shutter's width must be greater than the allowed picking range
            {
                if( shutterRightPos < sceneRect.width() )
                {
                    rect.setWidth( newWidth );
                }
                else
                {
                    rect.setWidth( sceneRect.width() - shutterCoordPair.first - abs(sceneRect.x()) );
                }
            }
            else
            {
                rect.setWidth( m_clickCatchRange );
            }

            update = true;
        }
        else if( m_isInteracting )
        {
            double offset          = coord.getX() - m_dragStartPoint.getX();
            double newX            = m_dragStartShutterPos.getX() + offset;
            double shutterRightPos = abs(sceneRect.x()) + newX + shutterWidth;

            if( newX >= sceneRect.x() && shutterRightPos < sceneRect.width() )
            {
                rect.setX( newX );
            }
            else if( newX < sceneRect.x() )
            {
                rect.setX( sceneRect.x() );
            }
            else if( shutterRightPos >= sceneRect.width() )
            {
                rect.setX( sceneRect.width() - shutterWidth - abs(sceneRect.x()) );
            }

            rect.setWidth( shutterWidth );
            update = true;
        }

        if( update )
        {
            // Update graphical shutter
            m_shutter->setRect( rect );
            m_layer->removeFromGroup( m_shutter );
            m_layer->addToGroup( m_shutter );

            // Update object
            this->updateViewportFromShutter( rect.x(), rect.y(), rect.width(), rect.height() );

            ::fwRenderQt::data::Viewport::sptr viewport =
                this->getInOut< ::fwRenderQt::data::Viewport>(s_VIEWPORT_INOUT);

            ::fwData::Object::ModifiedSignalType::sptr sig =
                viewport->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::updateViewportFromShutter( double _x, double _y, double _width, double _height )
{
    ::fwRenderQt::data::Viewport::sptr viewport =
        this->getInOut< ::fwRenderQt::data::Viewport>(s_VIEWPORT_INOUT);

    const Point2DType fromSceneCoord = this->mapSceneToAdaptor(Point2DType( _x, _y ), m_xAxis, m_yAxis );
    viewport->setX( fromSceneCoord.first );
    viewport->setY( fromSceneCoord.second );

    const Point2DType pair = this->mapSceneToAdaptor(Point2DType(_width, _height), m_xAxis, m_yAxis);
    viewport->setWidth( pair.first );
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterMiddle( ::fwRenderQt::data::Coord _coord)
{
    Point2DType shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene( Point2DType( m_shutter->rect().x(), m_shutter->rect().y()),
                                                m_xAxis, m_yAxis );

    return ( _coord.getX() > m_shutter->rect().x() + m_clickCatchRange )
           && ( _coord.getX() < m_shutter->rect().x() + m_shutter->rect().width() - m_clickCatchRange );
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterLeft( ::fwRenderQt::data::Coord _coord)
{
    Point2DType shutterCoordPair = this->mapAdaptorToScene(
        Point2DType( m_shutter->rect().x(), m_shutter->rect().y() ), m_xAxis, m_yAxis );

    return ( _coord.getX() >= shutterCoordPair.first - m_clickCatchRange )
           && ( _coord.getX() <= shutterCoordPair.first + m_clickCatchRange );
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterRight( ::fwRenderQt::data::Coord _coord)
{
    const Point2DType shutterCoordPair = this->mapAdaptorToScene(
        Point2DType( m_shutter->rect().x(), m_shutter->rect().y()),
        m_xAxis, m_yAxis );

    const double shutterRightPos = shutterCoordPair.first + m_shutter->rect().width() * m_xAxis->getScale();

    return ( _coord.getX() >= shutterRightPos - m_clickCatchRange )
           && ( _coord.getX() <= shutterRightPos + m_clickCatchRange );
}

//----------------------------------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SViewportRangeSelector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_VIEWPORT_INOUT, ::fwRenderQt::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D
