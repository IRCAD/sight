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
#include <data/dvec3.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
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

const core::com::slots::key_t parameter_adaptor::SET_BOOL_PARAMETER_SLOT    = "set_bool_parameter";
const core::com::slots::key_t parameter_adaptor::SET_COLOR_PARAMETER_SLOT   = "set_color_parameter";
const core::com::slots::key_t parameter_adaptor::SET_DOUBLE_PARAMETER_SLOT  = "set_double_parameter";
const core::com::slots::key_t parameter_adaptor::SET_DOUBLE2_PARAMETER_SLOT = "set_double2_parameter";
const core::com::slots::key_t parameter_adaptor::SET_DOUBLE3_PARAMETER_SLOT = "set_double3_parameter";
const core::com::slots::key_t parameter_adaptor::SET_INT_PARAMETER_SLOT     = "set_int_parameter";
const core::com::slots::key_t parameter_adaptor::SET_INT2_PARAMETER_SLOT    = "set_int2_parameter";
const core::com::slots::key_t parameter_adaptor::SET_INT3_PARAMETER_SLOT    = "set_int3_parameter";

//------------------------------------------------------------------------------

parameter_adaptor::parameter_adaptor() noexcept
{
    new_slot(SET_BOOL_PARAMETER_SLOT, &parameter_adaptor::set_bool_parameter, this);
    new_slot(SET_COLOR_PARAMETER_SLOT, &parameter_adaptor::set_color_parameter, this);
    new_slot(SET_DOUBLE_PARAMETER_SLOT, &parameter_adaptor::set_double_parameter, this);
    new_slot(SET_DOUBLE2_PARAMETER_SLOT, &parameter_adaptor::set_double2_parameter, this);
    new_slot(SET_DOUBLE3_PARAMETER_SLOT, &parameter_adaptor::set_double3_parameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &parameter_adaptor::set_int_parameter, this);
    new_slot(SET_INT2_PARAMETER_SLOT, &parameter_adaptor::set_int2_parameter, this);
    new_slot(SET_INT3_PARAMETER_SLOT, &parameter_adaptor::set_int3_parameter, this);
}

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

    if(config.count("shaderType") != 0U)
    {
        const auto shader_type = config.get<std::string>(CONFIG + "shaderType");
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
        else if(obj_class == "sight::data::point_list")
        {
            const auto point_list_value = std::dynamic_pointer_cast<const data::point_list>(obj.get_shared());
            SIGHT_ASSERT("The given pointList object is null", point_list_value);

            std::vector<data::point::sptr> points = point_list_value->get_points();
            int nb_points                         = static_cast<int>(points.size());

            auto* param_values = new float [static_cast<std::uint64_t>(nb_points * 3)];

            for(int i = 0 ; i < nb_points * 3 ; )
            {
                param_values[i] = static_cast<float>((*points[static_cast<std::size_t>(i)])[0]);
                i++;

                param_values[i] = static_cast<float>((*points[static_cast<std::size_t>(i)])[1]);
                i++;

                param_values[i] = static_cast<float>((*points[static_cast<std::size_t>(i)])[2]);
                i++;
            }

            params->setNamedConstant(m_param_name, param_values, points.size(), static_cast<std::size_t>(3));

            delete[] param_values;
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
        else if(obj_class == "sight::data::dvec3")
        {
            const auto vec = std::dynamic_pointer_cast<const data::dvec3>(obj.get_shared());
            SIGHT_ASSERT("The object is nullptr", vec);

            params->setNamedConstant(
                m_param_name,
                Ogre::Vector3((float) (*vec)[0], (float) (*vec)[1], (float) (*vec)[2])
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

void parameter_adaptor::set_bool_parameter(bool _value, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto bool_object  = std::dynamic_pointer_cast<data::boolean>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::boolean", bool_object);
            bool_object->set_value(_value);
        }
        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_color_parameter(std::array<uint8_t, 4> _color, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto color_object = std::dynamic_pointer_cast<data::color>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::color", color_object);
            color_object->set_rgba(
                float(_color[0]) / 255.F,
                float(_color[1]) / 255.F,
                float(_color[2]) / 255.F,
                float(_color[3]) / 255.F
            );
        }
        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_int_parameter(int _value, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto int_object   = std::dynamic_pointer_cast<data::integer>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::integer", int_object);
            int_object->set_value(_value);
        }
        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_int2_parameter(int _value1, int _value2, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto array_object = std::dynamic_pointer_cast<data::array>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::array", array_object);

            if(array_object->empty())
            {
                array_object->resize({2}, core::type::INT32);
            }

            const auto dump_lock = array_object->dump_lock();
            array_object->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value1);
            array_object->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value2);
        }
        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_int3_parameter(int _value1, int _value2, int _value3, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto array_object = std::dynamic_pointer_cast<data::array>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::array", array_object);

            if(array_object->empty())
            {
                array_object->resize({3}, core::type::INT32);
            }

            const auto dump_lock = array_object->dump_lock();
            array_object->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value1);
            array_object->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value2);
            array_object->at<std::uint32_t>(2) = static_cast<std::uint32_t>(_value3);
        }

        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_double_parameter(double _value, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto float_object = std::dynamic_pointer_cast<data::real>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::real", float_object);
            float_object->set_value(static_cast<float>(_value));
        }

        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_double2_parameter(double _value1, double _value2, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto array_object = std::dynamic_pointer_cast<data::array>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::array", array_object);
            if(array_object->empty())
            {
                array_object->resize({2}, core::type::DOUBLE);
            }

            const auto dump_lock = array_object->dump_lock();

            if(array_object->type() == core::type::FLOAT)
            {
                array_object->at<float>(0) = static_cast<float>(_value1);
                array_object->at<float>(1) = static_cast<float>(_value2);
            }
            else if(array_object->type() == core::type::DOUBLE)
            {
                array_object->at<double>(0) = _value1;
                array_object->at<double>(1) = _value2;
            }
        }

        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::set_double3_parameter(double _value1, double _value2, double _value3, std::string _name)
{
    if(_name == m_param_name)
    {
        {
            auto param_object = m_parameter.lock();
            auto array_object = std::dynamic_pointer_cast<data::array>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::array", array_object);

            if(array_object->empty())
            {
                array_object->resize({3}, core::type::DOUBLE);
            }

            const auto dump_lock = array_object->dump_lock();

            if(array_object->type() == core::type::FLOAT)
            {
                array_object->at<float>(0) = static_cast<float>(_value1);
                array_object->at<float>(1) = static_cast<float>(_value2);
                array_object->at<float>(2) = static_cast<float>(_value3);
            }
            else if(array_object->type() == core::type::DOUBLE)
            {
                array_object->at<double>(0) = _value1;
                array_object->at<double>(1) = _value2;
                array_object->at<double>(2) = _value3;
            }
        }

        this->lazy_update();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
