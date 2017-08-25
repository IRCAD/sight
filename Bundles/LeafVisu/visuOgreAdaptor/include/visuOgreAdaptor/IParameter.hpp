/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_IPARAMETER_HPP__
#define __VISUOGREADAPTOR_IPARAMETER_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Base interface containing common code to upload shader parameters from FW4SPL data.
 */
class VISUOGREADAPTOR_CLASS_API IParameter : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IParameter)(::fwRenderOgre::IAdaptor) );

    /**
     * @name Slots API
     * @{
     */

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE2_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE3_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_INT2_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_INT3_PARAMETER_SLOT;

    ///@}

    /// Constructor.
    VISUOGREADAPTOR_API IParameter() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~IParameter() noexcept;

    /// Set the shaderType by passing the value of the ShaderEnumType of this adaptor.
    VISUOGREADAPTOR_API void setShaderType(::Ogre::GpuProgramType shaderType);

    /// Set the name of the parameter m_paramName.
    VISUOGREADAPTOR_API void setParamName(const std::string& paramName);

    /// Get the name of the parameter m_paramName.
    VISUOGREADAPTOR_API const std::string& getParamName() const;

    /// Get the name of the parameter m_paramName.
    VISUOGREADAPTOR_API const std::string& getDefaultValue() const;

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API virtual void configuring() override;

    /// Updates the shader parameter values and requests a render of the scene.
    VISUOGREADAPTOR_API virtual void updating() override;

    /// Clear the mesh pointer.
    VISUOGREADAPTOR_API virtual void stopping() override;

    /// Set the parameter for a given technique
    VISUOGREADAPTOR_API virtual bool setParameter(::Ogre::Technique& technique);

    /// Set the material to update
    VISUOGREADAPTOR_API void setMaterial(const::Ogre::MaterialPtr& material);

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
    /// Dirty flag to know if we must trigger an update or not
    bool m_dirty;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_IPARAMETER_HPP__
