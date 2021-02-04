/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "SMeshList.hpp"

#include <core/com/Slots.hxx>

#include <data/Mesh.hpp>

namespace sight::modules::viz::ogre
{

static const core::com::Slots::SlotKeyType s_ADD_SLOT   = "add";
static const core::com::Slots::SlotKeyType s_CLEAR_SLOT = "clear";

static const std::string s_TEXTURE_INPUT   = "texture";
static const std::string s_TRANSFORM_INPUT = "transform";
static const std::string s_MESH_INOUT      = "mesh";

static const std::string s_CAPACITY_CONFIG      = "capacity";
static const std::string s_DROPPING_CONFIG      = "drop";
static const std::string s_TEXTURE_ALPHA_CONFIG = "textureAlpha";

//-----------------------------------------------------------------------------

SMeshList::SMeshList() noexcept
{
    newSlot(s_ADD_SLOT, &SMeshList::add, this);
    newSlot(s_CLEAR_SLOT, &SMeshList::clear, this);
}

//-----------------------------------------------------------------------------

SMeshList::~SMeshList() noexcept
{
}

//-----------------------------------------------------------------------------

void SMeshList::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_capacity = config.get(s_CAPACITY_CONFIG, m_capacity);
    m_dropData = config.get(s_DROPPING_CONFIG, m_dropData);
    SLM_ASSERT("Capacity must be > 0", m_capacity > 0);
    SLM_ASSERT("Drop ratio must be > 0", m_dropData > 0);
    m_generateAlpha = config.get(s_TEXTURE_ALPHA_CONFIG, m_generateAlpha);
}

//-----------------------------------------------------------------------------

void SMeshList::starting()
{
    this->initialize();

    // Get the inputs.
    const auto mesh           = this->getWeakInOut< data::Mesh >(s_MESH_INOUT);
    const auto transformInOut = this->getLockedInput< data::TransformationMatrix3D >(s_TRANSFORM_INPUT);
    const auto imageInput     = this->getLockedInput< data::Image >(s_TEXTURE_INPUT);

    // initialise N meshes adaptor
    for (size_t i = 0; i < m_capacity; ++i)
    {
        // Matrix and Image are copied because the input ones will change. Mesh is not copied because we want to use
        // the same mesh of all the adaptors
        const auto transform = data::TransformationMatrix3D::copy(transformInOut.get_shared());
        const auto image     = data::Image::copy(imageInput.get_shared());

        // Create adaptors configurations
        const std::string transformID = this->getID() + transform->getID();
        services::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", m_layerID);
        config.add("config.<xmlattr>." + s_TRANSFORM_INPUT, transformID);
        config.add("config.<xmlattr>.autoresetcamera", "no");

        // Create the transform adaptor.
        const sight::viz::ogre::IAdaptor::sptr transformAdaptor
            = this->registerService< sight::viz::ogre::IAdaptor >("::sight::modules::viz::ogre::adaptor::STransform");

        transformAdaptor->setLayerID(m_layerID);
        transformAdaptor->setRenderService(this->getRenderService());

        transformAdaptor->registerInOut(transform, "transform", true);

        transformAdaptor->configure(config);
        transformAdaptor->start();

        SLM_ASSERT("STransform is not started", transformAdaptor->isStarted());

        // Create the texture adaptor
        const sight::viz::ogre::IAdaptor::sptr textureAdaptor =
            this->registerService< sight::viz::ogre::IAdaptor >("::sight::modules::viz::ogre::adaptor::STexture");

        services::IService::ConfigType textureConfig = config;
        textureConfig.add("config.<xmlattr>.textureName", image->getID());
        textureConfig.add("config.<xmlattr>.useAlpha", "true");

        textureAdaptor->setLayerID(m_layerID);
        textureAdaptor->setRenderService(this->getRenderService());

        textureAdaptor->registerInput(image, "image", true);

        textureAdaptor->configure(textureConfig);
        textureAdaptor->start();
        SLM_ASSERT("STexture is not started", textureAdaptor->isStarted());

        // Creates the mesh adaptor.
        const sight::viz::ogre::IAdaptor::sptr meshAdaptor =
            this->registerService< sight::viz::ogre::IAdaptor >("::sight::modules::viz::ogre::adaptor::SMesh");

        services::IService::ConfigType meshConfig = config;
        meshConfig.add("config.<xmlattr>.textureName", image->getID());

        meshAdaptor->setLayerID(m_layerID);
        meshAdaptor->setRenderService(this->getRenderService());

        meshAdaptor->registerInOut(mesh.lock().get_shared(), "mesh", true);

        meshAdaptor->configure(meshConfig);
        meshAdaptor->updateVisibility(false);
        meshAdaptor->start();
        SLM_ASSERT("SMesh is not started", meshAdaptor->isStarted());

        // Store data.
        MeshInstance instance {transform, image, transformAdaptor, meshAdaptor, textureAdaptor};
        m_meshes.push_back(instance);
    }
}

//-----------------------------------------------------------------------------

