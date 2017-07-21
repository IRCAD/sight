/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderQt/Scene2DGraphicsView.hpp"

#include "fwRenderQt/data/Size.hpp"
#include "fwRenderQt/SRender.hpp"

#include <fwData/Composite.hpp>

#include <QMouseEvent>

namespace fwRenderQt
{

//-----------------------------------------------------------------------------

Scene2DGraphicsView::Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget) :
    QGraphicsView(scene, widget),
    m_viewport(nullptr)
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
    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::KeyPress);
    sceneEvent.setButton( ::fwRenderQt::data::Event::NoButton);
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::keyReleaseEvent(QKeyEvent* _event)
{
    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::KeyRelease);
    sceneEvent.setButton( ::fwRenderQt::data::Event::NoButton);
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );
    sceneEvent.setKey(_event->key());

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::resizeEvent(QResizeEvent* _event)
{
    this->updateFromViewport();

    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::Resize);
    sceneEvent.setButton( ::fwRenderQt::data::Event::NoButton);
    sceneEvent.setModifier( ::fwRenderQt::data::Event::NoModifier);
    sceneEvent.setSize( ::fwRenderQt::data::Size( _event->size().width(), _event->size().height() ) );
    sceneEvent.setOldSize( ::fwRenderQt::data::Size( _event->oldSize().width(), _event->oldSize().height() ) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mousePressEvent ( QMouseEvent* _event )
{
    OSLM_TRACE("Press in x = " <<  _event->localPos().x() << " y = " << _event->localPos().y() );

    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::MouseButtonPress );
    sceneEvent.setCoord( ::fwRenderQt::data::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Event::Modifier Scene2DGraphicsView::getScene2DModifierFromEvent( QInputEvent* _event )
{
    ::fwRenderQt::data::Event::Modifier modifier;

    if(_event->modifiers() == Qt::ControlModifier)
    {
        modifier = ::fwRenderQt::data::Event::ControlModifier;
    }
    else if(_event->modifiers() == Qt::AltModifier)
    {
        modifier = ::fwRenderQt::data::Event::AltModifier;
    }
    else if(_event->modifiers() == Qt::ShiftModifier)
    {
        modifier = ::fwRenderQt::data::Event::ShiftModifier;
    }
    else
    {
        modifier = ::fwRenderQt::data::Event::NoModifier;
    }

    // TODO: add support for combined modifiers
    return modifier;
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Event::Button Scene2DGraphicsView::getScene2DButtonFromEvent( QMouseEvent* _event )
{
    ::fwRenderQt::data::Event::Button button;

    if( _event->button() == Qt::LeftButton )
    {
        button = ::fwRenderQt::data::Event::LeftButton;
    }
    else if( _event->button() == Qt::RightButton )
    {
        button = ::fwRenderQt::data::Event::RightButton;
    }
    else if( _event->button() == Qt::MidButton )
    {
        button = ::fwRenderQt::data::Event::MidButton;
    }
    else
    {
        button = ::fwRenderQt::data::Event::NoButton;
    }

    return button;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseDoubleClickEvent ( QMouseEvent* _event )
{
    OSLM_TRACE("DoubleClick in x = " <<  _event->localPos().x() << " y = " << _event->localPos().y() );

    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::MouseButtonDoubleClick );
    sceneEvent.setCoord( ::fwRenderQt::data::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseReleaseEvent ( QMouseEvent* _event )
{
    OSLM_TRACE("Release in x = " <<  _event->localPos().x() << " y = " << _event->localPos().y() );
    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::MouseButtonRelease );
    sceneEvent.setCoord( ::fwRenderQt::data::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseMoveEvent ( QMouseEvent* _event )
{
    OSLM_TRACE("Move in x = " <<  _event->localPos().x() << " y = " << _event->localPos().y() );
    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( ::fwRenderQt::data::Event::MouseMove );
    sceneEvent.setCoord( ::fwRenderQt::data::Coord( _event->localPos().x(), _event->localPos().y() ) );
    sceneEvent.setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::wheelEvent ( QWheelEvent* _event )
{
    const bool scrollUp = _event->delta() > 0;
    OSLM_TRACE("Scroll " << (scrollUp ? "up" : "down") << " _event");
    ::fwRenderQt::data::Event sceneEvent;
    sceneEvent.setType( (scrollUp) ? ::fwRenderQt::data::Event::MouseWheelUp : ::fwRenderQt::data::Event::MouseWheelDown);
    sceneEvent.setCoord( ::fwRenderQt::data::Coord( _event->pos().x(), _event->pos().y() ) );
    sceneEvent.setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::setViewport( ::fwRenderQt::data::Viewport::sptr viewport )
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

