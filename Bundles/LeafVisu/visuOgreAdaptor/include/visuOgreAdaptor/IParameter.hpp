/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_IPARAMETER_HPP__
#define __VISUOGREADAPTOR_IPARAMETER_HPP__

#include <fwRenderOgre/IAdaptor.hpp>

#include "visuOgreAdaptor/config.hpp"

namespace visuOgreAdaptor
{

/**
 * @brief   Send a FW4SPL data as a shader parameter
 *
 *
 * Send parameters to vertex and fragment shaders
 *
 * @code{.xml}
        <service uid="paramAdaptor" class="::visuOgreAdaptor::IParameter">
            <config materialAdaptor="mtlAdaptorUID" parameter="u_value" shaderType="fragment" />
        </service>
       @endcode
 *  - \b materialName (mandatory) : the name of the associated Ogre material
 *  - \b parameter (mandatory) : name of the shader parameter to set
 *  - \b technique (optional) : name of the technique, default to the first in the material
 *  - \b shaderType (optional) : the type of the shader (vertex, geometry, fragment). Default to vertex.
 */
class VISUOGREADAPTOR_CLASS_API IParameter : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro ( (IParameter)(::fwRenderOgre::IAdaptor) );

    /// Enum containing the different values for the supported shader types.
    typedef enum ShaderEnum
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY
    } ShaderEnumType;

    /**
     * @name Slots API
     * @{
     */

    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;

    ///@}

    /// Constructor.
    VISUOGREADAPTOR_API IParameter() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~IParameter() throw();

    /// Set the shaderType by passing the value of the ShaderEnumType of this adaptor.
    VISUOGREADAPTOR_API void setShaderType(ShaderEnumType shaderType);

    /// Set the shaderType by passing the name of the ShaderEnumType of this adaptor (vp or fp).
    VISUOGREADAPTOR_API void setShaderType(std::string shaderType);

    /// Set the name of the parameter m_paramName.
    VISUOGREADAPTOR_API void setParamName(std::string paramName);

    /// Get the name of the parameter m_paramName.
    VISUOGREADAPTOR_API const std::string& getParamName() const;

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API virtual void doConfigure()  throw ( ::fwTools::Failed );

    /// Updates the shader parameter values and requests a render of the scene.
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );

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

    /// SLOT : Set the uniform from an float value
    void setFloatParameter(float value, std::string name);

    /// SLOT : Set the uniform from an double value
    void setDoubleParameter(double value, std::string name);

    /// Parameter name
    std::string m_paramName;
    /// Technique name
    std::string m_techniqueName;
    /// Stores the value of the enum representing the shader's type.
    ShaderEnumType m_shaderType;
    /// Pointer on the material
    ::Ogre::MaterialPtr m_material;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_IPARAMETER_HPP__
