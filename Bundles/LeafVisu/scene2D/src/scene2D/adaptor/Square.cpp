/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/Square.hpp"

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::Square, ::fwData::Composite );

namespace scene2D
{
namespace adaptor
{

//-----------------------------------------------------------------------------

Square::Square() noexcept :
    m_size(0),
    m_layer(nullptr),
    m_rec(nullptr),
    m_pointIsCaptured(false)
{
}

//-----------------------------------------------------------------------------

Square::~Square() noexcept
{
}

//-----------------------------------------------------------------------------

void Square::configuring()
{
    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    m_coord.setX( std::stod( m_configuration->getAttributeValue("x") ) );
    m_coord.setY( std::stod( m_configuration->getAttributeValue("y") ) );
    m_size = std::stoi( m_configuration->getAttributeValue("size") );
    if ( m_configuration->hasAttribute("color") )
    {
        this->setColor(m_configuration->getAttributeValue("color"));
    }
}

//-----------------------------------------------------------------------------

void Square::doStart()
{
    m_layer = new QGraphicsItemGroup();

    m_rec = new QGraphicsRectItem( m_coord.getX(), m_coord.getY(), m_size, m_size );
    QPen pen;
    pen.setColor( Qt::GlobalColor( Qt::black ) );
    m_rec->setPen( pen );
    m_rec->setBrush( m_color );
    m_layer->addToGroup(m_rec);
    m_layer->setOpacity(0.8);

    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//-----------------------------------------------------------------------------

void Square::doUpdate()
{
}

//-----------------------------------------------------------------------------

void Square::doSwap()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Square::doStop()
{
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//-----------------------------------------------------------------------------

void Square::setColor( std::string _color )
{
    if (_color == "red")
    {
        m_color = Qt::GlobalColor(Qt::red);
    }
    else if (_color == "green")
    {
        m_color = Qt::GlobalColor(Qt::green);
    }
    else if (_color == "blue")
    {
        m_color = Qt::GlobalColor(Qt::blue);
    }
}

//-----------------------------------------------------------------------------

void Square::processInteraction( ::fwRenderQt::data::Event& _event )
{
    if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress &&
         _event.getButton() == ::fwRenderQt::data::Event::LeftButton )
    {
        if ( this->coordViewIsInItem( _event.getCoord(), m_rec ) )
        {
            SLM_TRACE("Point is captured");
            m_pointIsCaptured = true;
            m_oldCoord        = this->coordViewToCoordItem( _event.getCoord(), m_rec );
            m_rec->setBrush( Qt::yellow );
            _event.setAccepted(true);
        }
    }
    else if ( m_pointIsCaptured && _event.getType() == ::fwRenderQt::data::Event::MouseMove )
    {
        ::fwRenderQt::data::Coord newCoord = this->coordViewToCoordItem( _event.getCoord(), m_rec );
        m_rec->moveBy( newCoord.getX() - m_oldCoord.getX(), newCoord.getY() - m_oldCoord.getY() );
        m_oldCoord = newCoord;
        _event.setAccepted(true);
    }
    else if ( m_pointIsCaptured && _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease )
    {
        m_rec->setBrush( m_color );
        m_pointIsCaptured = false;
        _event.setAccepted(true);
    }

}

//-----------------------------------------------------------------------------

bool Square::coordViewIsInItem( const ::fwRenderQt::data::Coord& coord, QGraphicsItem* item )
{
    ::fwRenderQt::data::Coord scenePoint = this->getScene2DRender()->mapToScene( coord );
    QPointF sp( scenePoint.getX(), scenePoint.getY() );
    QPointF ip = item->mapFromScene( sp );
    return item->contains( ip );
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Coord Square::coordViewToCoordItem( const ::fwRenderQt::data::Coord& coord, QGraphicsItem* item )
{
    ::fwRenderQt::data::Coord scenePoint = this->getScene2DRender()->mapToScene( coord );
    //QPointF sp ( scenePoint.getX(), scenePoint.getY() );
    //QPointF ip = item->mapFromScene( sp );
    //return ::fwRenderQt::data::Coord( ip.x(), ip.y() );
    return scenePoint;
}
//-----------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D

