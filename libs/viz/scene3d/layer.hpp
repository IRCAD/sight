/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <core/com/has_signals.hpp>
#include <core/com/has_slots.hpp>
#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>

#include <viz/scene3d/compositor/chain_manager.hpp>
#include <viz/scene3d/compositor/core.hpp>
#include <viz/scene3d/compositor/listener/auto_stereo.hpp>
#include <viz/scene3d/compositor/types.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/window_interactor.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreViewport.h>

#include <cstdint>
#include <map>
#include <vector>

namespace sight::data
{

class color;

} // namespace sight::data

namespace sight::viz::scene3d
{

class render;
class adaptor;
class light_adaptor;

} // namespace sight::viz::scene3d

namespace Ogre
{

class Camera;

} // namespace Ogre

namespace sight::viz::scene3d
{

/**
 * @brief Allows to render multiple scenes in the same render window with viewports.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API layer :
    public core::base_object,
    public core::com::has_signals,
    public core::com::has_slots
{
public:

    /// Generates default methods.
    SIGHT_DECLARE_CLASS(layer, viz::scene3d::layer);

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Defines the extrinsic x Intrinsic camera calibrations.
    using camera_calibrations_t = std::vector<Ogre::Matrix4>;

    /// Defines the viewport parameters relatively to the screen: left, top, width ,height.
    using viewport_config_t = std::tuple<float, float, float, float>;

    /// Contains the signal sent when the layer is initialized.
    SIGHT_VIZ_SCENE3D_API static const core::com::signals::key_t INIT_LAYER_SIG;
    using init_layer_signal_t = core::com::signal<void (viz::scene3d::layer::sptr)>;

    /// Contains the signal sent when the layer is resized.
    SIGHT_VIZ_SCENE3D_API static const core::com::signals::key_t RESIZE_LAYER_SIG;
    using resize_layer_signal_t = core::com::signal<void (int, int)>;

    /// Contains signals sent when the camera is modified.
    SIGHT_VIZ_SCENE3D_API static const core::com::signals::key_t CAMERA_RANGE_UPDATED_SIG;
    using camera_updated_signal_t = core::com::signal<void ()>;

    using interaction_slot_t = core::com::slot<void (viz::scene3d::window_interactor::interaction_info)>;
    using destroy_slot_t     = core::com::slot<void ()>;

    /// Contains the slot name that request the picker to do a ray cast according to the passed position.
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t INTERACTION_SLOT;

    /// Contains the slot name that request the reset of camera.
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t RESET_CAMERA_SLOT;

    /// Defines the default camera name.
    SIGHT_VIZ_SCENE3D_API static const std::string DEFAULT_CAMERA_NAME;

    /// Defines the default light name.
    SIGHT_VIZ_SCENE3D_API static const std::string DEFAULT_LIGHT_NAME;

    /// Defines the default camera node name.
    SIGHT_VIZ_SCENE3D_API static const std::string DEFAULT_CAMERA_NODE_NAME;

    /// Initializes signals and slots.
    SIGHT_VIZ_SCENE3D_API layer();

    /// Destoyres Ogre resources.
    SIGHT_VIZ_SCENE3D_API ~layer() override = default;

    /// Gets the render window containing this layer.
    SIGHT_VIZ_SCENE3D_API Ogre::RenderTarget* get_render_target() const;

    /// Sets the render window containing this layer.
    SIGHT_VIZ_SCENE3D_API void set_render_target(Ogre::RenderTarget* _render_target);

    /// Set the associated scene manager ID of this viewport
    SIGHT_VIZ_SCENE3D_API void set_id(const std::string& _id);

    /// @returns the name of this layer.
    SIGHT_VIZ_SCENE3D_API std::string get_name() const;

    /// @returns the ID of this layer.
    SIGHT_VIZ_SCENE3D_API const std::string& layer_id() const;

    /// @returns the scene manager associated to this viewport. Creates if it does not exists.
    SIGHT_VIZ_SCENE3D_API Ogre::SceneManager* get_scene_manager() const;

    /// Creates the scene.
    SIGHT_VIZ_SCENE3D_API void create_scene();

    /// Destroys the scene.
    SIGHT_VIZ_SCENE3D_API void destroy_scene();

    /// @returns true if the scene is created.
    SIGHT_VIZ_SCENE3D_API bool is_scene_created() const;

    /// Adds a disabled compositor name to the chain_manager.
    SIGHT_VIZ_SCENE3D_API void add_available_compositor(std::string _compositor_name);

    /// Enables/Disables a compositor according to the isEnabled flag.
    SIGHT_VIZ_SCENE3D_API void update_compositor_state(std::string _compositor_name, bool _is_enabled);

    /// Places and align camera's focal with the world boundingBox.
    SIGHT_VIZ_SCENE3D_API void reset_camera_coordinates();

    /// Computes camera's focal with the world boundingBox.
    SIGHT_VIZ_SCENE3D_API void compute_camera_parameters();

    /// Resets the camera clipping range (near and far).
    SIGHT_VIZ_SCENE3D_API void reset_camera_clipping_range() const;

    /// Appends a new interactor with the given priority. Interactors with higher priorities are executed first.
    SIGHT_VIZ_SCENE3D_API void add_interactor(
        const viz::scene3d::interactor::base::sptr& _interactor,
        int _priority = 0
    );

    /// Removes the given interactor.
    SIGHT_VIZ_SCENE3D_API void remove_interactor(const viz::scene3d::interactor::base::sptr& _interactor);

    /// @return the order of the layer.
    SIGHT_VIZ_SCENE3D_API int get_order() const;

    /// Sets the order of the layer.
    SIGHT_VIZ_SCENE3D_API void set_order(int _order);

    /// Sets the worker used by slots.
    SIGHT_VIZ_SCENE3D_API void set_worker(const core::thread::worker::sptr& _worker);

    /// @returns the render service.
    SIGHT_VIZ_SCENE3D_API SPTR(viz::scene3d::render) render_service() const;

    /// Sets the render service.
    SIGHT_VIZ_SCENE3D_API void set_render_service(const SPTR(viz::scene3d::render)& _service);

    /// Requests render.
    SIGHT_VIZ_SCENE3D_API void request_render();

    /// Sets stereoscopic rendering.
    SIGHT_VIZ_SCENE3D_API void set_stereo_mode(compositor::core::stereo_mode_t _mode);

    /// Sets background color : specific to background layer.
    SIGHT_VIZ_SCENE3D_API void set_background_color(std::string _top_color, std::string _bot_color);

    /// Sets background scale : specific to background layer.
    SIGHT_VIZ_SCENE3D_API void set_background_scale(float _top_scale, float _bot_scale);

    /// Sets background scale : specific to background layer.
    SIGHT_VIZ_SCENE3D_API void set_background_material(const std::string& _background);

    /// Sets if this layer need a layer's 3D scene.
    SIGHT_VIZ_SCENE3D_API void set_core_compositor_enabled(
        bool _enabled,
        std::string _transparency_technique          = "",
        std::string _num_peels                       = "",
        compositor::core::stereo_mode_t _stereo_mode =
        compositor::core::stereo_mode_t::none
    );

    /// Sets if this layer has a configured compositor chain.
    SIGHT_VIZ_SCENE3D_API void set_compositor_chain_enabled(const std::string& _compositor_chain);

    /// Sets the viewport parameters for this layer: left, top, width, height.
    SIGHT_VIZ_SCENE3D_API void set_viewport_config(const viewport_config_t& _vp_cfg);

    /// @returns true if this layer needs a layer's 3D scene.
    SIGHT_VIZ_SCENE3D_API bool is_core_compositor_enabled() const;

    /// @returns true if there is an XML configured compositor chain.
    SIGHT_VIZ_SCENE3D_API bool is_compositor_chain_enabled() const;

    /// @returns true if stereoscopic rendering is enabled.
    SIGHT_VIZ_SCENE3D_API bool is_3d() const;

    /// @returns true if the layer is initialized.
    SIGHT_VIZ_SCENE3D_API bool initialized() const;

    /// @returns the stereoscopic mode.
    SIGHT_VIZ_SCENE3D_API compositor::core::stereo_mode_t get_stereo_mode() const;

    /// @returns the compositor chain.
    SIGHT_VIZ_SCENE3D_API viz::scene3d::compositor::chain_manager::compositor_chain_t get_compositor_chain() const;

    /// @returns the list of adaptors in the chain manager.
    SIGHT_VIZ_SCENE3D_API service::has_services::service_vector_t get_registered_adaptors() const;

    /// @returns the viewport.
    SIGHT_VIZ_SCENE3D_API Ogre::Viewport* get_viewport() const;

    /// @returns the default camera.
    SIGHT_VIZ_SCENE3D_API Ogre::Camera* get_default_camera() const;

    /// Gets the projection matrix used to define nth viewpoint. The index must be lower than the number of viewpoints.
    SIGHT_VIZ_SCENE3D_API Ogre::Matrix4 get_camera_proj_mat(std::uint8_t _camera_idx) const;

    /// @returns the number of cameras (viewpoints) used by this layer. Defined by the stereo mode.
    SIGHT_VIZ_SCENE3D_API std::uint8_t num_cameras() const;

    /// Sets default light flag.
    SIGHT_VIZ_SCENE3D_API void set_has_default_light(bool _has_default_light);

    /// @returns the number of lights adaptors used in this layer.
    SIGHT_VIZ_SCENE3D_API int num_lights() const;

    /// @returns the light adaptors used in this layer.
    SIGHT_VIZ_SCENE3D_API std::vector<SPTR(viz::scene3d::light_adaptor)> get_light_adaptors() const;

    /**
     * Computes the bounding box of the scene.
     * @param _exclude_static Exclude static objects from the bounding box computation.
     * @return the computed bounding box of the scene.
     */
    SIGHT_VIZ_SCENE3D_API Ogre::AxisAlignedBox compute_world_bounding_box(bool _exclude_static = false) const;

