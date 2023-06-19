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
#include "modules/viz/scene3dQt/init.hpp"

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <core/tools/compare.hpp>

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
    if(not isVisible())
    {
        m_update_pending = true;
    }
    else
    {
        update();
    }
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

    m_renderTargets.clear();

    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
    meshManager.remove(m_fsQuadPlane);
    m_fsQuadPlane.reset();
}

// ----------------------------------------------------------------------------

void Window::renderNow()
{
    if(not isVisible())
    {
        m_update_pending = true;
    }
    else
    {
        update();
    }
}

//------------------------------------------------------------------------------

bool Window::event(QEvent* _e)
{
    switch(_e->type())
    {
        case QEvent::Gesture:
            // Filter gesture events
            gestureEvent(static_cast<QGestureEvent*>(_e));
            return true;

        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            // If we are currently in a gesture, filter mouse events
            if(m_gestureState != GestureState::NoGesture)
            {
                return true;
            }

            break;

        case QEvent::Show:
            if(m_update_pending)
            {
                update();
                m_update_pending = false;
            }

            break;

        default:
            break;
    }

    return QOpenGLWidget::event(_e);
}

// ----------------------------------------------------------------------------

void Window::keyPressEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::KEYPRESS;
    info.modifiers       = convertModifiers(QApplication::keyboardModifiers());
    info.key             = _e->key();

    auto cursorPosition = getCursorPosition(this);
    const auto ratio    = devicePixelRatioF();
    info.x = static_cast<int>(cursorPosition.x() * ratio);
    info.y = static_cast<int>(cursorPosition.y() * ratio);

    Q_EMIT interacted(info);
    requestRender();
}

// ----------------------------------------------------------------------------

