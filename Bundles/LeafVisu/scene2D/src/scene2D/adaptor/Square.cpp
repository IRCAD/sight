/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Composite.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/adaptor/Square.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Square  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

//-----------------------------------------------------------------------------

Square::Square() throw() : m_pointIsCaptured (false)
{}

//-----------------------------------------------------------------------------

Square::~Square() throw()
{}

//-----------------------------------------------------------------------------

void Square::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    m_coord.setX( ::boost::lexical_cast< double >( m_configuration->getAttributeValue("x") ) );
    m_coord.setY( ::boost::lexical_cast< double >( m_configuration->getAttributeValue("y") ) );
    m_size = ::boost::lexical_cast< ::boost::uint32_t >( m_configuration->getAttributeValue("size") );
    if ( m_configuration->hasAttribute("color") )
    {
        this->setColor(m_configuration->getAttributeValue("color"));
    }
}

//-----------------------------------------------------------------------------

void Square::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

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

void Square::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Square::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Square::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Square::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
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

void Square::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();
    if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress && _event->getButton() == ::scene2D::data::Event::LeftButton )
    {
        if ( this->coordViewIsInItem( _event->getCoord(), m_rec ) )
        {
            OSLM_TRACE("Point is captured");
            m_pointIsCaptured = true;
            m_oldCoord = this->coordViewToCoordItem( _event->getCoord(), m_rec );
            m_rec->setBrush( Qt::yellow );
            _event->setAccepted(true);
        }
    }
    else if ( m_pointIsCaptured && _event->getType() == ::scene2D::data::Event::MouseMove )
    {
        ::scene2D::data::Coord newCoord = this->coordViewToCoordItem( _event->getCoord(), m_rec );
        m_rec->moveBy( newCoord.getX() - m_oldCoord.getX(), newCoord.getY() - m_oldCoord.getY() );
        m_oldCoord = newCoord;
        _event->setAccepted(true);
    }
    else if ( m_pointIsCaptured && _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
    {
        m_rec->setBrush( m_color );
        m_pointIsCaptured = false;
        _event->setAccepted(true);
    }

}

//-----------------------------------------------------------------------------

bool Square::coordViewIsInItem( const ::scene2D::data::Coord & coord, QGraphicsItem * item )
{
    ::scene2D::data::Coord scenePoint = this->getScene2DRender()->mapToScene( coord );
    QPointF sp ( scenePoint.getX(), scenePoint.getY() );
    QPointF ip = item->mapFromScene( sp );
    return item->contains( ip );
}

//-----------------------------------------------------------------------------

::scene2D::data::Coord Square::coordViewToCoordItem( const ::scene2D::data::Coord & coord, QGraphicsItem * item )
{
    ::scene2D::data::Coord scenePoint = this->getScene2DRender()->mapToScene( coord );
    //QPointF sp ( scenePoint.getX(), scenePoint.getY() );
    //QPointF ip = item->mapFromScene( sp );
    //return ::scene2D::data::Coord( ip.x(), ip.y() );
    return scenePoint;
}
//-----------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D



