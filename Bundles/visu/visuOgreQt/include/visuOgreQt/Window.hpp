/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreQt/config.hpp"

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>

#include <Ogre.h>

#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>

#include <QPoint>

#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <QtWidgets/QApplication>
//#include <OGRE/SdkTrays.h>

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
    VISUOGREQT_API Window(QWindow* parent = NULL);
    /**
     * @brief ~Window Destructor. Destroy associated pointers
     */
    VISUOGREQT_API ~Window();

    /**
     * @brief render
     * In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
       class in any way we inform Qt that they will be unused.
     * @param painter The used painter
     */
    VISUOGREQT_API virtual void render(QPainter* painter);

    VISUOGREQT_API void setAnimating(bool animating);

    /// Returns Ogre render window.
    VISUOGREQT_API virtual ::Ogre::RenderWindow* getOgreRenderWindow();

    /// Returns Ogre render window.
    VISUOGREQT_API virtual ::Ogre::OverlaySystem* getOgreOverlaySystem();

    /// Get this window ID
    VISUOGREQT_API int getId();

    /// Request frame rendering
    VISUOGREQT_API virtual void requestRender();

    VISUOGREQT_API virtual void makeCurrent();

    /// Sets the list of overlays to be rendered in this window.
    VISUOGREQT_API void setEnabledOverlays(
        const ::fwRenderOgre::IRenderWindowInteractorManager::OverlaySetType& enabledOverlays);

    /// Destroy ogre window
    VISUOGREQT_API void destroyWindow();

    /// Called right before rendering in the viewport. Activates the overlays enabled for this viewport.
    VISUOGREQT_API virtual void preViewportUpdate(const ::Ogre::RenderTargetViewportEvent& evt) override;

    /// Called right after rendering in the viewport. Disables the overlays enabled for this viewport.
    VISUOGREQT_API virtual void postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;

    VISUOGREQT_API void setFullScreen(bool fullscreen);

    int getFrameId() const;

    /**
     * @brief initialise
     * Creates the Ogre renderWindow associated to this window, called by renderNow() once the window is first exposed
     */
    VISUOGREQT_API void initialise();

public Q_SLOTS:

    /**
     * @brief renderLater
     * Render the renderWindow later
     */
    VISUOGREQT_API virtual void renderLater();
    /**
     * @brief renderNow
     * Force the renderWindow update
     */
    VISUOGREQT_API virtual void renderNow();

    /// We use an event filter to be able to capture keyboard/mouse events. More on this later.
    VISUOGREQT_API virtual bool eventFilter(QObject* target, QEvent* event) override;

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
    VISUOGREQT_API virtual void render();

    /// Apply device pixel ratio on screen coordinates, needed only for MacOs currently
    static std::pair<int, int> getDeviceCoordinates(int _x, int _y);

    /// resizeEvent forwarding function
    void ogreResize(const QSize& newSize);

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

    /// Needed for multiple instances of ogreQt WIDGET
    static int m_counter;

    /// Used to instantiate the managers related to this instance with a proper name.
    int m_id;

    /*
       Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
     */
    Ogre::Root* m_ogreRoot;
    Ogre::RenderWindow* m_ogreRenderWindow;

    std::unordered_set< ::Ogre::Overlay* > m_enabledOverlays;

    /// Ogre overlay system.
    static ::Ogre::OverlaySystem* m_ogreOverlaySystem;
    /// Ogre tray manager
//    ::OgreBites::SdkTrayManager* m_trayMgr;

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
