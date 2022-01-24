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

#include "modules/viz/scene3dQt/Window.hpp"

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <viz/scene3d/Utils.hpp>
#include <viz/scene3d/WindowManager.hpp>

#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

namespace sight::module::viz::scene3dQt
{

// ----------------------------------------------------------------------------

static inline sight::viz::scene3d::interactor::IInteractor::Modifier convertModifiers(Qt::KeyboardModifiers _qmods)
{
    using SightOgreModType = sight::viz::scene3d::interactor::IInteractor::Modifier;
    SightOgreModType mods = SightOgreModType::NONE;
    mods |= (_qmods& Qt::ShiftModifier) ? SightOgreModType::SHIFT : SightOgreModType::NONE;
    mods |= (_qmods& Qt::ControlModifier) ? SightOgreModType::CONTROL : SightOgreModType::NONE;
    mods |= (_qmods& Qt::AltModifier) ? SightOgreModType::ALT : SightOgreModType::NONE;
    mods |= (_qmods& Qt::MetaModifier) ? SightOgreModType::META : SightOgreModType::NONE;

    return mods;
}

// ----------------------------------------------------------------------------

static inline QPoint getCursorPosition(const QWindow* const _w)
{
    const auto globalCursorPosition = QCursor::pos();
    const auto widgetCursorPosition = _w->mapFromGlobal(globalCursorPosition);

    if(_w->geometry().contains(widgetCursorPosition))
    {
        return widgetCursorPosition;
    }

    return QPoint();
}

// ----------------------------------------------------------------------------

int Window::m_counter = 0;

// ----------------------------------------------------------------------------

Window::Window(QWindow* _parent) :
    QWindow(_parent),
    m_id(Window::m_counter++)
{
    connect(this, &Window::screenChanged, this, &Window::onScreenChanged);
}

// ----------------------------------------------------------------------------

Window::~Window()
{
    this->QWindow::destroy();
}

// ----------------------------------------------------------------------------

void Window::render(QPainter*)
{
}

// ----------------------------------------------------------------------------

void Window::initialize()
{
    m_ogreRoot = sight::viz::scene3d::Utils::getOgreRoot();

    SIGHT_ASSERT(
        "OpenGL RenderSystem not found",
        m_ogreRoot->getRenderSystem()->getName().find("GL") != std::string::npos
    );

    Ogre::NameValuePairList parameters;

    // We share the OpenGL context on all windows. The first window will create the context, the other ones will
    // reuse the current context.
    parameters["currentGLContext"]  = "true";
    parameters["externalGLControl"] = "true"; // Let us handle buffer swapping and vsync.

    /*
       We need to supply the low level OS window handle to this QWindow so that Ogre3D knows where to draw
       the scene. Below is a cross-platform method on how to do this.
     */
#if defined(Q_OS_WIN)
    {
        const size_t winId = static_cast<size_t>(this->winId());
        parameters["externalWindowHandle"] = Ogre::StringConverter::toString(winId);
        parameters["parentWindowHandle"]   = Ogre::StringConverter::toString(winId);
    }
#else
    {
        const unsigned long winId = static_cast<unsigned long>(this->winId());
        parameters["externalWindowHandle"] = Ogre::StringConverter::toString(winId);
    }
#endif

    m_glContext = module::viz::scene3dQt::OpenGLContext::getGlobalOgreOpenGLContext();
    this->makeCurrent();

    const int width  = static_cast<int>(this->width() * this->devicePixelRatio());
    const int height = static_cast<int>(this->height() * this->devicePixelRatio());

    m_ogreRenderWindow = m_ogreRoot->createRenderWindow(
        "Widget-RenderWindow_" + std::to_string(m_id),
        static_cast<unsigned int>(width),
        static_cast<unsigned int>(height),
        false,
        &parameters
    );

    m_ogreRenderWindow->setVisible(true);
    m_ogreRenderWindow->setAutoUpdated(false);

    sight::viz::scene3d::WindowManager::sptr mgr = sight::viz::scene3d::WindowManager::get();
    mgr->registerWindow(m_ogreRenderWindow);

    sight::viz::scene3d::IWindowInteractor::InteractionInfo info;
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::RESIZE;
    info.x               = width;
    info.y               = height;
    info.dx              = 0;
    info.dy              = 0;
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

Ogre::RenderWindow* Window::getOgreRenderWindow() const
{
    return m_ogreRenderWindow;
}

// ----------------------------------------------------------------------------

int Window::getId() const
{
    return m_id;
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
        sight::viz::scene3d::WindowManager::sptr mgr = sight::viz::scene3d::WindowManager::get();
        mgr->unregisterWindow(m_ogreRenderWindow);
        m_ogreRenderWindow = nullptr;
    }
}

// ----------------------------------------------------------------------------

void Window::render()
{
    if(m_ogreRenderWindow == nullptr)
    {
        return;
    }

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

    try
    {
        m_ogreRoot->_fireFrameStarted();
        m_ogreRenderWindow->update();
        m_ogreRoot->_fireFrameRenderingQueued();
        m_ogreRoot->_fireFrameEnded();

        m_glContext->swapBuffers(this);
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Exception occured during Ogre rendering" << e.what());
    }
}

// ----------------------------------------------------------------------------

void Window::renderLater()
{
    /*
       This function forces QWindow to keep rendering. Omitting this causes the renderNow() function to
       only get called when the window is resized, moved, etc. as opposed to all of the time; which is
       generally what we need.
     */
    if(!m_update_pending)
    {
        m_update_pending = true;
        QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

// ----------------------------------------------------------------------------

bool Window::event(QEvent* _event)
{
    /*
       QWindow's "message pump". The base method that handles all QWindow events. As you will see there
       are other methods that actually process the keyboard/other events of Qt and the underlying OS.

       Note that we call the renderNow() function which checks to see if everything is initialized, etc.
       before calling the render() function.
     */

    switch(_event->type())
    {
        case QEvent::UpdateRequest:
            m_update_pending = false;
            renderNow();
            return true;

        case QEvent::Resize:
        {
            bool result = QWindow::event(_event);

            if(m_ogreRenderWindow != nullptr && m_ogreSize != this->size())
            {
                this->ogreResize(this->size());
            }

            return result;
        }

        default:
            break;
    }

    return QWindow::event(_event);
}

// ----------------------------------------------------------------------------

void Window::exposeEvent(QExposeEvent*)
{
    // Force rendering
    this->renderNow();
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

void Window::renderNow()
{
    // Small optimization to not render when not visible
    if(!this->isExposed())
    {
        return;
    }

    if(m_ogreSize != this->size())
    {
        this->ogreResize(this->size());
        return;
    }

    this->render();
}

// ----------------------------------------------------------------------------

void Window::keyPressEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info;
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::KEYPRESS;
    info.modifiers       = convertModifiers(QApplication::keyboardModifiers());
    info.key             = _e->key();

    auto cursorPosition = getCursorPosition(this);
    info.x = static_cast<int>(cursorPosition.x() * this->devicePixelRatio());
    info.y = static_cast<int>(cursorPosition.y() * this->devicePixelRatio());

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::keyReleaseEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info;
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::KEYRELEASE;
    info.modifiers       = convertModifiers(QApplication::keyboardModifiers());
    info.key             = _e->key();

    auto cursorPosition = getCursorPosition(this);
    info.x = static_cast<int>(cursorPosition.x() * this->devicePixelRatio());
    info.y = static_cast<int>(cursorPosition.y() * this->devicePixelRatio());

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

Window::InteractionInfo Window::convertMouseEvent(
    const QMouseEvent* const _evt,
    InteractionInfo::InteractionEnum _interactionType
) const
{
    InteractionInfo info;
    const auto button        = _evt->button();
    const auto activeButtons = _evt->buttons();

    switch(button)
    {
        case Qt::NoButton:
            if((activeButtons& Qt::LeftButton) == Qt::LeftButton)
            {
                info.button = sight::viz::scene3d::interactor::IInteractor::LEFT;
            }
            else if((activeButtons& Qt::MiddleButton) == Qt::MiddleButton)
            {
                info.button = sight::viz::scene3d::interactor::IInteractor::MIDDLE;
            }
            else if((activeButtons& Qt::RightButton) == Qt::RightButton)
            {
                info.button = sight::viz::scene3d::interactor::IInteractor::RIGHT;
            }
            else
            {
                info.button = sight::viz::scene3d::interactor::IInteractor::UNKNOWN;
            }

            break;

        case Qt::LeftButton:
            info.button = sight::viz::scene3d::interactor::IInteractor::LEFT;
            break;

        case Qt::MiddleButton:
            info.button = sight::viz::scene3d::interactor::IInteractor::MIDDLE;
            break;

        case Qt::RightButton:
            info.button = sight::viz::scene3d::interactor::IInteractor::RIGHT;
            break;

        default:
            info.button = sight::viz::scene3d::interactor::IInteractor::UNKNOWN;
            break;
    }

    info.interactionType = _interactionType;
    info.x               = static_cast<int>(_evt->x() * this->devicePixelRatio());
    info.y               = static_cast<int>(_evt->y() * this->devicePixelRatio());

    if(m_lastMousePosition)
    {
        const auto& point = m_lastMousePosition.value();
        info.dx = static_cast<int>((point.x() - _evt->x()) * this->devicePixelRatio());
        info.dy = static_cast<int>((point.y() - _evt->y()) * this->devicePixelRatio());
    }
    else
    {
        info.dx = 0;
        info.dy = 0;
    }

    info.modifiers = convertModifiers(_evt->modifiers());

    return info;
}

// ----------------------------------------------------------------------------

void Window::mouseMoveEvent(QMouseEvent* _e)
{
    const auto info = this->convertMouseEvent(_e, InteractionInfo::MOUSEMOVE);

    if(m_lastMousePosition)
    {
        m_lastMousePosition = _e->pos();
    }

    Q_EMIT interacted(info);

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::wheelEvent(QWheelEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info;
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::WHEELMOVE;

    // Only manage vertical wheel scroll.
    info.delta = static_cast<int>(_e->angleDelta().y() * this->devicePixelRatio());

    info.x         = static_cast<int>(_e->x() * this->devicePixelRatio());
    info.y         = static_cast<int>(_e->y() * this->devicePixelRatio());
    info.dx        = 0;
    info.dy        = 0;
    info.modifiers = convertModifiers(_e->modifiers());

    Q_EMIT interacted(info);

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::mousePressEvent(QMouseEvent* _e)
{
    m_lastMousePosition = _e->pos();

    const auto info = this->convertMouseEvent(_e, InteractionInfo::BUTTONPRESS);
    Q_EMIT interacted(info);

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::mouseDoubleClickEvent(QMouseEvent* _e)
{
    const auto info = this->convertMouseEvent(_e, InteractionInfo::BUTTONDOUBLEPRESS);
    Q_EMIT interacted(info);

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::mouseReleaseEvent(QMouseEvent* _e)
{
    m_lastMousePosition.reset();

    const auto info = this->convertMouseEvent(_e, InteractionInfo::BUTTONRELEASE);
    Q_EMIT interacted(info);

    this->requestRender();
}

// ----------------------------------------------------------------------------

void Window::ogreResize(const QSize& _newSize)
{
    if(!_newSize.isValid())
    {
        return;
    }

    m_ogreSize = _newSize;

    const int newWidth  = static_cast<int>(this->devicePixelRatio() * m_ogreSize.width());
    const int newHeight = static_cast<int>(this->devicePixelRatio() * m_ogreSize.height());

    this->makeCurrent();

#if defined(__unix__)
    m_ogreRenderWindow->resize(static_cast<unsigned int>(newWidth), static_cast<unsigned int>(newHeight));
#endif
    m_ogreRenderWindow->windowMovedOrResized();

    const auto numViewports = m_ogreRenderWindow->getNumViewports();

    Ogre::Viewport* viewport = nullptr;
    for(unsigned short i = 0 ; i < numViewports ; i++)
    {
        viewport = m_ogreRenderWindow->getViewport(i);

        const float vpWidth  = static_cast<float>(viewport->getActualWidth());
        const float vpHeight = static_cast<float>(viewport->getActualHeight());

        viewport->getCamera()->setAspectRatio(vpWidth / vpHeight);
    }

    if(viewport && Ogre::CompositorManager::getSingleton().hasCompositorChain(viewport))
    {
        Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(
            viewport
        );

        for(auto instance : chain->getCompositorInstances())
        {
            if(instance->getEnabled())
            {
                instance->setEnabled(false);
                instance->setEnabled(true);
            }
        }
    }

    sight::viz::scene3d::IWindowInteractor::InteractionInfo info;
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::RESIZE;
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
        this->ogreResize(this->size());
    }
}

// ----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
