/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/R2VBRenderable.hpp"
#include "viz/scene3d/vr/GridProxyGeometry.hpp"
#include "viz/scene3d/vr/IllumAmbientOcclusionSAT.hpp"
#include "viz/scene3d/vr/IVolumeRenderer.hpp"
#include "viz/scene3d/vr/RayEntryCompositor.hpp"

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

#include <optional>
#include <vector>

namespace sight::viz::scene3d
{

namespace vr
{

/**
 * @brief Implements a simple GPU ray-tracing renderer.
 */
class VIZ_SCENE3D_CLASS_API RayTracingVolumeRenderer : public IVolumeRenderer
{
public:

    /// Internal wrapper managing shadows information.
    struct shadows_parameters_t
    {
        /// Ambient-occlusion related information
        struct ambient_occlusion_info_t
        {
            bool enabled = false;
            float factor = 1.f;
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
        inline shadows_parameters_t()
        {
        }

        /// Enable soft shadows.
        bool soft_shadows = false;

        /// Ambient occlusion information
        ambient_occlusion_info_t ao {};

        /// Colour bleeding information
        colour_bleeding_info_t colour_bleeding {};

        /// Convenience function returning if one of the three parameters is enabled.
        bool enabled() const
        {
            return soft_shadows || ao.enabled || colour_bleeding.enabled;
        }
    };

