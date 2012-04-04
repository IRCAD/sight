/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>

#include <fwServices/IEditionService.hpp>

#include <scene2D/data/Viewport.hpp>
#include <scene2D/data/ViewportMsg.hpp>

#include <QGraphicsRectItem>

#include "scene2D/adaptor/ViewportRangeSelector.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::ViewportRangeSelector, ::scene2D::data::Viewport);

namespace scene2D
{
namespace adaptor
{


ViewportRangeSelector::ViewportRangeSelector() throw()
    : m_isLeftInteracting( false ), m_isRightInteracting( false ), m_isInteracting( false ), m_clickCatchRange( 15 )
{
     this->handlingEventOff(); // This service no handling event
}

//---------------------------------------------------------------------------------------------------------------

ViewportRangeSelector::~ViewportRangeSelector() throw()
{
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::scene2D::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();

    this->IAdaptor::configuring();

    const double viewportWidth = viewport->getWidth();
    const double defaultWidth = 2 * viewportWidth / 4;

    if (!m_configuration->getAttributeValue("initialWidth").empty())
    {
        m_initialWidth = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("initialWidth") );

        if( m_initialWidth > viewportWidth || m_initialWidth < m_clickCatchRange )
        {
            OSLM_WARN("Set viewport width to a default value instead of the given one because it can't be accepted.");
            m_initialWidth = defaultWidth;
        }
    }
    else
    {
        m_initialWidth = defaultWidth;
    }

    const double defaultPos = (viewportWidth - m_initialWidth) / 2;

    if (!m_configuration->getAttributeValue("initialPos").empty())
    {
        m_initialX = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("initialPos") );

        if( m_initialX < viewport->getX() || (m_initialX + m_initialWidth) > viewportWidth)
        {
            OSLM_WARN("Set viewport position to a default value since the given one is not correct.");
            m_initialX = defaultPos;
        }
    }
    else
    {
        m_initialX = defaultPos;
    }
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::doStart() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::scene2D::data::Viewport::sptr viewport = this->getObject< ::scene2D::data::Viewport>();

    QRectF sceneRect = this->getScene2DRender()->getScene()->sceneRect();

    std::pair< double, double > pair = this->mapAdaptorToScene(
            std::pair< double, double >( m_initialX, viewport->getHeight() ), m_xAxis, m_yAxis );
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

    ::scene2D::data::ViewportMsg::NewSptr msg;
    msg->addEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED);
    ::fwServices::IEditionService::notify( this->getSptr(), viewport, msg );
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::doStop() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::doUpdate() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::doSwap() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    // Event coordinates in scene
    ::scene2D::data::Coord coord;
    coord = this->getScene2DRender()->mapToScene( _event->getCoord() );

    // Shutter coordinates in scene
    std::pair< double, double > shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene(
            std::pair< double, double >( m_shutter->rect().x(), m_shutter->rect().y() ), m_xAxis, m_yAxis);
    double shutterWidth = m_shutter->rect().width() * m_xAxis->getScale();

    QRectF sceneRect = this->getScene2DRender()->getScene()->sceneRect();

    bool onShutterLeft = mouseOnShutterLeft( coord );
    bool onShutterRight = mouseOnShutterRight( coord );
    bool onShutterMiddle = mouseOnShutterMiddle( coord );

    QRectF rect = m_shutter->rect();

    if( _event->getType() == ::scene2D::data::Event::MouseButtonPress )
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
            m_isInteracting = true;
            m_dragStartPoint = coord;
            m_dragStartShutterPos.setX( shutterCoordPair.first );
            m_dragStartShutterPos.setY( shutterCoordPair.second );
        }
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
    {
        m_isInteracting = false;
        m_isLeftInteracting = false;
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
    else if( _event->getType() == ::scene2D::data::Event::MouseMove )
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
                this->getScene2DRender()->getView()->setCursor( Qt::OpenHandCursor );   // open hand, for moving the whole shutter
            }
            else
            {
                this->getScene2DRender()->getView()->setCursor( Qt::ArrowCursor );      // reset the cursor to the default cursor
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
            double newWidth = coord.getX() - shutterCoordPair.first;
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
            double offset = coord.getX() - m_dragStartPoint.getX();
            double newX = m_dragStartShutterPos.getX() + offset;
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
            updateViewportFromShutter( rect.x(), rect.y(), rect.width(), rect.height() );
            ::scene2D::data::ViewportMsg::NewSptr msg;
            msg->addEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED);
            ::fwServices::IEditionService::notify( this->getSptr(), this->getObject< ::scene2D::data::Viewport>(), msg );
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void ViewportRangeSelector::updateViewportFromShutter( double _x, double _y, double _width, double _height )
{
    SLM_TRACE_FUNC();

    ::scene2D::data::Viewport::sptr viewport = this->getObject< ::scene2D::data::Viewport>();

    Point2DType fromSceneCoord = this->mapSceneToAdaptor(Point2DType( _x, _y ), m_xAxis, m_yAxis );
    viewport->setX( fromSceneCoord.first );
    viewport->setY( fromSceneCoord.second );

    Point2DType pair = this->mapSceneToAdaptor(Point2DType(_width, _height), m_xAxis, m_yAxis);
    viewport->setWidth( pair.first );
    viewport->setHeight( this->getScene2DRender()->getViewport()->getHeight() );
}

//---------------------------------------------------------------------------------------------------------------

bool ViewportRangeSelector::mouseOnShutterMiddle( ::scene2D::data::Coord _coord)
{
    Point2DType shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene( Point2DType( m_shutter->rect().x(), m_shutter->rect().y() ), m_xAxis, m_yAxis );

    return ( _coord.getX() > m_shutter->rect().x() + m_clickCatchRange )
        && ( _coord.getX() < m_shutter->rect().x() + m_shutter->rect().width() - m_clickCatchRange );
}

//---------------------------------------------------------------------------------------------------------------

bool ViewportRangeSelector::mouseOnShutterLeft( ::scene2D::data::Coord _coord)
{
    std::pair< double, double > shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene( std::pair< double, double >( m_shutter->rect().x(), m_shutter->rect().y() ), m_xAxis, m_yAxis );

    return ( _coord.getX() >= shutterCoordPair.first - m_clickCatchRange )
        && ( _coord.getX() <= shutterCoordPair.first + m_clickCatchRange );
}

//---------------------------------------------------------------------------------------------------------------

bool ViewportRangeSelector::mouseOnShutterRight( ::scene2D::data::Coord _coord)
{
    std::pair< double, double > shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene( std::pair< double, double >( m_shutter->rect().x(), m_shutter->rect().y() ), m_xAxis, m_yAxis );

    double shutterRightPos = shutterCoordPair.first + m_shutter->rect().width() * m_xAxis->getScale();

    return ( _coord.getX() >=  shutterRightPos - m_clickCatchRange )
        && ( _coord.getX() <=  shutterRightPos + m_clickCatchRange );
}

//---------------------------------------------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D
