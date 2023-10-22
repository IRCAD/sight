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

const std::array<Ogre::Vector3, 8> volume_renderer::s_imagePositions =
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
    M_PARENT_ID(std::move(_parent_id)),
    M_SCENE_MANAGER(_scene_manager),
    m_3DOgreTexture(std::make_shared<sight::viz::scene3d::texture>(_image)),
    m_maskTexture(std::make_shared<sight::viz::scene3d::texture>(_mask)),
    m_gpuVolumeTF(std::make_shared<sight::viz::scene3d::transfer_function>(_tf)),
    M_WITH_BUFFER(_with_buffer),
    m_preintegration(_preintegration),
    m_volumeSceneNode(_volume_node),
    m_camera(M_SCENE_MANAGER->getCamera(viz::scene3d::layer::s_DEFAULT_CAMERA_NAME)),
    m_nbSlices(_samples),
    m_clippedImagePositions(s_imagePositions)
{
    //Transfer function and preintegration table
    {
        m_preIntegrationTable.createTexture(M_PARENT_ID);
    }

    // 3D source texture instantiation

    if(M_WITH_BUFFER)
    {
        // 3D source texture instantiation
        m_bufferingTexture = std::make_shared<sight::viz::scene3d::texture>(_image, "_buffered");
    }
}

//-----------------------------------------------------------------------------

volume_renderer::~volume_renderer()
{
    m_maskTexture.reset();
    m_3DOgreTexture.reset();

    if(m_bufferingTexture)
    {
        m_bufferingTexture.reset();
    }

    m_preIntegrationTable.removeTexture();
}

//------------------------------------------------------------------------------

void volume_renderer::loadImage()
{
    if(M_WITH_BUFFER)
    {
        m_bufferingTexture->update();

        // Swap texture pointers.
        {
            std::lock_guard<std::mutex> swap_lock(m_bufferSwapMutex);
            std::swap(m_3DOgreTexture, m_bufferingTexture);
        }
    }
    else
    {
        m_3DOgreTexture->update();
    }
}

//------------------------------------------------------------------------------

void volume_renderer::loadMask()
{
    m_maskTexture->update();
}

//-----------------------------------------------------------------------------

void volume_renderer::clipImage(const Ogre::AxisAlignedBox& _clipping_box)
{
    const Ogre::Vector3 min = _clipping_box.getMinimum();
    const Ogre::Vector3 max = _clipping_box.getMaximum();

    for(unsigned i = 0 ; i < 8 ; ++i)
    {
        m_clippedImagePositions[i] = Ogre::Vector3(
            boost::algorithm::clamp(s_imagePositions[i].x, min.x, max.x),
            boost::algorithm::clamp(s_imagePositions[i].y, min.y, max.y),
            boost::algorithm::clamp(s_imagePositions[i].z, min.z, max.z)
        );
    }
}

//-----------------------------------------------------------------------------

void volume_renderer::resizeViewport(int /*w*/, int /*h*/)
{
}

//-----------------------------------------------------------------------------

void volume_renderer::scaleTranslateCube(
    const data::image::Spacing& _spacing,
    const data::image::Origin& _origin
)
{
    // Scale the volume based on the image's spacing and move it to the image origin.
    m_volumeSceneNode->resetToInitialState();

    const double width  = static_cast<double>(m_3DOgreTexture->width()) * _spacing[0];
    const double height = static_cast<double>(m_3DOgreTexture->height()) * _spacing[1];
    const double depth  = static_cast<double>(m_3DOgreTexture->depth()) * _spacing[2];

    const Ogre::Vector3 scale_factors(
        static_cast<float>(width),
        static_cast<float>(height),
        static_cast<float>(depth));

    const Ogre::Vector3 ogre_origin(
        static_cast<float>(_origin[0]),
        static_cast<float>(_origin[1]),
        static_cast<float>(_origin[2]));

    m_volumeSceneNode->setScale(scale_factors);
    m_volumeSceneNode->setPosition(ogre_origin);
}

//-----------------------------------------------------------------------------

void volume_renderer::updateSampleDistance()
{
    //Update the plane with the current position and direction
    m_cameraInfo.plane = Ogre::Plane(
        m_volumeSceneNode->convertWorldToLocalDirection(m_camera->getRealDirection(), true).normalisedCopy(),
        m_volumeSceneNode->convertWorldToLocalPosition(m_camera->getRealPosition())
    );

    //Compares the distances to the camera plane to get the cube's closest and furthest vertex to the camera
    const auto comp = [this](const Ogre::Vector3& _v1, const Ogre::Vector3& _v2)
                      {return m_cameraInfo.plane.getDistance(_v1) < m_cameraInfo.plane.getDistance(_v2);};

    //Closest vertex
    {
        const auto iterator = std::min_element( // NOLINT(readability-qualified-auto,llvm-qualified-auto)
            m_clippedImagePositions.begin(),
            m_clippedImagePositions.end(),
            comp
        );
        const auto index = static_cast<std::size_t>(std::distance(m_clippedImagePositions.begin(), iterator));
        m_cameraInfo.closest       = *iterator;
        m_cameraInfo.closest_index = index;
    }

    //Furthest vertex
    {
        const auto iterator = std::max_element( // NOLINT(readability-qualified-auto,llvm-qualified-auto)
            m_clippedImagePositions.begin(),
            m_clippedImagePositions.end(),
            comp
        );
        const auto index = static_cast<std::size_t>(std::distance(m_clippedImagePositions.begin(), iterator));

        m_cameraInfo.furthest       = *iterator;
        m_cameraInfo.furthest_index = index;
    }

    //The total distance between the vertices
    const float total_distance =
        std::abs(
            m_cameraInfo.plane.getDistance(m_cameraInfo.closest)
            - m_cameraInfo.plane.getDistance(m_cameraInfo.furthest)
        );

    //Then simply uniformly divide it according to the total number of slices
    m_sampleDistance = total_distance / static_cast<float>(m_nbSlices);

    //Validation
    SIGHT_ASSERT("Sampled distance is NaN.", !std::isnan(m_sampleDistance)); //NaN
    SIGHT_ASSERT("Sample distance is denormalized.", m_sampleDistance > std::numeric_limits<float>::min());
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
