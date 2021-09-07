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

#include "Scene2DGraphicsView.hpp"

#include "viz/scene2d/data/Size.hpp"
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

    /* Change the method used by the scene to index its items:
     * 'NoIndex' is better than the default method 'BspTreeIndex' (Binary Space Partioning)
     * which consists in dividing the scene into multiple parts and store them into a tree.
     * With 'NoIndex', searching an item is faster, especially for scenes with a lot of items
     * (Histogram for instance) and for scenes which manage moves (ViewportRangeSelector and
     * ViewportUpdater for instance).
     */
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setOptimizationFlags(
        QGraphicsView::DontSavePainterState
        | QGraphicsView::DontAdjustForAntialiasing
    );                                                  // Prevent from rendering artifacts
    this->setCacheMode(QGraphicsView::CacheBackground); // Activates background cache
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
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyReleaseEvent(QKeyEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::KeyRelease);
    sceneEvent.setButton(scene2d::data::Event::NoButton);
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::resizeEvent(QResizeEvent* _event)
{
    this->updateFromViewport();

    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::Resize);
    sceneEvent.setButton(scene2d::data::Event::NoButton);
    sceneEvent.setModifier(scene2d::data::Event::NoModifier);
    sceneEvent.setSize(scene2d::data::Size(_event->size().width(), _event->size().height()));
    sceneEvent.setOldSize(scene2d::data::Size(_event->oldSize().width(), _event->oldSize().height()));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mousePressEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseButtonPress);
    sceneEvent.setCoord(scene2d::data::Coord(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(this->getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

scene2d::data::Event::Modifier Scene2DGraphicsView::getScene2DModifierFromEvent(QInputEvent* _event)
{
    scene2d::data::Event::Modifier modifier;

    if(_event->modifiers() == Qt::ControlModifier)
    {
        modifier = scene2d::data::Event::ControlModifier;
    }
    else if(_event->modifiers() == Qt::AltModifier)
    {
        modifier = scene2d::data::Event::AltModifier;
    }
    else if(_event->modifiers() == Qt::ShiftModifier)
    {
        modifier = scene2d::data::Event::ShiftModifier;
    }
    else
    {
        modifier = scene2d::data::Event::NoModifier;
    }

    // TODO: add support for combined modifiers
    return modifier;
}

//-----------------------------------------------------------------------------

scene2d::data::Event::Button Scene2DGraphicsView::getScene2DButtonFromEvent(QMouseEvent* _event)
{
    scene2d::data::Event::Button button;

    if(_event->button() == Qt::LeftButton)
    {
        button = scene2d::data::Event::LeftButton;
    }
    else if(_event->button() == Qt::RightButton)
    {
        button = scene2d::data::Event::RightButton;
    }
    else if(_event->button() == Qt::MidButton)
    {
        button = scene2d::data::Event::MidButton;
    }
    else
    {
        button = scene2d::data::Event::NoButton;
    }

    return button;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseDoubleClickEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseButtonDoubleClick);
    sceneEvent.setCoord(scene2d::data::Coord(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(this->getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseButtonRelease);
    sceneEvent.setCoord(scene2d::data::Coord(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(this->getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
    scene2d::data::Event sceneEvent;
    sceneEvent.setType(scene2d::data::Event::MouseMove);
    sceneEvent.setCoord(scene2d::data::Coord(_event->localPos().x(), _event->localPos().y()));
    sceneEvent.setButton(this->getScene2DButtonFromEvent(_event));
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::wheelEvent(QWheelEvent* _event)
{
    const bool scrollUp = _event->delta() > 0;
    scene2d::data::Event sceneEvent;
    sceneEvent.setType((scrollUp) ? scene2d::data::Event::MouseWheelUp : scene2d::data::Event::MouseWheelDown);
    sceneEvent.setCoord(scene2d::data::Coord(_event->pos().x(), _event->pos().y()));
    sceneEvent.setModifier(this->getScene2DModifierFromEvent(_event));

    m_scene2DRender.lock()->dispatchInteraction(sceneEvent);
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::setViewport(scene2d::data::Viewport::sptr viewport)
{
    m_viewport = viewport;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::updateFromViewport()
{
    SIGHT_ASSERT("Viewport is not set", m_viewport);

    this->fitInView(
        m_viewport->getX(),
        m_viewport->getY(),
        m_viewport->getWidth(),
        m_viewport->getHeight(),
        m_scene2DRender.lock()->getAspectRatioMode()
    );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::paintEvent(QPaintEvent* event)
{
    // QGraphicsView optimization
    QPaintEvent* newEvent = new QPaintEvent(event->region().boundingRect());
    QGraphicsView::paintEvent(newEvent);

    delete newEvent;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
