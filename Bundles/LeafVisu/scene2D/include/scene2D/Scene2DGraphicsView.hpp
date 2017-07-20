/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_SCENE2DGRAPHICSVIEW_HPP__
#define __SCENE2D_SCENE2DGRAPHICSVIEW_HPP__

#include "scene2D/config.hpp"
#include "scene2D/data/Event.hpp"
#include "scene2D/data/Viewport.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPaintEvent>
#include <QResizeEvent>

namespace scene2D
{

class SRender;

class SCENE2D_CLASS_API Scene2DGraphicsView : public QGraphicsView
{
Q_OBJECT

public:

    SCENE2D_API Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget);

    /// Events

    SCENE2D_API void keyPressEvent(QKeyEvent* _event);

    SCENE2D_API void keyReleaseEvent(QKeyEvent* _event);

    SCENE2D_API void resizeEvent(QResizeEvent* _event);

    SCENE2D_API void mouseReleaseEvent ( QMouseEvent* _event );

    SCENE2D_API void mousePressEvent ( QMouseEvent* _event );

    SCENE2D_API void mouseMoveEvent ( QMouseEvent* _event );

    SCENE2D_API void mouseDoubleClickEvent ( QMouseEvent* _event );

    SCENE2D_API void wheelEvent ( QWheelEvent* _event );

    SCENE2D_API void updateFromViewport();

    SCENE2D_API void setViewport( SPTR(::scene2D::data::Viewport) viewport );

    SCENE2D_API void setSceneRender( SPTR(::scene2D::SRender) sceneRender );

protected:

    WPTR(::scene2D::data::Viewport) m_viewport;
    WPTR(::scene2D::SRender) m_scene2DRender;

    ::scene2D::data::Event::Button getScene2DButtonFromEvent( QMouseEvent* _event );
    ::scene2D::data::Event::Modifier getScene2DModifierFromEvent( QInputEvent* _event );

    void paintEvent ( QPaintEvent* event );

};
} // namespace scene2D

#endif // __SCENE2D_SCENE2DGRAPHICSVIEW_HPP__

