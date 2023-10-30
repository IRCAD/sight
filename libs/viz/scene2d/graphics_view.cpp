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

#include "graphics_view.hpp"

#include "viz/scene2d/render.hpp"

#include <QMouseEvent>

namespace sight::viz::scene2d
{

//-----------------------------------------------------------------------------

graphics_view::graphics_view(QGraphicsScene* _scene, QWidget* _widget) :
    QGraphicsView(_scene, _widget)
{
    if(!this->hasMouseTracking())
    {
        this->setMouseTracking(true);
    }

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setFrameStyle(0);
}

//-----------------------------------------------------------------------------

void graphics_view::set_scene_render(viz::scene2d::render::sptr _scene_render)
{
    m_scene_2d_render = _scene_render;
}

//-----------------------------------------------------------------------------

void graphics_view::keyPressEvent(QKeyEvent* _event)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::key_press);
    scene_event.set_button(scene2d::data::event::no_button);
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));
    scene_event.set_key(_event->key());

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::keyReleaseEvent(QKeyEvent* _event)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::key_release);
    scene_event.set_button(scene2d::data::event::no_button);
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));
    scene_event.set_key(_event->key());

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::resizeEvent(QResizeEvent* _event)
{
    this->update_from_viewport(m_viewport);

    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::resize);
    scene_event.set_button(scene2d::data::event::no_button);
    scene_event.set_modifier(scene2d::data::event::no_modifier);
    scene_event.set_size(scene2d::vec2d_t(_event->size().width(), _event->size().height()));
    scene_event.set_old_size(scene2d::vec2d_t(_event->oldSize().width(), _event->oldSize().height()));

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::mousePressEvent(QMouseEvent* _event)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::mouse_button_press);
    scene_event.set_coord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.set_button(sight::viz::scene2d::graphics_view::get_scene_2d_button_from_event(_event));
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::mouseDoubleClickEvent(QMouseEvent* _event)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::mouse_button_double_click);
    scene_event.set_coord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.set_button(sight::viz::scene2d::graphics_view::get_scene_2d_button_from_event(_event));
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::mouseReleaseEvent(QMouseEvent* _event)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::mouse_button_release);
    scene_event.set_coord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.set_button(sight::viz::scene2d::graphics_view::get_scene_2d_button_from_event(_event));
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::mouseMoveEvent(QMouseEvent* _event)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::mouse_move);
    scene_event.set_coord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.set_button(sight::viz::scene2d::graphics_view::get_scene_2d_button_from_event(_event));
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::wheelEvent(QWheelEvent* _event)
{
    const bool scroll_up = _event->angleDelta().y() > 0;
    scene2d::data::event scene_event;
    scene_event.set_type((scroll_up) ? scene2d::data::event::mouse_wheel_up : scene2d::data::event::mouse_wheel_down);
    scene_event.set_coord(scene2d::vec2d_t(_event->position().x(), _event->position().y()));
    scene_event.set_modifier(sight::viz::scene2d::graphics_view::get_scene_2d_modifier_from_event(_event));

    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::enterEvent(QEvent* /*event*/)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::enter_event);
    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//-----------------------------------------------------------------------------

void graphics_view::leaveEvent(QEvent* /*event*/)
{
    scene2d::data::event scene_event;
    scene_event.set_type(scene2d::data::event::leave_event);
    m_scene_2d_render.lock()->dispatch_interaction(scene_event);
}

//------------------------------------------------------------------------------

QSize graphics_view::sizeHint() const
{
    return this->minimumSizeHint();
}

//-----------------------------------------------------------------------------

void graphics_view::update_from_viewport(const scene2d::data::viewport& _viewport)
{
    this->fitInView(
        _viewport.x(),
        _viewport.y(),
        _viewport.width(),
        _viewport.height(),
        m_scene_2d_render.lock()->get_aspect_ratio_mode()
    );

    m_viewport.set_x(_viewport.x());
    m_viewport.set_y(_viewport.y());
    m_viewport.set_width(_viewport.width());
    m_viewport.set_height(_viewport.height());
}

//-----------------------------------------------------------------------------

scene2d::data::event::modifier graphics_view::get_scene_2d_modifier_from_event(QInputEvent* _event)
{
    switch(_event->modifiers())
    {
        case Qt::ControlModifier:
            return scene2d::data::event::control_modifier;

        case Qt::AltModifier:
            return scene2d::data::event::alt_modifier;

        case Qt::ShiftModifier:
            return scene2d::data::event::shift_modifier;

        default:
            return scene2d::data::event::no_modifier;
    }
}

//-----------------------------------------------------------------------------

scene2d::data::event::button graphics_view::get_scene_2d_button_from_event(QMouseEvent* _event)
{
    switch(_event->button())
    {
        case Qt::LeftButton:
            return scene2d::data::event::left_button;

        case Qt::RightButton:
            return scene2d::data::event::right_button;

        case Qt::MiddleButton:
            return scene2d::data::event::mid_button;

        default:
            return scene2d::data::event::no_button;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