    /// Internal wrapper holding shadows data.
    struct shadows_data_t
    {
        /// Constructor.
        shadows_data_t(const shadows_parameters_t& parameters = {}) :
            parameters(parameters),
            factors(
                {
                    parameters.colour_bleeding.r,
                    parameters.colour_bleeding.g,
                    parameters.colour_bleeding.b,
                    parameters.ao.factor
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
     * @param parentId                  ID of the service using this renderer
     * @param sceneManager              The scene manager being used.
     * @param volumeNode                This object's node.
     * @param imageTexture (optional)   Texture holding the 3D image to be rendered. Created if not specified.
     * @param buffer (optional)         Enable buffering for the textures updates. Default is false.
     * @param preintegration (optional) Enable preintegration. Default is false.
     * @param shadows (optional)        Shadows parameters. Uses default if not specified.
     * @param sat (optional)            SAT parameters. Uses default if not specified.
     * @param shader (optional)         Raycasting fragment shader. Default is "RayTracedVolume_FP.glsl".
     */
    VIZ_SCENE3D_API RayTracingVolumeRenderer(
        std::string parentId,
        Layer::sptr layer,
        Ogre::SceneNode* const parentNode,
        std::optional<Ogre::TexturePtr> imageTexture                         = {},
        bool buffer                                                          = false,
        bool preintegration                                                  = false,
        const std::optional<shadows_parameters_t> shadows                    = {},
        const std::optional<IllumAmbientOcclusionSAT::sat_parameters_t>& sat = {},
        const std::optional<std::string>& shader                             = {});

    /// Does nothing.
    VIZ_SCENE3D_API ~RayTracingVolumeRenderer() override;

    /// Updates the raycasting material and sample distance. Also creates the entry point if not done yet.
    VIZ_SCENE3D_API void update() override;

    /// Function called when a new image is being rendered.
    VIZ_SCENE3D_API void imageUpdate(
        const data::Image::sptr image,
        const data::TransferFunction::sptr tf
    ) override;

    /// Called when the transfer function is updated.
    VIZ_SCENE3D_API void updateVolumeTF(std::shared_ptr<data::TransferFunction>& tf) override;

    /// Sets the number of samples per view ray.
    VIZ_SCENE3D_API void setSampling(uint16_t nbSamples) override;

    /// Sets the opacity correction factor.
    VIZ_SCENE3D_API void setOpacityCorrection(int opacityCorrection);

    /// Ambient occlusion factor setter.
    /// Returns 'true' if a render update is necessary, i.e. ambient occlusion is enabled.
    [[nodiscard]]
    VIZ_SCENE3D_API virtual bool setAOFactor(float factor);

    /// Color bleeding factor setter.
    /// Returns 'true' if a render update is necessary, i.e. colour bleeding is enabled.
    [[nodiscard]]
    VIZ_SCENE3D_API virtual bool setColorBleedingFactor(float factor);

    /// Color bleeding factor setter.
    /// Returns 'true' if a render update is necessary, i.e. colour bleeding is enabled.
    [[nodiscard]]
    VIZ_SCENE3D_API virtual bool setColorBleedingFactor(const Ogre::Vector3& factors);

    /// Toggles ambient occlusion.
    VIZ_SCENE3D_API virtual void toggleAmbientOcclusion(bool enable);

    /**
     * @brief Enables the ambient occlusion and soft shadows SAT.
     */
    VIZ_SCENE3D_API virtual void enableAmbientOcclusion();

    /// Disables ambient occlusion.
    VIZ_SCENE3D_API virtual void disableAmbientOcclusion();

    /// Sets pre-integrated mode.
    VIZ_SCENE3D_API void setPreIntegratedRendering(bool preIntegratedRendering) override;

    /// Sets color bleeding usage.
    VIZ_SCENE3D_API virtual void toggleColorBleeding(bool enable);

    /// Sets soft shadows usage.
    VIZ_SCENE3D_API virtual void toggleShadows(bool enable);

    /// Sets the focal distance used for stereo rendering.
    /// A focal length of 0 focuses on the front of the image and a length of 1 focuses on the back.
    VIZ_SCENE3D_API void setFocalLength(float focalLength);

    /// Computes image positions, updates the proxy geometry.
    VIZ_SCENE3D_API void clipImage(const Ogre::AxisAlignedBox& clippingBox) override;

    /// Returns whether or not the volume is visible.
    VIZ_SCENE3D_API bool isVisible() const;

    /// Returns the current shadows data used by the renderer.
    VIZ_SCENE3D_API const shadows_data_t& shadows() const;

    /**
     * @brief Sets the SAT size ratio.
     * @param ratio value of the SAT size ratio.
     */
    VIZ_SCENE3D_API void updateSATSizeRatio(unsigned ratio);

    /**
     * @brief Sets the SAT shells number.
     * @param count the number of shells used by the SAT.
     */
    VIZ_SCENE3D_API void updateSATShellsNumber(unsigned count);

    /**
     * @brief Sets the SAT shells radius.
     * @param radius the shells radius used by the SAT.
     */
    VIZ_SCENE3D_API void updateSATShellRadius(unsigned radius);

    /**
     * @brief Sets the SAT cone angle.
     * @param angle the cone angle size of the SAT. Cones ares used to compute soft shadows.
     */
    VIZ_SCENE3D_API void updateSATConeAngle(float angle);

    /**
     * @brief Sets the SAT cone samples.
     * @param samples the cone sample number of the SAT. Cones ares used to compute soft shadows.
     */
    VIZ_SCENE3D_API void updateSATConeSamples(unsigned samples);

    /// Updates the SAT with the current 3D texture, transfer function and sampling rate.
    VIZ_SCENE3D_API virtual void updateSAT();

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
        options_t(const std::string& vertex = "", const std::string& fragment = "") :
            vertex(vertex),
            fragment(fragment),
            hash(std::hash<std::string> {}(vertex + fragment))
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
            const Ogre::ColourValue& diffuse  = Ogre::ColourValue(1.2f, 1.2f, 1.2f, 1.f),
            const Ogre::ColourValue& specular = Ogre::ColourValue(2.5f, 2.5f, 2.5f, 1.f),
            float shininess                   = 20.f
        ) :

            diffuse(diffuse),
            specular(specular),
            shininess(shininess)
        {
        }

        /// Diffuse component default is {1.2f, 1.2f, 1.2f, 1.f} in RGBA.
        Ogre::ColourValue diffuse = Ogre::ColourValue(1.2f, 1.2f, 1.2f, 1.f);

        /// Specular component. Default is {2.5f, 2.5f, 2.5f, 1.f} in RGBA
        Ogre::ColourValue specular = Ogre::ColourValue(2.5f, 2.5f, 2.5f, 1.f);

        /// Shininess. Default is 20.f.
        float shininess = 20.f;
    };

