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

#include "Scene2DGraphicsView.hpp"

#include "viz/scene2d/render.hpp"

#include <QMouseEvent>

namespace sight::viz::scene2d
{

//-----------------------------------------------------------------------------

Scene2DGraphicsView::Scene2DGraphicsView(QGraphicsScene* _scene, QWidget* _widget) :
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

void Scene2DGraphicsView::setSceneRender(viz::scene2d::render::sptr _scene_render)
{
    m_scene2DRender = _scene_render;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyPressEvent(QKeyEvent* _event)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::KeyPress);
    scene_event.setButton(scene2d::data::Event::NoButton);
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));
    scene_event.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyReleaseEvent(QKeyEvent* _event)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::KeyRelease);
    scene_event.setButton(scene2d::data::Event::NoButton);
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));
    scene_event.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::resizeEvent(QResizeEvent* _event)
{
    this->updateFromViewport(m_viewport);

    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::Resize);
    scene_event.setButton(scene2d::data::Event::NoButton);
    scene_event.setModifier(scene2d::data::Event::NoModifier);
    scene_event.setSize(scene2d::vec2d_t(_event->size().width(), _event->size().height()));
    scene_event.setOldSize(scene2d::vec2d_t(_event->oldSize().width(), _event->oldSize().height()));

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mousePressEvent(QMouseEvent* _event)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::MouseButtonPress);
    scene_event.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseDoubleClickEvent(QMouseEvent* _event)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::MouseButtonDoubleClick);
    scene_event.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::MouseButtonRelease);
    scene_event.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::MouseMove);
    scene_event.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    scene_event.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::wheelEvent(QWheelEvent* _event)
{
    const bool scroll_up = _event->angleDelta().y() > 0;
    scene2d::data::Event scene_event;
    scene_event.setType((scroll_up) ? scene2d::data::Event::MouseWheelUp : scene2d::data::Event::MouseWheelDown);
    scene_event.setCoord(scene2d::vec2d_t(_event->position().x(), _event->position().y()));
    scene_event.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::enterEvent(QEvent* /*event*/)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::EnterEvent);
    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::leaveEvent(QEvent* /*event*/)
{
    scene2d::data::Event scene_event;
    scene_event.setType(scene2d::data::Event::LeaveEvent);
    m_scene2DRender.lock()->dispatchInteraction(scene_event);
}

//------------------------------------------------------------------------------

QSize Scene2DGraphicsView::sizeHint() const
{
    return this->minimumSizeHint();
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::updateFromViewport(const scene2d::data::Viewport& _viewport)
{
    this->fitInView(
        _viewport.x(),
        _viewport.y(),
        _viewport.width(),
        _viewport.height(),
        m_scene2DRender.lock()->getAspectRatioMode()
    );

    m_viewport.setX(_viewport.x());
    m_viewport.setY(_viewport.y());
    m_viewport.setWidth(_viewport.width());
    m_viewport.setHeight(_viewport.height());
}

//-----------------------------------------------------------------------------

scene2d::data::Event::Modifier Scene2DGraphicsView::getScene2DModifierFromEvent(QInputEvent* _event)
{
    switch(_event->modifiers())
    {
        case Qt::ControlModifier:
            return scene2d::data::Event::ControlModifier;

        case Qt::AltModifier:
            return scene2d::data::Event::AltModifier;

        case Qt::ShiftModifier:
            return scene2d::data::Event::ShiftModifier;

        default:
            return scene2d::data::Event::NoModifier;
    }
}

//-----------------------------------------------------------------------------

scene2d::data::Event::Button Scene2DGraphicsView::getScene2DButtonFromEvent(QMouseEvent* _event)
{
    switch(_event->button())
    {
        case Qt::LeftButton:
            return scene2d::data::Event::LeftButton;

        case Qt::RightButton:
            return scene2d::data::Event::RightButton;

        case Qt::MiddleButton:
            return scene2d::data::Event::MidButton;

        default:
            return scene2d::data::Event::NoButton;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
