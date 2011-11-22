/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "scene2D/data/Size.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"
#include "scene2D/Render.hpp"

#include <QMouseEvent>

namespace scene2D
{

//-----------------------------------------------------------------------------

Scene2DGraphicsView::Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget) : QGraphicsView(scene,widget)
{
    if( !this->hasMouseTracking() )
    {
        this->setMouseTracking( true );
    }
    
    /* Change the method usedn by the scene to index its items:
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
    this->setOptimizationFlags( QGraphicsView::DontSavePainterState          // 
                              | QGraphicsView::DontAdjustForAntialiasing );  // Prevent from rendering artifacts
    this->setCacheMode( QGraphicsView::CacheBackground );   // Activates background cache
    this->setAttribute( Qt::WA_TranslucentBackground, false );
    this->setFrameStyle( 0 ); 
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::setSceneRender( SPTR(::scene2D::Render) sceneRender )
{
    m_scene2DRender = sceneRender;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::resizeEvent(QResizeEvent *_event)
{
    this->updateFromViewport();

    ::scene2D::data::Event::NewSptr sceneEvent;
    sceneEvent->setType( ::scene2D::data::Event::Resize);
    sceneEvent->setButton( ::scene2D::data::Event::NoButton);
    sceneEvent->setModifier( ::scene2D::data::Event::NoModifier); 
    sceneEvent->setSize( ::scene2D::data::Size( _event->size().width(), _event->size().height() ) );
    sceneEvent->setOldSize( ::scene2D::data::Size( _event->oldSize().width(), _event->oldSize().height() ) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mousePressEvent ( QMouseEvent * _event )
{
    SLM_TRACE_FUNC();

    OSLM_TRACE("Press in x = " <<  _event->pos().x() << " y = " << _event->pos().y() );

    ::scene2D::data::Event::NewSptr sceneEvent;
    sceneEvent->setType( ::scene2D::data::Event::MouseButtonPress );
    sceneEvent->setCoord( ::scene2D::data::Coord( _event->pos().x(), _event->pos().y() ) );
    sceneEvent->setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent->setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

::scene2D::data::Event::Modifier Scene2DGraphicsView::getScene2DModifierFromEvent( QMouseEvent* _event )
{
    ::scene2D::data::Event::Modifier modifier;

    if(_event->modifiers() == Qt::ControlModifier)
    {
        modifier = ::scene2D::data::Event::ControlModifier;
    }
    else if(_event->modifiers() == Qt::AltModifier)
    {
        modifier = ::scene2D::data::Event::AltModifier; 
    }
    else if(_event->modifiers() == Qt::ShiftModifier)
    {
        modifier = ::scene2D::data::Event::ShiftModifier;
    }
    else if(_event->modifiers() == Qt::NoModifier)
    {
        modifier = ::scene2D::data::Event::NoModifier;
    }
    else
    {
        modifier = ::scene2D::data::Event::NoModifier;
    }

    // TODO: add support for combined modifiers
    return modifier;
}

//-----------------------------------------------------------------------------

::scene2D::data::Event::Button Scene2DGraphicsView::getScene2DButtonFromEvent( QMouseEvent* _event )
{
    ::scene2D::data::Event::Button button;

    if( _event->button() == Qt::LeftButton )
    {
        button = ::scene2D::data::Event::LeftButton;
    }
    else if( _event->button() == Qt::RightButton )
    {
        button = ::scene2D::data::Event::RightButton;
    }
    else if( _event->button() == Qt::MidButton )
    {
        button = ::scene2D::data::Event::MidButton;
    }
    else
    {
        button = ::scene2D::data::Event::NoButton;
    }

    return button;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseDoubleClickEvent ( QMouseEvent * _event )
{
    SLM_TRACE_FUNC();

    OSLM_TRACE("DoubleClick in x = " <<  _event->pos().x() << " y = " << _event->pos().y() );

    ::scene2D::data::Event::NewSptr sceneEvent;
    sceneEvent->setType( ::scene2D::data::Event::MouseButtonDoubleClick );
    sceneEvent->setCoord( ::scene2D::data::Coord( _event->pos().x(), _event->pos().y() ) );
    sceneEvent->setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent->setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseReleaseEvent ( QMouseEvent * _event )
{
    SLM_TRACE_FUNC();
    OSLM_TRACE("Release in x = " <<  _event->pos().x() << " y = " << _event->pos().y() );
    ::scene2D::data::Event::NewSptr sceneEvent;
    sceneEvent->setType( ::scene2D::data::Event::MouseButtonRelease );
    sceneEvent->setCoord( ::scene2D::data::Coord( _event->pos().x(), _event->pos().y() ) );
    sceneEvent->setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent->setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::mouseMoveEvent ( QMouseEvent * _event )
{
    SLM_TRACE_FUNC();

    OSLM_TRACE("Move in x = " <<  _event->pos().x() << " y = " << _event->pos().y() );
    ::scene2D::data::Event::NewSptr sceneEvent;
    sceneEvent->setType( ::scene2D::data::Event::MouseMove );
    sceneEvent->setCoord( ::scene2D::data::Coord( _event->pos().x(), _event->pos().y() ) );
    sceneEvent->setButton( this->getScene2DButtonFromEvent( _event ) );
    sceneEvent->setModifier( this->getScene2DModifierFromEvent( _event) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::wheelEvent ( QWheelEvent * _event )
{
    SLM_TRACE_FUNC();

    const bool scrollUp = _event->delta() > 0;
    OSLM_TRACE("Scroll " << (scrollUp ? "up" : "down") << " _event");
    ::scene2D::data::Event::NewSptr sceneEvent;
    sceneEvent->setType( (scrollUp) ? ::scene2D::data::Event::MouseWheelUp : ::scene2D::data::Event::MouseWheelDown);
    sceneEvent->setCoord( ::scene2D::data::Coord( _event->pos().x(), _event->pos().y() ) );

    m_scene2DRender.lock()->dispatchInteraction( sceneEvent );
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::setViewport( ::scene2D::data::Viewport::sptr viewport )
{
    m_viewport = viewport;
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::updateFromViewport()
{
    ::scene2D::data::Viewport::sptr viewport = m_viewport.lock();
    this->fitInView(
        viewport->getX(), viewport->getY(), viewport->getWidth(), viewport->getHeight(),
        m_scene2DRender.lock()->getAspectRatioMode() );
//   this->update();
}

//-----------------------------------------------------------------------------

void Scene2DGraphicsView::paintEvent(QPaintEvent * event)
{
    // QGraphicsView optimization
    QPaintEvent* newEvent = new QPaintEvent( event->region().boundingRect() );
    QGraphicsView::paintEvent(newEvent);

    delete newEvent;
}

//-----------------------------------------------------------------------------

} // namespace scene2D

