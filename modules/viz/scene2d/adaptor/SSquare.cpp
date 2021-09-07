/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene2d/adaptor/SSquare.hpp"

#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <QGraphicsItemGroup>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

const core::com::Slots::SlotKeyType SSquare::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
//-----------------------------------------------------------------------------

SSquare::SSquare() noexcept
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

    SIGHT_ASSERT("Attribute 'x' is missing", config.count("x"));
    SIGHT_ASSERT("Attribute 'y' is missing", config.count("y"));
    SIGHT_ASSERT("Attribute 'size' is missing", config.count("size"));

    m_coord.setX(config.get<double>("x"));
    m_coord.setY(config.get<double>("y"));
    m_size        = config.get<std::uint32_t>("size");
    m_autoRefresh = config.get<bool>("autoRefresh", m_autoRefresh);
    m_interaction = config.get<bool>("interaction", m_interaction);

    if(config.count("color"))
    {
        this->setColor(config.get<std::string>("color"));
    }
}

//-----------------------------------------------------------------------------

void SSquare::starting()
{
    m_layer = new QGraphicsItemGroup();

    m_rec = new QGraphicsRectItem(m_coord.getX(), m_coord.getY(), m_size, m_size);
    QPen pen;
    pen.setColor(Qt::GlobalColor(Qt::black));
    m_rec->setPen(pen);
    m_rec->setBrush(m_color);
    m_layer->addToGroup(m_rec);
    m_layer->setOpacity(0.8);

    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//-----------------------------------------------------------------------------

void SSquare::updating()
{
    m_rec->setPos(m_coord.getX(), m_coord.getY());
}

//-----------------------------------------------------------------------------

void SSquare::stopping()
{
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//-----------------------------------------------------------------------------

void SSquare::setColor(const std::string& _color)
{
    m_color = QColor(QString::fromStdString(_color));
    if(!m_color.isValid())
    {
        // Default value: black (if the color id cannot be parsed)
        m_color = Qt::GlobalColor(Qt::white);
        SIGHT_WARN("Color " + _color + " unknown, default value used.");
    }
}

//-----------------------------------------------------------------------------

void SSquare::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(m_interaction)
    {
        if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress
           && _event.getButton() == sight::viz::scene2d::data::Event::LeftButton)
        {
            if(this->coordViewIsInItem(_event.getCoord(), m_rec))
            {
                m_pointIsCaptured = true;
                m_oldCoord        = this->coordViewToCoordItem(_event.getCoord(), m_rec);
                m_rec->setBrush(Qt::yellow);
                _event.setAccepted(true);
            }
        }
        else if(m_pointIsCaptured && _event.getType() == sight::viz::scene2d::data::Event::MouseMove)
        {
            sight::viz::scene2d::data::Coord newCoord = this->coordViewToCoordItem(_event.getCoord(), m_rec);
            m_rec->moveBy(newCoord.getX() - m_oldCoord.getX(), newCoord.getY() - m_oldCoord.getY());
            m_oldCoord = newCoord;
            _event.setAccepted(true);
        }
        else if(m_pointIsCaptured && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
        {
            m_rec->setBrush(m_color);
            m_pointIsCaptured = false;
            _event.setAccepted(true);
        }
    }
}

//-----------------------------------------------------------------------------

bool SSquare::coordViewIsInItem(const sight::viz::scene2d::data::Coord& coord, QGraphicsItem* item)
{
    sight::viz::scene2d::data::Coord scenePoint = this->getScene2DRender()->mapToScene(coord);
    QPointF sp(scenePoint.getX(), scenePoint.getY());
    QPointF ip = item->mapFromScene(sp);
    return item->contains(ip);
}

//-----------------------------------------------------------------------------

sight::viz::scene2d::data::Coord SSquare::coordViewToCoordItem(
    const sight::viz::scene2d::data::Coord& coord,
    QGraphicsItem*
)
{
    sight::viz::scene2d::data::Coord scenePoint = this->getScene2DRender()->mapToScene(coord);
    //QPointF sp ( scenePoint.getX(), scenePoint.getY() );
    //QPointF ip = item->mapFromScene( sp );
    //return sight::viz::scene2d::data::Coord( ip.x(), ip.y() );
    return scenePoint;
}

//-----------------------------------------------------------------------------

void SSquare::setDoubleParameter(const double _val, std::string _key)
{
    this->configureParams();
    if(_key == "X")
    {
        m_coord.setX(_val);
    }
    else if(_key == "Y")
    {
        m_coord.setY(_val);
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }

    if(m_autoRefresh)
    {
        this->updating();
    }
}

} // namespace adaptor

} // namespace sight::module::viz::scene2d