    /// @returns the OIT selected.
    SIGHT_VIZ_SCENE3D_API compositor::transparency_technique get_transparency_technique();

    /// @returns the number of peels computed by Depth Peeling or x2 Dual Depth Peeling.
    SIGHT_VIZ_SCENE3D_API int get_transparency_depth();

    /// Sets the OIT desired. Deactivate OIT compositor.
    SIGHT_VIZ_SCENE3D_API bool set_transparency_technique(compositor::transparency_technique _technique);

    /// Sets the number of peels computed by Depth Peeling or x2 Dual Depth Peeling. Deactivate OIT compositor.
    SIGHT_VIZ_SCENE3D_API void set_transparency_depth(int _depth);

    /// Sets the camera calibrations for stereo rendering.
    SIGHT_VIZ_SCENE3D_API void set_camera_calibrations(const camera_calibrations_t& _calibrations);

    /// @returns true if a specified light is the default light in the layer.
    SIGHT_VIZ_SCENE3D_API bool is_default_light(const CSPTR(viz::scene3d::light_adaptor)& /*_light*/) const;

    /// Removes the default light in the layer.
    SIGHT_VIZ_SCENE3D_API void remove_default_light();

    /// Cancels interaction for all interactors with a lower priority than the one calling this.
    SIGHT_VIZ_SCENE3D_API void cancel_further_interaction();