    //-----------------------------------------------------------------------------

    /// Sets all texture units needed by the material during the ray casting pass.
    VIZ_SCENE3D_API virtual void setRayCastingPassTextureUnits(Ogre::Pass* const _rayCastingPass) const;

    /**
     * @brief Generates the material used to render the volume.
     */
    VIZ_SCENE3D_API virtual void updateRayTracingMaterial();

    /// Updates the current compositor name according to VR effects flags.
    VIZ_SCENE3D_API virtual void updateOptions();

    /// Sets the default diffuse, specular and shininess of a material.
    VIZ_SCENE3D_API static void setMaterialLightParams(
        Ogre::MaterialPtr mtl,
        const std::optional<material_light_t>& light = {});

    //-----------------------------------------------------------------------------

    /// Raycasting fragment shader
    std::string m_shader;

    /// Current options enabled for the raycasting pass.
    options_t m_options;

    /// Shadows data
    shadows_data_t m_shadows;

    /// Name of the material
    std::string m_currentMtlName;

    /// Image dimensions.
    data::Image::Size m_imageSize = data::Image::Size({1, 1, 1});

    /// Inverse of the sampling rate accounted by the TF.
    float m_opacityCorrectionFactor = 200.f;

    /// Focal distance in object space : 0 = object front, 1 = object back.
    float m_focalLength = 0.f;

    //------------------------------------------------------------

    /// Shared parameters used for Ray tracing. This should help avoiding using the listener.
    /// We resort to those parameters because setting them using:
    /// Ogre::MaterialManager::getSingletonPtr()->getByName("RTV_Mat")->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant(paramName,
    /// m_idvrAlphaCorrection);
    /// Only seems to update them when instancing the corresponding material
    Ogre::GpuSharedParametersPtr m_RTVSharedParameters;

    viz::scene3d::Layer::wptr m_layer;

    /// Compositor used to compute volume ray entry/exit points.
    RayEntryCompositor::uptr m_rayEntryCompositor {nullptr};

    /// Object containing the proxy geometry, this is a cube for now. Deletion is handled by the scene node.
    Ogre::ManualObject* m_entryPointGeometry {nullptr};

    /// Proxy geometry defining ray entry and exit points. Deletion is handled by the scene node.
    viz::scene3d::vr::GridProxyGeometry* m_proxyGeometry {nullptr};

    /// SAT used for the ambient occlusion.
    IllumAmbientOcclusionSAT m_sat;

    //------------------------------------------------------------

    /// Camera listener class used to compute the entry points textures before rendering.
    class CameraListener;

    /// Event listener dedicated to camera events.
    std::unique_ptr<CameraListener> m_cameraListener {nullptr};

private:

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Compute the focal length in camera space.
    void computeRealFocalLength();

    /// Updates the ray traced and volume illumination materials according to pre-integration and volume illumination
    /// flags.
    void updateVolumeIlluminationMaterial();
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline auto RayTracingVolumeRenderer::shadows() const -> const shadows_data_t&
{
    return m_shadows;
}

//------------------------------------------------------------------------------

inline void RayTracingVolumeRenderer::setMaterialLightParams(
    Ogre::MaterialPtr mtl,
    const std::optional<material_light_t>& light
)
{
    const material_light_t& components = light.value_or(material_light_t {});

    mtl->setDiffuse(components.diffuse);
    mtl->setSpecular(components.specular);
    mtl->setShininess(components.shininess);
}

} // namespace vr

} // namespace sight::viz::scene3d
