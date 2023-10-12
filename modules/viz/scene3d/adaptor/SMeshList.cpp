/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <core/com/slots.hxx>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t ADD_SLOT   = "add";
static const core::com::slots::key_t CLEAR_SLOT = "clear";

//-----------------------------------------------------------------------------

SMeshList::SMeshList() noexcept
{
    new_slot(ADD_SLOT, &SMeshList::add, this);
    new_slot(CLEAR_SLOT, &SMeshList::clear, this);
}

//-----------------------------------------------------------------------------

SMeshList::~SMeshList() noexcept =
    default;

//-----------------------------------------------------------------------------

void SMeshList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    static const std::string s_CAPACITY_CONFIG      = s_CONFIG + "capacity";
    static const std::string s_DROPPING_CONFIG      = s_CONFIG + "drop";
    static const std::string s_TEXTURE_ALPHA_CONFIG = s_CONFIG + "textureAlpha";

    m_capacity = config.get(s_CAPACITY_CONFIG, m_capacity);
    m_dropData = config.get(s_DROPPING_CONFIG, m_dropData);
    SIGHT_ASSERT("Capacity must be > 0", m_capacity > 0);
    SIGHT_ASSERT("Drop ratio must be > 0", m_dropData > 0);
    m_generateAlpha = config.get(s_TEXTURE_ALPHA_CONFIG, m_generateAlpha);
}

//-----------------------------------------------------------------------------

void SMeshList::starting()
{
    this->initialize();

    // Get the inputs.
    const auto transformInOut = m_transform.lock();
    const auto imageInput     = m_texture.lock();

    // initialise N meshes adaptor
    for(std::size_t i = 0 ; i < m_capacity ; ++i)
    {
        // Matrix and Image are copied because the input ones will change. Mesh is not copied because we want to use
        // the same mesh of all the adaptors
        const auto transform = data::Matrix4::copy(transformInOut.get_shared());
        const auto image     = data::Image::copy(imageInput.get_shared());

        // Create adaptors configurations
        const std::string transformID = this->get_id() + transform->get_id();
        service::config_t config;
        config.add("config.<xmlattr>.layer", m_layerID);
        config.add("config.<xmlattr>." + std::string(s_TRANSFORM_INPUT), transformID);
        config.add("config.<xmlattr>.autoresetcamera", "false");

        // Create the transform adaptor.
        const sight::viz::scene3d::adaptor::sptr transformAdaptor =
            this->registerService<sight::viz::scene3d::adaptor>(
                "sight::module::viz::scene3d::adaptor::STransform"
            );

        transformAdaptor->setLayerID(m_layerID);
        transformAdaptor->setRenderService(this->getRenderService());

        transformAdaptor->setInOut(transform, "transform", true);

        transformAdaptor->configure(config);
        transformAdaptor->start();

        SIGHT_ASSERT("STransform is not started", transformAdaptor->isStarted());

        // Create the texture adaptor
        const sight::viz::scene3d::adaptor::sptr textureAdaptor =
            this->registerService<sight::viz::scene3d::adaptor>("sight::module::viz::scene3d::adaptor::STexture");

        service::config_t textureConfig = config;
        textureConfig.add("config.<xmlattr>.textureName", image->get_id());
        textureConfig.add("config.<xmlattr>.useAlpha", "true");

        textureAdaptor->setLayerID(m_layerID);
        textureAdaptor->setRenderService(this->getRenderService());

        textureAdaptor->setInput(image, "image", false);

        textureAdaptor->configure(textureConfig);
        textureAdaptor->start();
        SIGHT_ASSERT("STexture is not started", textureAdaptor->isStarted());

        // Creates the mesh adaptor.
        const sight::viz::scene3d::adaptor::sptr meshAdaptor =
            this->registerService<sight::viz::scene3d::adaptor>("sight::module::viz::scene3d::adaptor::SMesh");

        service::config_t meshConfig = config;
        meshConfig.add("config.<xmlattr>.textureName", image->get_id());

        meshAdaptor->setLayerID(m_layerID);
        meshAdaptor->setRenderService(this->getRenderService());

        {
            const auto mesh = m_mesh.lock();
            meshAdaptor->setInput(mesh.get_shared(), "mesh", true);
        }

        meshAdaptor->configure(meshConfig);
        meshAdaptor->updateVisibility(false);
        meshAdaptor->start();
        SIGHT_ASSERT("SMesh is not started", meshAdaptor->isStarted());

        // Store data.
        MeshInstance instance {transform, image, transformAdaptor, meshAdaptor, textureAdaptor};
        m_meshes.push_back(instance);
    }
}

