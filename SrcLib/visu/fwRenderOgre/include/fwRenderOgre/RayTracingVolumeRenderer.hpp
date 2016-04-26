#ifndef __RAYTRACINGVOLUMERENDERER_HPP__
#define __RAYTRACINGVOLUMERENDERER_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IVolumeRenderer.hpp"

#include <OGRE/OgreManualObject.h>

namespace fwRenderOgre
{

class RayTracingVolumeRenderer : public IVolumeRenderer
{
public:

    FWRENDEROGRE_API RayTracingVolumeRenderer(std::string parentId,
                                              ::Ogre::SceneManager *sceneManager,
                                              ::Ogre::SceneNode *parentNode,
                                              ::Ogre::TexturePtr imageTexture,
                                              TransferFunction *gpuTF,
                                              PreIntegrationTable *preintegrationTable);

    FWRENDEROGRE_API virtual ~RayTracingVolumeRenderer();

    FWRENDEROGRE_API virtual void updateGeometry();

    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

private:

    void initEntryPoints();

    void updateEntryPoints();

    ::Ogre::ManualObject *m_entryPointGeometry;

};

} // namespace fwRenderOgre

#endif // __RAYTRACINGVOLUMERENDERER_HPP__
