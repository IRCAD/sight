/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SCOMPOSITORPARAMETER_HPP__
#define __VISUOGREADAPTOR_SCOMPOSITORPARAMETER_HPP__

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/IParameter.hpp"

namespace visuOgreAdaptor
{

class CompositorListener;

/**
 * @brief   Binds a FW4SPL data to a shader uniform from a specific compositor.
 *
 * @section Slots Slots
 *  - \b setBoolParameter(bool, std::string): Set the uniform from an integer value.
 *  - \b setColorParameter(std::array<std::uint8_t, 4>, std::string): Set the uniform from a color value.
 *  - \b setIntParameter(int, std::string): Set the uniform from an integer value.
 *  - \b setInt2Parameter(int, int, std::string): Set the uniform from two integer values.
 *  - \b setInt3Parameter(int, int, int, std::string): Set the uniform from three integer values.
 *  - \b setFloatParameter(float, std::string): Set the uniform from an float value.
 *  - \b setDoubleParameter(double, std::string): Set the uniform from an double value.
 *  - \b setDouble2Parameter(double, double, std::string): Set the uniform from two double values.
 *  - \b setDouble3Parameter(double, double, double, std::string): Set the uniform from three double values.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SCompositorParameter">
        <inout key="parameter" uid="collimation" />
        <config layer="..." compositorName="compositor" parameter="u_value" shaderType="fragment" />
    </service>
   @endcode
 * @subsection InOut InOut:
 * - \b parameter [::fwData::Object]: parameter containing the data to upload.
 * @subsection Configuration Configuration:
 * - \b compositorName (mandatory) : the name of the associated Ogre compositor
 * - \b parameter (mandatory) : name of the shader parameter to set
 * - \b technique (optional) : name of the technique, default to the first in the compositor
 * - \b shaderType (optional) : the type of the shader (vertex, geometry, fragment). Default to fragment.
 */
class VISUOGREADAPTOR_CLASS_API SCompositorParameter : public ::visuOgreAdaptor::IParameter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SCompositorParameter)(::visuOgreAdaptor::IParameter) );

    /// Constructor.
    VISUOGREADAPTOR_API SCompositorParameter() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SCompositorParameter() noexcept;

    /// Updates parameter according to the attached fwData::Object
    VISUOGREADAPTOR_API void updateValue(::Ogre::MaterialPtr& _mat);

    /// Return the compositor name
    const std::string& getCompositorName() const;

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API virtual void configuring() final override;
    /// Do nothing
    VISUOGREADAPTOR_API virtual void starting() final override;
    /// Do nothing
    VISUOGREADAPTOR_API virtual void stopping() final override;

private:

    /// Material name
    std::string m_compositorName;

    /// Ogre listener, we need to keep a pointer to unregister it
    CompositorListener* m_listener;

    /// Pointer on the compositor we work on
    ::Ogre::CompositorInstance* m_compositor;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SCOMPOSITORPARAMETER_HPP__
