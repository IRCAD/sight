/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/viz/scene3dQt/config.hpp"
#include "modules/viz/scene3dQt/OpenGLContext.hpp"

#include <viz/scene3d/IWindowInteractor.hpp>

#include <Ogre.h>

#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>

#include <QGestureEvent>
#include <QPoint>
#include <QScreen>

#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <QtWidgets/QApplication>

#include <optional>

namespace sight::module::viz::scene3dQt
{

class Window final :
    public QWindow,
    public Ogre::RenderTargetListener
{
Q_OBJECT

public:

    /**
     * @brief Initializes members and connect screenChanged to onScreenChanged.
     * @param _parent the parent container of the widget.
     */
    Window(QWindow* _parent = nullptr);

    /// Destroys associated pointers by calling @ref destroy().
    ~Window() override;

    /**
     * @brief Does nothing.
     * In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
     * class in any way we inform Qt that they will be unused.
     */
    void render(QPainter* /*unused*/);

    /// Creates the Ogre render window associated to this window,
    /// called by renderNow() once the window is first exposed.
    void initialize();

    /// Returns the Ogre render window.
    [[nodiscard]] Ogre::RenderWindow* getOgreRenderWindow() const;

    /// Gets this window ID.
    [[nodiscard]] int getId() const;

    /// Makes the OpenGL context as current one on this thread against this window.
    void makeCurrent();

    /// Destroy the Ogre window.
    void destroyWindow();

    /// Returns current frame number of the render window.
    [[nodiscard]] int getFrameId() const;

    /// Renders immediately the frame.
    void renderNow();

    /// Renders the frame as soon as possible.
    void requestRender();

    /// Manages gestures.
    void gestureEvent(QGestureEvent* _e);

Q_SIGNALS:

    /// Emits when the user interacts with the scene using the mouse and keyboard.
    void interacted(sight::viz::scene3d::IWindowInteractor::InteractionInfo);

    /// Emits to recompute the camera's clipping range.
    void cameraClippingComputation();

private:

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
    void mouseDoubleClickEvent(QMouseEvent* e) override;

    /// Manages mouse click on release.
    void mouseReleaseEvent(QMouseEvent* _e) override;

    /// Manages when window visibility in the windowing system changes.
    void exposeEvent(QExposeEvent* /*unused*/) override;

    /// Manages when window is moved.
    void moveEvent(QMoveEvent* /*unused*/) override;

    /// Manages generic events.
    bool event(QEvent* _event) override;

    using InteractionInfo = sight::viz::scene3d::IWindowInteractor::InteractionInfo;

    /// Converts the mouse event to be able to handle it with ogre.
    InteractionInfo convertMouseEvent(
        const QMouseEvent* _evt,
        InteractionInfo::InteractionEnum _interactionType
    ) const;

    /**
     * @brief Renders immediately the frame.
     * Calls OgreRoot renderOneFrame to update the current renderWindow.
     * If you want to update this window, call requestRender().
     */
    void render();

    /// Renders the frame as soon as possible.
    void renderLater();

    /// Forwards the resize event.
    /// @param _newSize size in hidpi, you must multiplicate with devicePixelRatio to get the real pixel value
    void ogreResize(const QSize& _newSize);

    /// Defines a counter to get the widget ID.
    static int m_counter;

    /// Defines the unique identifier of the widget
    int m_id;

    /// Contains the Ogre root.
    Ogre::Root* m_ogreRoot {nullptr};

    /// Contains the Ogre render window.
    Ogre::RenderWindow* m_ogreRenderWindow {nullptr};

    /// Tells if an update is requested
    bool m_update_pending {false};

    /// Stores previous mouse positions.
    std::optional<QPoint> m_lastMousePosition;

    /// Counts the number of frames rendered since the window's creation.
    int m_frameId {0};

    /// Contains the OpenGL context used for offscreen rendering.
    std::shared_ptr<QOpenGLContext> m_glContext;

    /// Defines the last size sent to Ogre. In hidpi
    QSize m_ogreSize;

    /// Workaround to fix multiple scenes rendering with NVidia Prime
    bool m_externalGLControl {true};

private Q_SLOTS:

    /// Called when the screen changes.
    void onScreenChanged(QScreen* /*unused*/);
};

//-----------------------------------------------------------------------------

inline int Window::getFrameId() const
{
    return m_frameId;
}

} // namespace sight::module::viz::scene3dQt.
