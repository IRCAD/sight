/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_SCENE2DGRAPHICVIEW_HPP_
#define _SCENE2D_SCENE2DGRAPHICVIEW_HPP_

#include <QGraphicsView>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QPaintEvent>

#include "scene2D/config.hpp"
#include "scene2D/data/Viewport.hpp"

namespace scene2D
{

class Render;

class SCENE2D_CLASS_API Scene2DGraphicsView : public QGraphicsView
{
    Q_OBJECT

public :

    SCENE2D_API Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget);

    /// Events

    SCENE2D_API void resizeEvent(QResizeEvent *_event);

    SCENE2D_API void mouseReleaseEvent ( QMouseEvent * _event );

    SCENE2D_API void mousePressEvent ( QMouseEvent * _event );

    SCENE2D_API void mouseMoveEvent ( QMouseEvent * _event );

    SCENE2D_API void mouseDoubleClickEvent ( QMouseEvent * _event );

    SCENE2D_API void wheelEvent ( QWheelEvent * _event );



    SCENE2D_API void updateFromViewport();

    SCENE2D_API void setViewport( SPTR(::scene2D::data::Viewport) viewport );

    SCENE2D_API void setSceneRender( SPTR(::scene2D::Render) sceneRender );

protected :

    WPTR(::scene2D::data::Viewport) m_viewport;
    WPTR(::scene2D::Render) m_scene2DRender;

    void paintEvent ( QPaintEvent * event );

};
} // namespace scene2D


#endif // _SCENE2D_SCENE2DGRAPHICVIEW_HPP_

