/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "viz/scene3d/parameter_adaptor.hpp"

#include <core/com/slots.hxx>

#include <data/array.hpp>
#include <data/boolean.hpp>
#include <data/color.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>
#include <data/matrix4.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d
{

//------------------------------------------------------------------------------

void parameter_adaptor::set_shader_type(Ogre::GpuProgramType _shader_type)
{
    m_shader_type = _shader_type;
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_param_name(const std::string& _param_name)
{
    m_param_name = _param_name;
}

//------------------------------------------------------------------------------

const std::string& parameter_adaptor::get_param_name() const
{
    return m_param_name;
}

//------------------------------------------------------------------------------

service::connections_t parameter_adaptor::auto_connections() const
{
    return {{PARAMETER_INOUT, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE}};
}

//------------------------------------------------------------------------------

void parameter_adaptor::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    m_param_name = config.get<std::string>(CONFIG + "parameter", "");
    SIGHT_ERROR_IF("parameter attribute not set", m_param_name.empty());

    m_technique_name = config.get<std::string>(CONFIG + "technique", "");

    if(config.count("shader_type") != 0U)
    {
        const auto shader_type = config.get<std::string>(CONFIG + "shader_type");
        if(shader_type == "vertex")
        {
            m_shader_type = Ogre::GPT_VERTEX_PROGRAM;
        }
        else if(shader_type == "fragment")
        {
            m_shader_type = Ogre::GPT_FRAGMENT_PROGRAM;
        }
        else if(shader_type == "geometry")
        {
            m_shader_type = Ogre::GPT_GEOMETRY_PROGRAM;
        }
        else
        {
            SIGHT_ERROR("This shader type " << shader_type << " isn't supported yet");
        }
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::updating()
{
    if(!m_material)
    {
        return;
    }

    this->render_service()->make_current();
    if(m_technique_name.empty())
    {
        bool b_set                                   = false;
        const Ogre::Material::Techniques& techniques = m_material->getTechniques();

        for(auto* const tech : techniques)
        {
            SIGHT_ASSERT("technique is not set", tech);

            b_set |= this->set_parameter(*tech);
        }

        if(!b_set)
        {
            SIGHT_DEBUG(
                "Couldn't set parameter '" + m_param_name + "' in any technique of material '"
                + m_material->getName() + "'"
            );
        }
        else
        {
            this->request_render();
        }
    }
    else
    {
        Ogre::Technique* tech = m_material->getTechnique(m_technique_name);
        SIGHT_FATAL_IF("Can't find technique " << m_technique_name, !tech);

        if(this->set_parameter(*tech))
        {
            SIGHT_DEBUG(
                "Couldn't set parameter '" + m_param_name + "' in technique '" + m_technique_name
                + "' from material '" + m_material->getName() + "'"
            );
        }
        else
        {
            this->request_render();
        }
    }

    this->update_done();
}

//------------------------------------------------------------------------------

void parameter_adaptor::stopping()
{
    this->render_service()->make_current();
    m_material.reset();
    m_texture.reset();
}

//------------------------------------------------------------------------------

bool parameter_adaptor::set_parameter(Ogre::Technique& _technique)
{
    /// Contains the different parameters for the shader
    Ogre::GpuProgramParametersSharedPtr params;

    // Get the parameters
    auto* pass = _technique.getPass(0);
    if(m_shader_type == Ogre::GPT_VERTEX_PROGRAM)
    {
        params = pass->getVertexProgramParameters();
    }
    else if(m_shader_type == Ogre::GPT_FRAGMENT_PROGRAM && pass->hasFragmentProgram())
    {
        params = pass->getFragmentProgramParameters();
    }
    else if(m_shader_type == Ogre::GPT_GEOMETRY_PROGRAM)
    {
        params = pass->getGeometryProgramParameters();
    }
    else
    {
        return false;
    }

    if(params->_findNamedConstantDefinition(m_param_name) == nullptr)
    {
        return false;
    }

    bool update_texture = false;
    {
        auto obj = m_parameter.const_lock();

        // Set shader parameters
        std::string obj_class = obj->get_classname();
        if(obj_class == "sight::data::integer")
        {
            const auto int_value = std::dynamic_pointer_cast<const data::integer>(obj.get_shared());
            SIGHT_ASSERT("The given integer object is null", int_value);

            params->setNamedConstant(m_param_name, static_cast<int>(int_value->value()));
        }
        else if(obj_class == "sight::data::real")
        {
            const auto float_value = std::dynamic_pointer_cast<const data::real>(obj.get_shared());
            SIGHT_ASSERT("The given float object is null", float_value);

            params->setNamedConstant(m_param_name, static_cast<float>(float_value->value()));
        }
        else if(obj_class == "sight::data::boolean")
        {
            const auto boolean_value = std::dynamic_pointer_cast<const data::boolean>(obj.get_shared());
            SIGHT_ASSERT("The given boolean object is null", boolean_value);

            params->setNamedConstant(m_param_name, static_cast<int>(boolean_value->value()));
        }
        else if(obj_class == "sight::data::color")
        {
            const auto color_value = std::dynamic_pointer_cast<const data::color>(obj.get_shared());
            SIGHT_ASSERT("The given color object is null", color_value);

            std::array<float, 4> param_values {};

            param_values[0] = color_value->red();
            param_values[1] = color_value->green();
            param_values[2] = color_value->blue();
            param_values[3] = color_value->alpha();

            Ogre::ColourValue color(param_values[0], param_values[1], param_values[2], param_values[3]);

            params->setNamedConstant(m_param_name, color);
        }
        else if(obj_class == "sight::data::matrix4")
        {
            const auto trans_value = std::dynamic_pointer_cast<const data::matrix4>(obj.get_shared());
            SIGHT_ASSERT("The given Matrix4 object is null", trans_value);

            std::array<float, 16> param_values {};

            for(std::size_t i = 0 ; i < 16 ; i++)
            {
                param_values[i] = static_cast<float>((*trans_value)[i]);
            }

            params->setNamedConstant(
                m_param_name,
                param_values.data(),
                static_cast<std::size_t>(16),
                static_cast<std::size_t>(1)
            );
        }
        else if(obj_class == "sight::data::ivec2")
        {
            const auto vec = std::dynamic_pointer_cast<const data::ivec2>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            const std::array array = {(int) vec->value()[0], (int) vec->value()[1]};
            params->setNamedConstant(m_param_name, array.data(), 1, 2);
        }
        else if(obj_class == "sight::data::ivec3")
        {
            const auto vec = std::dynamic_pointer_cast<const data::ivec3>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            const std::array array = {(int) vec->value()[0], (int) vec->value()[1], (int) vec->value()[2]};
            params->setNamedConstant(m_param_name, array.data(), 1, 3);
        }
        else if(obj_class == "sight::data::ivec4")
        {
            const auto vec = std::dynamic_pointer_cast<const data::ivec4>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            const std::array array = {(int) vec->value()[0], (int) vec->value()[1], (int) vec->value()[2],
                                      (int) vec->value()[3]
            };
            params->setNamedConstant(m_param_name, array.data(), 1, 4);
        }
        else if(obj_class == "sight::data::dvec2")
        {
            const auto vec = std::dynamic_pointer_cast<const data::dvec2>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            params->setNamedConstant(m_param_name, Ogre::Vector2((float) (*vec)[0], (float) (*vec)[1]));
        }
        else if(obj_class == "sight::data::dvec3")
        {
            const auto vec = std::dynamic_pointer_cast<const data::dvec3>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            params->setNamedConstant(
                m_param_name,
                Ogre::Vector3((float) (*vec)[0], (float) (*vec)[1], (float) (*vec)[2])
            );
        }
        else if(obj_class == "sight::data::dvec4")
        {
            const auto vec = std::dynamic_pointer_cast<const data::dvec4>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            params->setNamedConstant(
                m_param_name,
                Ogre::Vector4((float) (*vec)[0], (float) (*vec)[1], (float) (*vec)[2], (float) (*vec)[3])
            );
        }
        else if(obj_class == "sight::data::array")
        {
            const auto array_object = std::dynamic_pointer_cast<const data::array>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", array_object);

            const std::size_t num_components = array_object->size()[0];
            if(num_components <= 3)
            {
                const auto dump_lock = array_object->dump_lock();

                if(array_object->type() == core::type::FLOAT)
                {
                    const auto* float_value = static_cast<const float*>(array_object->buffer());
                    params->setNamedConstant(m_param_name, float_value, 1, num_components);
                }
                else if(array_object->type() == core::type::DOUBLE)
                {
                    const auto* double_value = static_cast<const double*>(array_object->buffer());
                    params->setNamedConstant(m_param_name, double_value, 1, num_components);
                }
                else if(array_object->type() == core::type::INT32)
                {
                    const int* int_value = static_cast<const int*>(array_object->buffer());
                    params->setNamedConstant(m_param_name, int_value, 1, num_components);
                }
                else
                {
                    SIGHT_ERROR("Array type not handled: " << array_object->type().name());
                }
            }
            else
            {
                SIGHT_ERROR("Array size not handled: " << array_object->size()[0]);
            }
        }
        else if(obj_class == "sight::data::image")
        {
            const auto image = std::dynamic_pointer_cast<const data::image>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", image);

            if(!m_texture)
            {
                m_texture = std::make_shared<sight::viz::scene3d::texture>(image);
            }

            // We can reach this code for an another reason than an image modification, for instance when the compositor
            // is resized. However I don't know how to discriminate the two cases so for now we always copy the image.
            // :/
            if(image->size_in_bytes() != 0U)
            {
                // Defer the update of the texture outside the scope of the data lock
                update_texture = true;
            }
        }
        // We allow to work on the render map and interact with slots instead
        else if(obj_class != "sight::data::map")
        {
            SIGHT_ERROR("This Type " << obj_class << " isn't supported.");
        }
    }

    if(update_texture)
    {
        m_texture->update();

        Ogre::TextureUnitState* tex_state = pass->getTextureUnitState(m_param_name);
        tex_state->setTexture(m_texture->get());
        auto tex_unit_index = pass->getTextureUnitStateIndex(tex_state);
        params->setNamedConstant(m_param_name, tex_unit_index);
    }

    return true;
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_material(const Ogre::MaterialPtr& _material)
{
    if(m_material != _material)
    {
        m_material = _material;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
