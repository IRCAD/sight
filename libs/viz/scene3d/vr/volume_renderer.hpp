/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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
#include "viz/scene3d/utils.hpp"
#include "viz/scene3d/vr/pre_integration_table.hpp"

#include <viz/scene3d/texture.hpp>

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
    enum cube_face
    {
        x_negative = 0,
        x_positive = 1,
        y_negative = 2,
        y_positive = 3,
        z_negative = 4,
        z_positive = 5
    };

    /// Array of 4 vertex indices.
    using cube_face_position_list_t = std::array<unsigned int, 4>;

    /// Maps a face name to an array of 4 vertex indices.
    using cube_face_positions_map_t = std::map<cube_face, cube_face_position_list_t>;

    /// Lists vertex indices pairs that form edges.
    using cube_edge_list_t = std::array<std::pair<unsigned int, unsigned int>, 12>;

    /// Maps each cube faces to 4 vertex indices.
    VIZ_SCENE3D_API static inline const cube_face_positions_map_t CUBE_FACES =
    {
        {volume_renderer::z_positive, {{3, 4, 1, 0}}},
        {volume_renderer::z_negative, {{2, 5, 7, 6}}},
        {volume_renderer::y_positive, {{2, 6, 3, 0}}},
        {volume_renderer::y_negative, {{1, 4, 7, 5}}},
        {volume_renderer::x_positive, {{0, 1, 5, 2}}},
        {volume_renderer::x_negative, {{6, 7, 4, 3}}}
    };

    /// Image local and texture coordinates /!\ The order matters to our intersection algorithm.
    VIZ_SCENE3D_API static const std::array<Ogre::Vector3, 8> IMAGE_POSITIONS;

    /// List of vertex indices pairs that make an edge.
    VIZ_SCENE3D_API static constexpr cube_edge_list_t CUBE_EDGES =
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
     * @param _parent_id                  ID of the service using this renderer
     * @param _scene_manager              The scene manager being used.
     * @param _volume_node                This object's node.
     * @param _with_buffer (optional)     Enable buffering for the textures updates. Default is false.
     * @param _preintegration (optional)  Enable preintegration. Default is false.
     */
    VIZ_SCENE3D_API volume_renderer(
        std::string _parent_id,
        Ogre::SceneManager* _scene_manager,
        Ogre::SceneNode* _volume_node,
        sight::data::image::csptr _image,
        sight::data::image::csptr _mask,
        sight::data::transfer_function::csptr _tf,
        std::uint16_t _samples,
        bool _with_buffer    = false,
        bool _preintegration = false
    );

    /// Destructor, does nothing.
    VIZ_SCENE3D_API virtual ~volume_renderer();

    ///@brief Update the renderer. Base implementation only updates the samples.
    VIZ_SCENE3D_API virtual void update(const data::transfer_function::csptr& _tf) = 0;

    /// Called when the image being rendered is modified.
    VIZ_SCENE3D_API virtual void update_image(data::image::csptr _image, data::transfer_function::csptr _tf) = 0;

    /// @brief Loads the 3D texture onto the GPU.
    VIZ_SCENE3D_API virtual void load_image();

    /// @brief Loads the mask onto the GPU.
    VIZ_SCENE3D_API virtual void load_mask();

    /// Called when the transfer function is updated.
    VIZ_SCENE3D_API virtual void update_volume_tf(const data::transfer_function::csptr&) = 0;

    /// Sets the number of samples per view ray.
    VIZ_SCENE3D_API virtual void set_sampling(uint16_t _nb_samples, const data::transfer_function::csptr& _tf) = 0;

    /// Sets/unsets pre-integrated rendering.
    VIZ_SCENE3D_API virtual void set_pre_integrated_rendering(bool _pre_integrated_rendering) = 0;

    ///@brief Returns 'true' if preintegration is used, 'false' otherwise.
    [[nodiscard]] VIZ_SCENE3D_API bool preintegration() const;

    /// Computes image positions.
    VIZ_SCENE3D_API virtual void clip_image(const Ogre::AxisAlignedBox& _clipping_box);

    /// Returns the sampling rate.
    [[nodiscard]] VIZ_SCENE3D_API float sampling_distance() const;

    ///@brief Returns the current camera information in use.
    [[nodiscard]] VIZ_SCENE3D_API const camera_info_t& camera_info() const;

    /// Called when the size of the viewport changes.
    VIZ_SCENE3D_API virtual void resize_viewport(int _w, int _h);

protected:

    /// Scale the volume based on the image's spacing and move it to the image origin.
    VIZ_SCENE3D_API void scale_translate_cube(
        const data::image::spacing_t& _spacing,
        const data::image::origin_t& _origin
    );

    /// Updates the sampling distance according to the current camera plane and slice number. Also updates
    /// m_cameraPlane.
    VIZ_SCENE3D_API void update_sample_distance();

    /// ID of this object's parent.
    const std::string m_parent_id;

    /// This object's scene manager.
    Ogre::SceneManager* const m_scene_manager;

    /// 3D Image texture.
    texture::sptr m_3d_ogre_texture;

    /// texture used for the mask.
    texture::sptr m_mask_texture;

    /// TF texture used for rendering.
    transfer_function::sptr m_gpu_volume_tf;

    /// Contains the buffering texture for the 3D image.
    texture::sptr m_buffering_texture;

    /// Prevents from accessing the textures while they are swapped.
    std::mutex m_buffer_swap_mutex;

    ///@brief Indicates if a intermediate buffer is used when converting to negato.
    const bool m_with_buffer;

    /// Pre-integration table.
    pre_integration_table m_pre_integration_table;

    /// Use pre-integration.
    bool m_preintegration = false;

    /// This object's scene node.
    Ogre::SceneNode* m_volume_scene_node;

    /// Camera used for rendering.
    Ogre::Camera* m_camera;

    ///@brief Camera information
    camera_info_t m_camera_info;

    /// Sampling rate.
    std::uint16_t m_nb_slices = 512;

    /// Distance between samples in local space.
    float m_sample_distance {};

    /// Intersection between the image and the clipping box.
    std::array<Ogre::Vector3, 8> m_clipped_image_positions;
};

//-----------------------------------------------------------------------------

inline float volume_renderer::sampling_distance() const
{
    return m_sample_distance;
}

//------------------------------------------------------------------------------

inline bool volume_renderer::preintegration() const
{
    return m_preintegration;
}

//------------------------------------------------------------------------------

inline auto volume_renderer::camera_info() const -> const camera_info_t&
{
    return m_camera_info;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
