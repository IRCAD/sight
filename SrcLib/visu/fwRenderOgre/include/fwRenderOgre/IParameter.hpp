/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IAdaptor.hpp"

#include <OGRE/OgreTexture.h>

namespace fwRenderOgre
{

/**
 * @brief   Base interface containing common code to upload shader parameters from FW4SPL data.
 */
class FWRENDEROGRE_CLASS_API IParameter : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IParameter)(::fwRenderOgre::IAdaptor) )

    /**
     * @name Slots API
     * @{
     */

    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE2_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE3_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_INT2_PARAMETER_SLOT;
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_SET_INT3_PARAMETER_SLOT;

    ///@}

    /// Constructor.
    FWRENDEROGRE_API IParameter() noexcept;

    /// Destructor. Does nothing
    FWRENDEROGRE_API virtual ~IParameter() noexcept;

    /// Set the shaderType by passing the value of the ShaderEnumType of this adaptor.
    FWRENDEROGRE_API void setShaderType(::Ogre::GpuProgramType shaderType);

    /// Set the name of the parameter m_paramName.
    FWRENDEROGRE_API void setParamName(const std::string& paramName);

    /// Get the name of the parameter m_paramName.
    FWRENDEROGRE_API const std::string& getParamName() const;

    /// Get the name of the parameter m_paramName.
    FWRENDEROGRE_API const std::string& getDefaultValue() const;

    /// Inform that the parameter value has changed. Its value will be uploaded on next update
    void setDirty();

protected:

    /// Configure the adaptor
    FWRENDEROGRE_API virtual void configuring() override;

    /// Updates the shader parameter values and requests a render of the scene.
    FWRENDEROGRE_API virtual void updating() override;

    /// Clear the mesh pointer.
    FWRENDEROGRE_API virtual void stopping() override;

    /// Set the parameter for a given technique
    FWRENDEROGRE_API virtual bool setParameter(::Ogre::Technique& technique);

    /// Set the material to update
    FWRENDEROGRE_API void setMaterial(const ::Ogre::MaterialPtr& material);

    /// Connect the input parameter modified signal to this service update slot.
    FWRENDEROGRE_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

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
    ::Ogre::GpuProgramType m_shaderType;
    /// Pointer on the material
    ::Ogre::MaterialPtr m_material;
    /// Pointer on a texture if the parameter is an image
    ::Ogre::TexturePtr m_texture;
    /// Dirty flag to know if we must trigger an update or not
    bool m_dirty;
};

//------------------------------------------------------------------------------

inline void IParameter::setDirty()
{
    m_dirty = true;
}

} // fwRenderOgre
