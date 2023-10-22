/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

const core::com::slots::key_t parameter_adaptor::SET_BOOL_PARAMETER_SLOT    = "setBoolParameter";
const core::com::slots::key_t parameter_adaptor::SET_COLOR_PARAMETER_SLOT   = "setColorParameter";
const core::com::slots::key_t parameter_adaptor::SET_DOUBLE_PARAMETER_SLOT  = "setDoubleParameter";
const core::com::slots::key_t parameter_adaptor::SET_DOUBLE2_PARAMETER_SLOT = "setDouble2Parameter";
const core::com::slots::key_t parameter_adaptor::SET_DOUBLE3_PARAMETER_SLOT = "setDouble3Parameter";
const core::com::slots::key_t parameter_adaptor::SET_INT_PARAMETER_SLOT     = "setIntParameter";
const core::com::slots::key_t parameter_adaptor::SET_INT2_PARAMETER_SLOT    = "setInt2Parameter";
const core::com::slots::key_t parameter_adaptor::SET_INT3_PARAMETER_SLOT    = "setInt3Parameter";

//------------------------------------------------------------------------------

parameter_adaptor::parameter_adaptor() noexcept
{
    new_slot(SET_BOOL_PARAMETER_SLOT, &parameter_adaptor::setBoolParameter, this);
    new_slot(SET_COLOR_PARAMETER_SLOT, &parameter_adaptor::setColorParameter, this);
    new_slot(SET_DOUBLE_PARAMETER_SLOT, &parameter_adaptor::setDoubleParameter, this);
    new_slot(SET_DOUBLE2_PARAMETER_SLOT, &parameter_adaptor::setDouble2Parameter, this);
    new_slot(SET_DOUBLE3_PARAMETER_SLOT, &parameter_adaptor::setDouble3Parameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &parameter_adaptor::setIntParameter, this);
    new_slot(SET_INT2_PARAMETER_SLOT, &parameter_adaptor::setInt2Parameter, this);
    new_slot(SET_INT3_PARAMETER_SLOT, &parameter_adaptor::setInt3Parameter, this);
}

//------------------------------------------------------------------------------

parameter_adaptor::~parameter_adaptor() noexcept =
    default;

//------------------------------------------------------------------------------

void parameter_adaptor::set_shader_type(Ogre::GpuProgramType _shader_type)
{
    m_shaderType = _shader_type;
}

//------------------------------------------------------------------------------

void parameter_adaptor::setParamName(const std::string& _param_name)
{
    m_paramName = _param_name;
}

//------------------------------------------------------------------------------

const std::string& parameter_adaptor::getParamName() const
{
    return m_paramName;
}

//------------------------------------------------------------------------------

service::connections_t parameter_adaptor::auto_connections() const
{
    return {{s_PARAMETER_INOUT, data::object::MODIFIED_SIG, service::slots::UPDATE}};
}

//------------------------------------------------------------------------------

