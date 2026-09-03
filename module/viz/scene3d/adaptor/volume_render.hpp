/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/graphics_worker.hpp>
#include <viz/scene3d/interactor/clipping_box_interactor.hpp>
#include <viz/scene3d/transformable.hpp>
#include <viz/scene3d/vr/illum_ambient_occlusion_sat.hpp>
#include <viz/scene3d/vr/ray_tracing_volume_renderer.hpp>

#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/matrix4.hpp>
#include <data/real.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a volume rendering.
 *
 * @section Slots Slots
 * - \b new_image(): called when a new image is loaded.
 * - \b update_image(): called when the image is updated.
 * - \b toggle_widgets(bool): toggles widget visibility.
 * - \b buffer_image(): called when the image buffer is modified, copies it into the texture buffer.
 * - \b update_visibility(bool): shows or hides the volume.
 * - \b toggle_visibility(): toggle whether the volume is shown or not.
 * - \b show(): shows the volume.
 * - \b hide(): hides the volume.
 * - \b update_clipping_box(): updates the cropping widget from the clipping matrix.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::volume_render" >
        <data image="${...}" />
        <data mask="${...}" />
        <data tf="${...}" />
        <data clipping_matrix="${...}" />
        <volume_rendering>
            <config pre_integration="${...}" ambient_occlusion="${...}" color_bleeding="${...}" shadows="${...}"
                    widgets="${...}" sampling="${...}" opacity_correction="${...}" sat_shells_number="${...}"
                    sat_shell_radius="${...}" sat_cone_samples="${...}"
                    color_bleeding_factor="${...}" ao_factor="${...}"
                    sat_cone_angle="${...}" sat_size_ratio="${...}" />
        </volume_rendering>
        <config dynamic="false" auto_reset_camera="true" transform="..."/>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data.image [sight::data::image]: input volume data.
 * - \b data.mask [sight::data::image]: mask used to crop regions.
 * - \b data.tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use
 * the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection In-Out In-Out
 * - \b data.clipping_matrix [sight::data::matrix4]: matrix used to clip the volume.
 *
 * - \b config.visible [sight::data::boolean] (optional, default=true): the visibility of the adaptor.
 *
 * @subsection Configuration Configuration
 * - \b samples (optional, unsigned int, default=512): maximum number of samples per ray or number of slices.
 * - \b dynamic (optional, bool, default=false): enables background buffering for dynamic images.
 * - \b priority (optional, int, default=2): interaction priority of the widget.
 * - \b auto_reset_camera (optional, true/false, default=true): reset the camera at image update to view the whole
 * volume.
 * - \b transform (optional, string, default=""): transform applied to the adaptor's scene node.
 */
class volume_render final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(volume_render, sight::viz::scene3d::adaptor);

    struct slots
    {
        static inline const slot_key_t NEW_IMAGE                    = "new_image";
        static inline const slot_key_t BUFFER_IMAGE                 = "buffer_image";
        static inline const slot_key_t UPDATE_IMAGE                 = "update_image";
        static inline const slot_key_t TOGGLE_PREINTEGRATION        = "toggle_preintegration";
        static inline const slot_key_t TOGGLE_AMBIENT_OCCLUSION     = "toggle_ambient_occlusion";
        static inline const slot_key_t TOGGLE_COLOR_BLEEDING        = "toggle_color_bleeding";
        static inline const slot_key_t TOGGLE_SHADOWS               = "toggle_shadows";
        static inline const slot_key_t TOGGLE_WIDGETS               = "toggle_widgets";
        static inline const slot_key_t UPDATE_SAMPLING              = "update_sampling";
        static inline const slot_key_t UPDATE_OPACITY_CORRECTION    = "update_opacity_correction";
        static inline const slot_key_t UPDATE_SAT_SHELLS_NUMBER     = "update_sat_shells_number";
        static inline const slot_key_t UPDATE_SAT_SHELL_RADIUS      = "update_sat_shell_radius";
        static inline const slot_key_t UPDATE_SAT_CONE_SAMPLES      = "update_sat_cone_samples";
        static inline const slot_key_t UPDATE_COLOR_BLEEDING_FACTOR = "update_color_bleeding_factor";
        static inline const slot_key_t UPDATE_AO_FACTOR             = "update_ao_factor";
        static inline const slot_key_t UPDATE_SAT_CONE_ANGLE        = "update_sat_cone_angle";
        static inline const slot_key_t UPDATE_SAT_SIZE_RATIO        = "update_sat_size_ratio";
        static inline const slot_key_t UPDATE_CLIPPING_BOX          = "update_clipping_box";
        static inline const slot_key_t UPDATE_TF                    = "update_tf";
        static inline const slot_key_t UPDATE_MASK                  = "update_mask";
    };

    /// Creates slots.
    volume_render() noexcept;

    /// Destroys the adaptor.
    ~volume_render() noexcept override = default;

    /**
     * @brief Sets the volume visibility.
     * @param _visible the visibility status of the volume.
     */
    void set_visible(bool _visible) override;

