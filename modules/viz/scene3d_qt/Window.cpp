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

#include "modules/viz/scene3d_qt/Window.hpp"
#include "modules/viz/scene3d_qt/init.hpp"

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <core/tools/compare.hpp>

#include <viz/scene3d/utils.hpp>
#include <viz/scene3d/window_manager.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OgreTextureManager.h>
#include <OgreRenderTarget.h>
#include <OgreRenderTexture.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreMeshManager.h>

#include <QOpenGLFunctions>

namespace sight::module::viz::scene3d_qt
{

// ----------------------------------------------------------------------------

static inline sight::viz::scene3d::interactor::base::modifier convert_modifiers(Qt::KeyboardModifiers _modifiers)
{
    using sight_ogre_mod_t = sight::viz::scene3d::interactor::base::modifier;
    sight_ogre_mod_t mods = sight_ogre_mod_t::none;
    mods |= (_modifiers& Qt::ShiftModifier) != 0U ? sight_ogre_mod_t::shift : sight_ogre_mod_t::none;
    mods |= (_modifiers& Qt::ControlModifier) != 0U ? sight_ogre_mod_t::control : sight_ogre_mod_t::none;
    mods |= (_modifiers& Qt::AltModifier) != 0U ? sight_ogre_mod_t::alt : sight_ogre_mod_t::none;
    mods |= (_modifiers& Qt::MetaModifier) != 0U ? sight_ogre_mod_t::meta : sight_ogre_mod_t::none;

    return mods;
}

// ----------------------------------------------------------------------------

static inline QPoint get_cursor_position(const QWidget* const _w)
{
    const auto global_cursor_position = QCursor::pos();
    const auto widget_cursor_position = _w->mapFromGlobal(global_cursor_position);

    if(_w->geometry().contains(widget_cursor_position))
    {
        return widget_cursor_position;
    }

    return {};
}

// ----------------------------------------------------------------------------

int window::s_counter = 0;

// ----------------------------------------------------------------------------

window::window() :
    QOpenGLWidget(nullptr),
    m_id(window::s_counter++)
{
    this->setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    this->setFocusPolicy(Qt::ClickFocus);
}

//------------------------------------------------------------------------------

void window::register_layer(sight::viz::scene3d::layer::wptr _layer)
{
    m_render_targets.push_back({_layer, nullptr, nullptr});
}

// ----------------------------------------------------------------------------

int window::get_id() const
{
    return m_id;
}

// ----------------------------------------------------------------------------

void window::request_render()
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

void window::makeCurrent()
{
    QOpenGLWidget::makeCurrent();
}

// ----------------------------------------------------------------------------

void window::destroy_window()
{
    m_init = false;

    auto& texture_mgr = Ogre::TextureManager::getSingleton();
    auto& mat_mgr     = Ogre::MaterialManager::getSingleton();

    for(auto& render_target : m_render_targets)
    {
        if(render_target.texture)
        {
            texture_mgr.remove(render_target.texture);
            render_target.texture.reset();

            mat_mgr.remove(render_target.material);
            render_target.material.reset();
        }
    }

    m_render_targets.clear();

    Ogre::MeshManager& mesh_manager = Ogre::MeshManager::getSingleton();
    mesh_manager.remove(m_fs_quad_plane);
    m_fs_quad_plane.reset();
}

// ----------------------------------------------------------------------------

void window::render_now()
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

bool window::event(QEvent* _e)
{
    switch(_e->type())
    {
        case QEvent::Gesture:
            // filter gesture events
            gesture_event(static_cast<QGestureEvent*>(_e));
            return true;

        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            // If we are currently in a gesture, filter mouse events
            if(m_gesture_state != gesture_state::no_gesture)
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

void window::keyPressEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::keypress;
    info.modifiers        = convert_modifiers(QApplication::keyboardModifiers());
    info.key              = _e->key();

    auto cursor_position = get_cursor_position(this);
    const auto ratio     = devicePixelRatioF();
    info.x = static_cast<int>(cursor_position.x() * ratio);
    info.y = static_cast<int>(cursor_position.y() * ratio);

    Q_EMIT interacted(info);
    request_render();
}

// ----------------------------------------------------------------------------

void window::keyReleaseEvent(QKeyEvent* _e)
{
    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::keyrelease;
    info.modifiers        = convert_modifiers(QApplication::keyboardModifiers());
    info.key              = _e->key();

    const auto cursor_position = get_cursor_position(this);
    const auto ratio           = devicePixelRatioF();
    info.x = static_cast<int>(cursor_position.x() * ratio);
    info.y = static_cast<int>(cursor_position.y() * ratio);

    Q_EMIT interacted(info);
    request_render();
}

// ----------------------------------------------------------------------------

window::interaction_info window::convert_mouse_event(
    const QMouseEvent* const _evt,
    interaction_info::interaction_enum _interaction_type
) const
{
    interaction_info info;
    const auto button         = _evt->button();
    const auto active_buttons = _evt->buttons();

    switch(button)
    {
        case Qt::NoButton:
            if((active_buttons& Qt::LeftButton) == Qt::LeftButton)
            {
                info.button = sight::viz::scene3d::interactor::base::left;
            }
            else if((active_buttons& Qt::MiddleButton) == Qt::MiddleButton)
            {
                info.button = sight::viz::scene3d::interactor::base::middle;
            }
            else if((active_buttons& Qt::RightButton) == Qt::RightButton)
            {
                info.button = sight::viz::scene3d::interactor::base::right;
            }
            else
            {
                info.button = sight::viz::scene3d::interactor::base::unknown;
            }

            break;

        case Qt::LeftButton:
            info.button = sight::viz::scene3d::interactor::base::left;
            break;

        case Qt::MiddleButton:
            info.button = sight::viz::scene3d::interactor::base::middle;
            break;

        case Qt::RightButton:
            info.button = sight::viz::scene3d::interactor::base::right;
            break;

        default:
            info.button = sight::viz::scene3d::interactor::base::unknown;
            break;
    }

    const auto ratio = devicePixelRatioF();
    info.interaction_type = _interaction_type;
    info.x                = static_cast<int>(_evt->x() * ratio);
    info.y                = static_cast<int>(_evt->y() * ratio);

    if(m_last_mouse_position)
    {
        const auto& point = m_last_mouse_position.value();
        info.dx = static_cast<int>((point.x() - _evt->x()) * ratio);
        info.dy = static_cast<int>((point.y() - _evt->y()) * ratio);
    }
    else
    {
        info.dx = 0;
        info.dy = 0;
    }

    info.modifiers = convert_modifiers(_evt->modifiers());

    return info;
}

// ----------------------------------------------------------------------------

void window::mouseMoveEvent(QMouseEvent* _e)
{
    if(m_gesture_state != gesture_state::no_gesture)
    {
        return;
    }

    const auto info = this->convert_mouse_event(_e, interaction_info::mousemove);

    if(m_last_mouse_position)
    {
        m_last_mouse_position = _e->pos();
    }

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::wheelEvent(QWheelEvent* _e)
{
    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::wheelmove;

    // Only manage vertical wheel scroll.
    const auto ratio = devicePixelRatioF();
    info.delta = static_cast<int>(_e->angleDelta().y() * ratio);

    info.x         = static_cast<int>(_e->position().x() * ratio);
    info.y         = static_cast<int>(_e->position().y() * ratio);
    info.dx        = 0;
    info.dy        = 0;
    info.modifiers = convert_modifiers(_e->modifiers());

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::mousePressEvent(QMouseEvent* _e)
{
    if(m_gesture_state != gesture_state::no_gesture)
    {
        return;
    }

    m_last_mouse_position = _e->pos();

    const auto info = this->convert_mouse_event(_e, interaction_info::buttonpress);
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::mouseDoubleClickEvent(QMouseEvent* _e)
{
    if(m_gesture_state != gesture_state::no_gesture)
    {
        return;
    }

    const auto info = this->convert_mouse_event(_e, interaction_info::buttondoublepress);
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::mouseReleaseEvent(QMouseEvent* _e)
{
    if(m_gesture_state != gesture_state::no_gesture)
    {
        return;
    }

    m_last_mouse_position.reset();

    const auto info = this->convert_mouse_event(_e, interaction_info::buttonrelease);
    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::leaveEvent(QEvent* /*_e*/)
{
    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::leave;

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::enterEvent(QEvent* /*_e*/)
{
    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::enter;

    Q_EMIT interacted(info);
}

//------------------------------------------------------------------------------

void window::gesture_event(QGestureEvent* _e)
{
    /// @note Both event could be triggered at the same time.
    auto* const pan_gesture   = static_cast<QPanGesture*>(_e->gesture(Qt::PanGesture));
    auto* const pinch_gesture = static_cast<QPinchGesture*>(_e->gesture(Qt::PinchGesture));

    // If no managed gesture
    if(pan_gesture == nullptr && pinch_gesture == nullptr)
    {
        // Early return
        m_gesture_state = gesture_state::no_gesture;
        _e->ignore();
        return;
    }

    // Get the pixel ratio
    const auto ratio = devicePixelRatioF();

    // If pan gesture
    if(pan_gesture != nullptr)
    {
        switch(pan_gesture->state())
        {
            case Qt::GestureStarted:
            case Qt::GestureUpdated:
                m_gesture_state = gesture_state::pan_gesture;
                break;

            default:
                m_gesture_state = gesture_state::no_gesture;
                break;
        }

        if(const auto& delta = pan_gesture->delta();
           core::tools::is_greater(std::abs(delta.x()), 0.0F) || core::tools::is_greater(std::abs(delta.y()), 0.0F))
        {
            sight::viz::scene3d::window_interactor::interaction_info info {};

            switch(pan_gesture->state())
            {
                case Qt::GestureStarted:
                case Qt::GestureUpdated:
                    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::pan_gesture_move;
                    break;

                default:
                    info.interaction_type =
                        sight::viz::scene3d::window_interactor::interaction_info::pan_gesture_release;
                    break;
            }

            const auto& position = pinch_gesture != nullptr
                                   ? pinch_gesture->centerPoint()
                                   : pan_gesture->hotSpot() + pan_gesture->offset();

            const auto& mapped = mapFromGlobal(position.toPoint());

            info.x = int(mapped.x() * ratio);
            info.y = int(mapped.y() * ratio);

            info.dx = int(-delta.x() * ratio);
            info.dy = int(-delta.y() * ratio);

            Q_EMIT interacted(info);
        }
    }

    if(pinch_gesture != nullptr)
    {
        switch(pinch_gesture->state())
        {
            case Qt::GestureStarted:
            case Qt::GestureUpdated:
                m_gesture_state = gesture_state::pinch_gesture;
                break;

            default:
                m_gesture_state = gesture_state::no_gesture;
                break;
        }

        // Ignore the gesture if the scale factor is 0 or too near 1.0
        if(const auto scale = pinch_gesture->scaleFactor();
           !core::tools::is_equal(scale, 0) && !core::tools::is_less(std::abs(scale - 1.0F), 0.01F))
        {
            sight::viz::scene3d::window_interactor::interaction_info info {};
            info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::pinch_gesture;

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

            const auto& center = mapFromGlobal(pinch_gesture->centerPoint().toPoint());
            info.x = int(center.x() * ratio);
            info.y = int(center.y() * ratio);

            Q_EMIT interacted(info);
        }
    }

    request_render();
}

// ----------------------------------------------------------------------------

void window::ogre_resize(const QSize& _new_size)
{
    if(!_new_size.isValid() || !m_init || _new_size == m_ogre_size)
    {
        return;
    }

    m_ogre_size = _new_size;

    const auto ratio     = devicePixelRatioF();
    const int new_width  = static_cast<int>(ratio * _new_size.width());
    const int new_height = static_cast<int>(ratio * _new_size.height());

    create_render_textures(new_width, new_height);

    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::resize;
    info.x                = new_width;
    info.y                = new_height;
    info.dx               = 0;
    info.dy               = 0;

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::create_render_textures(int _w, int _h)
{
    FW_PROFILE("createRenderTextures");
    auto& mgr = Ogre::TextureManager::getSingleton();

    unsigned i = 0;
    for(auto& render_target : m_render_targets)
    {
        const auto layer = render_target.layer.lock();
        if(render_target.texture)
        {
            mgr.remove(render_target.texture);
            render_target.texture.reset();
        }

        render_target.texture = mgr.createManual(
            "RttTex" + std::to_string(m_id) + "_" + std::to_string(i++),
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,
            static_cast<uint>(_w),
            static_cast<uint>(_h),
            0,
            Ogre::PF_BYTE_RGBA,
            Ogre::TU_RENDERTARGET
        );
        SIGHT_ASSERT("texture could not be created", render_target.texture);

        auto* rt = render_target.texture->getBuffer()->getRenderTarget();
        layer->set_render_target(rt);

        if(!layer->initialized())
        {
            layer->create_scene();
        }
        else
        {
            // This will be skipped on initialisation
            Ogre::Camera* camera = layer->get_default_camera();
            // Save last viewport and current aspect ratio
            Ogre::Viewport* old_viewport = camera->getViewport();
            Ogre::Real aspect_ratio      = camera->getAspectRatio();

            // If the layer viewport is smaller than the widget size, then only the viewport is cleared.
            // This lefts all other areas uninitialized, which work on some platforms (NVidia) but not on others (Intel)
            // To overcome this, we add a fullscreen viewport to clear the whole frame
            if(old_viewport->getLeft() != 0.F || old_viewport->getTop() != 0.F
               || old_viewport->getWidth() != 1.F || old_viewport->getHeight() != 1.F)
            {
                Ogre::Viewport* clear_vp = rt->addViewport(camera, old_viewport->getZOrder() - 1, 0.F, 0.F, 1.F, 1.F);
                clear_vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
                clear_vp->setVisibilityMask(0); // Exclude all objects, only keep the clear
            }

            Ogre::Viewport* vp = rt->addViewport(
                camera,
                old_viewport->getZOrder(),
                old_viewport->getLeft(),
                old_viewport->getTop(),
                old_viewport->getWidth(),
                old_viewport->getHeight()
            );

            // Should restore aspect ratio, in case of auto aspect ratio
            // enabled, it'll changed when add new viewport.
            SIGHT_ASSERT("Width and height should be strictly positive", !std::isnan(aspect_ratio));
            camera->setAspectRatio(aspect_ratio);

            if(layer->get_order() != 0)
            {
                vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
            }

            Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
            if((vp != nullptr) && compositor_manager.hasCompositorChain(old_viewport))
            {
                Ogre::CompositorChain* old_chain = compositor_manager.getCompositorChain(old_viewport);

                std::vector<std::string> compositors;
                std::ranges::for_each(
                    old_chain->getCompositorInstances(),
                    [&compositors, &compositor_manager, vp](auto& _x)
                    {
                        const std::string name = _x->getCompositor()->getName();
                        compositors.push_back(name);
                        compositor_manager.addCompositor(vp, name);
                        compositor_manager.setCompositorEnabled(vp, name, _x->getEnabled());
                    });
                std::ranges::for_each(
                    compositors,
                    [&compositor_manager, old_viewport](auto& _name)
                    {
                        compositor_manager.setCompositorEnabled(old_viewport, _name, false);
                        compositor_manager.removeCompositor(old_viewport, _name);
                    });
            }
        }

        const Ogre::Material::Techniques& techniques = render_target.material->getTechniques();

        for(const auto* const tech : techniques)
        {
            Ogre::Pass* const pass           = tech->getPass(0);
            Ogre::TextureUnitState* tex_unit = pass->getTextureUnitState("RT");
            tex_unit->setTexture(render_target.texture);
        }

        const auto num_viewports = rt->getNumViewports();
        for(std::uint16_t v = 0 ; v < num_viewports ; v++)
        {
            auto* viewport = rt->getViewport(v);

            const auto vp_width  = static_cast<float>(viewport->getActualWidth());
            const auto vp_height = static_cast<float>(viewport->getActualHeight());
            SIGHT_ASSERT("Width and height should be strictly positive", vp_width > 0 && vp_height > 0);

            viewport->getCamera()->setAspectRatio(vp_width / vp_height);
        }
    }
}

//------------------------------------------------------------------------------

void window::initializeGL()
{
    m_ogre_root = sight::viz::scene3d::utils::get_ogre_root();

    SIGHT_ASSERT(
        "OpenGL RenderSystem not found",
        m_ogre_root->getRenderSystem()->getName().find("GL") != std::string::npos
    );

    init_resources();

    const auto ratio = devicePixelRatioF();
    const int width  = static_cast<int>(this->width() * ratio);
    const int height = static_cast<int>(this->height() * ratio);

    Ogre::MeshManager& mesh_manager = Ogre::MeshManager::getSingleton();
    Ogre::MovablePlane p(Ogre::Vector3::UNIT_Z, 0);
    m_fs_quad_plane =
        mesh_manager.createPlane("plane" + std::to_string(m_id), sight::viz::scene3d::RESOURCE_GROUP, p, 2, 2);

    unsigned int i = 0;
    for(auto& render_target : m_render_targets)
    {
        auto material = Ogre::MaterialManager::getSingleton().getByName("Blit", sight::viz::scene3d::RESOURCE_GROUP);
        render_target.material = material->clone("mat" + std::to_string(m_id) + "_" + std::to_string(i++));
    }

    create_render_textures(width, height);

    m_init = true;

    sight::viz::scene3d::window_interactor::interaction_info info {};
    info.interaction_type = sight::viz::scene3d::window_interactor::interaction_info::resize;
    info.x                = width;
    info.y                = height;
    info.dx               = 0;
    info.dy               = 0;

    Q_EMIT interacted(info);
}

// ----------------------------------------------------------------------------

void window::paintGL()
{
    if(!m_init)
    {
        return;
    }

    m_update_pending = false;
    ++m_frame_id;

    FW_PROFILE("paintGL");

    try
    {
        for(auto& render_target : m_render_targets)
        {
            auto layer = render_target.layer.lock();
            layer->reset_camera_clipping_range();

            auto* rt = render_target.texture->getBuffer()->getRenderTarget();
            rt->update(false);

            this->makeCurrent();

            const Ogre::Material::Techniques& techniques = render_target.material->getTechniques();
            SIGHT_ASSERT("Can't find any technique", !techniques.empty());

            const auto* const tech           = techniques[0];
            Ogre::Pass* const pass           = tech->getPass(0);
            Ogre::TextureUnitState* tex_unit = pass->getTextureUnitState("RT");
            tex_unit->setTexture(render_target.texture);

            Ogre::RenderOperation op;
            m_fs_quad_plane->getSubMesh(0)->_getRenderOperation(op);

            const auto ratio = devicePixelRatioF();
            this->context()->functions()->glViewport(
                0,
                0,
                int(ratio * this->width()),
                int(ratio * this->height())
            );

            auto* scene = layer->get_scene_manager();
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

void window::resizeGL(int _w, int _h)
{
    ogre_resize(QSize(_w, _h));
}

// ----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt
