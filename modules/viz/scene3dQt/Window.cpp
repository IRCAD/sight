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

#include "modules/viz/scene3dQt/Window.hpp"

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <ui/qt/gestures/QPanGestureRecognizer.hpp>
#include <modules/viz/scene3dQt/init.hpp>

#include <viz/scene3d/Utils.hpp>
#include <viz/scene3d/WindowManager.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OgreTextureManager.h>
#include <OgreRenderTarget.h>
#include <OgreRenderTexture.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreMeshManager.h>
#include <QOpenGLFunctions>

namespace sight::module::viz::scene3dQt
{

// ----------------------------------------------------------------------------

static inline sight::viz::scene3d::interactor::IInteractor::Modifier convertModifiers(Qt::KeyboardModifiers _modifiers)
{
    using SightOgreModType = sight::viz::scene3d::interactor::IInteractor::Modifier;
    SightOgreModType mods = SightOgreModType::NONE;
    mods |= (_modifiers& Qt::ShiftModifier) != 0U ? SightOgreModType::SHIFT : SightOgreModType::NONE;
    mods |= (_modifiers& Qt::ControlModifier) != 0U ? SightOgreModType::CONTROL : SightOgreModType::NONE;
    mods |= (_modifiers& Qt::AltModifier) != 0U ? SightOgreModType::ALT : SightOgreModType::NONE;
    mods |= (_modifiers& Qt::MetaModifier) != 0U ? SightOgreModType::META : SightOgreModType::NONE;

    return mods;
}

// ----------------------------------------------------------------------------

static inline QPoint getCursorPosition(const QWidget* const _w)
{
    const auto globalCursorPosition = QCursor::pos();
    const auto widgetCursorPosition = _w->mapFromGlobal(globalCursorPosition);

    if(_w->geometry().contains(widgetCursorPosition))
    {
        return widgetCursorPosition;
    }

    return {};
}

// ----------------------------------------------------------------------------

int Window::m_counter = 0;

// ----------------------------------------------------------------------------

Window::Window() :
    QOpenGLWidget(nullptr),
    m_id(Window::m_counter++)
{
    this->setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    this->setFocusPolicy(Qt::ClickFocus);
}

// ----------------------------------------------------------------------------

Window::~Window()
{
    auto& textureMgr = Ogre::TextureManager::getSingleton();
    auto& matMgr     = Ogre::MaterialManager::getSingleton();

    for(auto& renderTarget : m_renderTargets)
    {
        if(renderTarget.texture)
        {
            textureMgr.remove(renderTarget.texture);
            renderTarget.texture.reset();

            matMgr.remove(renderTarget.material);
            renderTarget.material.reset();
        }
    }
}

//------------------------------------------------------------------------------

void Window::registerLayer(sight::viz::scene3d::Layer::wptr _layer)
{
    m_renderTargets.push_back({_layer, nullptr, nullptr});
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
    QOpenGLWidget::makeCurrent();
}

// ----------------------------------------------------------------------------

void Window::destroyWindow()
{
    m_init = false;
    m_renderTargets.clear();
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
        update();
    }
}

// ----------------------------------------------------------------------------

void Window::renderNow()
{
    this->update();
}

// ----------------------------------------------------------------------------

void Window::keyPressEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
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
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
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
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::WHEELMOVE;

    // Only manage vertical wheel scroll.
    info.delta = static_cast<int>(_e->angleDelta().y() * this->devicePixelRatio());

    info.x         = static_cast<int>(_e->position().x() * this->devicePixelRatio());
    info.y         = static_cast<int>(_e->position().y() * this->devicePixelRatio());
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

//------------------------------------------------------------------------------