void parameter_adaptor::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    m_paramName = config.get<std::string>(s_CONFIG + "parameter", "");
    SIGHT_ERROR_IF("parameter attribute not set", m_paramName.empty());

    m_techniqueName = config.get<std::string>(s_CONFIG + "technique", "");

    if(config.count("shaderType") != 0U)
    {
        const auto shader_type = config.get<std::string>(s_CONFIG + "shaderType");
        if(shader_type == "vertex")
        {
            m_shaderType = Ogre::GPT_VERTEX_PROGRAM;
        }
        else if(shader_type == "fragment")
        {
            m_shaderType = Ogre::GPT_FRAGMENT_PROGRAM;
        }
        else if(shader_type == "geometry")
        {
            m_shaderType = Ogre::GPT_GEOMETRY_PROGRAM;
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
    if(!m_material || !m_dirty)
    {
        return;
    }

    this->getRenderService()->makeCurrent();
    if(m_techniqueName.empty())
    {
        bool b_set                                   = false;
        const Ogre::Material::Techniques& techniques = m_material->getTechniques();

        for(auto* const tech : techniques)
        {
            SIGHT_ASSERT("technique is not set", tech);

            b_set |= this->setParameter(*tech);
        }

        if(!b_set)
        {
            SIGHT_DEBUG(
                "Couldn't set parameter '" + m_paramName + "' in any technique of material '"
                + m_material->getName() + "'"
            );
        }
        else
        {
            this->requestRender();
        }
    }
    else
    {
        Ogre::Technique* tech = m_material->getTechnique(m_techniqueName);
        SIGHT_FATAL_IF("Can't find technique " << m_techniqueName, !tech);

        if(this->setParameter(*tech))
        {
            SIGHT_DEBUG(
                "Couldn't set parameter '" + m_paramName + "' in technique '" + m_techniqueName
                + "' from material '" + m_material->getName() + "'"
            );
        }
        else
        {
            this->requestRender();
        }
    }

    m_dirty = false;
}

//------------------------------------------------------------------------------

void parameter_adaptor::stopping()
{
    this->getRenderService()->makeCurrent();
    m_material.reset();
    m_texture.reset();
}

//------------------------------------------------------------------------------

bool parameter_adaptor::setParameter(Ogre::Technique& _technique)
{
    /// Contains the different parameters for the shader
    Ogre::GpuProgramParametersSharedPtr params;

    // Get the parameters
    auto* pass = _technique.getPass(0);
    if(m_shaderType == Ogre::GPT_VERTEX_PROGRAM)
    {
        params = pass->getVertexProgramParameters();
    }
    else if(m_shaderType == Ogre::GPT_FRAGMENT_PROGRAM && pass->hasFragmentProgram())
    {
        params = pass->getFragmentProgramParameters();
    }
    else if(m_shaderType == Ogre::GPT_GEOMETRY_PROGRAM)
    {
        params = pass->getGeometryProgramParameters();
    }
    else
    {
        return false;
    }

    if(params->_findNamedConstantDefinition(m_paramName) == nullptr)
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

            params->setNamedConstant(m_paramName, static_cast<int>(int_value->value()));
        }
        else if(obj_class == "sight::data::real")
        {
            const auto float_value = std::dynamic_pointer_cast<const data::real>(obj.get_shared());
            SIGHT_ASSERT("The given float object is null", float_value);

            params->setNamedConstant(m_paramName, static_cast<float>(float_value->value()));
        }
        else if(obj_class == "sight::data::boolean")
        {
            const auto boolean_value = std::dynamic_pointer_cast<const data::boolean>(obj.get_shared());
            SIGHT_ASSERT("The given boolean object is null", boolean_value);

            params->setNamedConstant(m_paramName, static_cast<int>(boolean_value->value()));
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

            params->setNamedConstant(m_paramName, color);
        }
        else if(obj_class == "sight::data::point_list")
        {
            const auto point_list_value = std::dynamic_pointer_cast<const data::point_list>(obj.get_shared());
            SIGHT_ASSERT("The given pointList object is null", point_list_value);

            std::vector<data::point::sptr> points = point_list_value->getPoints();
            int nb_points                         = static_cast<int>(points.size());

            auto* param_values = new float [static_cast<std::uint64_t>(nb_points * 3)];

            for(int i = 0 ; i < nb_points * 3 ; )
            {
                param_values[i] = static_cast<float>(points[static_cast<std::size_t>(i)]->getCoord()[0]);
                i++;

                param_values[i] = static_cast<float>(points[static_cast<std::size_t>(i)]->getCoord()[1]);
                i++;

                param_values[i] = static_cast<float>(points[static_cast<std::size_t>(i)]->getCoord()[2]);
                i++;
            }

            params->setNamedConstant(m_paramName, param_values, points.size(), static_cast<std::size_t>(3));

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
                m_paramName,
                param_values.data(),
                static_cast<std::size_t>(16),
                static_cast<std::size_t>(1)
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

                if(array_object->getType() == core::type::FLOAT)
                {
                    const auto* float_value = static_cast<const float*>(array_object->buffer());
                    params->setNamedConstant(m_paramName, float_value, 1, num_components);
                }
                else if(array_object->getType() == core::type::DOUBLE)
                {
                    const auto* double_value = static_cast<const double*>(array_object->buffer());
                    params->setNamedConstant(m_paramName, double_value, 1, num_components);
                }
                else if(array_object->getType() == core::type::INT32)
                {
                    const int* int_value = static_cast<const int*>(array_object->buffer());
                    params->setNamedConstant(m_paramName, int_value, 1, num_components);
                }
                else
                {
                    SIGHT_ERROR("Array type not handled: " << array_object->getType().name());
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
            if(image->getSizeInBytes() != 0U)
            {
                // Defer the update of the texture outside the scope of the data lock
                update_texture = true;
            }
        }
        // We allow to work on the render composite and interact with slots instead
        else if(obj_class != "sight::data::composite")
        {
            SIGHT_ERROR("This Type " << obj_class << " isn't supported.");
        }
    }

    if(update_texture)
    {
        m_texture->update();

        Ogre::TextureUnitState* tex_state = pass->getTextureUnitState(m_paramName);
        tex_state->setTexture(m_texture->get());
        auto tex_unit_index = pass->getTextureUnitStateIndex(tex_state);
        params->setNamedConstant(m_paramName, tex_unit_index);
    }

    return true;
}

