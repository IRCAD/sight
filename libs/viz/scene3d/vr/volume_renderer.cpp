/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

// cspell:ignore NOLINT

#include "viz/scene3d/vr/volume_renderer.hpp"

#include "viz/scene3d/layer.hpp"

#include <boost/algorithm/clamp.hpp>

#include <utility>

namespace sight::viz::scene3d::vr
{

const std::array<Ogre::Vector3, 8> volume_renderer::IMAGE_POSITIONS =
{
    Ogre::Vector3(1, 1, 1),
    Ogre::Vector3(1, 0, 1),
    Ogre::Vector3(1, 1, 0),
    Ogre::Vector3(0, 1, 1),
    Ogre::Vector3(0, 0, 1),
    Ogre::Vector3(1, 0, 0),
    Ogre::Vector3(0, 1, 0),
    Ogre::Vector3(0, 0, 0)
};

//-----------------------------------------------------------------------------

volume_renderer::volume_renderer(
    std::string _parent_id,
    Ogre::SceneManager* const _scene_manager,
    Ogre::SceneNode* const _volume_node,
    sight::data::image::csptr _image,
    sight::data::image::csptr _mask,
    sight::data::transfer_function::csptr _tf,
    std::uint16_t _samples,
    bool _with_buffer,
    bool _preintegration
) :
    m_parent_id(std::move(_parent_id)),
    m_scene_manager(_scene_manager),
    m_3d_ogre_texture(std::make_shared<sight::viz::scene3d::texture>(_image)),
    m_mask_texture(std::make_shared<sight::viz::scene3d::texture>(_mask)),
    m_gpu_volume_tf(std::make_shared<sight::viz::scene3d::transfer_function>(_tf)),
    m_with_buffer(_with_buffer),
    m_preintegration(_preintegration),
    m_volume_scene_node(_volume_node),
    m_camera(m_scene_manager->getCamera(viz::scene3d::layer::DEFAULT_CAMERA_NAME)),
    m_nb_slices(_samples),
    m_clipped_image_positions(IMAGE_POSITIONS)
{
    //Transfer function and preintegration table
    {
        m_pre_integration_table.create_texture(m_parent_id);
    }

    // 3D source texture instantiation

    if(m_with_buffer)
    {
        // 3D source texture instantiation
        m_buffering_texture = std::make_shared<sight::viz::scene3d::texture>(_image, "_buffered");
    }
}

//-----------------------------------------------------------------------------

volume_renderer::~volume_renderer()
{
    m_mask_texture.reset();
    m_3d_ogre_texture.reset();

    if(m_buffering_texture)
    {
        m_buffering_texture.reset();
    }

    m_pre_integration_table.remove_texture();
}

//------------------------------------------------------------------------------

void volume_renderer::load_image()
{
    if(m_with_buffer)
    {
        m_buffering_texture->update();

        // Swap texture pointers.
        {
            std::lock_guard<std::mutex> swap_lock(m_buffer_swap_mutex);
            std::swap(m_3d_ogre_texture, m_buffering_texture);
        }
    }
    else
    {
        m_3d_ogre_texture->update();
    }
}

//------------------------------------------------------------------------------

void volume_renderer::load_mask()
{
    m_mask_texture->update();
}

//-----------------------------------------------------------------------------

void volume_renderer::clip_image(const Ogre::AxisAlignedBox& _clipping_box)
{
    const Ogre::Vector3 min = _clipping_box.getMinimum();
    const Ogre::Vector3 max = _clipping_box.getMaximum();

    for(unsigned i = 0 ; i < 8 ; ++i)
    {
        m_clipped_image_positions[i] = Ogre::Vector3(
            boost::algorithm::clamp(IMAGE_POSITIONS[i].x, min.x, max.x),
            boost::algorithm::clamp(IMAGE_POSITIONS[i].y, min.y, max.y),
            boost::algorithm::clamp(IMAGE_POSITIONS[i].z, min.z, max.z)
        );
    }
}

//-----------------------------------------------------------------------------

void volume_renderer::resize_viewport(int /*w*/, int /*h*/)
{
}

//-----------------------------------------------------------------------------

void volume_renderer::scale_translate_cube(
    const data::image::spacing_t& _spacing,
    const data::image::origin_t& _origin
)
{
    // Scale the volume based on the image's spacing and move it to the image origin.
    m_volume_scene_node->resetToInitialState();

    const double width  = static_cast<double>(m_3d_ogre_texture->width()) * _spacing[0];
    const double height = static_cast<double>(m_3d_ogre_texture->height()) * _spacing[1];
    const double depth  = static_cast<double>(m_3d_ogre_texture->depth()) * _spacing[2];

    const Ogre::Vector3 scale_factors(
        static_cast<float>(width),
        static_cast<float>(height),
        static_cast<float>(depth));

    const Ogre::Vector3 ogre_origin(
        static_cast<float>(_origin[0]),
        static_cast<float>(_origin[1]),
        static_cast<float>(_origin[2]));

    m_volume_scene_node->setScale(scale_factors);
    m_volume_scene_node->setPosition(ogre_origin);
}

//-----------------------------------------------------------------------------

void volume_renderer::update_sample_distance()
{
    //Update the plane with the current position and direction
    m_camera_info.plane = Ogre::Plane(
        m_volume_scene_node->convertWorldToLocalDirection(m_camera->getRealDirection(), true).normalisedCopy(),
        m_volume_scene_node->convertWorldToLocalPosition(m_camera->getRealPosition())
    );

    //Compares the distances to the camera plane to get the cube's closest and furthest vertex to the camera
    const auto comp = [this](const Ogre::Vector3& _v1, const Ogre::Vector3& _v2)
                      {return m_camera_info.plane.getDistance(_v1) < m_camera_info.plane.getDistance(_v2);};

    //Closest vertex
    {
        const auto iterator = std::min_element( // NOLINT(readability-qualified-auto,llvm-qualified-auto)
            m_clipped_image_positions.begin(),
            m_clipped_image_positions.end(),
            comp
        );
        const auto index = static_cast<std::size_t>(std::distance(m_clipped_image_positions.begin(), iterator));
        m_camera_info.closest       = *iterator;
        m_camera_info.closest_index = index;
    }

    //Furthest vertex
    {
        const auto iterator = std::max_element( // NOLINT(readability-qualified-auto,llvm-qualified-auto)
            m_clipped_image_positions.begin(),
            m_clipped_image_positions.end(),
            comp
        );
        const auto index = static_cast<std::size_t>(std::distance(m_clipped_image_positions.begin(), iterator));

        m_camera_info.furthest       = *iterator;
        m_camera_info.furthest_index = index;
    }

    //The total distance between the vertices
    const float total_distance =
        std::abs(
            m_camera_info.plane.getDistance(m_camera_info.closest)
            - m_camera_info.plane.getDistance(m_camera_info.furthest)
        );

    //Then simply uniformly divide it according to the total number of slices
    m_sample_distance = total_distance / static_cast<float>(m_nb_slices);

    //Validation
    SIGHT_ASSERT("Sampled distance is NaN.", !std::isnan(m_sample_distance)); //NaN
    SIGHT_ASSERT("Sample distance is denormalized.", m_sample_distance > std::numeric_limits<float>::min());
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
