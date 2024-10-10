/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/material.hpp"

#include "modules/viz/scene3d/adaptor/shader_parameter.hpp"
#include "modules/viz/scene3d/adaptor/texture.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/field.hpp>
#include <data/map.hpp>
#include <data/matrix4.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/helper/shading.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/utils.hpp>

#include <string>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::slots::key_t material::UPDATE_FIELD_SLOT   = "updateField";
const core::com::slots::key_t material::SWAP_TEXTURE_SLOT   = "swapTexture";
const core::com::slots::key_t material::ADD_TEXTURE_SLOT    = "addTexture";
const core::com::slots::key_t material::REMOVE_TEXTURE_SLOT = "removeTexture";

const std::string material::MATERIAL_INOUT = "material";

//------------------------------------------------------------------------------

material::material() noexcept
{
    new_slot(UPDATE_FIELD_SLOT, &material::update_field, this);
    new_slot(SWAP_TEXTURE_SLOT, &material::swap_texture, this);
    new_slot(ADD_TEXTURE_SLOT, &material::create_texture_adaptor, this);
    new_slot(REMOVE_TEXTURE_SLOT, &material::remove_texture_adaptor, this);

    m_representation_dict["SURFACE"]   = data::material::surface;
    m_representation_dict["POINT"]     = data::material::point;
    m_representation_dict["WIREFRAME"] = data::material::wireframe;
    m_representation_dict["EDGE"]      = data::material::edge;
}

//------------------------------------------------------------------------------

void material::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_MATERIAL_TEMPLATE_NAME_CONFIG = CONFIG + "materialTemplate";
    static const std::string s_MATERIAL_NAME_CONFIG          = CONFIG + "materialName";
    static const std::string s_TEXTURE_NAME_CONFIG           = CONFIG + "textureName";
    static const std::string s_SHADING_MODE_CONFIG           = CONFIG + "shadingMode";
    static const std::string s_REPRESENTATION_MODE_CONFIG    = CONFIG + "representationMode";

    m_material_template_name = config.get(s_MATERIAL_TEMPLATE_NAME_CONFIG, m_material_template_name);
    m_material_name          = config.get(s_MATERIAL_NAME_CONFIG, this->get_id());
    m_texture_name           = config.get(s_TEXTURE_NAME_CONFIG, m_texture_name);
    m_shading_mode           = config.get(s_SHADING_MODE_CONFIG, m_shading_mode);
    m_representation_mode    = config.get(s_REPRESENTATION_MODE_CONFIG, m_representation_mode);

    auto it = m_representation_dict.find(m_representation_mode);
    if(it == m_representation_dict.end())
    {
        SIGHT_ERROR(
            "Value: " + m_representation_mode + " is not valid for 'representationMode'."
                                                " Accepted values are: SURFACE/POINT/WIREFRAME/EDGE."
                                                "'representationMode' is reset to default value (SURFACE). "
        );
        m_representation_mode = "SURFACE";
    }
}

//------------------------------------------------------------------------------

void material::configure(
    const std::string& _id,
    const std::string& _name,
    sight::viz::scene3d::render::sptr _service,
    const std::string& _layer,
    const std::string& _shading_mode,
    const std::string& _template
)
{
    this->base::configure(service::config_t());
    this->set_id(_id);
    this->set_material_name(_name);
    this->set_render_service(_service);
    this->set_layer_id(_layer);
    this->set_shading_mode(_shading_mode);
    this->set_material_template_name(_template);
}

//------------------------------------------------------------------------------

