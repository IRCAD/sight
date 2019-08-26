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

#include "visuOgreQt/Window.hpp"

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwGui/Application.hpp>
#include <fwGui/Cursor.hpp>

#include <fwRenderOgre/Utils.hpp>
#include <fwRenderOgre/WindowManager.hpp>

#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

#define ZOOM_SPEED 0.2

namespace visuOgreQt
{

int Window::m_counter = 0;
::Ogre::OverlaySystem* Window::m_ogreOverlaySystem = nullptr;

// ----------------------------------------------------------------------------

Window::Window(QWindow* parent) :
    QWindow(parent),
    m_id(Window::m_counter++),
    m_ogreRoot(nullptr),
    m_ogreRenderWindow(nullptr),
    m_update_pending(false),
    m_animating(false),
    m_fullscreen(false),
    m_lastPosLeftClick(nullptr),
    m_lastPosMiddleClick(nullptr),
    m_frameId(0)
{
    setAnimating(false);
    installEventFilter(this);

    connect(this,  &Window::screenChanged, this, &Window::onScreenChanged);
}

// ----------------------------------------------------------------------------

Window::~Window()
{
    destroy();
}

// ----------------------------------------------------------------------------

void Window::setEnabledOverlays(const ::fwRenderOgre::IRenderWindowInteractorManager::OverlaySetType& enabledOverlays)
{
    m_enabledOverlays = enabledOverlays;
}
// ----------------------------------------------------------------------------

void Window::render(QPainter* painter)
{
    Q_UNUSED(painter);
}

// ----------------------------------------------------------------------------

void Window::initialise()
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    SLM_ASSERT("OpenGL RenderSystem not found",
               m_ogreRoot->getRenderSystem()->getName().find("GL") != std::string::npos);

    Ogre::NameValuePairList parameters;

    // We share the OpenGL context on all windows. The first window will create the context, the other ones will
    // reuse the current context.
    parameters["currentGLContext"] = "true";

    /*
       We need to supply the low level OS window handle to this QWindow so that Ogre3D knows where to draw
       the scene. Below is a cross-platform method on how to do this.
     */
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    {
        size_t winId = static_cast<size_t>(this->winId());
        parameters["externalWindowHandle"] = Ogre::StringConverter::toString(winId);
        parameters["parentWindowHandle"]   = Ogre::StringConverter::toString(winId);
    }
#else
    {
        unsigned long winId = static_cast<unsigned long>(this->winId());
        parameters["externalWindowHandle"] = Ogre::StringConverter::toString(winId);
    }
#endif

    m_glContext = ::visuOgreQt::OpenGLContext::getGlobalOgreOpenGLContext();
    this->makeCurrent();

    m_ogreRenderWindow = m_ogreRoot->createRenderWindow("Widget-RenderWindow_" + std::to_string(m_id),
                                                        static_cast<unsigned int>(this->width()),
                                                        static_cast<unsigned int>(this->height()),
                                                        false,
                                                        &parameters);

    m_ogreRenderWindow->setVisible(true);
    m_ogreRenderWindow->setAutoUpdated(false);
    m_ogreRenderWindow->addListener(this);

