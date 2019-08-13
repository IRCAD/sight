/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"
#include "fwRenderOgre/vr/GridProxyGeometry.hpp"
#include "fwRenderOgre/vr/IVolumeRenderer.hpp"
#include "fwRenderOgre/vr/RayEntryCompositor.hpp"
#include "fwRenderOgre/vr/SATVolumeIllumination.hpp"

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

#include <vector>

namespace fwRenderOgre
{

namespace vr
{

// We put proxy geometry in render queue 101. Rq 101 is not used by default and must be explicitly called.
static const std::uint8_t s_PROXY_GEOMETRY_RQ_GROUP = 101;

/**
 * @brief Implements a simple GPU ray-tracing renderer.
 */
class FWRENDEROGRE_CLASS_API RayTracingVolumeRenderer : public IVolumeRenderer
{
public:

    /**
     * @brief Constructor.
     * @param parentId             ID of the service using this renderer
     * @param sceneManager         The scene manager being used.
     * @param volumeNode           This object's node.
     * @param imageTexture         Texture holding the 3D image to be rendered.
     * @param gpuVolumeTF          Texture holding the transfer function.
     * @param preintegrationTable  Texture holding the pre-integration table.
     * @param mode3D               Stereoscopic volume rendering flag.
     * @param ambientOcclusion     Ambient occlusion flag.
     * @param colorBleeding        Color bleeding flag.
     * @param shadows              Soft shadows flag.
     */
    FWRENDEROGRE_API RayTracingVolumeRenderer(std::string parentId,
                                              Layer::sptr layer,
                                              Ogre::SceneNode* const parentNode,
                                              ::Ogre::TexturePtr imageTexture,
                                              const TransferFunction::sptr& gpuVolumeTF,
                                              PreIntegrationTable& preintegrationTable,
                                              bool ambientOcclusion      = false,
                                              bool colorBleeding         = false,
                                              bool shadows               = false,
                                              double aoFactor            = 1.,
                                              double colorBleedingFactor = 1.);

    /// Does nothing.
    FWRENDEROGRE_API virtual ~RayTracingVolumeRenderer();

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(const fwData::Image::sptr image,
                                              const fwData::TransferFunction::sptr tf) override;

    /// Called when the transfer function is updated.
    FWRENDEROGRE_API virtual void updateVolumeTF() override;

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples) override;

    /// Sets the opacity correction factor.
    FWRENDEROGRE_API void setOpacityCorrection(int opacityCorrection);

    /// Ambient occlusion factor setter.
    FWRENDEROGRE_API virtual void setAOFactor(double aoFactor);

    /// Color bleeding factor setter.
    FWRENDEROGRE_API virtual void setColorBleedingFactor(double colorBleedingFactor);

    FWRENDEROGRE_API virtual void setIlluminationVolume(SATVolumeIllumination::sptr illuminationVolume);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering) override;

    /// Sets ambient occlusion usage.
    FWRENDEROGRE_API virtual void setAmbientOcclusion(bool ambientOcclusion);

    /// Sets color bleeding usage.
    FWRENDEROGRE_API virtual void setColorBleeding(bool colorBleeding);

    /// Sets soft shadows usage.
    FWRENDEROGRE_API virtual void setShadows(bool shadows);

    /// Sets the focal distance used for stereo rendering.
    /// A focal length of 0 focuses on the front of the image and a length of 1 focuses on the back.
    FWRENDEROGRE_API void setFocalLength(float focalLength);

    /// Computes image positions, updates the proxy geometry.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox) override;

    /// IllumVolume getter.
    FWRENDEROGRE_API SATVolumeIllumination::sptr getIllumVolume();

    /// Returns whether or not the volume is visible.
    FWRENDEROGRE_API bool isVisible() const;

    /// Layer getter
    ::fwRenderOgre::Layer::sptr getLayer() const;

    /// Sets texture.
    FWRENDEROGRE_API void set3DTexture(const ::Ogre::TexturePtr& _texture );

protected:

    /// Updates the current compositor name according to VR effects flags.
    /// @return tuple containing a
    /// - Comma separated list of preprocessor defines to use in vertex shaders.
    /// - Comma separated list of preprocessor defines to use in fragment shaders.
    /// - Hash allowing to identify the material
    FWRENDEROGRE_API virtual std::tuple<std::string, std::string, size_t> computeRayTracingDefines() const;

    /// Sets all texture units needed by the material during the ray casting pass.
    FWRENDEROGRE_API virtual void setRayCastingPassTextureUnits(Ogre::Pass* const _rayCastingPass,
                                                                const std::string& _fpPPDefines) const;
    /**
     * @brief Generates the material used to render the volume.
     * @param _sourceFile, fragment shader's file name.
     */
    FWRENDEROGRE_API virtual void createRayTracingMaterial(const std::string& _sourceFile = "RayTracedVolume_FP.glsl");

    /// Shared parameters used for Ray tracing. This should help avoiding using the listener.
    /// We resort to those parameters because setting them using:
    /// ::Ogre::MaterialManager::getSingletonPtr()->getByName("RTV_Mat")->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant(paramName,
    /// m_idvrAlphaCorrection);
    /// Only seems to update them when instancing the corresponding material
    ::Ogre::GpuSharedParametersPtr m_RTVSharedParameters;

    /// Name of the material
    std::string m_currentMtlName;

    /// List of file names to attach to the fragment shader for compiling.
    std::vector<std::string> m_fragmentShaderAttachements;

    /// Compositor used to compute volume ray entry/exit points.
    RayEntryCompositor::uptr m_rayEntryCompositor {nullptr};

private:

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Compute the focal length in camera space.
    void computeRealFocalLength();

    /// Updates the ray traced and volume illumination materials according to pre-integration and volume illumination
    /// flags.
    void updateVolIllumMat();

    /// Sets the default diffuse, specular and shininess in the material.
    void setMaterialLightParams(::Ogre::MaterialPtr mtl);

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject* m_entryPointGeometry;

    /// Proxy geometry defining ray entry and exit points.
    ::fwRenderOgre::vr::GridProxyGeometry* m_proxyGeometry;

    /// Image dimensions.
    ::fwData::Image::SizeType m_imageSize;

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion;

    /// Sets usage of color bleeding.
    bool m_colorBleeding;

    /// Sets usage of soft shadows.
    bool m_shadows;

    /// TF texture used for rendering.
    TransferFunction::wptr m_gpuVolumeTF;

    /// Factor parameter used to weight ambient occlusion (A channel) and color bleeding (RGB channels).
    ::Ogre::Vector4 m_volIllumFactor;

    /// Inverse of the sampling rate accounted by the TF.
    float m_opacityCorrectionFactor;

    std::weak_ptr<SATVolumeIllumination> m_illumVolume;

    /// Focal distance in object space : 0 = object front, 1 = object back.
    float m_focalLength;

    /// Camera listener class used to compute the entry points textures before rendering.
    struct CameraListener;
    CameraListener* m_cameraListener;

    ::fwRenderOgre::Layer::wptr m_layer;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::fwRenderOgre::vr::SATVolumeIllumination::sptr RayTracingVolumeRenderer::getIllumVolume()
{
    return m_illumVolume.lock();
}

//-----------------------------------------------------------------------------

inline ::fwRenderOgre::Layer::sptr RayTracingVolumeRenderer::getLayer() const
{
    return m_layer.lock();
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
