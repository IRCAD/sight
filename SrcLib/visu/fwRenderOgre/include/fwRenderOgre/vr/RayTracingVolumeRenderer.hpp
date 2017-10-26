/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__
#define __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__

#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"
#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"
#include "fwRenderOgre/vr/GridProxyGeometry.hpp"
#include "fwRenderOgre/vr/IVolumeRenderer.hpp"
#include "fwRenderOgre/vr/SATVolumeIllumination.hpp"

#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRectangle2D.h>
#include <OGRE/OgreTechnique.h>

#include <vector>

namespace fwRenderOgre
{

namespace vr
{

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
     * @param gpuTF                Texture holding the transfer function.
     * @param preintegrationTable  Texture holding the pre-integration table.
     * @param mode3D               Stereoscopic volume rendering flag.
     * @param ambientOcclusion     Ambient occlusion flag.
     * @param colorBleeding        Color bleeding flag.
     * @param shadows              Soft shadows flag.
     */
    FWRENDEROGRE_API RayTracingVolumeRenderer(std::string parentId,
                                              Layer::sptr layer,
                                              ::Ogre::SceneNode* parentNode,
                                              ::Ogre::TexturePtr imageTexture,
                                              TransferFunction& gpuTF,
                                              PreIntegrationTable& preintegrationTable,
                                              ::fwRenderOgre::Layer::StereoModeType stereoMode,
                                              bool ambientOcclusion = false,
                                              bool colorBleeding = false,
                                              bool shadows = false,
                                              double aoFactor = 1.,
                                              double colorBleedingFactor = 1.);

    /// Does nothing.
    FWRENDEROGRE_API virtual ~RayTracingVolumeRenderer();

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    /// Called when the transfer function is updated.
    FWRENDEROGRE_API virtual void tfUpdate(::fwData::TransferFunction::sptr tf);

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    /// Sets the opacity correction factor.
    FWRENDEROGRE_API void setOpacityCorrection(int opacityCorrection);

    /// Ambient occlusion factor setter.
    FWRENDEROGRE_API virtual void setAOFactor(double aoFactor);

    /// Color bleeding factor setter.
    FWRENDEROGRE_API virtual void setColorBleedingFactor(double colorBleedingFactor);

    FWRENDEROGRE_API virtual void setIlluminationVolume(SATVolumeIllumination::sptr illuminationVolume);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

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
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

    /// Slot: Called when the size of the viewport changes.
    FWRENDEROGRE_API virtual void resizeViewport(int w, int h);

    /// IllumVolume getter.
    FWRENDEROGRE_API SATVolumeIllumination::sptr getIllumVolume();

    /// Layer getter
    ::fwRenderOgre::Layer::sptr getLayer() const;

protected:

    /// Updates the current compositor name according to VR effects flags.
    /// @return tuple containing a
    /// - Comma separated list of preprocessor defines to use in vertex shaders.
    /// - Comma separated list of preprocessor defines to use in fragment shaders.
    /// - Hash allowing to identify the material
    FWRENDEROGRE_API virtual std::tuple<std::string, std::string, size_t> computeRayTracingDefines() const;

    /// Sets all texture units needed by the material during the ray casting pass.
    FWRENDEROGRE_API virtual void setRayCastingPassTextureUnits(::Ogre::Pass* _rayCastingPass,
                                                                const std::string& _fpPPDefines) const;

    /// When using AutoStereo compositor, initialize the raytracing material.
    FWRENDEROGRE_API void createRayTracingMaterial();

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

    /// Ray entry and exit points for each pixel of each viewpoint.
    std::vector< ::Ogre::TexturePtr> m_entryPointsTextures;

private:
    void compileShaderPrograms(const std::string& _vpPPDefines, const std::string& _fpPPDefines);

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Renders the proxy geometry to fill the entry point texture.
    void computeEntryPointsTexture();

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

    /// Sets stereoscopic volume rendering for autostereoscopic monitors.
    ::fwRenderOgre::Layer::StereoModeType m_stereoMode;

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion;

    /// Sets usage of color bleeding.
    bool m_colorBleeding;

    /// Sets usage of soft shadows.
    bool m_shadows;

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

    /// Autostereo listener
    compositor::listener::AutoStereoCompositorListener* m_autostereoListener;

    ::Ogre::Rectangle2D* m_fullScreenQuad;
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

#endif // __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__
