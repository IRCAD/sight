/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "data/Image.hpp"
#include "data/TransferFunction.hpp"

#include "viz/ogre/config.hpp"
#include "viz/ogre/TransferFunction.hpp"
#include "viz/ogre/vr/PreIntegrationTable.hpp"

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

namespace sight::viz::ogre
{

namespace vr
{

/**
 * @brief Interface for volume renderers.
 */
class VIZ_OGRE_CLASS_API IVolumeRenderer
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
    VIZ_OGRE_API static const CubeFacePositionsMap s_cubeFaces;

    /// List of vertex indices pairs that make an edge.
    VIZ_OGRE_API static const CubeEdgeList s_cubeEdges;

    /**
     * @brief Constructor.
     * @param parentId             ID of the service using this renderer
     * @param sceneManager         The scene manager being used.
     * @param volumeNode           This object's node.
     * @param imageTexture         Texture holding the 3D image to be rendered.
     * @param preintegrationTable  Texture holding the pre-integration table.
     */
    VIZ_OGRE_API IVolumeRenderer(std::string parentId,
                                 ::Ogre::SceneManager* const sceneManager,
                                 ::Ogre::SceneNode* const volumeNode,
                                 ::Ogre::TexturePtr imageTexture,
                                 PreIntegrationTable& preintegrationTable);

    /// Destructor, does nothing.
    VIZ_OGRE_API virtual ~IVolumeRenderer();

    /// Called when the image being rendered is modified.
    VIZ_OGRE_API virtual void imageUpdate(data::Image::sptr image, data::TransferFunction::sptr tf) = 0;

    /// Called when the transfer function is updated.
    VIZ_OGRE_API virtual void updateVolumeTF();

    /// Sets the number of samples per view ray.
    VIZ_OGRE_API virtual void setSampling(uint16_t nbSamples) = 0;

    /// Sets/unsets pre-integrated rendering.
    VIZ_OGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering) = 0;

    /// Computes image positions.
    VIZ_OGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

    /// Returns the sampling rate.
    VIZ_OGRE_API float getSamplingRate() const;

    /// Called when the size of the viewport changes.
    VIZ_OGRE_API virtual void resizeViewport(int w, int h);

protected:

    /// Scale the volume based on the image's spacing and move it to the image origin.
    /// @deprecated Use scaleTranslateCube(const data::Image::Spacing&, const data::Image::Origin&). It will be
    /// removed in sight 22.0
    [[deprecated("It will be remove in sight 22.0")]]
    VIZ_OGRE_API void scaleTranslateCube(const data::Image::SpacingType& spacing,
                                         const data::Image::OriginType& origin);

    /// Scale the volume based on the image's spacing and move it to the image origin.
    VIZ_OGRE_API void scaleTranslateCube(const data::Image::Spacing& spacing,
                                         const data::Image::Origin& origin);

    /// Computes the camera's plane.
    VIZ_OGRE_API ::Ogre::Plane getCameraPlane() const;

    /// Computes the sample distance and returns the index to the closest vertex to the camera.
    VIZ_OGRE_API unsigned computeSampleDistance(const ::Ogre::Plane& cameraPlane);

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

} // namespace sight::viz::ogre