services::IService::KeyConnectionsMap SMeshList::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INPUT, data::TransformationMatrix3D::s_MODIFIED_SIG, s_ADD_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void SMeshList::updating()
{
}

//-----------------------------------------------------------------------------

void SMeshList::stopping()
{
    for (const auto& instance: m_meshes)
    {
        this->unregisterService(instance.m_transform);
        this->unregisterService(instance.m_mesh);
        this->unregisterService(instance.m_texture);
    }
    m_meshes.clear();
    m_meshCount = 0;
}

//-----------------------------------------------------------------------------

void SMeshList::setVisible(bool _visible)
{
    for (const auto& instance: m_meshes)
    {
        if (instance.m_isEnabled)
        {
            const sight::viz::ogre::IAdaptor::sptr mesh = instance.m_mesh;
            mesh->updateVisibility(_visible);
        }
    }
}

//-----------------------------------------------------------------------------

void SMeshList::add()
{
    if (m_dropCount % m_dropData == 0)
    {
        // Reset the drop count.
        m_dropCount = 0;

        auto& instance = m_meshes.at(m_meshCount);
        ++m_meshCount;

        if (m_meshCount == m_capacity)
        {
            m_meshCount = 0;
        }

        instance.m_isEnabled = true;

        // update the transform
        const sight::viz::ogre::IAdaptor::sptr textureAdp = instance.m_texture;
        {
            // set current image
            const auto image = textureAdp->getLockedInput< data::Image >("image");

            const auto textureInput = this->getLockedInput< data::Image >(s_TEXTURE_INPUT);

            if (m_generateAlpha && textureInput->getType() == core::tools::Type::s_UINT8 &&
                (textureInput->getPixelFormat() == data::Image::PixelFormat::GRAY_SCALE ||
                 textureInput->getNumberOfComponents() == 1))
            {
                // transform the image into RGBA with a transparent texture

                if (textureInput->getAllocatedSizeInBytes()*4 != instance.m_image->getAllocatedSizeInBytes())
                {
                    instance.m_image->copyInformation(textureInput.get_shared());
                    instance.m_image->setPixelFormat(data::Image::PixelFormat::RGBA);
                    instance.m_image->setNumberOfComponents(4);
                    instance.m_image->resize();
                }

                auto inItr = textureInput->begin< std::uint8_t >();
                auto inEnd = textureInput->end< std::uint8_t >();
                auto itr   = instance.m_image->begin< data::iterator::RGBA >();

                for (; inItr != inEnd; ++inItr, ++itr)
                {
                    itr->r = *inItr;
                    itr->g = *inItr;
                    itr->b = *inItr;
                    itr->a = *inItr;
                }
            }
            else if (m_generateAlpha && textureInput->getType() == core::tools::Type::s_UINT8 &&
                     (textureInput->getPixelFormat() == data::Image::PixelFormat::RGB ||
                      textureInput->getNumberOfComponents() == 3))
            {
                // transform the image into RGBA with a transparent texture

                if (textureInput->getAllocatedSizeInBytes()*4/3 != instance.m_image->getAllocatedSizeInBytes())
                {
                    instance.m_image->copyInformation(textureInput.get_shared());
                    instance.m_image->setPixelFormat(data::Image::PixelFormat::RGBA);
                    instance.m_image->setNumberOfComponents(4);
                    instance.m_image->resize();
                }

                auto inItr = textureInput->begin< data::iterator::RGB >();
                auto inEnd = textureInput->end< data::iterator::RGB >();
                auto itr   = instance.m_image->begin< data::iterator::RGBA >();

                for (; inItr != inEnd; ++inItr, ++itr)
                {
                    itr->r = inItr->r;
                    itr->g = inItr->g;
                    itr->b = inItr->b;
                    // luminance
                    itr->a = static_cast< std::uint8_t>(0.2126*inItr->r + 0.7152*inItr->g + 0.0722*inItr->b);
                }
            }
            else
            {
                instance.m_image->deepCopy(textureInput.get_shared());
            }
        }
        textureAdp->update();

        // update the texture
        const sight::viz::ogre::IAdaptor::sptr transformAdp = instance.m_transform;
        {
            // set current matrix
            const auto transform = transformAdp->getLockedInOut< data::TransformationMatrix3D >("transform");

            const auto transformInOut = this->getLockedInput< data::TransformationMatrix3D >(s_TRANSFORM_INPUT);
            transform->deepCopy(transformInOut.get_shared());

        }
        transformAdp->update();

        // update mesh adaptor visibility
        const sight::viz::ogre::IAdaptor::sptr meshAdp = instance.m_mesh;
        meshAdp->updateVisibility(m_isVisible);
    }
    ++m_dropCount;
}

//-----------------------------------------------------------------------------

void SMeshList::clear()
{
    this->setVisible(false);
    for (auto& instance: m_meshes)
    {
        instance.m_isEnabled = false;
    }
    m_meshCount = 0;
}

} // namespace sight::modules::viz::ogre.
