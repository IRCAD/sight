#include "fwRenderOgre/IVolumeRenderer.hpp"

#include <boost/algorithm/clamp.hpp>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const IVolumeRenderer::CubeFacePositionsMap IVolumeRenderer::s_cubeFaces = {
    { IVolumeRenderer::Z_POSITIVE, { 3, 4, 1, 0 } },
    { IVolumeRenderer::Z_NEGATIVE, { 2, 5, 7, 6 } },
    { IVolumeRenderer::Y_POSITIVE, { 2, 6, 3, 0 } },
    { IVolumeRenderer::Y_NEGATIVE, { 1, 4, 7, 5 } },
    { IVolumeRenderer::X_POSITIVE, { 0, 1, 5, 2 } },
    { IVolumeRenderer::X_NEGATIVE, { 6, 7, 4, 3 } }
};

//-----------------------------------------------------------------------------

const IVolumeRenderer::CubeEdgeList IVolumeRenderer::s_cubeEdges = { {
    { 0, 1 }, { 1, 4 }, { 4, 3 }, { 3, 0 },
    { 0, 2 }, { 1, 5 }, { 4, 7 }, { 3, 6 },
    { 2, 5 }, { 5, 7 }, { 7, 6 }, { 6, 2 }
} };

//-----------------------------------------------------------------------------

IVolumeRenderer::IVolumeRenderer(std::string parentId,
                                 ::Ogre::SceneManager *sceneManager,
                                 ::Ogre::SceneNode *volumeNode,
                                 ::Ogre::TexturePtr imageTexture,
                                 TransferFunction *gpuTF,
                                 PreIntegrationTable *preintegrationTable) :
    m_parentId               (parentId),
    m_sceneManager           (sceneManager),
    m_3DOgreTexture          (imageTexture),
    m_gpuTF                  (gpuTF),
    m_preIntegrationTable    (preintegrationTable),
    m_volumeSceneNode        (volumeNode),
    m_nbSlices               (512),
    m_preIntegratedRendering (false)
{
    m_camera = m_sceneManager->getCamera("PlayerCam");

    std::copy(m_imagePositions, m_imagePositions + 8, m_clippedImagePositions);
}

//-----------------------------------------------------------------------------

IVolumeRenderer::~IVolumeRenderer()
{

}

//-----------------------------------------------------------------------------

void IVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    const ::Ogre::Vector3 min = clippingBox.getMinimum();
    const ::Ogre::Vector3 max = clippingBox.getMaximum();

    for(unsigned i = 0; i < 8; ++ i)
    {
        m_clippedImagePositions[i] = ::Ogre::Vector3(
                    ::boost::algorithm::clamp(m_imagePositions[i].x, min.x, max.x),
                    ::boost::algorithm::clamp(m_imagePositions[i].y, min.y, max.y),
                    ::boost::algorithm::clamp(m_imagePositions[i].z, min.z, max.z));
    }
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::scaleCube(const fwData::Image::SpacingType& spacing)
{
    const double width  = static_cast< double > (m_3DOgreTexture->getWidth() ) * spacing[0];
    const double height = static_cast< double > (m_3DOgreTexture->getHeight()) * spacing[1];
    const double depth  = static_cast< double > (m_3DOgreTexture->getDepth() ) * spacing[2];

    const double maxDim = std::max(width, std::max(height, depth));

    const ::Ogre::Vector3 scaleFactors(
                static_cast<float>(width  / maxDim),
                static_cast<float>(height / maxDim),
                static_cast<float>(depth  / maxDim));

    m_volumeSceneNode->setScale(scaleFactors);
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