protected:

    ///@brief Internal wrapper holding config defines.
    struct config
    {
        static inline const std::string AUTORESET_CAMERA = CONFIG + "auto_reset_camera";
        static inline const std::string DYNAMIC          = CONFIG + "dynamic";
        static inline const std::string PRIORITY         = CONFIG + "priority";
    };

    /// Internal wrapper holding the inital config
    struct config_data_t
    {
        using sat_parameters_t     = sight::viz::scene3d::vr::illum_ambient_occlusion_sat::sat_parameters_t;
        using shadows_parameters_t = sight::viz::scene3d::vr::ray_tracing_volume_renderer::shadows_parameters_t;

        /// Enables whether the camera must be auto reset when a mesh is updated or not.
        bool camera_autoreset {false};

        /// Enables dynamic buffering.
        bool dynamic {false};

        /// Interactor priority.
        int priority {2};
    };

    /// Internal wrapper holding object keys
    struct objects
    {
        static constexpr std::string_view IMAGE_IN              = "data.image";
        static constexpr std::string_view MASK_IN               = "data.mask";
        static constexpr std::string_view VOLUME_TF_IN          = "data.tf";
        static constexpr std::string_view CLIPPING_MATRIX_INOUT = "data.clipping_matrix";
    };

    /// Configures the service.
    void configuring(const config_t& _config) override;

    /// Starts the service and initializes scene objects.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     */
    service::connections_t auto_connections() const override;

    /// Does nothing.
    void updating() override;

    /// Cleans up scene objects.
    void stopping() override;

    ///@brief Configuration loaded.
    config_data_t m_config {}; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

