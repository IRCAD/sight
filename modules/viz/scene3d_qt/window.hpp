/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "modules/viz/scene3d_qt/config.hpp"
#include "modules/viz/scene3d_qt/open_gl_context.hpp"

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/window_interactor.hpp>

#include <Ogre.h>
#include <QGestureEvent>
#include <QOpenGLWidget>
#include <QPoint>
#include <QScreen>

#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <QtWidgets/QApplication>

#include <optional>

namespace sight::module::viz::scene3d_qt
{

class window final : public QOpenGLWidget
{
Q_OBJECT

public:

    /**
     * @brief Initializes members and connect screenChanged to onScreenChanged.
     */
    window();

    /// Destroys associated pointers by calling @ref destroy().
    ~window() override = default;

    void register_layer(sight::viz::scene3d::layer::wptr _layer);
    void create_render_textures(int _w, int _h);

    /// Gets this window ID.
    [[nodiscard]] int get_id() const;

    /// Makes the OpenGL context as current one on this thread against this window.
    void makeCurrent();

    /// Destroy the Ogre window.
    void destroy_window();

    /// Returns current frame number of the render window.
    [[nodiscard]] int get_frame_id() const;

    /// Renders immediately the frame.
    void render_now();

    /// Renders the frame as soon as possible.
    void request_render();

Q_SIGNALS:

    /// Emits when the user interacts with the scene using the mouse and keyboard.
    void interacted(sight::viz::scene3d::window_interactor::interaction_info);

private:

    /// Manages events
    bool event(QEvent* _e) override;

    /// Manages keyboard action.
    void keyPressEvent(QKeyEvent* _e) override;

    /// Nanage keyboard action.
    void keyReleaseEvent(QKeyEvent* _e) override;

    /// Manages mouse move.
    void mouseMoveEvent(QMouseEvent* _e) override;

    /// Manages wheel action.
    void wheelEvent(QWheelEvent* _e) override;

    /// Manages mouse click.
    void mousePressEvent(QMouseEvent* _e) override;

    /// Manages mouse double click.
    void mouseDoubleClickEvent(QMouseEvent* _e) override;

    /// Manages mouse click on release.
    void mouseReleaseEvent(QMouseEvent* _e) override;

    /// Manages mouse leaving the widget
    void leaveEvent(QEvent* _e) override;

    /// Manages mouse entering the widget
    void enterEvent(QEvent* _e) override;

    /// Manages gestures.
    void gesture_event(QGestureEvent* _e);

    using interaction_info = sight::viz::scene3d::window_interactor::interaction_info;

    /// Converts the mouse event to be able to handle it with ogre.
    interaction_info convert_mouse_event(
        const QMouseEvent* _evt,
        interaction_info::interaction_enum _interaction_type
    ) const;

    /// Creates the Ogre render window associated to this window,
    /// called by renderNow() once the window is first exposed.
    void initializeGL() override;
    void resizeGL(int _w, int _h) override;
    void paintGL() override;

    /// Forwards the resize event.
    /// @param _newSize size in hidpi, you must multiplicate with devicePixelRatioF to get the real pixel value
    void ogre_resize(const QSize& _new_size);

    /// Defines a counter to get the widget ID.
    static int s_counter;

    /// Defines the unique identifier of the widget
    int m_id;

    /// Contains the Ogre root.
    Ogre::Root* m_ogre_root {nullptr};

    /// Tells if an update is requested
    bool m_update_pending {false};

    /// Stores previous mouse positions.
    std::optional<QPoint> m_last_mouse_position;

    /// Counts the number of frames rendered since the window's creation.
    int m_frame_id {0};

    /// Contains the OpenGL context used for offscreen rendering.
    // std::shared_ptr<QOpenGLContext> m_glContext;

    /// Defines the last size sent to Ogre. In hidpi
    QSize m_ogre_size;

    /// Workaround to fix multiple scenes rendering with NVidia Prime
    bool m_init {false};

    Ogre::MeshPtr m_fs_quad_plane {};

    struct render_target
    {
        sight::viz::scene3d::layer::wptr layer;
        Ogre::MaterialPtr material;
        Ogre::TexturePtr texture;
    };
    std::vector<render_target> m_render_targets;

    enum class gesture_state : std::uint8_t
    {
        no_gesture = 0,
        pan_gesture,
        pinch_gesture
    };

    gesture_state m_gesture_state {gesture_state::no_gesture};
};

//-----------------------------------------------------------------------------

inline int window::get_frame_id() const
{
    return m_frame_id;
}

} // namespace sight::module::viz::scene3d_qt.
