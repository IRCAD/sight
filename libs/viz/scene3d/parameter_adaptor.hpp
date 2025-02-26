/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#pragma once

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/adaptor.hpp"

#include <viz/scene3d/texture.hpp>

namespace sight::viz::scene3d
{

/**
 * @brief   Base interface containing common code to upload shader parameters from Sight data.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API parameter_adaptor : public sight::viz::scene3d::adaptor
{
public:

    SIGHT_DECLARE_CLASS(parameter_adaptor, viz::scene3d::adaptor);

    /**
     * @name Slots API
     * @{
     */

    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_COLOR_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_DOUBLE2_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_DOUBLE3_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_INT_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_INT2_PARAMETER_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SET_INT3_PARAMETER_SLOT;

    ///@}

    SIGHT_VIZ_SCENE3D_API static constexpr std::string_view PARAMETER_INOUT = "parameter";

    /// Constructor.
    SIGHT_VIZ_SCENE3D_API parameter_adaptor() noexcept;

    /// Destructor. Does nothing
    SIGHT_VIZ_SCENE3D_API ~parameter_adaptor() noexcept override = default;

    /// Set the shaderType by passing the value of the shader_enum_t of this adaptor.
    SIGHT_VIZ_SCENE3D_API void set_shader_type(Ogre::GpuProgramType _shader_type);

    /// Set the name of the parameter m_paramName.
    SIGHT_VIZ_SCENE3D_API void set_param_name(const std::string& _param_name);

    /// Get the name of the parameter m_paramName.
    SIGHT_VIZ_SCENE3D_API const std::string& get_param_name() const;

    /// Get the name of the parameter m_paramName.
    SIGHT_VIZ_SCENE3D_API const std::string& get_default_value() const;

protected:

    /// Configure the adaptor
    SIGHT_VIZ_SCENE3D_API void configuring() override;

    /// Updates the shader parameter values and requests a render of the scene.
    SIGHT_VIZ_SCENE3D_API void updating() override;

    /// Clear the mesh pointer.
    SIGHT_VIZ_SCENE3D_API void stopping() override;

    /// Set the parameter for a given technique
    SIGHT_VIZ_SCENE3D_API virtual bool set_parameter(Ogre::Technique& _technique);

    /// Set the material to update
    SIGHT_VIZ_SCENE3D_API void set_material(const Ogre::MaterialPtr& _material);

    /// Connect the input parameter modified signal to this service update slot.
    SIGHT_VIZ_SCENE3D_API service::connections_t auto_connections() const override;

private:

    /// SLOT : Set the uniform from an integer value
    void set_bool_parameter(bool _value, std::string _name);

    /// SLOT : Set the uniform from a color value
    void set_color_parameter(std::array<std::uint8_t, 4> _color, std::string _name);

    /// SLOT : Set the uniform from an integer value
    void set_int_parameter(int _value, std::string _name);

    /// SLOT : Set the uniform from an integer value
    void set_int2_parameter(int _value1, int _value2, std::string _name);

    /// SLOT : Set the uniform from an integer value
    void set_int3_parameter(int _value1, int _value2, int _value3, std::string _name);

    /// SLOT : Set the uniform from an double value
    void set_double_parameter(double _value, std::string _name);

    /// SLOT : Set the uniform from an double value
    void set_double2_parameter(double _value1, double _value2, std::string _name);

    /// SLOT : Set the uniform from an double value
    void set_double3_parameter(double _value1, double _value2, double _value3, std::string _name);

    /// Parameter name
    std::string m_param_name;
    /// technique name
    std::string m_technique_name;
    /// Parameter name
    std::string m_default_value;
    /// Stores the value of the enum representing the shader's type.
    Ogre::GpuProgramType m_shader_type {Ogre::GPT_FRAGMENT_PROGRAM};
    /// Pointer on the material
    Ogre::MaterialPtr m_material;
    /// Pointer on a texture if the parameter is an image
    sight::viz::scene3d::texture::sptr m_texture;

    data::ptr<data::object, data::access::inout> m_parameter {this, PARAMETER_INOUT};
};

} // namespace sight::viz::scene3d
