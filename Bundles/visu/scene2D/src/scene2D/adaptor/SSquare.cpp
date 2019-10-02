/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "scene2D/adaptor/SSquare.hpp"

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>
#include <fwCom/Slots.hxx>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::SSquare);

namespace scene2D
{
namespace adaptor
{
const ::fwCom::Slots::SlotKeyType SSquare::s_SET_DOUBLE_PARAMETER_SLOT   = "setDoubleParameter";
//-----------------------------------------------------------------------------

SSquare::SSquare() noexcept :
    m_size(0),
    m_layer(nullptr),
    m_rec(nullptr),
    m_pointIsCaptured(false)
{
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SSquare::setDoubleParameter, this);
}

//-----------------------------------------------------------------------------

SSquare::~SSquare() noexcept
{
}

//-----------------------------------------------------------------------------

void SSquare::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    SLM_ASSERT("Attribute 'x' is missing", config.count("x"));
    SLM_ASSERT("Attribute 'y' is missing", config.count("y"));
    SLM_ASSERT("Attribute 'size' is missing", config.count("size"));

    m_coord.setX( config.get<double>("x") );
    m_coord.setY( config.get<double>("y") );
    m_size = config.get<std::uint32_t>("size");

    if ( config.count("color") )
    {
        this->setColor(config.get<std::string>("color"));
    }
}

//-----------------------------------------------------------------------------

void SSquare::starting()
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

void SSquare::updating()
{
     this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//-----------------------------------------------------------------------------

void SSquare::stopping()
{
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//-----------------------------------------------------------------------------

void SSquare::setColor(const std::string& _color )
{
    m_color = QColor(QString::fromStdString(_color));
    if (!m_color.isValid())
    {
        // Default value: black (if the color id cannot be parsed)
        m_color = Qt::GlobalColor(Qt::white);
        SLM_WARN("Color "+_color+" unknown, default value used.");
    }
}

//-----------------------------------------------------------------------------

void SSquare::processInteraction( ::fwRenderQt::data::Event& _event )
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

bool SSquare::coordViewIsInItem( const ::fwRenderQt::data::Coord& coord, QGraphicsItem* item )
{
    ::fwRenderQt::data::Coord scenePoint = this->getScene2DRender()->mapToScene( coord );
    QPointF sp( scenePoint.getX(), scenePoint.getY() );
    QPointF ip = item->mapFromScene( sp );
    return item->contains( ip );
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Coord SSquare::coordViewToCoordItem( const ::fwRenderQt::data::Coord& coord, QGraphicsItem* item )
{
    ::fwRenderQt::data::Coord scenePoint = this->getScene2DRender()->mapToScene( coord );
    //QPointF sp ( scenePoint.getX(), scenePoint.getY() );
    //QPointF ip = item->mapFromScene( sp );
    //return ::fwRenderQt::data::Coord( ip.x(), ip.y() );
    return scenePoint;
}
//-----------------------------------------------------------------------------

void SSquare::setDoubleParameter(const double _val, std::string _key)
{
    this->configureParams();
    if(_key == "minDepth")
    {
      m_coord.setX(_val);
    }
    else if(_key == "maxDepth")
    {
     m_coord.setY(_val);
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
    this->updating();
    this->starting();
}
} // namespace adaptor
} // namespace scene2D