//-----------------------------------------------------------------------------

service::connections_t SMeshList::getAutoConnections() const
{
    service::connections_t connections;
    connections.push(s_TRANSFORM_INPUT, data::Matrix4::MODIFIED_SIG, ADD_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void SMeshList::updating()
{
}

//-----------------------------------------------------------------------------

void SMeshList::stopping()
{
    for(const auto& instance : m_meshes)
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
    for(const auto& instance : m_meshes)
    {
        if(instance.m_isEnabled)
        {
            const sight::viz::scene3d::adaptor::sptr mesh = instance.m_mesh;
            mesh->updateVisibility(_visible);
        }
    }
}

//-----------------------------------------------------------------------------

void SMeshList::add()
{
    if(m_dropCount % m_dropData == 0)
    {
        // Reset the drop count.
        m_dropCount = 0;

        auto& instance = m_meshes.at(m_meshCount);
        ++m_meshCount;

        if(m_meshCount == m_capacity)
        {
            m_meshCount = 0;
        }

        instance.m_isEnabled = true;

        const sight::viz::scene3d::adaptor::sptr textureAdp = instance.m_texture;
        {
            const auto image        = textureAdp->input<data::Image>("image").lock();
            const auto textureInput = m_texture.lock();

            if(m_generateAlpha && textureInput->getType() == core::type::UINT8
               && (textureInput->getPixelFormat() == data::Image::PixelFormat::GRAY_SCALE
                   || textureInput->numComponents() == 1))
            {
                // transform the image into RGBA with a transparent texture
                if(textureInput->getAllocatedSizeInBytes() * 4 != instance.m_image->getAllocatedSizeInBytes())
                {
                    instance.m_image->copyInformation(textureInput.get_shared());
                    instance.m_image->resize(
                        instance.m_image->size(),
                        instance.m_image->getType(),
                        data::Image::PixelFormat::RGBA
                    );
                }

                auto inItr = textureInput->begin<std::uint8_t>();
                auto inEnd = textureInput->end<std::uint8_t>();
                auto itr   = instance.m_image->begin<data::iterator::rgba>();

                for( ; inItr != inEnd ; ++inItr, ++itr)
                {
                    itr->r = *inItr;
                    itr->g = *inItr;
                    itr->b = *inItr;
                    itr->a = *inItr;
                }
            }
            else if(m_generateAlpha && textureInput->getType() == core::type::UINT8
                    && (textureInput->getPixelFormat() == data::Image::PixelFormat::RGB
                        || textureInput->numComponents() == 3))
            {
                // transform the image into RGBA with a transparent texture

                if(textureInput->getAllocatedSizeInBytes() * 4 / 3 != instance.m_image->getAllocatedSizeInBytes())
                {
                    instance.m_image->copyInformation(textureInput.get_shared());
                    instance.m_image->resize(
                        instance.m_image->size(),
                        instance.m_image->getType(),
                        data::Image::PixelFormat::RGBA
                    );
                }

                auto inItr = textureInput->begin<data::iterator::rgb>();
                auto inEnd = textureInput->end<data::iterator::rgb>();
                auto itr   = instance.m_image->begin<data::iterator::rgba>();

                for( ; inItr != inEnd ; ++inItr, ++itr)
                {
                    itr->r = inItr->r;
                    itr->g = inItr->g;
                    itr->b = inItr->b;
                    // luminance
                    itr->a = static_cast<std::uint8_t>(0.2126 * inItr->r + 0.7152 * inItr->g + 0.0722 * inItr->b);
                }
            }
            else
            {
                instance.m_image->deep_copy(textureInput.get_shared());
            }
        }
        textureAdp->update();

        // update the texture
        const sight::viz::scene3d::adaptor::sptr transformAdp = instance.m_transform;
        {
            // set current matrix
            const auto transform = transformAdp->inout<data::Matrix4>("transform").lock();

            const auto transformInOut = m_transform.lock();
            transform->deep_copy(transformInOut.get_shared());
        }
        transformAdp->update();

        // update mesh adaptor visibility
        const sight::viz::scene3d::adaptor::sptr meshAdp = instance.m_mesh;
        meshAdp->updateVisibility(m_isVisible);
    }

    ++m_dropCount;
}

//-----------------------------------------------------------------------------

void SMeshList::clear()
{
    this->setVisible(false);
    for(auto& instance : m_meshes)
    {
        instance.m_isEnabled = false;
    }

    m_meshCount = 0;
}

} // namespace sight::module::viz::scene3d::adaptor
