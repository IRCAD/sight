#ifndef __RAYTRACINGVOLUMERENDERER_HPP__
#define __RAYTRACINGVOLUMERENDERER_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IVolumeRenderer.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"

#include <OGRE/OgreManualObject.h>

namespace fwRenderOgre
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
     */
    FWRENDEROGRE_API RayTracingVolumeRenderer(std::string parentId,
                                              ::Ogre::SceneManager *sceneManager,
                                              ::Ogre::SceneNode *parentNode,
                                              ::Ogre::TexturePtr imageTexture,
                                              TransferFunction *gpuTF,
                                              PreIntegrationTable *preintegrationTable,
                                              bool mode3D);

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

    /// Configures to layer to handle stereoscopic rendering by adding the stereo VR compositor to the chain.
    FWRENDEROGRE_API void configure3DViewport(Layer::sptr layer);

    /// Sets the focal distance used for stereo rendering.
    /// A focal length of 0 focuses on the front of the image and a length of 1 focuses on the back.
    FWRENDEROGRE_API void setFocalLength(float focalLength);

    /// Computes image positions, updates the proxy geometry.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

    /// Slot: Called when the size of the viewport changes.
    FWRENDEROGRE_API virtual void resizeViewport(int w, int h);

private:

    /// Camera listener class used to compute the entry points textures before rendering.
    struct CameraListener;

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Creates a new grid texture and sets shader parameters for volume bricking.
    void createGridTexture();

    /// Renders the proxy geometry too fill the entry point texture.
    void computeEntryPointsTexture();

    /// Computes the shear warp to apply to a frustum for multi-view rendering based on the angle with the original camera.
    ::Ogre::Matrix4 frustumShearTransform(float angle) const;

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject *m_entryPointGeometry;

    /// Creates and updates the proxy geometry;
    R2VBRenderable *m_proxyGeometryGenerator;

    /// Entity holding the source geometry used for proxy geometry rendering.
    ::Ogre::Entity *m_r2vbSource;

    /// Grid defining volume bricks.
    ::Ogre::TexturePtr m_gridTexture;

    /// Ray entry and exit points for each pixel of each viewpoint.
    std::vector< ::Ogre::TexturePtr> m_entryPointsTextures;

    /// Inverse world-view-projection matrices of each viewpoint.
    std::vector< ::Ogre::Matrix4> m_viewPointMatrices;

    /// Render operation used to compute the brick grid.
    ::Ogre::RenderOperation m_gridRenderOp;

    /// Image dimensions.
    ::fwData::Image::SizeType m_imageSize;

    /// Brick Grid dimensions.
    int m_gridSize[3];

    /// Size of a volume brick.
    int m_bricksSize[3];

    /// Sets stereoscopic volume rendering for Alioscopy monitors.
    bool m_mode3D;
};

} // namespace fwRenderOgre

#endif // __RAYTRACINGVOLUMERENDERER_HPP__
