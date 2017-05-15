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
#include "fwRenderOgre/vr/IVolumeRenderer.hpp"
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
                                              ::fwRenderOgre::Layer::StereoModeType mode3D,
                                              bool ambientOcclusion = false,
                                              bool colorBleeding = false,
                                              bool shadows = false,
                                              double aoFactor = 1.f,
                                              double colorBleedingFactor = 1.f);

    /// Does nothing.
    FWRENDEROGRE_API virtual ~RayTracingVolumeRenderer();

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    /// Called when the transfer function is updated.
    FWRENDEROGRE_API virtual void tfUpdate(::fwData::TransferFunction::sptr tf);

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    /// Ambient occlusion factor setter.
    FWRENDEROGRE_API virtual void setAOFactor(double aoFactor);

    /// Color bleeding factor setter.
    FWRENDEROGRE_API virtual void setColorBleedingFactor(double colorBleedingFactor);

    FWRENDEROGRE_API virtual void setIlluminationVolume(SATVolumeIllumination* illuminationVolume);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

    /// Sets ambient occlusion usage.
    FWRENDEROGRE_API virtual void setAmbientOcclusion(bool ambientOcclusion);

    /// Sets color bleeding usage.
    FWRENDEROGRE_API virtual void setColorBleeding(bool colorBleeding);

    /// Sets soft shadows usage.
    FWRENDEROGRE_API virtual void setShadows(bool shadows);

    /// Computes image positions, updates the proxy geometry.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

    /// Slot: Called when the size of the viewport changes.
    FWRENDEROGRE_API virtual void resizeViewport(int w, int h);

    /// IllumVolume getter.
    FWRENDEROGRE_API SATVolumeIllumination* getIllumVolume();

    /// Layer getter
    ::fwRenderOgre::Layer::sptr getLayer() const;

private:

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Creates a new grid texture and sets shader parameters for volume bricking.
    void createGridTexture();

    /// Renders the proxy geometry too fill the entry point texture.
    void computeEntryPointsTexture();

    /// Updates the ray traced and volume illumination materials according to pre-integration and volume illumination
    /// flags.
    void updateMatNames();

    /// Set a named constant into the current fragment shader.
    template<typename T>
    void setFpNamedConstant(const std::string& _name, T value);

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject* m_entryPointGeometry;

    /// Creates and updates the proxy geometry.
    R2VBRenderable* m_proxyGeometryGenerator;

    /// Entity holding the source geometry used for proxy geometry rendering.
    ::Ogre::Entity* m_r2vbSource;

    /// Grid defining volume bricks.
    ::Ogre::TexturePtr m_gridTexture;

    /// Ray entry and exit points for each pixel of each viewpoint.
    std::vector< ::Ogre::TexturePtr> m_entryPointsTextures;

    std::vector< ::Ogre::TextureUnitState* > m_rayTracedTexUnitStates;

    /// Inverse world-view-projection matrices of each viewpoint.
    std::vector< ::Ogre::Matrix4> m_viewPointMatrices;

    /// Render operation used to compute the brick grid.
    ::Ogre::RenderOperation m_gridRenderOp;

    /// Image dimensions.
    ::fwData::Image::SizeType m_imageSize;

    /// Brick Grid dimensions.
    std::array< int, 3 > m_gridSize;

    /// Size of a volume brick.
    std::array< int, 3 > m_bricksSize;

    /// Sets stereoscopic volume rendering for autostereoscopic monitors.
    ::fwRenderOgre::Layer::StereoModeType m_stereoMode;

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion;

    /// Sets usage of color bleeding.
    bool m_colorBleeding;

    /// Sets usage of soft shadows.
    bool m_shadows;

    /// Factor parameter used to weight the ambient occlusion. It is replicated three times to fill the RGB channels of
    /// a GLSL vec4.
    double m_aoFactor;

    /// Factor parameter used to weight the color bleeding. It is used to fill the alpha channel of a GLSL vec4.
    double m_colorBleedingFactor;

    /// Name of the current volume illumination material.
    std::string m_currentMtlName;

    SATVolumeIllumination* m_illumVolume;

    /// Camera listener class used to compute the entry points textures before rendering.
    struct CameraListener;
    CameraListener* m_cameraListener;

    std::string m_compositorName;

    ::fwRenderOgre::Layer::wptr m_layer;

    /// Autostereo listener
    compositor::listener::AutoStereoCompositorListener* m_autostereoListener;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::fwRenderOgre::vr::SATVolumeIllumination* RayTracingVolumeRenderer::getIllumVolume()
{
    return m_illumVolume;
}

//-----------------------------------------------------------------------------

inline ::fwRenderOgre::Layer::sptr RayTracingVolumeRenderer::getLayer() const
{
    return m_layer.lock();
}

//-----------------------------------------------------------------------------

template<typename T>
void RayTracingVolumeRenderer::setFpNamedConstant(const std::string& _name, T _value)
{
    auto& mtlMgr = ::Ogre::MaterialManager::getSingleton();
    ::Ogre::MaterialPtr currentMtl = mtlMgr.getByName(m_currentMtlName,
                                                      ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    OSLM_ASSERT("Material '" + m_currentMtlName + "' not found", !currentMtl.isNull());

    ::Ogre::Material::TechniqueIterator techIt = currentMtl->getTechniqueIterator();
    while( techIt.hasMoreElements())
    {
        ::Ogre::Technique* technique = techIt.getNext();

        SLM_ASSERT("Technique is not set", technique);

        ::Ogre::Technique::PassIterator passIt = technique->getPassIterator();

        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* ogrePass = passIt.getNext();

            ogrePass->getFragmentProgramParameters()->setNamedConstant(_name, _value);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__
