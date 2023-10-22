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

#include "mesh_list.hpp"

#include <core/com/slots.hxx>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t ADD_SLOT   = "add";
static const core::com::slots::key_t CLEAR_SLOT = "clear";

//-----------------------------------------------------------------------------

mesh_list::mesh_list() noexcept
{
    new_slot(ADD_SLOT, &mesh_list::add, this);
    new_slot(CLEAR_SLOT, &mesh_list::clear, this);
}

//-----------------------------------------------------------------------------

mesh_list::~mesh_list() noexcept =
    default;

//-----------------------------------------------------------------------------

void mesh_list::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

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

void mesh_list::starting()
{
    this->initialize();

    // Get the inputs.
    const auto transform_in_out = m_transform.lock();
    const auto image_input      = m_texture.lock();

    // initialise N meshes adaptor
    for(std::size_t i = 0 ; i < m_capacity ; ++i)
    {
        // Matrix and image are copied because the input ones will change. Mesh is not copied because we want to use
        // the same mesh of all the adaptors
        const auto transform = data::matrix4::copy(transform_in_out.get_shared());
        const auto image     = data::image::copy(image_input.get_shared());

        // Create adaptors configurations
        const std::string transform_id = this->get_id() + transform->get_id();
        service::config_t config;
        config.add("config.<xmlattr>.layer", m_layerID);
        config.add("config.<xmlattr>." + std::string(s_TRANSFORM_INPUT), transform_id);
        config.add("config.<xmlattr>.autoresetcamera", "false");

        // Create the transform adaptor.
        const sight::viz::scene3d::adaptor::sptr transform_adaptor =
            this->registerService<sight::viz::scene3d::adaptor>(
                "sight::module::viz::scene3d::adaptor::transform"
            );

        transform_adaptor->setLayerID(m_layerID);
        transform_adaptor->setRenderService(this->getRenderService());

        transform_adaptor->set_inout(transform, "transform", true);

        transform_adaptor->configure(config);
        transform_adaptor->start();

        SIGHT_ASSERT("transform is not started", transform_adaptor->started());

        // Create the texture adaptor
        const sight::viz::scene3d::adaptor::sptr texture_adaptor =
            this->registerService<sight::viz::scene3d::adaptor>("sight::module::viz::scene3d::adaptor::texture");

        service::config_t texture_config = config;
        texture_config.add("config.<xmlattr>.textureName", image->get_id());
        texture_config.add("config.<xmlattr>.useAlpha", "true");

        texture_adaptor->setLayerID(m_layerID);
        texture_adaptor->setRenderService(this->getRenderService());

        texture_adaptor->set_input(image, "image", false);

        texture_adaptor->configure(texture_config);
        texture_adaptor->start();
        SIGHT_ASSERT("texture is not started", texture_adaptor->started());

        // Creates the mesh adaptor.
        const sight::viz::scene3d::adaptor::sptr mesh_adaptor =
            this->registerService<sight::viz::scene3d::adaptor>("sight::module::viz::scene3d::adaptor::mesh");

        service::config_t mesh_config = config;
        mesh_config.add("config.<xmlattr>.textureName", image->get_id());

        mesh_adaptor->setLayerID(m_layerID);
        mesh_adaptor->setRenderService(this->getRenderService());

        {
            const auto mesh = m_mesh.lock();
            mesh_adaptor->set_input(mesh.get_shared(), "mesh", true);
        }

        mesh_adaptor->configure(mesh_config);
        mesh_adaptor->updateVisibility(false);
        mesh_adaptor->start();
        SIGHT_ASSERT("mesh is not started", mesh_adaptor->started());

        // Store data.
        MeshInstance instance {transform, image, transform_adaptor, mesh_adaptor, texture_adaptor};
        m_meshes.push_back(instance);
    }
}

//-----------------------------------------------------------------------------

service::connections_t mesh_list::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_TRANSFORM_INPUT, data::matrix4::MODIFIED_SIG, ADD_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void mesh_list::updating()
{
}

//-----------------------------------------------------------------------------

void mesh_list::stopping()
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

void mesh_list::setVisible(bool _visible)
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

void mesh_list::add()
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

        const sight::viz::scene3d::adaptor::sptr texture_adp = instance.m_texture;
        {
            const auto image         = texture_adp->input<data::image>("image").lock();
            const auto texture_input = m_texture.lock();

            if(m_generateAlpha && texture_input->getType() == core::type::UINT8
               && (texture_input->getPixelFormat() == data::image::PixelFormat::GRAY_SCALE
                   || texture_input->numComponents() == 1))
            {
                // transform the image into RGBA with a transparent texture
                if(texture_input->getAllocatedSizeInBytes() * 4 != instance.m_image->getAllocatedSizeInBytes())
                {
                    instance.m_image->copyInformation(texture_input.get_shared());
                    instance.m_image->resize(
                        instance.m_image->size(),
                        instance.m_image->getType(),
                        data::image::PixelFormat::RGBA
                    );
                }

                auto in_itr = texture_input->begin<std::uint8_t>();
                auto in_end = texture_input->end<std::uint8_t>();
                auto itr    = instance.m_image->begin<data::iterator::rgba>();

                for( ; in_itr != in_end ; ++in_itr, ++itr)
                {
                    itr->r = *in_itr;
                    itr->g = *in_itr;
                    itr->b = *in_itr;
                    itr->a = *in_itr;
                }
            }
            else if(m_generateAlpha && texture_input->getType() == core::type::UINT8
                    && (texture_input->getPixelFormat() == data::image::PixelFormat::RGB
                        || texture_input->numComponents() == 3))
            {
                // transform the image into RGBA with a transparent texture

                if(texture_input->getAllocatedSizeInBytes() * 4 / 3 != instance.m_image->getAllocatedSizeInBytes())
                {
                    instance.m_image->copyInformation(texture_input.get_shared());
                    instance.m_image->resize(
                        instance.m_image->size(),
                        instance.m_image->getType(),
                        data::image::PixelFormat::RGBA
                    );
                }

                auto in_itr = texture_input->begin<data::iterator::rgb>();
                auto in_end = texture_input->end<data::iterator::rgb>();
                auto itr    = instance.m_image->begin<data::iterator::rgba>();

                for( ; in_itr != in_end ; ++in_itr, ++itr)
                {
                    itr->r = in_itr->r;
                    itr->g = in_itr->g;
                    itr->b = in_itr->b;
                    // luminance
                    itr->a = static_cast<std::uint8_t>(0.2126 * in_itr->r + 0.7152 * in_itr->g + 0.0722 * in_itr->b);
                }
            }
            else
            {
                instance.m_image->deep_copy(texture_input.get_shared());
            }
        }
        texture_adp->update();

        // update the texture
        const sight::viz::scene3d::adaptor::sptr transform_adp = instance.m_transform;
        {
            // set current matrix
            const auto transform = transform_adp->inout<data::matrix4>("transform").lock();

            const auto transform_in_out = m_transform.lock();
            transform->deep_copy(transform_in_out.get_shared());
        }
        transform_adp->update();

        // update mesh adaptor visibility
        const sight::viz::scene3d::adaptor::sptr mesh_adp = instance.m_mesh;
        mesh_adp->updateVisibility(m_isVisible);
    }

    ++m_dropCount;
}

//-----------------------------------------------------------------------------

void mesh_list::clear()
{
    this->setVisible(false);
    for(auto& instance : m_meshes)
    {
        instance.m_isEnabled = false;
    }

    m_meshCount = 0;
}

} // namespace sight::module::viz::scene3d::adaptor
