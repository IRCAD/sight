/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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
#include <viz/scene3d/vr/pre_integration_table.hpp>
#include <viz/scene3d/vr/ray_tracing_volume_renderer.hpp>

#include <OGRE/OgreTexture.h>

#include <array>
#include <vector>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a volume rendering.
 *
 * @section Slots Slots
 * - \b new_image(): called when a new image is loaded.
 * - \b update_image(): called when the image is updated.
 * - \b toggle_widgets(bool): toggles widget visibility.
 * - \b bufferImage(): called when the image buffer is modified, copies it into the texture buffer.
 * - \b update_visibility(bool): shows or hides the volume.
 * - \b toggle_visibility(): toggle whether the volume is shown or not.
 * - \b show(): shows the volume.
 * - \b hide(): hides the volume.
 * - \b update_clipping_box(): updates the cropping widget from the clipping matrix.
 * - \b set_bool_parameter(bool, string): calls a bool parameter slot according to the given key.
 *  - preIntegration: toggles the pre-integration.
 *  - ambientOcclusion: toggles the ambient occlusion.
 *  - colorBleeding: toggles the color bleeding.
 *  - shadows: toggles soft shadows.
 *  - widgets: toggles the clipping box widget's visibility.
 * - \b set_int_parameter(int, string): calls an int parameter slot according to the given key.
 *  - sampling: sets the number of volume samples used by the renderer. More samples yield more details but slow down
 *    rendering.
 *  - opacityCorrection: sets the volume opacity correction factor.
 *  - satSizeRatio: sets the SAT ratio and computes it again with the new corresponding size.
 *  - satShellsNumber: sets the number of SAT shells and compute the SAT.
 *  - satShellRadius: sets the SAT shell radius and computes the SAT.
 *  - satConeAngle: sets the SAT cone angle and computes the SAT.
 *  - satConeSamples: sets the SAT cone samples number and computes the SAT.
 * - \b set_double_parameter(double, string): calls a double parameter slot according to the given key.
 *  - aoFactor: sets the ambient occlusion factor and computes the SAT.
 *  - colorBleedingFactor: sets the color bleeding factor and computes the SAT.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::volume_render" >
        <in key="image" uid="..." auto_connect="true" />
        <in key="mask" uid="..." auto_connect="true" />
        <in key="tf" uid="..." />
        <inout key="clippingMatrix" uid="..." />
        <config samples="1024" preintegration="true" dynamic="false" ao="false" colorBleeding="false" shadows="false"
                satSizeRatio="0.25" satShells="3" satShellRadius="7" satConeAngle="0.1" satConeSamples="50"
                aoFactor="0.5" colorBleedingFactor="0.5" autoresetcamera="true" transform="..."/>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::image]: input volume data.
 * - \b mask [sight::data::image]: mask used to crop regions.
 * - \b tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection In-Out In-Out
 * - \b clippingMatrix [sight::data::matrix4]: matrix used to clip the volume.
 *
 * @subsection Configuration Configuration
 * - \b samples (optional, unsigned int, default=512): maximum number of samples per ray or number of slices.
 * - \b preintegration (optional, true/false, default=false): use pre-integration.
 * - \b dynamic (optional, bool, default=false): enables background buffering for dynamic images.
 * - \b widgets (optional, true/false, default=true): display VR widgets.
 * - \b priority (optional, int, default=2): interaction priority of the widget.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 * - \b ao (optional, bool, default=false): ambient occlusion usage.
 * - \b colorBleeding (optional, bool, default=false): color bleeding usage.
 * - \b shadows (optional, bool, default=false): soft shadows usage.
 * - \b satSizeRatio (optional, float, default=0.25): ratio used to determine the size of the SAT regarding of the
 *      associated image size.
 * - \b satShells (optional, int, default=3): number of shells used to compute the volume illumination from the SAT.
 * - \b satShellRadius (optional, int, default=7): radius of the shells used to compute the volume illumination
 *      from the SAT.
 * - \b satConeAngle (optional, float, default=0.1): angle used to define the soft shadows cones.
 * - \b satConeSamples (optional, float, default=50): number of samples along the soft shadows cones.
 * - \b aoFactor (optional, double, default=1.0): factor used to weight the ambient occlusion.
 * - \b colorBleedingFactor (optional, double, default=1.0): factor used to weight the color bleeding.
 * - \b autoresetcamera (optional, true/false, default=true): reset the camera at image update to view the whole volume.
 * - \b transform (optional, string, default=""): transform applied to the adaptor's scene node.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class volume_render final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(volume_render, sight::viz::scene3d::adaptor);

    /// Creates slots.
    volume_render() noexcept;

    /// Destroys the adaptor.
    ~volume_render() noexcept override;