void material::starting()
{
    adaptor::init();
    {
        const auto material = m_material_data.lock();

        if(!m_shading_mode.empty())
        {
            data::material::shading_t shading_mode = data::material::shading_t::phong;
            if(m_shading_mode == "ambient")
            {
                shading_mode = data::material::shading_t::ambient;
            }
            else if(m_shading_mode == "flat")
            {
                shading_mode = data::material::shading_t::flat;
            }

            // Force the shading mode of the material if it has been set in the configuration of the adaptor
            material->set_shading_mode(shading_mode);
        }

        material->set_representation_mode(m_representation_dict[m_representation_mode]);

        m_material_fw = std::make_unique<sight::viz::scene3d::material>(m_material_name, m_material_template_name);

        data::string::sptr string = std::make_shared<data::string>();
        string->set_value(m_material_template_name);

        data::helper::field helper(material.get_shared());
        helper.set_field("ogreMaterial", string);
        helper.notify();
    }

    this->create_shader_parameter_adaptors();

    // A texture adaptor is configured in the XML scene, we can retrieve it
    if(!m_texture_name.empty())
    {
        if(!m_tex_adaptor)
        {
            this->set_texture_name(m_texture_name);
        }

        if(m_tex_adaptor->get_texture_name().empty())
        {
            m_tex_adaptor->set_render_service(this->render_service());
            m_tex_adaptor->set_layer_id(m_layer_id);
        }

        m_texture_connection.connect(
            m_tex_adaptor,
            module::viz::scene3d::adaptor::texture::TEXTURE_SWAPPED_SIG,
            this->get_sptr(),
            module::viz::scene3d::adaptor::material::SWAP_TEXTURE_SLOT
        );

        if(m_tex_adaptor->started())
        {
            this->swap_texture();
        }
    }
    else
    {
        this->create_texture_adaptor();
    }

    const auto config_tree = this->get_config();

    if(config_tree.find("config") != config_tree.not_found())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

service::connections_t material::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(MATERIAL_INOUT, data::material::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(MATERIAL_INOUT, data::material::ADDED_FIELDS_SIG, UPDATE_FIELD_SLOT);
    connections.push(MATERIAL_INOUT, data::material::CHANGED_FIELDS_SIG, UPDATE_FIELD_SLOT);
    connections.push(MATERIAL_INOUT, data::material::ADDED_TEXTURE_SIG, ADD_TEXTURE_SLOT);
    connections.push(MATERIAL_INOUT, data::material::REMOVED_TEXTURE_SIG, REMOVE_TEXTURE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void material::updating()
{
    const auto material = m_material_data.lock();

    if(m_r2vb_object != nullptr)
    {
        m_material_fw->set_primitive_type(m_r2vb_object->get_input_primitive_type());
    }

    // Set up representation mode
    m_material_fw->update_polygon_mode(material->get_representation_mode());
    m_material_fw->update_options_mode(material->get_options_mode());
    m_material_fw->update_shading_mode(
        material->get_shading_mode(),
        this->layer()->num_lights(),
        this->has_diffuse_texture(),
        m_tex_adaptor ? m_tex_adaptor->get_use_alpha() : false
    );
    m_material_fw->update_rgba_mode(material.get_shared());

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void material::stopping()
{
    m_material_fw.reset();
    m_texture_connection.disconnect();
    this->unregister_services();

    const auto material = m_material_data.lock();

    if(material->get_field("shaderParameters"))
    {
        material->remove_field("shaderParameters");
    }

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void material::create_shader_parameter_adaptors()
{
    auto material = this->get_material();

    SIGHT_ASSERT("Material '" + m_material_template_name + "'' not found", material);

    const auto constants = sight::viz::scene3d::helper::shading::find_material_constants(*material);
    for(const auto& constant : constants)
    {
        const std::string& constant_name = std::get<0>(constant);
        const auto& constant_type        = std::get<1>(constant);
        const auto& constant_value       = std::get<3>(constant);

        auto obj = sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
            constant_type,
            constant_value
        );
        if(obj != nullptr)
        {
            const auto shader_type            = std::get<2>(constant);
            const std::string shader_type_str = shader_type == Ogre::GPT_VERTEX_PROGRAM ? "vertex"
                                                                                        : shader_type
                                                == Ogre::GPT_FRAGMENT_PROGRAM ? "fragment"
                                                                              :
                                                "geometry";
            const core::id::type id =
                std::string(this->get_id()) + "_" + shader_type_str + "-" + constant_name;

            // Creates an Ogre adaptor and associates it with the Sight object
            auto srv =
                this->register_service<sight::module::viz::scene3d::adaptor::shader_parameter>(
                    "sight::module::viz::scene3d::adaptor::shader_parameter",
                    id
                );
            srv->set_inout(obj, "parameter", true);

            // Naming convention for shader parameters
            srv->set_render_service(this->render_service());

            service::config_t config;
            config.add("config.<xmlattr>.parameter", constant_name);
            config.add("config.<xmlattr>.shaderType", shader_type_str);
            config.add("config.<xmlattr>.materialName", m_material_name);

            srv->set_layer_id(m_layer_id);
            srv->set_config(config);
            srv->configure();
            srv->start();

            // Add the object to the shaderParameter map of the Material to keep the object alive
            const auto material_data = m_material_data.lock();

            data::map::sptr map = material_data->set_default_field(
                "shaderParameters",
                std::make_shared<data::map>()
            );
            (*map)[constant_name] = obj;
        }
    }
}

//------------------------------------------------------------------------------

void material::set_texture_name(const std::string& _texture_name)
{
    if(_texture_name.empty())
    {
        m_tex_adaptor = nullptr;
    }
    else
    {
        auto texture_adaptors = this->render_service()->get_adaptors<sight::module::viz::scene3d::adaptor::texture>();
        auto result           =
            std::find_if(
                texture_adaptors.begin(),
                texture_adaptors.end(),
                [_texture_name](const module::viz::scene3d::adaptor::texture::sptr& _srv)
            {
                return _srv->get_texture_name() == _texture_name;
            });

        SIGHT_ASSERT(
            "texture adaptor managing texture '" + _texture_name + "' is not found",
            result != texture_adaptors.end()
        );
        m_tex_adaptor = *result;
    }

    m_texture_name = _texture_name;
}

//------------------------------------------------------------------------------

void material::update_field(data::object::fields_container_t _fields)
{
    for(const auto& elt : _fields)
    {
        if(elt.first == "ogreMaterial")
        {
            this->unregister_services("sight::module::viz::scene3d::adaptor::shader_parameter");
            {
                const auto material = m_material_data.lock();

                data::string::csptr string = std::dynamic_pointer_cast<data::string>(elt.second);
                if(string->value() == m_material_template_name)
                {
                    // Avoid useless update if this is the same template material
                    continue;
                }

                this->set_material_template_name(string->get_value());

                m_material_fw->set_template(m_material_template_name);

                if(material->get_field("shaderParameters"))
                {
                    material->remove_field("shaderParameters");
                }
            }
            this->create_shader_parameter_adaptors();
            this->updating();

            // When resetting the material template, all techniques and passes will be destroyed,
            // so we need to reset the texture unit states
            Ogre::TexturePtr current_texture = m_tex_adaptor->get_texture();
            if(current_texture)
            {
                m_material_fw->set_diffuse_texture(current_texture);
            }
        }
    }
}

//------------------------------------------------------------------------------

void material::swap_texture()
{
    SIGHT_ASSERT("Missing texture adaptor", m_tex_adaptor);

    Ogre::TexturePtr current_texture = m_tex_adaptor->get_texture();
    SIGHT_ASSERT("texture not set in texture adaptor", current_texture);

    m_material_fw->set_diffuse_texture(current_texture);

    // Update the shaders
    const auto material = m_material_data.lock();

    m_material_fw->update_shading_mode(
        material->get_shading_mode(),
        this->layer()->num_lights(),
        this->has_diffuse_texture(),
        m_tex_adaptor->get_use_alpha()
    );

    this->request_render();
}

//------------------------------------------------------------------------------

void material::create_texture_adaptor()
{
    SIGHT_ASSERT("texture adaptor already configured in XML", m_texture_name.empty());

    const auto material = m_material_data.lock();

    // If the associated material has a texture, we have to create a texture adaptor to handle it
    if(material->get_diffuse_texture())
    {
        // Creates an Ogre adaptor and associates it with the Sight texture object
        auto texture = material->get_diffuse_texture();
        m_tex_adaptor = this->register_service<module::viz::scene3d::adaptor::texture>(
            "sight::module::viz::scene3d::adaptor::texture"
        );
        m_tex_adaptor->set_input(texture, "image", true);

        m_tex_adaptor->set_id(this->get_id() + "_" + m_tex_adaptor->get_id());
        m_tex_adaptor->set_render_service(this->render_service());
        m_tex_adaptor->set_layer_id(m_layer_id);

        const std::string material_name = material->get_id();
        m_tex_adaptor->set_texture_name(material_name + "_Texture");

        m_texture_connection.connect(
            m_tex_adaptor,
            module::viz::scene3d::adaptor::texture::TEXTURE_SWAPPED_SIG,
            this->get_sptr(),
            module::viz::scene3d::adaptor::material::SWAP_TEXTURE_SLOT
        );

        m_tex_adaptor->start();
    }
}

//------------------------------------------------------------------------------

void material::remove_texture_adaptor()
{
    SIGHT_ASSERT("Missing texture adaptor", m_tex_adaptor);
    SIGHT_ASSERT("texture adaptor already configured in XML", m_texture_name.empty());

    this->render_service()->make_current();

    m_material_fw->set_diffuse_texture(Ogre::TexturePtr());

    m_texture_connection.disconnect();
    this->unregister_services("sight::module::viz::scene3d::adaptor::texture");
    m_tex_adaptor.reset();

    // Update the shaders
    const auto material = m_material_data.lock();

    m_material_fw->update_shading_mode(
        material->get_shading_mode(),
        this->layer()->num_lights(),
        this->has_diffuse_texture(),
        false
    );

    this->request_render();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
