/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/config.hpp"

#include <viz/scene3d/Texture.hpp>

namespace sight::viz::scene3d
{

/**
 * @brief   Base interface containing common code to upload shader parameters from Sight data.
 */
class VIZ_SCENE3D_CLASS_API IParameter : public sight::viz::scene3d::adaptor
{
public:

    SIGHT_DECLARE_CLASS(IParameter, viz::scene3d::adaptor);

    /**
     * @name Slots API
     * @{
     */

    VIZ_SCENE3D_API static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_COLOR_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_DOUBLE2_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_DOUBLE3_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_INT_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_INT2_PARAMETER_SLOT;
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_INT3_PARAMETER_SLOT;

    ///@}

    VIZ_SCENE3D_API static constexpr std::string_view s_PARAMETER_INOUT = "parameter";

    /// Constructor.
    VIZ_SCENE3D_API IParameter() noexcept;

    /// Destructor. Does nothing
    VIZ_SCENE3D_API ~IParameter() noexcept override;

    /// Set the shaderType by passing the value of the ShaderEnumType of this adaptor.
    VIZ_SCENE3D_API void setShaderType(Ogre::GpuProgramType shaderType);

    /// Set the name of the parameter m_paramName.
    VIZ_SCENE3D_API void setParamName(const std::string& paramName);

    /// Get the name of the parameter m_paramName.
    VIZ_SCENE3D_API const std::string& getParamName() const;

    /// Get the name of the parameter m_paramName.
    VIZ_SCENE3D_API const std::string& getDefaultValue() const;

    /// Inform that the parameter value has changed. Its value will be uploaded on next update
    void setDirty();

protected:

    /// Configure the adaptor
    VIZ_SCENE3D_API void configuring() override;

    /// Updates the shader parameter values and requests a render of the scene.
    VIZ_SCENE3D_API void updating() override;

    /// Clear the mesh pointer.
    VIZ_SCENE3D_API void stopping() override;

    /// Set the parameter for a given technique
    VIZ_SCENE3D_API virtual bool setParameter(Ogre::Technique& technique);

    /// Set the material to update
    VIZ_SCENE3D_API void setMaterial(const Ogre::MaterialPtr& material);

    /// Connect the input parameter modified signal to this service update slot.
    VIZ_SCENE3D_API service::connections_t auto_connections() const override;

private:

    /// SLOT : Set the uniform from an integer value
    void setBoolParameter(bool value, std::string name);

    /// SLOT : Set the uniform from a color value
    void setColorParameter(std::array<std::uint8_t, 4> color, std::string name);

    /// SLOT : Set the uniform from an integer value
    void setIntParameter(int value, std::string name);

    /// SLOT : Set the uniform from an integer value
    void setInt2Parameter(int value1, int value2, std::string name);

    /// SLOT : Set the uniform from an integer value
    void setInt3Parameter(int value1, int value2, int value3, std::string name);

    /// SLOT : Set the uniform from an double value
    void setDoubleParameter(double value, std::string name);

    /// SLOT : Set the uniform from an double value
    void setDouble2Parameter(double value1, double value2, std::string name);

    /// SLOT : Set the uniform from an double value
    void setDouble3Parameter(double value1, double value2, double value3, std::string name);

    /// Parameter name
    std::string m_paramName;
    /// Technique name
    std::string m_techniqueName;
    /// Parameter name
    std::string m_defaultValue;
    /// Stores the value of the enum representing the shader's type.
    Ogre::GpuProgramType m_shaderType {Ogre::GPT_FRAGMENT_PROGRAM};
    /// Pointer on the material
    Ogre::MaterialPtr m_material;
    /// Pointer on a texture if the parameter is an image
    sight::viz::scene3d::Texture::sptr m_texture;
    /// Dirty flag to know if we must trigger an update or not
    bool m_dirty {true};

    data::ptr<data::object, data::Access::inout> m_parameter {this, s_PARAMETER_INOUT, true};
};

//------------------------------------------------------------------------------

inline void IParameter::setDirty()
{
    m_dirty = true;
}

} // namespace sight::viz::scene3d