    ::fwRenderOgre::WindowManager::sptr mgr = ::fwRenderOgre::WindowManager::get();
    mgr->registerWindow(m_ogreRenderWindow);

    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType     = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::RESIZE;
    std::tie(info.x, info.y) = Window::getDeviceCoordinates(this->width(), this->height());
    info.dx                  = 0;
    info.dy                  = 0;
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::requestRender()
{
    this->renderLater();
}

//------------------------------------------------------------------------------

void Window::makeCurrent()
{
    if(m_glContext)
    {
        m_glContext->makeCurrent(this);
    }
}

// ----------------------------------------------------------------------------

void Window::destroyWindow()
{
    if(m_ogreRenderWindow)
    {
        m_ogreRenderWindow->removeListener(this);
        ::fwRenderOgre::WindowManager::sptr mgr = ::fwRenderOgre::WindowManager::get();
        mgr->unregisterWindow(m_ogreRenderWindow);
        m_ogreRenderWindow = nullptr;
    }

    if (m_lastPosLeftClick)
    {
        delete m_lastPosLeftClick;
    }
    if (m_lastPosMiddleClick)
    {
        delete m_lastPosMiddleClick;
    }
}

// ----------------------------------------------------------------------------

void Window::render()
{
    if(m_ogreRenderWindow == nullptr)
    {
        return;
    }

    Q_EMIT cameraClippingComputation();

    ++m_frameId;
    /*
       How we tied in the render function for OGre3D with QWindow's render function. This is what gets call
       repeatedly. Note that we don't call this function directly; rather we use the renderNow() function
       to call this method as we don't want to render the Ogre3D scene unless everything is set up first.
       That is what renderNow() does.

       Theoretically you can have one function that does this check but from my experience it seems better
       to keep things separate and keep the render function as simple as possible.
     */

    FW_PROFILE_FRAME_AVG("Ogre", 3);
    FW_PROFILE_AVG("Ogre", 3);
    this->makeCurrent();

    m_ogreRoot->_fireFrameStarted();
    m_ogreRenderWindow->update();
    m_ogreRoot->_fireFrameRenderingQueued();
    m_ogreRoot->_fireFrameEnded();
}

// ----------------------------------------------------------------------------

std::pair<int, int> Window::getDeviceCoordinates(int _x, int _y)
{
#ifdef Q_OS_MAC
    const qreal pixelRatio = this->devicePixelRatio();
    const int x            = static_cast<int>(std::ceil(_x * pixelRatio));
    const int y            = static_cast<int>(std::ceil(_y * pixelRatio));
#else
    const int x = _x;
    const int y = _y;
#endif

    return std::make_pair(x, y);
}

// ----------------------------------------------------------------------------

void Window::renderLater()
{
    /*
       This function forces QWindow to keep rendering. Omitting this causes the renderNow() function to
       only get called when the window is resized, moved, etc. as opposed to all of the time; which is
       generally what we need.
     */
    if (!m_update_pending)
    {
        m_update_pending = true;
        QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

// ----------------------------------------------------------------------------

bool Window::event(QEvent* event)
{
    /*
       QWindow's "message pump". The base method that handles all QWindow events. As you will see there
       are other methods that actually process the keyboard/other events of Qt and the underlying OS.

       Note that we call the renderNow() function which checks to see if everything is initialized, etc.
       before calling the render() function.
     */

    switch (event->type())
    {
        case QEvent::UpdateRequest:
            m_update_pending = false;
            renderNow();
            return true;

        default:
            return QWindow::event(event);
    }
}
// ----------------------------------------------------------------------------

void Window::exposeEvent(QExposeEvent* exposeEvent)
{
    const bool nonEmptyRegion = !exposeEvent->region().isEmpty();

    // Force rendering
    this->renderNow(nonEmptyRegion);
}

// ----------------------------------------------------------------------------

void Window::moveEvent(QMoveEvent*)
{
    if(m_ogreRenderWindow != nullptr)
    {
        m_ogreRenderWindow->reposition(x(), y());
    }
}

// ----------------------------------------------------------------------------

void Window::renderNow(const bool force)
{
    // Small optimization to not render when not visible
    if(!force && !isExposed())
    {
        return;
    }

    this->render();

    if (m_animating)
    {
        this->renderLater();
    }
}

// ----------------------------------------------------------------------------

bool Window::eventFilter(QObject* target, QEvent* event)
{
    if (target == this
        && m_ogreRenderWindow != nullptr
        && event->type() == QEvent::Resize)
    {
        this->ogreResize(static_cast<QResizeEvent*>(event)->size());
    }

    return QWindow::eventFilter(target, event);
}

// ----------------------------------------------------------------------------

void Window::keyPressEvent(QKeyEvent* e)
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::KEYPRESS;

    switch(e->key())
    {
        case ::Qt::Key_Shift: info.key   = ::fwRenderOgre::interactor::IInteractor::SHIFT; break;
        case ::Qt::Key_Control: info.key = ::fwRenderOgre::interactor::IInteractor::CONTROL; break;
        case ::Qt::Key_Meta: info.key    = ::fwRenderOgre::interactor::IInteractor::META; break;
        case ::Qt::Key_Alt: info.key     = ::fwRenderOgre::interactor::IInteractor::ALT; break;
        default:
            info.key = e->key();
    }

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::keyReleaseEvent(QKeyEvent* e)
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::KEYRELEASE;
    switch(e->key())
    {
        case ::Qt::Key_Shift: info.key   = ::fwRenderOgre::interactor::IInteractor::SHIFT; break;
        case ::Qt::Key_Control: info.key = ::fwRenderOgre::interactor::IInteractor::CONTROL; break;
        case ::Qt::Key_Meta: info.key    = ::fwRenderOgre::interactor::IInteractor::META; break;
        case ::Qt::Key_Alt: info.key     = ::fwRenderOgre::interactor::IInteractor::ALT; break;
        default:
            info.key = e->key();
    }

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::mouseMoveEvent( QMouseEvent* e )
{
    if (e->buttons() & ::Qt::LeftButton && m_lastPosLeftClick)
    {
        int x  = m_lastPosLeftClick->x();
        int y  = m_lastPosLeftClick->y();
        int dx = x - e->x();
        int dy = y - e->y();
        ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
        info.interactionType       = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::MOUSEMOVE;
        std::tie(info.x, info.y)   = Window::getDeviceCoordinates(x, y);
        std::tie(info.dx, info.dy) = Window::getDeviceCoordinates(dx, dy);
        info.button                = ::fwRenderOgre::interactor::IInteractor::LEFT;
        Q_EMIT interacted(info);

        m_lastPosLeftClick->setX(e->x());
        m_lastPosLeftClick->setY(e->y());
        this->requestRender();
    }
    else if (e->buttons() & ::Qt::MiddleButton && m_lastPosMiddleClick )
    {
        int x  = m_lastPosMiddleClick->x();
        int y  = m_lastPosMiddleClick->y();
        int dx = x - e->x();
        int dy = y - e->y();

        ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
        info.interactionType       = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::MOUSEMOVE;
        std::tie(info.x, info.y)   = Window::getDeviceCoordinates(x, y);
        std::tie(info.dx, info.dy) = Window::getDeviceCoordinates(dx, dy);
        info.button                = ::fwRenderOgre::interactor::IInteractor::MIDDLE;
        Q_EMIT interacted(info);

        m_lastPosMiddleClick->setX(e->x());
        m_lastPosMiddleClick->setY(e->y());
        this->requestRender();
    }
    else if (e->buttons() & ::Qt::RightButton && m_lastPosRightClick )
    {
        int x  = m_lastPosRightClick->x();
        int y  = m_lastPosRightClick->y();
        int dx = x - e->x();
        int dy = y - e->y();

        ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
        info.interactionType       = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::MOUSEMOVE;
        std::tie(info.x, info.y)   = Window::getDeviceCoordinates(x, y);
        std::tie(info.dx, info.dy) = Window::getDeviceCoordinates(dx, dy);
        info.button                = ::fwRenderOgre::interactor::IInteractor::RIGHT;
        Q_EMIT interacted(info);

        m_lastPosRightClick->setX(e->x());
        m_lastPosRightClick->setY(e->y());
        this->requestRender();
    }
}

// ----------------------------------------------------------------------------

void Window::wheelEvent(QWheelEvent* e)
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType     = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::WHEELMOVE;
    info.delta               = static_cast<int>(e->delta()*ZOOM_SPEED);
    std::tie(info.x, info.y) = Window::getDeviceCoordinates(e->x(), e->y());
    info.dx                  = 0;
    info.dy                  = 0;

    Q_EMIT interacted(info);
    Q_EMIT cameraClippingComputation();

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::mousePressEvent( QMouseEvent* e )
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType     = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::BUTTONPRESS;
    info.button              = ::fwRenderOgre::interactor::IInteractor::UNKNOWN;
    info.delta               = 0;
    std::tie(info.x, info.y) = Window::getDeviceCoordinates(e->x(), e->y());
    info.dx                  = 0;
    info.dy                  = 0;

    if(e->button() == Qt::LeftButton)
    {
        m_lastPosLeftClick = new QPoint(e->x(), e->y());

        info.button = ::fwRenderOgre::interactor::IInteractor::LEFT;
    }
    else if(e->button() == Qt::MiddleButton)
    {
        m_lastPosMiddleClick = new QPoint(e->x(), e->y());

        info.button = ::fwRenderOgre::interactor::IInteractor::MIDDLE;
    }
    else if(e->button() == Qt::RightButton)
    {
        m_lastPosRightClick = new QPoint(e->x(), e->y());

        info.button = ::fwRenderOgre::interactor::IInteractor::RIGHT;
    }

    Q_EMIT interacted(info);
    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::mouseReleaseEvent( QMouseEvent* e )
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType     = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::BUTTONRELEASE;
    info.button              = ::fwRenderOgre::interactor::IInteractor::UNKNOWN;
    info.delta               = 0;
    std::tie(info.x, info.y) = Window::getDeviceCoordinates(e->x(), e->y());
    info.dx                  = 0;
    info.dy                  = 0;

    if(e->button() == Qt::LeftButton && m_lastPosLeftClick)
    {
        delete m_lastPosLeftClick;
        m_lastPosLeftClick = nullptr;

        info.button = ::fwRenderOgre::interactor::IInteractor::LEFT;
    }
    else if(e->button() == Qt::MiddleButton && m_lastPosMiddleClick)
    {
        delete m_lastPosMiddleClick;
        m_lastPosMiddleClick = nullptr;

        info.button = ::fwRenderOgre::interactor::IInteractor::MIDDLE;
    }
    else if(e->button() == Qt::RightButton)
    {
        delete m_lastPosRightClick;
        m_lastPosRightClick = nullptr;

        info.button = ::fwRenderOgre::interactor::IInteractor::RIGHT;
    }

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::focusInEvent(QFocusEvent*)
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::FOCUSIN;
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::focusOutEvent(QFocusEvent*)
{
    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::FOCUSOUT;
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::ogreResize(const QSize& newSize)
{
    if(!newSize.isValid())
    {
        return;
    }

    int newWidth, newHeight;
    std::tie(newWidth, newHeight) = Window::getDeviceCoordinates(newSize.width(), newSize.height());

    this->makeCurrent();

#if defined(linux) || defined(__linux) || defined(__APPLE__)
    m_ogreRenderWindow->resize(static_cast< unsigned int >(newWidth), static_cast< unsigned int >(newHeight));
#endif
    m_ogreRenderWindow->windowMovedOrResized();

    const auto numViewports = m_ogreRenderWindow->getNumViewports();

    ::Ogre::Viewport* viewport = nullptr;
    for (unsigned short i = 0; i < numViewports; i++)
    {
        viewport = m_ogreRenderWindow->getViewport(i);
        viewport->getCamera()->setAspectRatio(::Ogre::Real(newWidth) / ::Ogre::Real(newHeight));
    }

    if (viewport && ::Ogre::CompositorManager::getSingleton().hasCompositorChain(viewport))
    {
        ::Ogre::CompositorChain* chain = ::Ogre::CompositorManager::getSingleton().getCompositorChain(
            viewport);

        for(auto instance : chain->getCompositorInstances())
        {
            if( instance->getEnabled() )
            {
                instance->setEnabled(false);
                instance->setEnabled(true);
            }
        }
    }

    ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info;
    info.interactionType = ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::RESIZE;
    info.x               = newWidth;
    info.y               = newHeight;
    info.dx              = 0;
    info.dy              = 0;
    Q_EMIT interacted(info);

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::onScreenChanged(QScreen*)
{
    if(m_ogreRenderWindow != nullptr)
    {
        // This allows correct rendering on dual screen displays when dragging the window to another screen.
        QWindow* parent = this->parent();
        if(parent != nullptr)
        {
            parent->requestUpdate();
        }
        this->ogreResize(this->size());
    }
}

// ----------------------------------------------------------------------------

void Window::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
    {
        renderLater();
    }
}

// ----------------------------------------------------------------------------

Ogre::RenderWindow* Window::getOgreRenderWindow()
{
    return m_ogreRenderWindow;
}

// ----------------------------------------------------------------------------

Ogre::OverlaySystem* Window::getOgreOverlaySystem()
{
    return m_ogreOverlaySystem;
}

// ----------------------------------------------------------------------------

int Window::getId()
{
    return m_id;
}

//-----------------------------------------------------------------------------

void Window::preViewportUpdate(const ::Ogre::RenderTargetViewportEvent& evt)
{
    auto overlayIterator = ::Ogre::OverlayManager::getSingleton().getOverlayIterator();
    for(auto overlayMapElt : overlayIterator)
    {
        ::Ogre::Overlay* overlay = overlayMapElt.second;

        if(m_enabledOverlays.find(overlay) != m_enabledOverlays.end())
        {
            overlay->show();
        }
        else
        {
            overlay->hide();
        }
    }

    ::Ogre::RenderTargetListener::preViewportUpdate(evt);
}

//-----------------------------------------------------------------------------

void Window::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
    for(::Ogre::Overlay* o : m_enabledOverlays)
    {
        o->hide();
    }

    ::Ogre::RenderTargetListener::postViewportUpdate(evt);
}

} // namespace visuOgreQt
