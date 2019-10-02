/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreQt/config.hpp"
#include "visuOgreQt/OpenGLContext.hpp"

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>

#include <Ogre.h>

#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>

#include <QPoint>
#include <QScreen>

#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <QtWidgets/QApplication>

namespace visuOgreQt
{
class Window : public QWindow,
               public ::Ogre::RenderTargetListener
{
Q_OBJECT

public:
    /**
     * @brief Window Initialise attributes
     * @param parent This window parent's
     */
    Window(QWindow* parent = nullptr);
    /**
     * @brief ~Window Destructor. Destroy associated pointers
     */
    virtual ~Window() override;

    /**
     * @brief render
     * In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
       class in any way we inform Qt that they will be unused.
     * @param painter The used painter
     */
    virtual void render(QPainter* painter);

    void setAnimating(bool animating);

    /// Returns Ogre render window.
    virtual ::Ogre::RenderWindow* getOgreRenderWindow();

    /// Get this window ID
    int getId();

    /// Request frame rendering
    void requestRender();

    /// Enables visuOgreQt's shared gl context on this thread against this window.
    void makeCurrent();

    /// Destroy ogre window
    void destroyWindow();

    void setFullScreen(bool fullscreen);

    int getFrameId() const;

    /**
     * @brief initialise
     * Creates the Ogre renderWindow associated to this window, called by renderNow() once the window is first exposed
     */
    void initialise();

    /**
     * @brief renderNow
     * Force the renderWindow update
     */
    void renderNow( const bool force = false);

protected:

    /*
     * Qt events to manage keyboard and mouse input
     */
    /// Qt event to manage keyboard action
    virtual void keyPressEvent(QKeyEvent* e) override;
    /// Qt event to manage keyboard action
    virtual void keyReleaseEvent(QKeyEvent* e) override;
    /// Qt event to manage mouse move
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    /// Qt event to manage wheel action
    virtual void wheelEvent(QWheelEvent* e) override;
    /// Qt event to manage mouse clic
    virtual void mousePressEvent(QMouseEvent* e) override;
    /// Qt event to manage mouse clic on release
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    /// Qt event to manage when window visibility in the windowing system changes.
    virtual void exposeEvent(QExposeEvent* event) override;
    /// Qt event to manage when window is moved.
    virtual void moveEvent(QMoveEvent* event) override;
    /// Qt event to manage generic events
    virtual bool event(QEvent* event) override;
    /// Qt event to manage focus
    virtual void focusInEvent(QFocusEvent* event) override;
    /// Qt event to manage focus
    virtual void focusOutEvent(QFocusEvent* event) override;

Q_SIGNALS:
    /// When the render window is created
    void renderWindowCreated();

    /// When the render window is created
    void interacted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo);

    /// When a ray cast request is emitted
    void rayCastRequested(int, int, int, int);

    /// When the clipping range has to match the last updating of the scene bounding box
    void cameraClippingComputation();

private:

    /**
     * @brief render
     * Calls OgreRoot renderOneFrame to update the current renderWindow
     * If you want to update this window, call requestRender()
     */
    void render();

    /**
     * @brief renderLater
     * Render the renderWindow later
     */
    void renderLater();

    /// Apply device pixel ratio on screen coordinates, needed only for MacOs currently
    std::pair<int, int> getDeviceCoordinates(int _x, int _y);

    /// resizeEvent forwarding function
    void ogreResize(const QSize& newSize);

    /// Needed for multiple instances of ogreQt WIDGET
    static int m_counter;

    /// Used to instantiate the managers related to this instance with a proper name.
    int m_id;

    /*
       Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
     */
    Ogre::Root* m_ogreRoot;
    Ogre::RenderWindow* m_ogreRenderWindow;

    /// Tells if an update is requested
    bool m_update_pending;
    /// Tells if the window is currently showed
    bool m_animating;
    /// Tells if the window fills the screen.
    bool m_fullscreen;

    /// Logs left click positions.
    QPoint* m_lastPosLeftClick;
    /// Logs middle click positions.
    QPoint* m_lastPosMiddleClick;
    /// Logs right click positions.
    QPoint* m_lastPosRightClick;

    int m_frameId;

    std::shared_ptr<QOpenGLContext> m_glContext;

    /// Last size sent to ogre
    QSize m_ogreSize;

private Q_SLOTS:

    /// Called when the screen change
    void onScreenChanged(QScreen*);
};

//-----------------------------------------------------------------------------

inline void Window::setFullScreen(bool fullscreen)
{
    m_fullscreen = fullscreen;
}

//-----------------------------------------------------------------------------

inline int Window::getFrameId() const
{
    return m_frameId;
}

//-----------------------------------------------------------------------------

}
