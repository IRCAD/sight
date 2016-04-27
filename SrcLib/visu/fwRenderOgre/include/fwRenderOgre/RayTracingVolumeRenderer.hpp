#ifndef __RAYTRACINGVOLUMERENDERER_HPP__
#define __RAYTRACINGVOLUMERENDERER_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IVolumeRenderer.hpp"

#include <OGRE/OgreManualObject.h>

namespace fwRenderOgre
{

/**
 * @brief Implements a simple GPU ray-tracing renderer.
 */
class RayTracingVolumeRenderer : public IVolumeRenderer
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
     */
    FWRENDEROGRE_API RayTracingVolumeRenderer(std::string parentId,
                                              ::Ogre::SceneManager *sceneManager,
                                              ::Ogre::SceneNode *parentNode,
                                              ::Ogre::TexturePtr imageTexture,
                                              TransferFunction *gpuTF,
                                              PreIntegrationTable *preintegrationTable);

    /// Does nothing.
    FWRENDEROGRE_API virtual ~RayTracingVolumeRenderer();

    /// Does nothing.
    FWRENDEROGRE_API virtual void updateGeometry();

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

    /// Computes image positions, updates the proxy geometry.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

private:

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Updates the proxy geometry.
    void updateEntryPoints();

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject *m_entryPointGeometry;

};

} // namespace fwRenderOgre

#endif // __RAYTRACINGVOLUMERENDERER_HPP__
