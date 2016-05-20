#ifndef __RAYTRACINGVOLUMERENDERER_HPP__
#define __RAYTRACINGVOLUMERENDERER_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IVolumeRenderer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"

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

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    /// Called when the transfer function is updated.
    FWRENDEROGRE_API virtual void tfUpdate(::fwData::TransferFunction::sptr tf);

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

    /// Computes image positions, updates the proxy geometry.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

private:

//    struct RenderListener : public ::Ogre::MovableObject::Listener
//    {
//        RayTracingVolumeRenderer *m_renderer;

//        RenderListener(RayTracingVolumeRenderer *renderer) :
//            m_renderer(renderer)
//        {
//        }

//        virtual bool objectRendering(const ::Ogre::MovableObject *, const ::Ogre::Camera *)
//        {
//            m_renderer->computeEntryPoints();
//            return true; // Always render object
//        }

//    } *m_renderListener;
    struct CameraListener : public ::Ogre::Camera::Listener
    {
        RayTracingVolumeRenderer *m_renderer;

        CameraListener(RayTracingVolumeRenderer *renderer) :
            m_renderer(renderer)
        {
        }

        virtual void cameraPreRenderScene(::Ogre::Camera*)
        {
            m_renderer->computeEntryPoints();
        }
    };


    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Updates the proxy geometry.
    void updateEntryPoints();

    void computeEntryPoints();

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject *m_entryPointGeometry;

    /// Creates and updates the proxy geometry;
    R2VBRenderable *m_proxyGeometryGenerator;

    ::Ogre::TexturePtr m_gridTexture;

    ::Ogre::TexturePtr m_entryPointsTexture;

    ::Ogre::RenderOperation m_gridRenderOp;

};

} // namespace fwRenderOgre

#endif // __RAYTRACINGVOLUMERENDERER_HPP__
