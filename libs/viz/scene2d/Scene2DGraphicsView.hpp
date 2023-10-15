/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

class render;

class VIZ_SCENE2D_CLASS_API Scene2DGraphicsView : public QGraphicsView
{
Q_OBJECT

public:

    VIZ_SCENE2D_API Scene2DGraphicsView(QGraphicsScene* scene, QWidget* widget);

    /// Events

    VIZ_SCENE2D_API void keyPressEvent(QKeyEvent* _event) override;

    VIZ_SCENE2D_API void keyReleaseEvent(QKeyEvent* _event) override;

    VIZ_SCENE2D_API void resizeEvent(QResizeEvent* _event) override;

    VIZ_SCENE2D_API void mouseReleaseEvent(QMouseEvent* _event) override;

    VIZ_SCENE2D_API void mousePressEvent(QMouseEvent* _event) override;

    VIZ_SCENE2D_API void mouseMoveEvent(QMouseEvent* _event) override;

    VIZ_SCENE2D_API void mouseDoubleClickEvent(QMouseEvent* _event) override;

    VIZ_SCENE2D_API void wheelEvent(QWheelEvent* _event) override;

    VIZ_SCENE2D_API void enterEvent(QEvent* event) override;
    VIZ_SCENE2D_API void leaveEvent(QEvent* event) override;

    VIZ_SCENE2D_API QSize sizeHint() const override;

    VIZ_SCENE2D_API void updateFromViewport(const scene2d::data::Viewport& viewport);

    VIZ_SCENE2D_API void setSceneRender(SPTR(viz::scene2d::render) sceneRender);

protected:

    scene2d::data::Viewport m_viewport;
    WPTR(viz::scene2d::render) m_scene2DRender;

    static scene2d::data::Event::Button getScene2DButtonFromEvent(QMouseEvent* _event);
    static scene2d::data::Event::Modifier getScene2DModifierFromEvent(QInputEvent* _event);
};

} // namespace sight::viz::scene2d