    /// Need to be enable if using camera with orthographic mode.
    SIGHT_VIZ_SCENE3D_API void set_orthographic_camera(bool _ortho);
    /// Returns value of setOrthographicCamera.
    SIGHT_VIZ_SCENE3D_API bool is_orthographic_camera_force() const;

    /**
     * @brief Get the origin scene node used for image origin and image orientation
     *
     * @return Ogre::SceneNode*
     */
    inline Ogre::SceneNode* camera_origin_node();

private:

    /// Slot: interacts with the scene.
    void interaction(viz::scene3d::window_interactor::interaction_info /*info*/);

    /// Compute the scene size, notably in order to determine the zoom scale
    float compute_scene_length(const Ogre::AxisAlignedBox& _world_bounding_box);

    /// Setups default compositor for a layer's 3D scene.
    void setup_core();

    /// Stops and starts all adaptors belonging to this layer. Subadaptors are expected to be managed by their parent.
    void restart_adaptors();

    /// Calls a function on all interactors and deletes the ones that expired.
    void for_all_interactors(const std::function<void(const interactor::base::sptr&)>&& _f);

    /// Contains the Ogre scene manager of this viewport.
    Ogre::SceneManager* m_scene_manager {nullptr};

    /// Contains the Ogre render window containing this viewport.
    Ogre::RenderTarget* m_render_target {nullptr};