void Window::keyReleaseEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::KEYRELEASE;
    info.modifiers       = convertModifiers(QApplication::keyboardModifiers());
    info.key             = _e->key();

    const auto cursorPosition = getCursorPosition(this);
    const auto ratio          = devicePixelRatioF();
    info.x = static_cast<int>(cursorPosition.x() * ratio);
    info.y = static_cast<int>(cursorPosition.y() * ratio);

    Q_EMIT interacted(info);
    requestRender();
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

    const auto ratio = devicePixelRatioF();
    info.interactionType = _interactionType;
    info.x               = static_cast<int>(_evt->x() * ratio);
    info.y               = static_cast<int>(_evt->y() * ratio);

    if(m_lastMousePosition)
    {
        const auto& point = m_lastMousePosition.value();
        info.dx = static_cast<int>((point.x() - _evt->x()) * ratio);
        info.dy = static_cast<int>((point.y() - _evt->y()) * ratio);
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
    if(m_gestureState != GestureState::NoGesture)
    {
        return;
    }

    const auto info = this->convertMouseEvent(_e, InteractionInfo::MOUSEMOVE);

    if(m_lastMousePosition)
    {
        m_lastMousePosition = _e->pos();
    }

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::wheelEvent(QWheelEvent* _e)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::WHEELMOVE;

    // Only manage vertical wheel scroll.
    const auto ratio = devicePixelRatioF();
    info.delta = static_cast<int>(_e->angleDelta().y() * ratio);

    info.x         = static_cast<int>(_e->position().x() * ratio);
    info.y         = static_cast<int>(_e->position().y() * ratio);
    info.dx        = 0;
    info.dy        = 0;
    info.modifiers = convertModifiers(_e->modifiers());

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::mousePressEvent(QMouseEvent* _e)
{
    if(m_gestureState != GestureState::NoGesture)
    {
        return;
    }

    m_lastMousePosition = _e->pos();

    const auto info = this->convertMouseEvent(_e, InteractionInfo::BUTTONPRESS);
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::mouseDoubleClickEvent(QMouseEvent* _e)
{
    if(m_gestureState != GestureState::NoGesture)
    {
        return;
    }

    const auto info = this->convertMouseEvent(_e, InteractionInfo::BUTTONDOUBLEPRESS);
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::mouseReleaseEvent(QMouseEvent* _e)
{
    if(m_gestureState != GestureState::NoGesture)
    {
        return;
    }

    m_lastMousePosition.reset();

    const auto info = this->convertMouseEvent(_e, InteractionInfo::BUTTONRELEASE);
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::leaveEvent(QEvent* /*_e*/)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::LEAVE;

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void Window::enterEvent(QEvent* /*_e*/)
{
    sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::ENTER;

    Q_EMIT interacted(info);
}

//------------------------------------------------------------------------------

void Window::gestureEvent(QGestureEvent* _e)
{
    /// @note Both event could be triggered at the same time.
    auto* const panGesture   = static_cast<QPanGesture*>(_e->gesture(Qt::PanGesture));
    auto* const pinchGesture = static_cast<QPinchGesture*>(_e->gesture(Qt::PinchGesture));

    // If no managed gesture
    if(panGesture == nullptr && pinchGesture == nullptr)
    {
        // Early return
        m_gestureState = GestureState::NoGesture;
        _e->ignore();
        return;
    }

    // Get the pixel ratio
    const auto ratio = devicePixelRatioF();

    // If pan gesture
    if(panGesture != nullptr)
    {
        switch(panGesture->state())
        {
            case Qt::GestureStarted:
            case Qt::GestureUpdated:
                m_gestureState = GestureState::PanGesture;
                break;

            default:
                m_gestureState = GestureState::NoGesture;
                break;
        }

        if(const auto& delta = panGesture->delta();
           core::tools::is_greater(std::abs(delta.x()), 0.0F) || core::tools::is_greater(std::abs(delta.y()), 0.0F))
        {
            sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};

            switch(panGesture->state())
            {
                case Qt::GestureStarted:
                case Qt::GestureUpdated:
                    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::PAN_GESTURE_MOVE;
                    break;

                default:
                    info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::PAN_GESTURE_RELEASE;
                    break;
            }

            const auto& position = pinchGesture != nullptr
                                   ? pinchGesture->centerPoint()
                                   : panGesture->hotSpot() + panGesture->offset();

            const auto& mapped = mapFromGlobal(position.toPoint());

            info.x = int(mapped.x() * ratio);
            info.y = int(mapped.y() * ratio);

            info.dx = int(-delta.x() * ratio);
            info.dy = int(-delta.y() * ratio);

            Q_EMIT interacted(info);
        }
    }

    if(pinchGesture != nullptr)
    {
        switch(pinchGesture->state())
        {
            case Qt::GestureStarted:
            case Qt::GestureUpdated:
                m_gestureState = GestureState::PinchGesture;
                break;

            default:
                m_gestureState = GestureState::NoGesture;
                break;
        }

        // Ignore the gesture if the scale factor is 0 or too near 1.0
        if(const auto scale = pinchGesture->scaleFactor();
           !core::tools::is_equal(scale, 0) && !core::tools::is_less(std::abs(scale - 1.0F), 0.01F))
        {
            sight::viz::scene3d::IWindowInteractor::InteractionInfo info {};
            info.interactionType = sight::viz::scene3d::IWindowInteractor::InteractionInfo::PINCH_GESTURE;

            // scaleFactor is a positive number, where a number inferior to 1 means that the distance between the
            // fingers increases, and a number superior to 1 means that the distance between the fingers decreases.
            // In order to interface with the mouse wheel methods, where angleDelta is positive if the wheel is
            // rotated away from the user and negative if the wheel is rotated toward the user, the following
            // transformation is done.
            if(core::tools::is_less(scale, 1.0F))
            {
                info.delta = -1 / scale;
            }
            else
            {
                info.delta = scale;
            }

            const auto& center = mapFromGlobal(pinchGesture->centerPoint().toPoint());
            info.x = int(center.x() * ratio);
            info.y = int(center.y() * ratio);

            Q_EMIT interacted(info);
        }
    }

    requestRender();
}

// ----------------------------------------------------------------------------

void Window::ogreResize(const QSize& _newSize)
{
    if(!_newSize.isValid() || !m_init || _newSize == m_ogreSize)
    {
        return;
    }

    m_ogreSize = _newSize;

    const auto ratio    = devicePixelRatioF();
    const int newWidth  = static_cast<int>(ratio * _newSize.width());
    const int newHeight = static_cast<int>(ratio * _newSize.height());

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
        SIGHT_ASSERT("Texture could not be created", renderTarget.texture);

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
            SIGHT_ASSERT("Width and height should be strictly positive", !std::isnan(aspectRatio));
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
            SIGHT_ASSERT("Width and height should be strictly positive", vpWidth > 0 && vpHeight > 0);

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

    const auto ratio = devicePixelRatioF();
    const int width  = static_cast<int>(this->width() * ratio);
    const int height = static_cast<int>(this->height() * ratio);

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

            const auto ratio = devicePixelRatioF();
            this->context()->functions()->glViewport(
                0,
                0,
                int(ratio * this->width()),
                int(ratio * this->height())
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