void Window::gestureEvent(QGestureEvent* _e)
{
    // QWindow doesn't support gestures by itself; gesture events are sent by the event filter in WindowInteractor
    // thanks to GestureFilter.
    _e->accept();
    if(QGesture* pinchGesture = _e->gesture(Qt::PinchGesture),
       *pan2Gesture = _e->gesture(sight::ui::qt::gestures::QPanGestureRecognizer::get<2>());
       pinchGesture != nullptr || pan2Gesture != nullptr)
    {
        if(pan2Gesture != nullptr)
        {
            _e->accept(pan2Gesture);
            auto* pan = static_cast<sight::ui::qt::gestures::PanGesture*>(pan2Gesture);
            sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
            info.interactionType =
                (pan2Gesture->state()
                 == Qt::GestureFinished ? sight::viz::scene3d::IWindowInteractor::InteractionInfo::PAN2_GESTURE_RELEASE
                                        : sight::viz::scene3d::IWindowInteractor::InteractionInfo::PAN2_GESTURE_MOVE);
            QPoint position = mapFromGlobal(pan->position().toPoint());
            info.x = position.x();
            info.y = position.y();
            QPoint deltaPoint = pan->delta().toPoint();
            info.dx = -deltaPoint.x();
            info.dy = -deltaPoint.y();
            Q_EMIT interacted(info);
            this->requestRender();
        }

        if(pinchGesture != nullptr)
        {
            auto* pinch = static_cast<QPinchGesture*>(pinchGesture);
            sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
            info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::PINCH_GESTURE;
            double delta = pinch->scaleFactor();
            if(delta == 0)
            {
                _e->ignore();
                return;
            }

            // scaleFactor is a positive number, where a number inferior to 1 means that the distance between the
            // fingers
            // increases, and a number superior to 1 means that the distance between the fingers decreases. In order to
            // interface with the mouse wheel methods, where angleDelta is positive if the wheel is rotated away from
            // the
            // user and negative if the wheel is rotated toward the user, the following transformation is done.
            if(delta < 1)
            {
                info.delta = -1 / delta;
            }
            else
            {
                info.delta = delta;
            }

            _e->accept(pinchGesture);
            QPoint localCenterPoint = mapFromGlobal(pinch->centerPoint().toPoint());
            info.x = localCenterPoint.x();
            info.y = localCenterPoint.y();
            Q_EMIT interacted(info);
            this->requestRender();
        }
    }
    else if(QGesture* panGesture = _e->gesture(sight::ui::qt::gestures::QPanGestureRecognizer::get<1>()))
    {
        _e->accept(panGesture);
        auto* pan = static_cast<sight::ui::qt::gestures::PanGesture*>(panGesture);
        sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
        info.interactionType =
            (panGesture->state()
             == Qt::GestureFinished ? sight::viz::scene3d::IWindowInteractor::InteractionInfo::PAN_GESTURE_RELEASE
                                    : sight::viz::scene3d::IWindowInteractor::InteractionInfo::PAN_GESTURE_MOVE);
        QPoint position = mapFromGlobal(pan->position().toPoint());
        info.x = position.x();
        info.y = position.y();
        QPoint deltaPoint = pan->delta().toPoint();
        info.dx = -deltaPoint.x();
        info.dy = -deltaPoint.y();
        Q_EMIT interacted(info);
        this->requestRender();
    }
    else if(QGesture* tapAndHoldGesture = _e->gesture(Qt::TapAndHoldGesture))
    {
        if(tapAndHoldGesture->state() == Qt::GestureFinished)
        {
            _e->accept(tapAndHoldGesture);
            auto* tap = static_cast<QTapAndHoldGesture*>(tapAndHoldGesture);
            sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
            info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::LONG_TAP_GESTURE;
            QPoint position = mapFromGlobal(tap->position().toPoint());
            info.x = position.x();
            info.y = position.y();
            Q_EMIT interacted(info);
            this->requestRender();
        }
    }
    else if(QGesture* tapGesture = _e->gesture(Qt::TapGesture))
    {
        if(tapGesture->state() == Qt::GestureCanceled && _e->activeGestures().isEmpty())
        {
            _e->accept(tapGesture);
            auto* tap = static_cast<QTapGesture*>(tapGesture);
            sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
            info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::BUTTONRELEASE;
            QPoint position = mapFromGlobal(tap->position().toPoint());
            info.x = position.x();
            info.y = position.y();
            Q_EMIT interacted(info);
            this->requestRender();
        }
    }
    else
    {
        _e->ignore();
    }
}

