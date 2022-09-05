/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "viz/scene2d/SRender.hpp"

#include <QMouseEvent>

namespace sight::viz::scene2d
{

//-----------------------------------------------------------------------------

Scene2DGraphicsView::Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget) :
    QGraphicsView(scene, widget)
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

void Scene2DGraphicsView::setSceneRender(viz::scene2d::SRender::sptr sceneRender)
{
    m_scene2DRender = sceneRender;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyPressEvent(QKeyEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::KeyPress);
    sceneEvent.setButton(scene2d::data::Event::NoButton);
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyReleaseEvent(QKeyEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::KeyRelease);
    sceneEvent.setButton(scene2d::data::Event::NoButton);
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::resizeEvent(QResizeEvent* _event)
{
    this->updateFromViewport(m_viewport);

    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::Resize);
    sceneEvent.setButton(scene2d::data::Event::NoButton);
    sceneEvent.setModifier(scene2d::data::Event::NoModifier);
    sceneEvent.setSize(scene2d::vec2d_t(_event->size().width(), _event->size().height()));
    sceneEvent.setOldSize(scene2d::vec2d_t(_event->oldSize().width(), _event->oldSize().height()));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mousePressEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseButtonPress);
    sceneEvent.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseDoubleClickEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseButtonDoubleClick);
    sceneEvent.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseButtonRelease);
    sceneEvent.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseMove);
    sceneEvent.setCoord(scene2d::vec2d_t(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(sight::viz::scene2d::Scene2DGraphicsView::getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::wheelEvent(QWheelEvent* _event)
{
    const bool scrollUp = _event->angleDelta().y() > 0;
    scene2d::data::Event sceneEvent;
    sceneEvent.setType((scrollUp) ? scene2d::data::Event::MouseWheelUp : scene2d::data::Event::MouseWheelDown);
    sceneEvent.setCoord(scene2d::vec2d_t(_event->position().x(), _event->position().y()));
    sceneEvent.setModifier(sight::viz::scene2d::Scene2DGraphicsView::getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::enterEvent(QEvent* /*event*/)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::EnterEvent);
    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::leaveEvent(QEvent* /*event*/)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::LeaveEvent);
    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//------------------------------------------------------------------------------

QSize Scene2DGraphicsView::sizeHint() const
{
    return this->minimumSizeHint();
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::updateFromViewport(const scene2d::data::Viewport& viewport)
{
    this->fitInView(
        viewport.x(),
        viewport.y(),
        viewport.width(),
        viewport.height(),
        m_scene2DRender.lock()->getAspectRatioMode()
    );

    m_viewport.setX(viewport.x());
    m_viewport.setY(viewport.y());
    m_viewport.setWidth(viewport.width());
    m_viewport.setHeight(viewport.height());
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
