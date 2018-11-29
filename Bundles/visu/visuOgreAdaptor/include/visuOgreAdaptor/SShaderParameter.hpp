/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IParameter.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Send a FW4SPL data as a shader parameter.
 *
 * Send parameters to vertex and fragment shaders
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
    <service uid="paramAdaptor" type="::visuOgreAdaptor::SShaderParameter">
        <inout key="parameter" uid="collimation" />
        <config materialName="mat" parameter="u_value" shaderType="fragment" />
    </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b parameter [::fwData::Object]: parameter containing the data to upload.
 * @subsection Configuration Configuration:
 * - \b materialName (mandatory) : the name of the associated Ogre material
 * - \b parameter (mandatory) : name of the shader parameter to set
 * - \b technique (optional) : name of the technique, default to the first in the material
 * - \b shaderType (optional) : the type of the shader (vertex, geometry, fragment). Default to vertex.
 */
class VISUOGREADAPTOR_CLASS_API SShaderParameter : public ::fwRenderOgre::IParameter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SShaderParameter)(::fwRenderOgre::IParameter) )

    /// Constructor.
    VISUOGREADAPTOR_API SShaderParameter() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SShaderParameter() noexcept;

    /// Set the value of the member m_materialName.
    VISUOGREADAPTOR_API void setMaterialName(const std::string& matName);

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API virtual void configuring() final override;
    /// Do nothing
    VISUOGREADAPTOR_API virtual void starting() final override;
    /// Do nothing
    VISUOGREADAPTOR_API virtual void stopping() final override;
    /// Do nothing
    VISUOGREADAPTOR_API virtual void swapping() final override;

private:

    /// Material name
    std::string m_materialName;
};

} // visuOgreAdaptor
