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

#include "fwRenderQt/Scene2DGraphicsView.hpp"

#include "fwRenderQt/data/Size.hpp"
#include "fwRenderQt/SRender.hpp"

#include <QMouseEvent>

namespace fwRenderQt
{

//-----------------------------------------------------------------------------

Scene2DGraphicsView::Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget) :
    QGraphicsView(scene, widget)
{
    if( !this->hasMouseTracking() )
    {
        this->setMouseTracking( true );
    }

    /* Change the method used by the scene to index its items:
     * 'NoIndex' is better than the default method 'BspTreeIndex' (Binary Space Partioning)
     * which consists in dividing the scene into multiple parts and store them into a tree.
     * With 'NoIndex', searching an item is faster, especially for scenes with a lot of items
     * (Histogram for instance) and for scenes which manage moves (ViewportRangeSelector and
     * ViewportUpdater for instance).
     */
    scene->setItemIndexMethod( QGraphicsScene::NoIndex );

    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    this->setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );
    this->setOptimizationFlags( QGraphicsView::DontSavePainterState
                                | QGraphicsView::DontAdjustForAntialiasing ); // Prevent from rendering artifacts
    this->setCacheMode( QGraphicsView::CacheBackground );   // Activates background cache
    this->setAttribute( Qt::WA_TranslucentBackground, false );
    this->setFrameStyle( 0 );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::setSceneRender( ::fwRenderQt::SRender::sptr sceneRender )
{
    m_scene2DRender = sceneRender;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyPressEvent(QKeyEvent* _event)
{
    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::KeyPress);
    sceneEvent.setButton( ::fwRenderQtdata::Event::NoButton);
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyReleaseEvent(QKeyEvent* _event)
{
    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::KeyRelease);
    sceneEvent.setButton( ::fwRenderQtdata::Event::NoButton);
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::resizeEvent(QResizeEvent* _event)
{
    this->updateFromViewport();

    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::Resize);
    sceneEvent.setButton( ::fwRenderQtdata::Event::NoButton);
    sceneEvent.setModifier( ::fwRenderQtdata::Event::NoModifier);
    sceneEvent.setSize( ::fwRenderQtdata::Size( _event->size().width(), _event->size().height() ) );
    sceneEvent.setOldSize( ::fwRenderQtdata::Size( _event->oldSize().width(), _event->oldSize().height() ) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mousePressEvent ( QMouseEvent* _event )
{

    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::MouseButtonPress );
    sceneEvent.setCoord( ::fwRenderQtdata::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

::fwRenderQtdata::Event::Modifier Scene2DGraphicsView::getScene2DModifierFromEvent( QInputEvent* _event )
{
    ::fwRenderQtdata::Event::Modifier modifier;

    if(_event->modifiers() == Qt::ControlModifier)
    {
        modifier = ::fwRenderQtdata::Event::ControlModifier;
    }
    else if(_event->modifiers() == Qt::AltModifier)
    {
        modifier = ::fwRenderQtdata::Event::AltModifier;
    }
    else if(_event->modifiers() == Qt::ShiftModifier)
    {
        modifier = ::fwRenderQtdata::Event::ShiftModifier;
    }
    else
    {
        modifier = ::fwRenderQtdata::Event::NoModifier;
    }

    // TODO: add support for combined modifiers
    return modifier;
}

//-----------------------------------------------------------------------------

::fwRenderQtdata::Event::Button Scene2DGraphicsView::getScene2DButtonFromEvent( QMouseEvent* _event )
{
    ::fwRenderQtdata::Event::Button button;

    if( _event->button() == Qt::LeftButton )
    {
        button = ::fwRenderQtdata::Event::LeftButton;
    }
    else if( _event->button() == Qt::RightButton )
    {
        button = ::fwRenderQtdata::Event::RightButton;
    }
    else if( _event->button() == Qt::MidButton )
    {
        button = ::fwRenderQtdata::Event::MidButton;
    }
    else
    {
        button = ::fwRenderQtdata::Event::NoButton;
    }

    return button;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseDoubleClickEvent ( QMouseEvent* _event )
{

    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::MouseButtonDoubleClick );
    sceneEvent.setCoord( ::fwRenderQtdata::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseReleaseEvent ( QMouseEvent* _event )
{
    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::MouseButtonRelease );
    sceneEvent.setCoord( ::fwRenderQtdata::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseMoveEvent ( QMouseEvent* _event )
{
    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQtdata::Event::MouseMove );
    sceneEvent.setCoord( ::fwRenderQtdata::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::wheelEvent ( QWheelEvent* _event )
{
    const bool scrollUp = _event->delta() > 0;
    ::fwRenderQtdata::Event sceneEvent;
    sceneEvent.setType( (scrollUp) ? ::fwRenderQtdata::Event::MouseWheelUp : ::fwRenderQtdata::Event::MouseWheelDown);
    sceneEvent.setCoord( ::fwRenderQtdata::Coord( _event->pos().x(), _event->pos().y() ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::setViewport( ::fwRenderQtdata::Viewport::sptr viewport )
{
    m_viewport = viewport;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::updateFromViewport()
{
    SLM_ASSERT("Viewport is not set", m_viewport);

    this->fitInView(m_viewport->getX(), m_viewport->getY(),
                    m_viewport->getWidth(), m_viewport->getHeight(),
                    m_scene2DRender.lock()->getAspectRatioMode() );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::paintEvent(QPaintEvent* event)
{
    // QGraphicsView optimization
    QPaintEvent* newEvent = new QPaintEvent( event->region().boundingRect() );
    QGraphicsView::paintEvent(newEvent);

    delete newEvent;
}

//-----------------------------------------------------------------------------

} // namespace fwRenderQt