    /// Defines stereoscopic rendering mode.
    compositor::core::stereo_mode_t m_stereo_mode {compositor::core::stereo_mode_t::none};

    /// Stores configured compositor chain raw string if there is a one.
    std::string m_raw_compositor_chain;

    /// Contains the Ogre default compositor for this layer.
    SPTR(viz::scene3d::compositor::core) m_core_compositor {nullptr};

    /// Contains the Ogre default compositor default transparency technique.
    viz::scene3d::compositor::transparency_technique m_transparency_technique {viz::scene3d::compositor::DEFAULT};

    /// Defines the number of peels used by OIT.
    int m_num_peels {8};

    /// Manages the list of available compositors.
    /// The names are associated to a boolean value which indicates whether the compositor is enabled or not.
    viz::scene3d::compositor::chain_manager::uptr m_compositor_chain_manager;

    /// Defines the Z order of this viewport.
    int m_order {1};

    /// Defines the top background color : specific to background layer.
    std::string m_top_color {"#333333"};

    /// Defines the bottom background color : specific to background layer.
    std::string m_bottom_color {"#333333"};

    /// Defines the top background scale : specific to background layer.
    float m_top_scale {0.F};

    /// Defines the bottom background scale : specific to background layer.
    float m_bottom_scale {1.F};

    /// Defines the bottom background scale : specific to background layer.
    std::string m_background_material {};

    /// Contains the Ogre camera.
    Ogre::Camera* m_camera {nullptr};

    /// Stores the list of interactors, sorted by priority.
    std::multimap<int, viz::scene3d::interactor::base::wptr, std::greater<> > m_interactors;

    /// Handles flag cancelling all further interaction when enabled.
    bool m_cancel_further_interaction {false};

    /// Handles all connections.
    core::com::helper::sig_slot_connection m_connections;

    /// Contains the render service which this layer is attached.
    WPTR(viz::scene3d::render) m_render_service;

    /// Defines the layer identifier as referenced in render.
    std::string m_id;

    /// Enables default compositor's widgets (gui displays before scene creation).
    bool m_has_core_compositor {false};

    /// Indicates if a compositor chain is attached to the layer.
    bool m_has_compositor_chain {false};

    /// Indicates if the scene has been created.
    bool m_scene_created {false};

    /// Indicates if the scene has a default light.
    bool m_has_default_light {true};

    /// Contains the abstract light used to set the default light.
    SPTR(viz::scene3d::light_adaptor) m_light_adaptor {nullptr};

    /// Contains the diffuse color of the default light.
    SPTR(data::color) m_default_light_diffuse_color {nullptr};

    /// Contains the specular color of the specular light.
    SPTR(data::color) m_default_light_specular_color {nullptr};

    /// Defines the camera listener class used to pass the projection matrix for autostereo shaders.
    struct LayerCameraListener;
    LayerCameraListener* m_camera_listener {nullptr};

    /// Contains the autostereo listener.
    compositor::listener::auto_stereo_compositor_listener* m_autostereo_listener {nullptr};

    /// Holds pairs of intrinsic/extrinsic calibrations for stereo cameras.
    camera_calibrations_t m_stereo_camera_calibration;

    /// Defines viewport parameters: left, top, width, height.
    viewport_config_t m_viewport_cfg {0.F, 0.F, 1.F, 1.F};

    /// True when we are using dedicated camera adaptor with orthographic projection.
    bool m_camera_orthographic {false};

    /// Contains the scene node used for image origin and orientation.
    Ogre::SceneNode* m_camera_origin_node {nullptr};
};

//------------------------------------------------------------------------------

inline Ogre::SceneNode* layer::camera_origin_node()
{
    return m_camera_origin_node;
}

} // namespace sight::viz::scene3d.