// ----------------------------------------------------------------------------

void Window::ogreResize(const QSize& _newSize)
{
    if(!_newSize.isValid() || !m_init)
    {
        return;
    }

    m_ogreSize = _newSize;

    const int newWidth  = static_cast<int>(this->devicePixelRatio() * _newSize.width());
    const int newHeight = static_cast<int>(this->devicePixelRatio() * _newSize.height());

    createRenderTextures(newWidth, newHeight);

    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::RESIZE;
    info.x               = newWidth;
    info.y               = newHeight;
    info.dx              = 0;
    info.dy              = 0;
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::createRenderTextures(int w, int h)
{
    FW_PROFILE("createRenderTextures");
    auto& mgr = Ogre::TextureManager::getSingleton();

    unsigned i = 0;
    for(auto& renderTarget : m_renderTargets)
    {
        const auto layer = renderTarget.layer.lock();
        if(renderTarget.texture)
        {
            mgr.remove(renderTarget.texture);
            renderTarget.texture.reset();
        }

        renderTarget.texture = mgr.createManual(
            "RttTex" + std::to_string(m_id) + "_" + std::to_string(i++),
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,
            static_cast<uint>(w),
            static_cast<uint>(h),
            0,
            Ogre::PF_BYTE_RGBA,
            Ogre::TU_RENDERTARGET
        );

        auto* rt = renderTarget.texture->getBuffer()->getRenderTarget();
        layer->setRenderTarget(rt);

        if(!layer->initialized())
        {
            layer->createScene();
        }
        else
        {
            // This will be skipped on initialisation
            Ogre::Camera* camera = layer->getDefaultCamera();
            // Save last viewport and current aspect ratio
            Ogre::Viewport* oldViewport = camera->getViewport();
            Ogre::Real aspectRatio      = camera->getAspectRatio();

            // If the layer viewport is smaller than the widget size, then only the viewport is cleared.
            // This lefts all other areas uninitialized, which work on some platforms (NVidia) but not on others (Intel)
            // To overcome this, we add a fullscreen viewport to clear the whole frame
            if(oldViewport->getLeft() != 0.F || oldViewport->getTop() != 0.F
               || oldViewport->getWidth() != 1.F || oldViewport->getHeight() != 1.F)
            {
                Ogre::Viewport* clearVp = rt->addViewport(camera, oldViewport->getZOrder() - 1, 0.F, 0.F, 1.F, 1.F);
                clearVp->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
                clearVp->setVisibilityMask(0); // Exclude all objects, only keep the clear
            }

            Ogre::Viewport* vp = rt->addViewport(
                camera,
                oldViewport->getZOrder(),
                oldViewport->getLeft(),
                oldViewport->getTop(),
                oldViewport->getWidth(),
                oldViewport->getHeight()
            );

            // Should restore aspect ratio, in case of auto aspect ratio
            // enabled, it'll changed when add new viewport.
            camera->setAspectRatio(aspectRatio);
            // Should restore last viewport, i.e. never disturb user code
            // which might based on that.
            camera->_notifyViewport(oldViewport);
            if(layer->getOrder() != 0)
            {
                vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
            }

            Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
            if((vp != nullptr) && compositorManager.hasCompositorChain(oldViewport))
            {
                Ogre::CompositorChain* oldChain = compositorManager.getCompositorChain(oldViewport);

                std::vector<std::string> compositors;
                std::ranges::for_each(
                    oldChain->getCompositorInstances(),
                    [&compositors, &compositorManager, vp](auto& x)
                    {
                        const std::string name = x->getCompositor()->getName();
                        compositors.push_back(name);
                        compositorManager.addCompositor(vp, name);
                        compositorManager.setCompositorEnabled(vp, name, x->getEnabled());
                    });
                std::ranges::for_each(
                    compositors,
                    [&compositorManager, oldViewport](auto& name)
                    {
                        compositorManager.setCompositorEnabled(oldViewport, name, false);
                        compositorManager.removeCompositor(oldViewport, name);
                    });
            }
        }

        const Ogre::Material::Techniques& techniques = renderTarget.material->getTechniques();

        for(const auto* const tech : techniques)
        {
            Ogre::Pass* const pass          = tech->getPass(0);
            Ogre::TextureUnitState* texUnit = pass->getTextureUnitState("RT");
            texUnit->setTexture(renderTarget.texture);
        }

        const auto numViewports = rt->getNumViewports();
        for(std::uint16_t v = 0 ; v < numViewports ; v++)
        {
            auto* viewport = rt->getViewport(v);

            const auto vpWidth  = static_cast<float>(viewport->getActualWidth());
            const auto vpHeight = static_cast<float>(viewport->getActualHeight());

            viewport->getCamera()->setAspectRatio(vpWidth / vpHeight);
        }
    }
}

//------------------------------------------------------------------------------

void Window::initializeGL()
{
    m_ogreRoot = sight::viz::scene3d::Utils::getOgreRoot();

    SIGHT_ASSERT(
        "OpenGL RenderSystem not found",
        m_ogreRoot->getRenderSystem()->getName().find("GL") != std::string::npos
    );

    initResources();

    const int width  = static_cast<int>(this->width() * this->devicePixelRatio());
    const int height = static_cast<int>(this->height() * this->devicePixelRatio());

    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
    Ogre::MovablePlane p(Ogre::Vector3::UNIT_Z, 0);
    m_fsQuadPlane =
        meshManager.createPlane("plane" + std::to_string(m_id), sight::viz::scene3d::RESOURCE_GROUP, p, 2, 2);

    unsigned int i = 0;
    for(auto& renderTarget : m_renderTargets)
    {
        auto material = Ogre::MaterialManager::getSingleton().getByName("Blit", sight::viz::scene3d::RESOURCE_GROUP);
        renderTarget.material = material->clone("mat" + std::to_string(m_id) + "_" + std::to_string(i++));
    }

    createRenderTextures(width, height);

    m_init = true;

    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::RESIZE;
    info.x               = width;
    info.y               = height;
    info.dx              = 0;
    info.dy              = 0;
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::paintGL()
{
    if(!m_init)
    {
        return;
    }

    m_update_pending = false;
    ++m_frameId;

    FW_PROFILE("paintGL");

    try
    {
        for(auto& renderTarget : m_renderTargets)
        {
            auto* rt = renderTarget.texture->getBuffer()->getRenderTarget();
            rt->update(false);

            this->makeCurrent();

            const Ogre::Material::Techniques& techniques = renderTarget.material->getTechniques();
            SIGHT_ASSERT("Can't find any technique", !techniques.empty());

            const auto* const tech          = techniques[0];
            Ogre::Pass* const pass          = tech->getPass(0);
            Ogre::TextureUnitState* texUnit = pass->getTextureUnitState("RT");
            texUnit->setTexture(renderTarget.texture);

            Ogre::RenderOperation op;
            m_fsQuadPlane->getSubMesh(0)->_getRenderOperation(op);

            this->context()->functions()->glViewport(
                0,
                0,
                this->devicePixelRatio() * this->width(),
                this->devicePixelRatio() * this->height()
            );

            auto* scene = renderTarget.layer.lock()->getSceneManager();
            // Somehow this variable is set to true when compositors are run, and this messes things up here
            // We just reset it here and Ogre will reset it when it needs it
            scene->setLateMaterialResolving(false);
            scene->manualRender(
                &op,
                pass,
                nullptr,
                Ogre::Affine3::IDENTITY,
                Ogre::Affine3::IDENTITY,
                Ogre::Matrix4::IDENTITY
            );
        }
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Exception occurred during Ogre rendering" << e.what());
    }
}

//------------------------------------------------------------------------------

void Window::resizeGL(int w, int h)
{
    ogreResize(QSize(w, h));
}

// ----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
