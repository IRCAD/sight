/************************************************************************
 *
 * Copyright (C) 2016-2018 IRCAD France
 * Copyright (C) 2016-2018 IHU Strasbourg
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

#include "fwRenderOgre/vr/IVolumeRenderer.hpp"

#include "fwRenderOgre/Layer.hpp"

#include <boost/algorithm/clamp.hpp>

namespace fwRenderOgre
{

namespace vr
{

//-----------------------------------------------------------------------------

const IVolumeRenderer::CubeFacePositionsMap IVolumeRenderer::s_cubeFaces = {
    { IVolumeRenderer::Z_POSITIVE, {{ 3, 4, 1, 0 }} },
    { IVolumeRenderer::Z_NEGATIVE, {{ 2, 5, 7, 6 }} },
    { IVolumeRenderer::Y_POSITIVE, {{ 2, 6, 3, 0 }} },
    { IVolumeRenderer::Y_NEGATIVE, {{ 1, 4, 7, 5 }} },
    { IVolumeRenderer::X_POSITIVE, {{ 0, 1, 5, 2 }} },
    { IVolumeRenderer::X_NEGATIVE, {{ 6, 7, 4, 3 }} }
};

/// Image local and texture coordinates /!\ This order matters to our intersection algorithm.
static const ::Ogre::Vector3 s_imagePositions[8] = {
    ::Ogre::Vector3(1, 1, 1),
    ::Ogre::Vector3(1, 0, 1),
    ::Ogre::Vector3(1, 1, 0),
    ::Ogre::Vector3(0, 1, 1),
    ::Ogre::Vector3(0, 0, 1),
    ::Ogre::Vector3(1, 0, 0),
    ::Ogre::Vector3(0, 1, 0),
    ::Ogre::Vector3(0, 0, 0)
};

//-----------------------------------------------------------------------------

const IVolumeRenderer::CubeEdgeList IVolumeRenderer::s_cubeEdges = { {
                                                                         { 0, 1 }, { 1, 4 }, { 4, 3 }, { 3, 0 },
                                                                         { 0, 2 }, { 1, 5 }, { 4, 7 }, { 3, 6 },
                                                                         { 2, 5 }, { 5, 7 }, { 7, 6 }, { 6, 2 }
                                                                     } };

//-----------------------------------------------------------------------------

IVolumeRenderer::IVolumeRenderer(std::string parentId,
                                 ::Ogre::SceneManager* const sceneManager,
                                 ::Ogre::SceneNode* const volumeNode,
                                 ::Ogre::TexturePtr imageTexture,
                                 PreIntegrationTable& preintegrationTable) :
    m_parentId(parentId),
    m_sceneManager(sceneManager),
    m_3DOgreTexture(imageTexture),
    m_preIntegrationTable(preintegrationTable),
    m_volumeSceneNode(volumeNode),
    m_nbSlices(512),
    m_preIntegratedRendering(false)
{
    m_camera = m_sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);

    std::copy(s_imagePositions, s_imagePositions + 8, m_clippedImagePositions);
}

//-----------------------------------------------------------------------------

IVolumeRenderer::~IVolumeRenderer()
{

}

//-----------------------------------------------------------------------------

void IVolumeRenderer::updateVolumeTF()
{

}

//-----------------------------------------------------------------------------

void IVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    const ::Ogre::Vector3 min = clippingBox.getMinimum();
    const ::Ogre::Vector3 max = clippingBox.getMaximum();

    for(unsigned i = 0; i < 8; ++i)
    {
        m_clippedImagePositions[i] = ::Ogre::Vector3(
            ::boost::algorithm::clamp(s_imagePositions[i].x, min.x, max.x),
            ::boost::algorithm::clamp(s_imagePositions[i].y, min.y, max.y),
            ::boost::algorithm::clamp(s_imagePositions[i].z, min.z, max.z));
    }
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::resizeViewport(int /*w*/, int /*h*/)
{

}

//-----------------------------------------------------------------------------

void IVolumeRenderer::scaleTranslateCube(const ::fwData::Image::SpacingType& spacing,
                                         const ::fwData::Image::OriginType& origin)
{
    SLM_ASSERT("Image origin and spacing must be three-dimensional for volume rendering.",
               spacing.size() == 3 && origin.size() == 3);

    m_volumeSceneNode->resetToInitialState();

    const double width  = static_cast< double > (m_3DOgreTexture->getWidth() ) * spacing[0];
    const double height = static_cast< double > (m_3DOgreTexture->getHeight()) * spacing[1];
    const double depth  = static_cast< double > (m_3DOgreTexture->getDepth() ) * spacing[2];

    const ::Ogre::Vector3 scaleFactors(
        static_cast<float>(width ),
        static_cast<float>(height),
        static_cast<float>(depth ));

    const ::Ogre::Vector3 ogreOrigin(
        static_cast<float>(origin[0]),
        static_cast<float>(origin[1]),
        static_cast<float>(origin[2]));

    m_volumeSceneNode->setScale(scaleFactors);
    m_volumeSceneNode->setPosition(ogreOrigin);
}

//-----------------------------------------------------------------------------

::Ogre::Plane IVolumeRenderer::getCameraPlane() const
{
    return ::Ogre::Plane(
        m_volumeSceneNode->convertWorldToLocalDirection(m_camera->getRealDirection(), true).normalisedCopy(),
        m_volumeSceneNode->convertWorldToLocalPosition(m_camera->getRealPosition())
        );
}

//-----------------------------------------------------------------------------

unsigned IVolumeRenderer::computeSampleDistance(const ::Ogre::Plane& cameraPlane)
{
    // get the cube's closest and furthest vertex to the camera
    const auto comp = [&cameraPlane](const ::Ogre::Vector3& v1, const ::Ogre::Vector3& v2)
                      { return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2); };

    const auto closestVtxIterator = std::min_element(m_clippedImagePositions, m_clippedImagePositions + 8, comp);
    const auto closestVtxIndex    = std::distance(m_clippedImagePositions, closestVtxIterator);

    const ::Ogre::Vector3 furthestVtx = *std::max_element(m_clippedImagePositions, m_clippedImagePositions + 8, comp);
    const ::Ogre::Vector3 closestVtx  = *closestVtxIterator;

    // get distance between slices
    const float closestVtxDistance  = cameraPlane.getDistance(closestVtx);
    const float furthestVtxDistance = cameraPlane.getDistance(furthestVtx);

    const float firstToLastSliceDistance = std::abs(closestVtxDistance - furthestVtxDistance);

    m_sampleDistance = firstToLastSliceDistance / m_nbSlices;

    return static_cast<unsigned>(closestVtxIndex);
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
