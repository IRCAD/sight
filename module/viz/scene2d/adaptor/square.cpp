/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "module/viz/scene2d/adaptor/square.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <QGraphicsItemGroup>

namespace sight::module::viz::scene2d::adaptor
{

const core::com::slots::key_t square::SET_DOUBLE_PARAMETER_SLOT = "set_double_parameter";
//-----------------------------------------------------------------------------

square::square() noexcept
{
    new_slot(SET_DOUBLE_PARAMETER_SLOT, &square::set_double_parameter, this);
}

//-----------------------------------------------------------------------------

square::~square() noexcept =
    default;

//-----------------------------------------------------------------------------

void square::configuring()
{
    this->configure_params();

    const config_t config = this->get_config().get_child("config.<xmlattr>");

    SIGHT_ASSERT("Attribute 'x' is missing", config.count("x"));
    SIGHT_ASSERT("Attribute 'y' is missing", config.count("y"));
    SIGHT_ASSERT("Attribute 'size' is missing", config.count("size"));

    m_coord.x      = config.get<double>("x");
    m_coord.y      = config.get<double>("y");
    m_size         = config.get<std::uint32_t>("size");
    m_auto_refresh = config.get<bool>("autoRefresh", m_auto_refresh);
    m_interaction  = config.get<bool>("interaction", m_interaction);

    if(config.count("color") != 0U)
    {
        this->set_color(config.get<std::string>("color"));
    }
}

//-----------------------------------------------------------------------------

void square::starting()
{
    m_layer = new QGraphicsItemGroup();

    m_rec = new QGraphicsRectItem(m_coord.x, m_coord.y, m_size, m_size);
    QPen pen;
    pen.setColor(Qt::GlobalColor(Qt::black));
    m_rec->setPen(pen);
    m_rec->setBrush(m_color);
    m_layer->addToGroup(m_rec);
    m_layer->setOpacity(0.8);

    m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
    m_layer->setZValue(m_z_value);
    this->get_scene_2d_render()->get_scene()->addItem(m_layer);
}

//-----------------------------------------------------------------------------

void square::updating()
{
    m_rec->setPos(m_coord.x, m_coord.y);
}

//-----------------------------------------------------------------------------

void square::stopping()
{
    this->get_scene_2d_render()->get_scene()->removeItem(m_layer);
}

//-----------------------------------------------------------------------------

void square::set_color(const std::string& _color)
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

void square::process_interaction(sight::viz::scene2d::data::event& _event)
{
    if(m_interaction)
    {
        if(_event.type() == sight::viz::scene2d::data::event::mouse_button_press
           && _event.get_button() == sight::viz::scene2d::data::event::left_button)
        {
            if(this->coord_view_is_in_item(_event.get_coord(), m_rec))
            {
                m_point_is_captured = true;
                m_old_coord         = this->coord_view_to_coord_item(_event.get_coord(), m_rec);
                m_rec->setBrush(Qt::yellow);
                _event.set_accepted(true);
            }
        }
        else if(m_point_is_captured && _event.type() == sight::viz::scene2d::data::event::mouse_move)
        {
            sight::viz::scene2d::vec2d_t new_coord = this->coord_view_to_coord_item(_event.get_coord(), m_rec);
            m_rec->moveBy(new_coord.x - m_old_coord.x, new_coord.y - m_old_coord.y);
            m_old_coord = new_coord;
            _event.set_accepted(true);
        }
        else if(m_point_is_captured && _event.type() == sight::viz::scene2d::data::event::mouse_button_release)
        {
            m_rec->setBrush(m_color);
            m_point_is_captured = false;
            _event.set_accepted(true);
        }
    }
}

//-----------------------------------------------------------------------------

bool square::coord_view_is_in_item(const sight::viz::scene2d::vec2d_t& _coord, QGraphicsItem* _item)
{
    sight::viz::scene2d::vec2d_t scene_point = this->get_scene_2d_render()->map_to_scene(_coord);
    QPointF sp(scene_point.x, scene_point.y);
    QPointF ip = _item->mapFromScene(sp);
    return _item->contains(ip);
}

//-----------------------------------------------------------------------------

sight::viz::scene2d::vec2d_t square::coord_view_to_coord_item(
    const sight::viz::scene2d::vec2d_t& _coord,
    QGraphicsItem*
    /*unused*/
)
{
    sight::viz::scene2d::vec2d_t scene_point = this->get_scene_2d_render()->map_to_scene(_coord);
    //QPointF sp ( scenePoint.x, scenePoint.getY() );
    //QPointF ip = item->mapFromScene( sp );
    //return sight::viz::scene2d::vec2d_t( ip.x(), ip.y() );
    return scene_point;
}

//-----------------------------------------------------------------------------

void square::set_double_parameter(const double _val, std::string _key)
{
    this->configure_params();
    if(_key == "X")
    {
        m_coord.x = _val;
    }
    else if(_key == "Y")
    {
        m_coord.y = _val;
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }

    if(m_auto_refresh)
    {
        this->updating();
    }
}

} // namespace sight::module::viz::scene2d::adaptor
