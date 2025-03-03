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
// cspell:ignore NOLINT hicpp
#pragma once

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/r2vb_renderable.hpp"
#include "viz/scene3d/vr/grid_proxy_geometry.hpp"
#include "viz/scene3d/vr/illum_ambient_occlusion_sat.hpp"
#include "viz/scene3d/vr/ray_entry_compositor.hpp"
#include "viz/scene3d/vr/volume_renderer.hpp"

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

#include <optional>
#include <vector>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Implements a simple GPU ray-tracing renderer.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API ray_tracing_volume_renderer : public volume_renderer
{
public:

    /// Internal wrapper managing shadows information.
    struct shadows_parameters_t
    {
        /// Ambient-occlusion related information
        struct ambient_occlusion_info_t
        {
            bool enabled = false;
            float factor = 1.F;
        };

        /// Colour-bleeding related information
        struct colour_bleeding_info_t
        {
            bool enabled = false;
            float r      = 1.0;
            float g      = 1.0;
            float b      = 1.0;
        };

        /// Constructor. Need for shadows_data_t.
        inline shadows_parameters_t() // NOLINT(hicpp-use-equals-default,modernize-use-equals-default)
        {
        }

        /// Enable soft shadows.
        bool soft_shadows = false;

        /// Ambient occlusion information
        ambient_occlusion_info_t ao {};

        /// Colour bleeding information
        colour_bleeding_info_t colour_bleeding {};

        /// Convenience function returning if one of the three parameters is enabled.
        [[nodiscard]] bool enabled() const
        {
            return soft_shadows || ao.enabled || colour_bleeding.enabled;
        }
    };

    /// Internal wrapper holding shadows data.
    struct shadows_data_t
    {
        /// Constructor.
        shadows_data_t(const shadows_parameters_t& _parameters = {}) :
            parameters(_parameters),
            factors(
            {
                _parameters.colour_bleeding.r,
                _parameters.colour_bleeding.g,
                _parameters.colour_bleeding.b,
                _parameters.ao.factor
            })
        {
        }

        /// Shadow parameters
        shadows_parameters_t parameters;

        /// Convenience attribute.
        /// Equivalent to {cb.r, cb.g, cb.g, ao.factor} (cb being shadows_parameters::colour_bleeding).
        Ogre::Vector4 factors;
    };

    /**
     * @brief Constructor.
     * @param _parent_id                 ID of the service using this renderer
     * @param _scene_manager             The scene manager being used.
     * @param _volume_node               This object's node.
     * @param _image                     texture holding the 3D image to be rendered. Created if not specified.
     * @param _buffer (optional)         Enable buffering for the textures updates. Default is false.
     * @param _preintegration (optional) Enable preintegration. Default is false.
     * @param _shadows (optional)        Shadows parameters. Uses default if not specified.
     * @param _sat (optional)            SAT parameters. Uses default if not specified.
     * @param _shader (optional)         Raycasting fragment shader. Default is "RayTracedVolume_FP.glsl".
     * @todo update this comment
     */
    SIGHT_VIZ_SCENE3D_API ray_tracing_volume_renderer(
        std::string _parent_id,
        layer::sptr _layer,
        Ogre::SceneNode* _parent_node,
        sight::data::image::csptr _image,
        sight::data::image::csptr _mask,
        sight::data::transfer_function::csptr _tf,
        std::uint16_t _samples,
        bool _buffer                                                      = false,
        bool _preintegration                                              = false,
        std::optional<shadows_parameters_t> _shadows                      = {},
        std::optional<illum_ambient_occlusion_sat::sat_parameters_t> _sat = {},
        std::optional<std::string> _shader                                = {});

    /// Does nothing.
    SIGHT_VIZ_SCENE3D_API ~ray_tracing_volume_renderer() override;

    /// Updates the raycasting material and sample distance. Also creates the entry point if not done yet.
    SIGHT_VIZ_SCENE3D_API void update(const data::transfer_function::csptr& _tf) override;

    /// Function called when a new image is being rendered.
    SIGHT_VIZ_SCENE3D_API void update_image(data::image::csptr _image, data::transfer_function::csptr _tf) override;

    /// Updates clipping box when the mask is updated.
    SIGHT_VIZ_SCENE3D_API void update_clipping_box(data::image::csptr _mask);

    /// Called when the transfer function is updated.
    SIGHT_VIZ_SCENE3D_API void update_volume_tf(const data::transfer_function::csptr& _tf) override;

    /// Sets the number of samples per view ray.
    SIGHT_VIZ_SCENE3D_API void set_sampling(uint16_t _nb_samples, const data::transfer_function::csptr& _tf) override;

    /// Sets the opacity correction factor.
    SIGHT_VIZ_SCENE3D_API void set_opacity_correction(int _opacity_correction);

    /// Ambient occlusion factor setter.
    /// Returns 'true' if a render update is necessary, i.e. ambient occlusion is enabled.
    [[nodiscard]]
    SIGHT_VIZ_SCENE3D_API virtual bool set_ao_factor(float _factor);

    /// Color bleeding factor setter.
    /// Returns 'true' if a render update is necessary, i.e. colour bleeding is enabled.
    [[nodiscard]]
    SIGHT_VIZ_SCENE3D_API virtual bool set_color_bleeding_factor(float _factor);

    /// Color bleeding factor setter.
    /// Returns 'true' if a render update is necessary, i.e. colour bleeding is enabled.
    [[nodiscard]]
    SIGHT_VIZ_SCENE3D_API virtual bool set_color_bleeding_factor(const Ogre::Vector3& _factors);

    /// Toggles ambient occlusion.
    SIGHT_VIZ_SCENE3D_API virtual void toggle_ambient_occlusion(bool _enable);

    /**
     * @brief Enables the ambient occlusion and soft shadows SAT.
     */
    SIGHT_VIZ_SCENE3D_API virtual void enable_ambient_occlusion();

    /// Disables ambient occlusion.
    SIGHT_VIZ_SCENE3D_API virtual void disable_ambient_occlusion();

    /// Sets pre-integrated mode.
    SIGHT_VIZ_SCENE3D_API void set_pre_integrated_rendering(bool _pre_integrated_rendering) override;

    /// Sets color bleeding usage.
    SIGHT_VIZ_SCENE3D_API virtual void toggle_color_bleeding(bool _enable);

    /// Sets soft shadows usage.
    SIGHT_VIZ_SCENE3D_API virtual void toggle_shadows(bool _enable);

    /// Sets the focal distance used for stereo rendering.
    /// A focal length of 0 focuses on the front of the image and a length of 1 focuses on the back.
    SIGHT_VIZ_SCENE3D_API void set_focal_length(float _focal_length);

    /// Computes image positions, updates the proxy geometry.
    SIGHT_VIZ_SCENE3D_API void clip_image(const Ogre::AxisAlignedBox& _clipping_box) override;

    /// Returns whether or not the volume is visible.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API bool is_visible() const;

    /// Returns the current shadows data used by the renderer.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API const shadows_data_t& shadows() const;

    /**
     * @brief Sets the SAT size ratio.
     * @param _ratio value of the SAT size ratio.
     */
    SIGHT_VIZ_SCENE3D_API void update_sat_size_ratio(float _ratio);

    /**
     * @brief Sets the SAT shells number.
     * @param _count the number of shells used by the SAT.
     */
    SIGHT_VIZ_SCENE3D_API void update_sat_shells_number(unsigned _count);

    /**
     * @brief Sets the SAT shells radius.
     * @param _radius the shells radius used by the SAT.
     */
    SIGHT_VIZ_SCENE3D_API void update_sat_shell_radius(unsigned _radius);

    /**
     * @brief Sets the SAT cone angle.
     * @param _angle the cone angle size of the SAT. Cones ares used to compute soft shadows.
     */
    SIGHT_VIZ_SCENE3D_API void update_sat_cone_angle(float _angle);

    /**
     * @brief Sets the SAT cone samples.
     * @param _samples the cone sample number of the SAT. Cones ares used to compute soft shadows.
     */
    SIGHT_VIZ_SCENE3D_API void update_sat_cone_samples(unsigned _samples);

    /// Updates the SAT with the current 3D texture, transfer function and sampling rate.
    SIGHT_VIZ_SCENE3D_API virtual void update_sat();

protected:

    /// Internal wrapper holding convenience defines
    struct defines
    {
        /// Proxy geometry render queue. We put proxy geometry in render queue 101. Rq 101 is not used by default
        /// and must be explicitly called.
        static inline constexpr std::uint8_t PROXY_GEOMETRY_RQ_GROUP = 101;

        /// Enabled autostereo define. Equivalent to "AUTOSTEREO=1".
        static inline const std::string AUTOSTEREO = "AUTOSTEREO=1";

        /// Enabled ambient occlusion define. Equivalent to "AMBIENT_OCCLUSION=1".
        static inline const std::string AO = "AMBIENT_OCCLUSION=1";

        /// Enabled colour bleeding define. Equivalent to "COLOR_BLEEDING=1".
        static inline const std::string COLOR_BLEEDING = "COLOR_BLEEDING=1";

        /// Enabled shadows define. Equivalent to "SHADOWS=1".
        static inline const std::string SHADOWS = "SHADOWS=1";

        /// Enabled preintegration. Equivalent to "PREINTEGRATION=1".
        static inline const std::string PREINTEGRATION = "PREINTEGRATION=1";

        /// Volume transfer function name. Equivalent to "volumeTransferFunction".
        static inline const std::string VOLUME_TF_TEXUNIT_NAME = "volumeTransferFunction";
    };

    /// Internal wrapper holding the current options (defines) enabled for the raytracing (raycasting) shaders.
    struct options_t
    {
        ///Constructor
        options_t(const std::string& _vertex = "", const std::string& _fragment = "") :
            vertex(_vertex),
            fragment(_fragment),
            hash(std::hash<std::string> {}(_vertex + _fragment))
        {
        }

        /// Vertex options (defines).
        std::string vertex;

        /// Fragment options (defines).
        std::string fragment;

        /// Hash. Computed as std::hash<std::string> {}(vertex + fragment);
        std::size_t hash;
    };

    /// Material lighting parameters.
    struct material_light_t
    {
        material_light_t(
            const Ogre::ColourValue& _diffuse  = Ogre::ColourValue(1.2F, 1.2F, 1.2F, 1.F),
            const Ogre::ColourValue& _specular = Ogre::ColourValue(2.5F, 2.5F, 2.5F, 1.F),
            float _shininess                   = 20.F
        ) :

            diffuse(_diffuse),
            specular(_specular),
            shininess(_shininess)
        {
        }

        /// Diffuse component default is {1.2f, 1.2f, 1.2f, 1.f} in RGBA.
        Ogre::ColourValue diffuse = Ogre::ColourValue(1.2F, 1.2F, 1.2F, 1.F);

        /// Specular component. Default is {2.5f, 2.5f, 2.5f, 1.f} in RGBA
        Ogre::ColourValue specular = Ogre::ColourValue(2.5F, 2.5F, 2.5F, 1.F);

        /// Shininess. Default is 20.f.
        float shininess = 20.F;
    };

    //-----------------------------------------------------------------------------

    /// Sets all texture units needed by the material during the ray casting pass.
    SIGHT_VIZ_SCENE3D_API virtual void set_ray_casting_pass_texture_units(Ogre::Pass* _ray_casting_pass) const;

    /**
     * @brief Generates the material used to render the volume.
     */
    SIGHT_VIZ_SCENE3D_API virtual void update_ray_tracing_material();

    /// Updates the current compositor name according to VR effects flags.
    SIGHT_VIZ_SCENE3D_API virtual void update_options();

    /// Sets the default diffuse, specular and shininess of a material.
    SIGHT_VIZ_SCENE3D_API static void set_material_light_params(
        Ogre::MaterialPtr _mtl,
        const std::optional<material_light_t>& _light = {});

    //-----------------------------------------------------------------------------

    /// Raycasting fragment shader
    std::string m_shader;

    /// Current options enabled for the raycasting pass.
    options_t m_options;

    /// Shadows data
    shadows_data_t m_shadows;

    /// Name of the material
    std::string m_current_mtl_name;

    /// image dimensions.
    data::image::size_t m_image_size = data::image::size_t({1, 1, 1});

    /// Inverse of the sampling rate accounted by the TF.
    float m_opacity_correction_factor = 200.F;

    /// Focal distance in object space : 0 = object front, 1 = object back.
    float m_focal_length = 0.F;

    //------------------------------------------------------------

    /// Shared parameters used for Ray tracing. This should help avoiding using the listener.
    /// We resort to those parameters because setting them using:
    /// Ogre::MaterialManager::getSingletonPtr()->getByName("RTV_Mat")->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant(paramName,
    /// m_idvrAlphaCorrection);
    /// Only seems to update them when instancing the corresponding material
    Ogre::GpuSharedParametersPtr m_rtv_shared_parameters;

    viz::scene3d::layer::wptr m_layer;

    /// Compositor used to compute volume ray entry/exit points.
    ray_entry_compositor::uptr m_ray_entry_compositor {nullptr};

    /// Object containing the proxy geometry, this is a cube for now. Deletion is handled by the scene node.
    Ogre::ManualObject* m_entry_point_geometry {nullptr};

    /// Proxy geometry defining ray entry and exit points. Deletion is handled by the scene node.
    viz::scene3d::vr::grid_proxy_geometry* m_proxy_geometry {nullptr};

    /// SAT used for the ambient occlusion.
    illum_ambient_occlusion_sat m_sat;

    /// Last computed freehand crop box
    Ogre::AxisAlignedBox m_freehand_crop_box;

    /// Camera listener class used to compute the entry points textures before rendering.
    class CameraListener;

    /// Event listener dedicated to camera events.
    std::unique_ptr<CameraListener> m_camera_listener {nullptr};

private:

    /// Creates the proxy geometry defining the entry points for rays.
    void init_entry_points();

    /// Compute the focal length in camera space.
    void compute_real_focal_length();

    /// Updates the ray traced and volume illumination materials according to pre-integration and volume illumination
    /// flags.
    void update_volume_illumination_material();
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline auto ray_tracing_volume_renderer::shadows() const -> const shadows_data_t&
{
    return m_shadows;
}

//------------------------------------------------------------------------------

inline void ray_tracing_volume_renderer::set_material_light_params(
    Ogre::MaterialPtr _mtl,
    const std::optional<material_light_t>& _light
)
{
    const material_light_t& components = _light.value_or(material_light_t {});

    _mtl->setDiffuse(components.diffuse);
    _mtl->setSpecular(components.specular);
    _mtl->setShininess(components.shininess);
}

} // namespace sight::viz::scene3d::vr