private:

    /// Defines volume rendering effects.
    enum class vr_effect_type : std::uint8_t
    {
        vr_ambient_occlusion,
        vr_color_bleeding,
        vr_shadows
    };

    /// Updates the transfer function applied to the volume.
    void update_volume_tf();

    /// Sets a new image.
    void new_image();

    /// Updates renderer and the GPU volume texture with the new input image data.
    void update_image();

    /// Updates renderer and the GPU volume texture with the new mask data.
    void update_mask();

    /// Starts a parallel task to copy the updated image buffer into the texture buffer.
    void buffer_image();

    /**
     * @brief Updates the sampling.
     * @param _nb_samples number of sample.
     *
     * @pre _nbSamples must fit in a 16 bit unsigned int.
     */
    void update_sampling(unsigned _nb_samples);

    /**
     * @brief Sets the opacity correction.
     * @param _opacity_correction value of the opacity correction.
     */
    void update_opacity_correction(unsigned _opacity_correction);

    /**
     * @brief Sets the ambient occlusion factor.
     * @param _ao_factor value of the ambient occlusion factor.
     */
    void update_ao_factor(float _ao_factor);

    /**
     * @brief Sets the color bleeding factor.
     * @param _color_bleeding_factor value of the color bleeding factor.
     */
    void update_color_bleeding_factor(float _color_bleeding_factor);

    /**
     * @brief Sets the SAT size ratio.
     * @param _size_ratio value of the SAT size ratio.
     */
    void update_sat_size_ratio(unsigned int _size_ratio);

    /**
     * @brief Sets the SAT shells number.
     * @param _shells_number the number of shells used by the SAT.
     */
    void update_sat_shells_number(unsigned _shells_number);

    /**
     * @brief Sets the SAT shells radius.
     * @param _shell_radius the shells radius used by the SAT.
     */
    void update_sat_shell_radius(unsigned _shell_radius);

    /**
     * @brief Sets the SAT cone angle.
     * @param _cone_angle the cone angle size of the SAT. Cones ares used to compute soft shadows.
     */
    void update_sat_cone_angle(float _cone_angle);

    /**
     * @brief Sets the SAT cone samples.
     * @param _nb_cone_samples the cone sample number of the SAT. Cones ares used to compute soft shadows.
     */
    void update_sat_cone_samples(unsigned _nb_cone_samples);

    /**
     * @brief Enables/disables the pre integration table.
     * @param _preintegration state of the pre integration.
     */
    void toggle_preintegration(bool _preintegration);

    /**
     * @brief Enables/disables the ambient occlusion.
     * @param _ambient_occlusion state of the ambient occlusion.
     */
    void toggle_ambient_occlusion(bool _ambient_occlusion);

    /**
     * @brief Enables/disables the color bleeding.
     * @param _color_bleeding state of the color bleeding.
     */
    void toggle_color_bleeding(bool _color_bleeding);

    /**
     * @brief Enables/disables the volume self-shadowing.
     * @param _shadows state of soft shadows computation.
     */
    void toggle_shadows(bool _shadows);

    /**
     * @brief Displays/Hides the widget.
     * @param _visible state of the widget visibility.
     */
    void toggle_widgets(bool _visible);

    /**
     * @brief Sets the focal distance.
     * @param _focal_distance value of the focal distance.
     */
    void set_focal_distance(int _focal_distance);

    /// Creates widgets and connects its slots to interactor signals.
    void create_widget();

    /// Removes the widgets from the interactor and deletes it.
    void destroy_widget();

    /**
     * @brief Updates or creates the illumination volume according to the given VR effect.
     * @param _vr_effect volume rendering effects.
     * @param _enable enable that effect or not.
     */
    void toggle_vr_effect(vr_effect_type _vr_effect, bool _enable);

    /// Updates the clipping box position from the inout clipping matrix.
    void update_clipping_box();

    /// Updates the inout clipping matrix from the clipping box positions.
    void update_clipping_matrix();

    enum class update_flags : std::uint8_t
    {
        image,
        image_buffer,
        mask_buffer,
        clipping_box,
        tf
    };

    ///Prevents concurrent access on certain operations (texture update, etc.)
    std::mutex m_mutex;

    /// Implements a simple GPU ray-tracing renderer.
    std::unique_ptr<sight::viz::scene3d::vr::ray_tracing_volume_renderer> m_volume_renderer {nullptr};

    /// Fills the incoming image texture in a parallel thread.
    std::unique_ptr<sight::viz::scene3d::graphics_worker> m_buffering_worker;

    /// Stores the scene manager.
    Ogre::SceneManager* m_scene_manager {nullptr};

    /// Stores the scene node of the volume.
    Ogre::SceneNode* m_volume_scene_node {nullptr};

    /// Stores the widgets used for clipping.
    std::shared_ptr<sight::viz::scene3d::interactor::clipping_box_interactor> m_widget;

private:

    ptr_in<sight::data::image> m_image {this, objects::IMAGE_IN};
    ptr_in<sight::data::image> m_mask {this, objects::MASK_IN};
    ptr_in<sight::data::transfer_function> m_tf {this, objects::VOLUME_TF_IN};
    ptr_inout<sight::data::matrix4> m_clipping_matrix {this, objects::CLIPPING_MATRIX_INOUT, true};

    ptr_in<data::boolean> m_preintegration {this, "volume_rendering.config.pre_integration", false};
    ptr_in<data::boolean> m_ambient_occlusion {this, "volume_rendering.config.ambient_occlusion", false};
    ptr_in<data::boolean> m_color_bleeding {this, "volume_rendering.config.color_bleeding", false};
    ptr_in<data::boolean> m_shadows {this, "volume_rendering.config.shadows", false};
    ptr_in<data::boolean> m_widgets {this, "volume_rendering.config.widgets", true};
    ptr_in<data::integer> m_sampling {this, "volume_rendering.config.sampling", 512};
    ptr_in<data::integer> m_opacity_correction {this, "volume_rendering.config.opacity_correction", 0};
    ptr_in<data::integer> m_sat_shells_number {this, "volume_rendering.config.sat_shells_number", 4};
    ptr_in<data::integer> m_sat_shell_radius {this, "volume_rendering.config.sat_shell_radius", 4};
    ptr_in<data::integer> m_sat_cone_samples {this, "volume_rendering.config.sat_cone_samples", 50};
    ptr_in<data::real> m_color_bleeding_factor {this, "volume_rendering.config.color_bleeding_factor", 1.};
    ptr_in<data::real> m_ao_factor {this, "volume_rendering.config.ao_factor", 1.};
    ptr_in<data::real> m_sat_cone_angle {this, "volume_rendering.config.sat_cone_angle", 0.1};
    ptr_in<data::integer> m_sat_size_ratio {this, "volume_rendering.config.sat_size_ratio", 2};
};

} // namespace sight::module::viz::scene3d::adaptor.