protected:

    // Slot keys
    static inline const sight::core::com::slots::key_t NEW_IMAGE_SLOT            = "new_image";
    static inline const sight::core::com::slots::key_t BUFFER_IMAGE_SLOT         = "bufferImage";
    static inline const sight::core::com::slots::key_t UPDATE_IMAGE_SLOT         = "update_image";
    static inline const sight::core::com::slots::key_t TOGGLE_WIDGETS_SLOT       = "toggle_widgets";
    static inline const sight::core::com::slots::key_t SET_BOOL_PARAMETER_SLOT   = "set_bool_parameter";
    static inline const sight::core::com::slots::key_t SET_INT_PARAMETER_SLOT    = "set_int_parameter";
    static inline const sight::core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT = "set_double_parameter";
    static inline const sight::core::com::slots::key_t UPDATE_CLIPPING_BOX_SLOT  = "update_clipping_box";
    static inline const sight::core::com::slots::key_t UPDATE_TF_SLOT            = "update_tf";
    static inline const sight::core::com::slots::key_t UPDATE_MASK_SLOT          = "update_mask";

    ///@brief Internal wrapper holding config defines.
    struct config
    {
        static inline const std::string AUTORESET_CAMERA      = CONFIG + "autoresetcamera";
        static inline const std::string PREINTEGRATION        = CONFIG + "preintegration";
        static inline const std::string DYNAMIC               = CONFIG + "dynamic";
        static inline const std::string WIDGETS               = CONFIG + "widgets";
        static inline const std::string PRIORITY              = CONFIG + "priority";
        static inline const std::string LAYER_ORDER_DEPENDANT = CONFIG + "layerOrderDependant";
        static inline const std::string SAMPLES               = CONFIG + "samples";
        static inline const std::string SAT_SIZE_RATIO        = CONFIG + "satSizeRatio";
        static inline const std::string SAT_SHELLS            = CONFIG + "satShells";
        static inline const std::string SAT_SHELL_RADIUS      = CONFIG + "satShellRadius";
        static inline const std::string SAT_CONE_ANGLE        = CONFIG + "satConeAngle";
        static inline const std::string SAT_CONE_SAMPLES      = CONFIG + "satConeSamples";
        static inline const std::string AO_FACTOR             = CONFIG + "aoFactor";
        static inline const std::string COLOR_BLEEDING_FACTOR = CONFIG + "colorBleedingFactor";
        static inline const std::string AO                    = CONFIG + "ao";
        static inline const std::string COLOR_BLEEDING        = CONFIG + "colorBleeding";
        static inline const std::string SHADOWS               = CONFIG + "shadows";
    };

    /// Internal wrapper holding the inital config
    struct config_data_t
    {
        using sat_parameters_t     = sight::viz::scene3d::vr::illum_ambient_occlusion_sat::sat_parameters_t;
        using shadows_parameters_t = sight::viz::scene3d::vr::ray_tracing_volume_renderer::shadows_parameters_t;

        /// Enables whether the camera must be auto reset when a mesh is updated or not.
        bool camera_autoreset {false};

        /// Enables preintegration.
        bool preintegration {false};

        /// Enables dynamic buffering.
        bool dynamic {false};

        /// Sets the node visible or not.
        bool visible {false};

        /// Interactor priority.
        int priority {2};

        /// Indicates the the clipping box interactor layer is order-dependant.
        bool order_dependent {false};

        /// Sampling rate.
        std::uint16_t samples {512};

        /// SAT parameters.
        sat_parameters_t sat {};

        ///Shadows parameters
        shadows_parameters_t shadows {};
    };

    /// Internal wrapper holding object keys
    struct objects
    {
        static constexpr std::string_view IMAGE_IN              = "image";
        static constexpr std::string_view MASK_IN               = "mask";
        static constexpr std::string_view VOLUME_TF_IN          = "tf";
        static constexpr std::string_view CLIPPING_MATRIX_INOUT = "clippingMatrix";
    };

    /// Configures the service.
    void configuring(const config_t& _config) override;

    /// Starts the service and initializes scene objects.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_IMAGE_INOUT to NEW_IMAGE_SLOT
     * Connect data::image::BUFFER_MODIFIED_SIG of s_IMAGE_INOUT to BUFFER_IMAGE_SLOT
     * Connect data::image::MODIFIED_SIG of s_CLIPPING_MATRIX_INOUT to UPDATE_CLIPPING_BOX_SLOT
     */
    service::connections_t auto_connections() const override;

    /// Does nothing.
    void updating() override;

    /// Cleans up scene objects.
    void stopping() override;

    /**
     * @brief Sets the volume visibility.
     * @param _visible the visibility status of the volume.
     */
    void set_visible(bool _visible) override;

    ///@brief Configuration loaded.
    config_data_t m_config {};

