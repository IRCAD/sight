/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_SCENE2DGRAPHICSVIEW_HPP__
#define __FWRENDERQT_SCENE2DGRAPHICSVIEW_HPP__

#include "fwRenderQt/config.hpp"
#include "fwRenderQt/data/Event.hpp"
#include "fwRenderQt/data/Viewport.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPaintEvent>
#include <QResizeEvent>

namespace fwRenderQt
{

class SRender;

class FWRENDERQT_CLASS_API Scene2DGraphicsView : public QGraphicsView
{
Q_OBJECT

public:

    FWRENDERQT_API Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget);

    /// Events

    FWRENDERQT_API void keyPressEvent(QKeyEvent* _event);

    FWRENDERQT_API void keyReleaseEvent(QKeyEvent* _event);

    FWRENDERQT_API void resizeEvent(QResizeEvent* _event);

    FWRENDERQT_API void mouseReleaseEvent ( QMouseEvent* _event );

    FWRENDERQT_API void mousePressEvent ( QMouseEvent* _event );

    FWRENDERQT_API void mouseMoveEvent ( QMouseEvent* _event );

    FWRENDERQT_API void mouseDoubleClickEvent ( QMouseEvent* _event );

    FWRENDERQT_API void wheelEvent ( QWheelEvent* _event );

    FWRENDERQT_API void updateFromViewport();

    FWRENDERQT_API void setViewport( ::fwRenderQt::data::Viewport::sptr viewport );

    FWRENDERQT_API void setSceneRender( SPTR(::fwRenderQt::SRender) sceneRender );

protected:

    ::fwRenderQt::data::Viewport::sptr m_viewport;
    WPTR(::fwRenderQt::SRender) m_scene2DRender;

    ::fwRenderQt::data::Event::Button getScene2DButtonFromEvent( QMouseEvent* _event );
    ::fwRenderQt::data::Event::Modifier getScene2DModifierFromEvent( QInputEvent* _event );

    void paintEvent ( QPaintEvent* event );

};
} // namespace fwRenderQt

#endif // __FWRENDERQT_SCENE2DGRAPHICSVIEW_HPP__

