/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/viz/scene2d/config.hpp>

#include "viz/scene2d/data/event.hpp"
#include "viz/scene2d/data/viewport.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPaintEvent>
#include <QResizeEvent>

namespace sight::viz::scene2d
{

class render;

class SIGHT_VIZ_SCENE2D_CLASS_API graphics_view : public QGraphicsView
{
Q_OBJECT

public:

    SIGHT_VIZ_SCENE2D_API graphics_view(QGraphicsScene* _scene, QWidget* _widget);

    /// Events

    SIGHT_VIZ_SCENE2D_API void keyPressEvent(QKeyEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void keyReleaseEvent(QKeyEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void resizeEvent(QResizeEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void mouseReleaseEvent(QMouseEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void mousePressEvent(QMouseEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void mouseMoveEvent(QMouseEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void mouseDoubleClickEvent(QMouseEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void wheelEvent(QWheelEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API void enterEvent(QEvent* _event) override;
    SIGHT_VIZ_SCENE2D_API void leaveEvent(QEvent* _event) override;

    SIGHT_VIZ_SCENE2D_API QSize sizeHint() const override;

    SIGHT_VIZ_SCENE2D_API void update_from_viewport(const scene2d::data::viewport& _viewport);

    SIGHT_VIZ_SCENE2D_API void set_scene_render(SPTR(viz::scene2d::render) _scene_render);

protected:

    scene2d::data::viewport m_viewport;
    WPTR(viz::scene2d::render) m_scene_2d_render;

    static scene2d::data::event::button get_scene_2d_button_from_event(QMouseEvent* _event);
    static scene2d::data::event::modifier get_scene_2d_modifier_from_event(QInputEvent* _event);
};

} // namespace sight::viz::scene2d
