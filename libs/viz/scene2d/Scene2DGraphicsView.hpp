/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#pragma once

#include "viz/scene2d/config.hpp"
#include "viz/scene2d/data/Event.hpp"
#include "viz/scene2d/data/Viewport.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPaintEvent>
#include <QResizeEvent>

namespace sight::viz::scene2d
{

class SRender;

class VIZ_SCENE2D_CLASS_API Scene2DGraphicsView : public QGraphicsView
{
Q_OBJECT

public:

    VIZ_SCENE2D_API Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget);

    /// Events

    VIZ_SCENE2D_API void keyPressEvent(QKeyEvent* _event);

    VIZ_SCENE2D_API void keyReleaseEvent(QKeyEvent* _event);

    VIZ_SCENE2D_API void resizeEvent(QResizeEvent* _event);

    VIZ_SCENE2D_API void mouseReleaseEvent(QMouseEvent* _event);

    VIZ_SCENE2D_API void mousePressEvent(QMouseEvent* _event);

    VIZ_SCENE2D_API void mouseMoveEvent(QMouseEvent* _event);

    VIZ_SCENE2D_API void mouseDoubleClickEvent(QMouseEvent* _event);

    VIZ_SCENE2D_API void wheelEvent(QWheelEvent* _event);

    VIZ_SCENE2D_API void updateFromViewport();

    VIZ_SCENE2D_API void setViewport(scene2d::data::Viewport::sptr viewport);

    VIZ_SCENE2D_API void setSceneRender(SPTR(viz::scene2d::SRender) sceneRender);

protected:

    scene2d::data::Viewport::sptr m_viewport;
    WPTR(viz::scene2d::SRender) m_scene2DRender;

    scene2d::data::Event::Button getScene2DButtonFromEvent(QMouseEvent* _event);
    scene2d::data::Event::Modifier getScene2DModifierFromEvent(QInputEvent* _event);

    void paintEvent(QPaintEvent* event);
};

} // namespace sight::viz::scene2d
