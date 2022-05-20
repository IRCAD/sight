/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "viz/scene3d/vr/IVolumeRenderer.hpp"

#include "viz/scene3d/Layer.hpp"

#include <boost/algorithm/clamp.hpp>

namespace sight::viz::scene3d
{

namespace vr
{

const std::array<Ogre::Vector3, 8> IVolumeRenderer::s_imagePositions =
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

IVolumeRenderer::IVolumeRenderer(
    const std::string& parentId,
    Ogre::SceneManager* const sceneManager,
    Ogre::SceneNode* const volumeNode,
    std::optional<Ogre::TexturePtr> imageTexture,
    bool with_buffer,
    bool preintegration
) :
    m_parentId(parentId),
    m_sceneManager(sceneManager),
    m_3DOgreTexture(imageTexture.value_or(Ogre::TextureManager::getSingleton().create(
                                              m_parentId + "_Texture",
                                              sight::viz::scene3d::RESOURCE_GROUP,
                                              true
                                          ))),
    m_with_buffer(with_buffer),
    m_preintegration(preintegration),
    m_volumeSceneNode(volumeNode)
{
    m_camera = m_sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);

    m_clippedImagePositions = s_imagePositions;

    //Transfer function and preintegration table
    {
        m_gpuVolumeTF->createTexture(m_parentId + "_VolumeGpuTF");
        m_preIntegrationTable.createTexture(m_parentId);
    }
}

//------------------------------------------------------------------------------

void IVolumeRenderer::update()
{
    this->setSampling(m_nbSlices);
    m_update_pending = false;
}

//-----------------------------------------------------------------------------

IVolumeRenderer::~IVolumeRenderer()
{
    Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture->getHandle());
    m_3DOgreTexture.reset();

    if(m_bufferingTexture)
    {
        Ogre::TextureManager::getSingleton().remove(m_bufferingTexture->getHandle());
        m_bufferingTexture.reset();
    }

    m_preIntegrationTable.removeTexture();
}

//------------------------------------------------------------------------------

void IVolumeRenderer::loadImage(const std::shared_ptr<sight::data::Image>& source)
{
    SIGHT_ASSERT("source cannot be nullptr", source != nullptr);

    if(m_with_buffer)
    {
        if(m_bufferingTexture == nullptr)
        {
            m_bufferingTexture = Ogre::TextureManager::getSingleton().create(
                m_parentId + "_Texture2",
                sight::viz::scene3d::RESOURCE_GROUP,
                true
            );
        }

        Utils::convertImageForNegato(
            m_bufferingTexture.get(),
            source
        );

        // Swap texture pointers.
        {
            std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);
            std::swap(m_3DOgreTexture, m_bufferingTexture);
        }
    }
    else
    {
        sight::viz::scene3d::Utils::convertImageForNegato(m_3DOgreTexture.get(), source);
    }
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::updateVolumeTF(std::shared_ptr<data::TransferFunction>&)
{
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::clipImage(const Ogre::AxisAlignedBox& clippingBox)
{
    const Ogre::Vector3 min = clippingBox.getMinimum();
    const Ogre::Vector3 max = clippingBox.getMaximum();

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

void IVolumeRenderer::resizeViewport(int /*w*/, int /*h*/)
{
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::scaleTranslateCube(
    const data::Image::Spacing& spacing,
    const data::Image::Origin& origin
)
{
    // Scale the volume based on the image's spacing and move it to the image origin.
    m_volumeSceneNode->resetToInitialState();

    const double width  = static_cast<double>(m_3DOgreTexture->getWidth()) * spacing[0];
    const double height = static_cast<double>(m_3DOgreTexture->getHeight()) * spacing[1];
    const double depth  = static_cast<double>(m_3DOgreTexture->getDepth()) * spacing[2];

    const Ogre::Vector3 scaleFactors(
        static_cast<float>(width),
        static_cast<float>(height),
        static_cast<float>(depth));

    const Ogre::Vector3 ogreOrigin(
        static_cast<float>(origin[0]),
        static_cast<float>(origin[1]),
        static_cast<float>(origin[2]));

    m_volumeSceneNode->setScale(scaleFactors);
    m_volumeSceneNode->setPosition(ogreOrigin);
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::updateSampleDistance()
{
    //Update the plane with the current position and direction
    m_cameraInfo.plane = Ogre::Plane(
        m_volumeSceneNode->convertWorldToLocalDirection(m_camera->getRealDirection(), true).normalisedCopy(),
        m_volumeSceneNode->convertWorldToLocalPosition(m_camera->getRealPosition())
    );

    //Compares the distances to the camera plane to get the cube's closest and furthest vertex to the camera
    const auto comp = [this](const Ogre::Vector3& v1, const Ogre::Vector3& v2)
                      {return m_cameraInfo.plane.getDistance(v1) < m_cameraInfo.plane.getDistance(v2);};

    //Closest vertex
    {
        const auto iterator = std::min_element(
            m_clippedImagePositions.begin(),
            m_clippedImagePositions.end(),
            comp
        );
        const std::size_t index = static_cast<std::size_t>(std::distance(m_clippedImagePositions.begin(), iterator));
        m_cameraInfo.closest       = *iterator;
        m_cameraInfo.closest_index = index;
    }

    //Furthest vertex
    {
        const auto iterator = std::max_element(
            m_clippedImagePositions.begin(),
            m_clippedImagePositions.end(),
            comp
        );
        const std::size_t index = static_cast<std::size_t>(std::distance(m_clippedImagePositions.begin(), iterator));

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
    m_sampleDistance = total_distance / m_nbSlices;

    //Validation
    SIGHT_ASSERT("Sampled distance is NaN.", !std::isnan(m_sampleDistance)); //NaN
    SIGHT_ASSERT("Sample distance is denormalized.", m_sampleDistance > std::numeric_limits<float>::min());
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
