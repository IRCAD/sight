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

    struct CameraListener : public ::Ogre::Camera::Listener
    {
        RayTracingVolumeRenderer *m_renderer;

        CameraListener(RayTracingVolumeRenderer *renderer) :
            m_renderer(renderer)
        {
        }

        virtual void cameraPreRenderScene(::Ogre::Camera*)
        {
            m_renderer->computeEntryPointsTexture();
        }
    };


    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Creates a new grid texture and sets shader parameters for volume bricking.
    void createGridTexture();

    /// Renders the proxy geometry too fill the entry point texture.
    void computeEntryPointsTexture();

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject *m_entryPointGeometry;

    /// Creates and updates the proxy geometry;
    R2VBRenderable *m_proxyGeometryGenerator;

    /// Entity holding the source geometry used for proxy geometry rendering.
    ::Ogre::Entity *m_r2vbSource;

    /// Grid defining volume bricks.
    ::Ogre::TexturePtr m_gridTexture;

    /// Texture holding ray entry and exit points for each screen pixel.
    ::Ogre::TexturePtr m_entryPointsTexture;

    /// Render operation used to compute the brick grid.
    ::Ogre::RenderOperation m_gridRenderOp;

    /// Image dimensions.
    ::fwData::Image::SizeType m_imageSize;

    /// Brick Grid dimensions.
    int m_gridSize[3];

    /// Size of a volume brick.
    int m_bricksSize[3];
};

} // namespace fwRenderOgre

#endif // __RAYTRACINGVOLUMERENDERER_HPP__
