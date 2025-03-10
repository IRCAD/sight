/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/factory/new.hpp"
#include "viz/scene3d/graphics_worker.hpp"
#include "viz/scene3d/interactor/base.hpp"
#include "viz/scene3d/registry/detail.hpp"

#include <service/base.hpp>

#include <ui/__/container/widget.hpp>

namespace sight::viz::scene3d
{

/**
 * @brief   Defines a class to manage ogreRenderwindow_interactor in a window.
 *
 */
class SIGHT_VIZ_SCENE3D_CLASS_API window_interactor : public core::base_object
{
public:

    /// Inner class used to send informations about mouse events.
    class interaction_info
    {
    public:

        enum interaction_enum
        {
            mousemove,
            wheelmove,
            resize,
            keypress,
            keyrelease,
            buttonrelease,
            buttonpress,
            buttondoublepress,
            pinch_gesture,
            pan_gesture_move,
            pan_gesture_release,
            long_tap_gesture,
            enter,
            leave
        };

        /**
         * @brief
         */

        int x;
        int y;
        int dx;
        int dy;
        double delta;
        int key;
        interactor::base::mouse_button button;
        interactor::base::modifier modifiers;
        interaction_enum interaction_type;
    };

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class registry
    {
    public:

        registry(std::string _functor_key)
        {
            viz::scene3d::registry::get()->add_factory(_functor_key, &viz::scene3d::factory::make<T>);
        }
    };

    /**
     * @brief Class used to register an offscreen window interactor factory in the factory registry.
     *
     * @tparam T factory product type
     */
    template<typename T>
    class offscreen_mgr_registry
    {
    public:

        offscreen_mgr_registry(std::string _functor_key)
        {
            auto fact = [](std::pair<unsigned int, unsigned int> _dims) -> std::shared_ptr<T>
                        {
                            // Capture the factory inside a lambda to distinguish it from overloaded methods.
                            return viz::scene3d::offscreen_interactor_mgr_factory::make<T>(_dims);
                        };
            viz::scene3d::registry::get_offscreen_mgr()->add_factory(_functor_key, fact);
        }
    };

    SIGHT_DECLARE_CLASS(window_interactor, core::base_object);

    using factory_registry_key_t = std::string;

    SIGHT_VIZ_SCENE3D_API static const factory_registry_key_t REGISTRY_KEY;

    SIGHT_VIZ_SCENE3D_API static const factory_registry_key_t OFFSCREEN_REGISTRY_KEY;

    SIGHT_VIZ_SCENE3D_API static window_interactor::sptr create_manager();

    /**
     * @brief Creates an offscreen window using the factory.
     *
     * @param _width  width of underlying render texture.
     * @param _height height of underlying render texture.
     */
    SIGHT_VIZ_SCENE3D_API static window_interactor::sptr create_offscreen_manager(
        unsigned int _width,
        unsigned int _height
    );

    /// Constructor. Do nothing.
    SIGHT_VIZ_SCENE3D_API window_interactor() = default;

    /// Destructor. Do nothing.
    SIGHT_VIZ_SCENE3D_API ~window_interactor() override = default;

    /// Call Ogre Widget render immediately
    SIGHT_VIZ_SCENE3D_API virtual void render_now() = 0;

    /// Call Ogre Widget render as soon as possible
    SIGHT_VIZ_SCENE3D_API virtual void request_render() = 0;

    /// Creates an interactor and installs it in window.
    SIGHT_VIZ_SCENE3D_API virtual void create_container(
        ui::container::widget::sptr _parent,
        bool _fullscreen,
        const std::string& _id
    ) = 0;

    /// Connects widget and render signals and slots.
    SIGHT_VIZ_SCENE3D_API virtual void connect_to_container() = 0;

    /// Deletes interactor and manage correctly the window (removing layout).
    SIGHT_VIZ_SCENE3D_API virtual void disconnect_interactor() = 0;

    /// Returns Ogre widget
    SIGHT_VIZ_SCENE3D_API virtual int get_widget_id() const = 0;

    /// Returns frame ID
    SIGHT_VIZ_SCENE3D_API virtual int get_frame_id() const = 0;

    /// Sets the rendering context as being enabled against this window and on this thread.
    SIGHT_VIZ_SCENE3D_API virtual void make_current() = 0;

    /// Returns the texture in which this window manager is rendering. Only implemented for offscreen windows.
    SIGHT_VIZ_SCENE3D_API virtual Ogre::TexturePtr get_render_texture() = 0;

    /// Spawns a worker able to handle graphics resources in parallel.
    SIGHT_VIZ_SCENE3D_API virtual graphics_worker* create_graphics_worker() = 0;

    /// Gets the vertical logical DPI of the monitor on which the window is displayed.
    /// The logical DPI takes accessibility features and desktop zoom into account and is used for font rendering.
    SIGHT_VIZ_SCENE3D_API virtual float get_logical_dots_per_inch() const = 0;

    /**
     * @brief Sets the fullscreen or windowed rendering mode.
     *
     * @param _fullscreen whether to render in fullscreen mode. Use windowed mode otherwise.
     * @param _screen_number index of the screen on which to render in fullscreen mode.
     */
    SIGHT_VIZ_SCENE3D_API virtual void set_fullscreen(bool _fullscreen, int _screen_number);

    /// Set the render service using the IOgreRenderwindow_interactor
    virtual void set_render_service(service::base::sptr _srv)
    {
        m_render_service = _srv;
    }

protected:

    service::base::wptr m_render_service;
};

} // namespace sight::viz::scene3d