private:

    /// Defines volume rendering effects.
    enum class vr_effect_type
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
    void update_sat_size_ratio(unsigned _size_ratio);

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

    /**
     * @brief Updates a bool parameter.
     * @param _val new value of the parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - preIntegration: toggles the pre-integration.
     *  - ambientOcclusion: toggles the ambient occlusion.
     *  - colorBleeding: toggles the color bleeding.
     *  - shadows: toggles soft shadows.
     *  - widgets: toggles the clipping box widget's visibility.
     *
     * @see togglePreintegration(bool)
     * @see toggleAmbientOcclusion(bool)
     * @see toggleColorBleeding(bool)
     * @see toggleShadows(bool)
     * @see toggleWidgets(bool)
     */
    void set_bool_parameter(bool _val, std::string _key);

    /**
     * @brief Updates a int parameter.
     * @param _val New value of the parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - sampling: Sets the number of volume samples used by the renderer. More samples yield more details but slow
     *    down rendering.
     *  - opacityCorrection: sets the volume opacity correction factor.
     *  - satSizeRatio: sets the SAT ratio and computes it again with the new corresponding size.
     *  - satShellsNumber: sets the number of SAT shells and compute the SA.
     *  - satShellRadius: sets the SAT shell radius and computes the SAT.
     *  - satConeAngle: sets the SAT cone angle and computes the SAT.
     *  - satConeSamples: sets the SAT cone samples number and computes the SAT.
     *
     * @see updateSampling(int)
     * @see updateOpacityCorrection(int)
     * @see updateSatSizeRatio(int)
     * @see updateSatShellsNumber(int)
     * @see updateSatShellRadius(int)
     * @see updateSatConeAngle(int)
     * @see updateSatConeSamples(int)
     */
    void set_int_parameter(int _val, std::string _key);

    /**
     * @brief Updates a double parameter.
     * @param _val the new value of the parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - aoFactor: sets the ambient occlusion factor and computes the SAT.
     *  - colorBleedingFactor: sets the color bleeding factor and computes the SAT.
     *
     * @see updateAOFactor(double)
     * @see updateColorBleedingFactor(double)
     */
    void set_double_parameter(double _val, std::string _key);

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

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, objects::IMAGE_IN, true};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_mask {this, objects::MASK_IN, true};
    sight::data::ptr<sight::data::transfer_function, sight::data::access::in> m_tf {this, objects::VOLUME_TF_IN, true};
    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_clipping_matrix
    {
        this,
        objects::CLIPPING_MATRIX_INOUT,
        true,
        true
    };
};

} // namespace sight::module::viz::scene3d::adaptor.