//------------------------------------------------------------------------------

void parameter_adaptor::setMaterial(const Ogre::MaterialPtr& _material)
{
    if(m_material != _material)
    {
        this->setDirty();
        m_material = _material;
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setBoolParameter(bool _value, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;
        {
            auto param_object = m_parameter.lock();
            auto bool_object  = std::dynamic_pointer_cast<data::boolean>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::boolean", bool_object);
            bool_object->setValue(_value);
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setColorParameter(std::array<uint8_t, 4> _color, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

        {
            auto param_object = m_parameter.lock();
            auto color_object = std::dynamic_pointer_cast<data::color>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::color", color_object);
            color_object->setRGBA(
                float(_color[0]) / 255.F,
                float(_color[1]) / 255.F,
                float(_color[2]) / 255.F,
                float(_color[3]) / 255.F
            );
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setIntParameter(int _value, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

        {
            auto param_object = m_parameter.lock();
            auto int_object   = std::dynamic_pointer_cast<data::integer>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::integer", int_object);
            int_object->setValue(_value);
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setInt2Parameter(int _value1, int _value2, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

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
        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setInt3Parameter(int _value1, int _value2, int _value3, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

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

        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setDoubleParameter(double _value, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

        {
            auto param_object = m_parameter.lock();
            auto float_object = std::dynamic_pointer_cast<data::real>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::real", float_object);
            float_object->setValue(static_cast<float>(_value));
        }

        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setDouble2Parameter(double _value1, double _value2, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

        {
            auto param_object = m_parameter.lock();
            auto array_object = std::dynamic_pointer_cast<data::array>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::array", array_object);
            if(array_object->empty())
            {
                array_object->resize({2}, core::type::DOUBLE);
            }

            const auto dump_lock = array_object->dump_lock();

            if(array_object->getType() == core::type::FLOAT)
            {
                array_object->at<float>(0) = static_cast<float>(_value1);
                array_object->at<float>(1) = static_cast<float>(_value2);
            }
            else if(array_object->getType() == core::type::DOUBLE)
            {
                array_object->at<double>(0) = _value1;
                array_object->at<double>(1) = _value2;
            }
        }

        this->updating();
    }
}

//------------------------------------------------------------------------------

void parameter_adaptor::setDouble3Parameter(double _value1, double _value2, double _value3, std::string _name)
{
    if(_name == m_paramName)
    {
        m_dirty = true;

        {
            auto param_object = m_parameter.lock();
            auto array_object = std::dynamic_pointer_cast<data::array>(param_object.get_shared());
            SIGHT_ASSERT("Shader parameter '" + _name + "' is not of type sight::data::array", array_object);

            if(array_object->empty())
            {
                array_object->resize({3}, core::type::DOUBLE);
            }

            const auto dump_lock = array_object->dump_lock();

            if(array_object->getType() == core::type::FLOAT)
            {
                array_object->at<float>(0) = static_cast<float>(_value1);
                array_object->at<float>(1) = static_cast<float>(_value2);
                array_object->at<float>(2) = static_cast<float>(_value3);
            }
            else if(array_object->getType() == core::type::DOUBLE)
            {
                array_object->at<double>(0) = _value1;
                array_object->at<double>(1) = _value2;
                array_object->at<double>(2) = _value3;
            }
        }

        this->updating();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
