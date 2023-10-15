/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "data/image.hpp"
#include "data/transfer_function.hpp"

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/transfer_function.hpp"
#include "viz/scene3d/Utils.hpp"
#include "viz/scene3d/vr/pre_integration_table.hpp"

#include <viz/scene3d/Texture.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <optional>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Interface for volume renderers.
 */
class VIZ_SCENE3D_CLASS_API volume_renderer
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
    typedef std::array<unsigned, 4> CubeFacePositionList;

    /// Maps a face name to an array of 4 vertex indices.
    typedef std::map<CubeFace, CubeFacePositionList> CubeFacePositionsMap;

    /// Lists vertex indices pairs that form edges.
    typedef std::array<std::pair<unsigned, unsigned>, 12> CubeEdgeList;

    /// Maps each cube faces to 4 vertex indices.
    VIZ_SCENE3D_API static inline const CubeFacePositionsMap s_cubeFaces =
    {
        {volume_renderer::Z_POSITIVE, {{3, 4, 1, 0}}},
        {volume_renderer::Z_NEGATIVE, {{2, 5, 7, 6}}},
        {volume_renderer::Y_POSITIVE, {{2, 6, 3, 0}}},
        {volume_renderer::Y_NEGATIVE, {{1, 4, 7, 5}}},
        {volume_renderer::X_POSITIVE, {{0, 1, 5, 2}}},
        {volume_renderer::X_NEGATIVE, {{6, 7, 4, 3}}}
    };

    /// Image local and texture coordinates /!\ The order matters to our intersection algorithm.
    VIZ_SCENE3D_API static const std::array<Ogre::Vector3, 8> s_imagePositions;

    /// List of vertex indices pairs that make an edge.
    VIZ_SCENE3D_API static constexpr CubeEdgeList s_cubeEdges =
    {
        {
            {0, 1}, {1, 4}, {4, 3}, {3, 0},
            {0, 2}, {1, 5}, {4, 7}, {3, 6},
            {2, 5}, {5, 7}, {7, 6}, {6, 2}
        }
    };

    ///@brief Internal wrapper holding camera information.
    struct camera_info_t
    {
        /// Camera plane
        Ogre::Plane plane {};

        /// Closest vertex
        Ogre::Vector3 closest {};

        /// Furthest vertex
        Ogre::Vector3 furthest {};

        /// Closest vertex index
        std::size_t closest_index {};

        /// Furthest vertex index
        std::size_t furthest_index {};

        /// Distance between the vertices
        float distance {};
    };

    /**
     * @brief Constructor.
     * @param parentId                  ID of the service using this renderer
     * @param sceneManager              The scene manager being used.
     * @param volumeNode                This object's node.
     * @param buffer (optional)         Enable buffering for the textures updates. Default is false.
     * @param preintegration (optional) Enable preintegration. Default is false.
     */
    VIZ_SCENE3D_API volume_renderer(
        std::string parentId,
        Ogre::SceneManager* sceneManager,
        Ogre::SceneNode* volumeNode,
        sight::data::image::csptr image,
        sight::data::image::csptr mask,
        sight::data::transfer_function::csptr tf,
        std::uint16_t samples,
        bool with_buffer    = false,
        bool preintegration = false
    );

    /// Destructor, does nothing.
    VIZ_SCENE3D_API virtual ~volume_renderer();

    ///@brief Update the renderer. Base implementation only updates the samples.
    VIZ_SCENE3D_API virtual void update(const data::transfer_function::csptr& tf) = 0;

    /// Called when the image being rendered is modified.
    VIZ_SCENE3D_API virtual void updateImage(data::image::csptr image, data::transfer_function::csptr tf) = 0;

    /// @brief Loads the 3D texture onto the GPU.
    VIZ_SCENE3D_API virtual void loadImage();

    /// @brief Loads the mask onto the GPU.
    VIZ_SCENE3D_API virtual void loadMask();

    /// Called when the transfer function is updated.
    VIZ_SCENE3D_API virtual void updateVolumeTF(const data::transfer_function::csptr&) = 0;

    /// Sets the number of samples per view ray.
    VIZ_SCENE3D_API virtual void setSampling(uint16_t nbSamples, const data::transfer_function::csptr& tf) = 0;

    /// Sets/unsets pre-integrated rendering.
    VIZ_SCENE3D_API virtual void setPreIntegratedRendering(bool preIntegratedRendering) = 0;

    ///@brief Returns 'true' if preintegration is used, 'false' otherwise.
    [[nodiscard]] VIZ_SCENE3D_API bool preintegration() const;

    /// Computes image positions.
    VIZ_SCENE3D_API virtual void clipImage(const Ogre::AxisAlignedBox& clippingBox);

    /// Returns the sampling rate.
    [[nodiscard]] VIZ_SCENE3D_API float samplingDistance() const;

    ///@brief Returns the current camera information in use.
    [[nodiscard]] VIZ_SCENE3D_API const camera_info_t& cameraInfo() const;

    /// Called when the size of the viewport changes.
    VIZ_SCENE3D_API virtual void resizeViewport(int w, int h);

protected:

    /// Scale the volume based on the image's spacing and move it to the image origin.
    VIZ_SCENE3D_API void scaleTranslateCube(
        const data::image::Spacing& spacing,
        const data::image::Origin& origin
    );

    /// Updates the sampling distance according to the current camera plane and slice number. Also updates
    /// m_cameraPlane.
    VIZ_SCENE3D_API void updateSampleDistance();

    /// ID of this object's parent.
    const std::string m_parentId;

    /// This object's scene manager.
    Ogre::SceneManager* const m_sceneManager;

    /// 3D Image texture.
    Texture::sptr m_3DOgreTexture;

    /// Texture used for the mask.
    Texture::sptr m_maskTexture;

    /// TF texture used for rendering.
    transfer_function::sptr m_gpuVolumeTF;

    /// Contains the buffering texture for the 3D image.
    Texture::sptr m_bufferingTexture;

    /// Prevents from accessing the textures while they are swapped.
    std::mutex m_bufferSwapMutex;

    ///@brief Indicates if a intermediate buffer is used when converting to negato.
    const bool m_with_buffer;

    /// Pre-integration table.
    pre_integration_table m_preIntegrationTable;

    /// Use pre-integration.
    bool m_preintegration = false;

    /// This object's scene node.
    Ogre::SceneNode* m_volumeSceneNode;

    /// Camera used for rendering.
    Ogre::Camera* m_camera;

    ///@brief Camera information
    camera_info_t m_cameraInfo;

    /// Sampling rate.
    std::uint16_t m_nbSlices = 512;

    /// Distance between samples in local space.
    float m_sampleDistance {};

    /// Intersection between the image and the clipping box.
    std::array<Ogre::Vector3, 8> m_clippedImagePositions;
};

//-----------------------------------------------------------------------------

inline float volume_renderer::samplingDistance() const
{
    return m_sampleDistance;
}

//------------------------------------------------------------------------------

inline bool volume_renderer::preintegration() const
{
    return m_preintegration;
}

//------------------------------------------------------------------------------

inline auto volume_renderer::cameraInfo() const -> const camera_info_t&
{
    return m_cameraInfo;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
