/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "fwData/Image.hpp"
#include "fwData/TransferFunction.hpp"

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/TransferFunction.hpp"
#include "fwRenderOgre/vr/PreIntegrationTable.hpp"

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace vr
{

/**
 * @brief Interface for volume renderers.
 */
class FWRENDEROGRE_CLASS_API IVolumeRenderer
{
public:

    /// Image cube faces.
    enum CubeFace
    {
        X_NEGATIVE = 0,
        X_POSITIVE = 1,
        Y_NEGATIVE = 2,
        Y_POSITIVE = 3,
        Z_NEGATIVE = 4,
        Z_POSITIVE = 5
    };

    /// Array of 4 vertex indices.
    typedef std::array< unsigned, 4 > CubeFacePositionList;

    /// Maps a face name to an array of 4 vertex indices.
    typedef std::map< CubeFace, CubeFacePositionList > CubeFacePositionsMap;

    /// Lists vertex indices pairs that form edges.
    typedef std::array< std::pair<unsigned, unsigned>, 12 > CubeEdgeList;

    /// Maps each cube faces to 4 vertex indices.
    FWRENDEROGRE_API static const CubeFacePositionsMap s_cubeFaces;

    /// List of vertex indices pairs that make an edge.
    FWRENDEROGRE_API static const CubeEdgeList s_cubeEdges;

    /**
     * @brief Constructor.
     * @param parentId             ID of the service using this renderer
     * @param sceneManager         The scene manager being used.
     * @param volumeNode           This object's node.
     * @param imageTexture         Texture holding the 3D image to be rendered.
     * @param preintegrationTable  Texture holding the pre-integration table.
     */
    FWRENDEROGRE_API IVolumeRenderer(std::string parentId,
                                     ::Ogre::SceneManager* const sceneManager,
                                     ::Ogre::SceneNode* const volumeNode,
                                     ::Ogre::TexturePtr imageTexture,
                                     PreIntegrationTable& preintegrationTable);

    /// Destructor, does nothing.
    FWRENDEROGRE_API virtual ~IVolumeRenderer();

    /// Called when the image being rendered is modified.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf) = 0;

    /// Called when the transfer function is updated.
    FWRENDEROGRE_API virtual void updateVolumeTF();

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples) = 0;

    /// Sets/unsets pre-integrated rendering.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering) = 0;

    /// Computes image positions.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

    /// Returns the sampling rate.
    FWRENDEROGRE_API float getSamplingRate() const;

    /// Called when the size of the viewport changes.
    FWRENDEROGRE_API virtual void resizeViewport(int w, int h);

protected:

    /// Scale the volume based on the image's spacing and move it to the image origin.
    FWRENDEROGRE_API void scaleTranslateCube(const ::fwData::Image::SpacingType& spacing,
                                             const ::fwData::Image::OriginType& origin);

    /// Computes the camera's plane.
    FWRENDEROGRE_API ::Ogre::Plane getCameraPlane() const;

    /// Computes the sample distance and returns the index to the closest vertex to the camera.
    FWRENDEROGRE_API unsigned computeSampleDistance(const ::Ogre::Plane& cameraPlane);

    /// ID of this object's parent.
    std::string m_parentId;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager;

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Pre-integration table.
    PreIntegrationTable& m_preIntegrationTable;

    /// This object's scene node.
    ::Ogre::SceneNode* m_volumeSceneNode;

    /// Camera used for rendering.
    ::Ogre::Camera* m_camera;

    /// Sampling rate.
    uint16_t m_nbSlices;

    /// Distance between samples in local space.
    float m_sampleDistance;

    /// Use pre-integration.
    bool m_preIntegratedRendering;

    /// Intersection between the image and the clipping box.
    ::Ogre::Vector3 m_clippedImagePositions[8];

};

//-----------------------------------------------------------------------------

inline float IVolumeRenderer::getSamplingRate() const
{
    return m_sampleDistance;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
