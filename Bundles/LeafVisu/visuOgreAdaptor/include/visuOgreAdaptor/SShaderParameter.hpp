/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SSHADERPARAMETER_HPP__
#define __VISUOGREADAPTOR_SSHADERPARAMETER_HPP__



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
        <service uid="paramAdaptor" class="::visuOgreAdaptor::SShaderParameter">
            <config materialAdaptor="mtlAdaptorUID" parameter="u_value" shaderType="fragment" />
        </service>
       @endcode
 *  - \b materialName (mandatory) : the name of the associated Ogre material
 *  - \b parameter (mandatory) : name of the shader parameter to set
 *  - \b technique (optional) : name of the technique, default to the first in the material
 *  - \b shaderType (optional) : the type of the shader (vertex, geometry, fragment). Default to vertex.
 */
class VISUOGREADAPTOR_CLASS_API SShaderParameter : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SShaderParameter)(::fwRenderOgre::IAdaptor) );

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

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_FLOAT_PARAMETER_SLOT;

    ///@}

    /// Constructor.
    VISUOGREADAPTOR_API SShaderParameter() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SShaderParameter() throw();

    /// Sets the shaderType by passing the value of the ShaderEnumType of this adaptor.
    VISUOGREADAPTOR_API void setShaderType(ShaderEnumType shaderType);

    /// Sets the shaderType by passing the name of the ShaderEnumType of this adaptor (vp or fp).
    VISUOGREADAPTOR_API void setShaderType(std::string shaderType);

    /// Sets the value of the member m_materialName.
    VISUOGREADAPTOR_API void setMaterialName(std::string matName);

    /// Sets the name of the parameter m_paramName.
    VISUOGREADAPTOR_API void setParamName(std::string paramName);

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API virtual void doConfigure()  throw ( ::fwTools::Failed );
    /// Do nothing
    VISUOGREADAPTOR_API virtual void doStart()  throw ( ::fwTools::Failed );
    /// Do nothing
    VISUOGREADAPTOR_API virtual void doStop()  throw ( ::fwTools::Failed );
    /// Do nothing
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );
    /// Updates the shaderparameter values via the private method updateValue(), and requests a render of the scene.
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );

private:

    /**
     * @brief updateValue(), updates parameters function of the attached fwData::Object
     * Updates some ::Ogre::GpuProgramParametersSharedPtr
     * and directly sends them to the programmable pipeline of the GPU.
     *
     * - \b m_paramvalues is an array filled with the corresponding data
     * - \b m_paramType is the type (from the enum Types) of the data which values are stored in m_paramValues.
     */
    void updateValue(const fwData::Object::sptr& paramObject);

    /// Set the parameter for a given technique
    bool setParameter(::Ogre::Technique& technique, const ::fwData::Object::sptr& paramObject);

    /// SLOT : Set the uniform from an integer value
    void setIntParameter(int value);

    /// SLOT : Set the uniform from an float value
    void setFloatParameter(float value);

    /// Material name
    std::string m_materialName;
    /// Parameter name
    std::string m_paramName;
    /// Technique name
    std::string m_techniqueName;
    /// Stores the value of the enum representing the shader's type.
    ShaderEnumType m_shaderType;
    /// Dummy object to store the value when we use signal/slot instead of a real object
    ::fwData::Object::sptr m_paramObject;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SSHADERPARAMETER_HPP__
