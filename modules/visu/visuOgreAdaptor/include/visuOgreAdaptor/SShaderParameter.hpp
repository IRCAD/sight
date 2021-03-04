/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IParameter.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor sends a Sight data as a shader parameter.
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
 *
 * @subsection In-Out In-Out:
 * - \b parameter [::fwData::Object]: parameter containing the data to upload.
 *
 * @subsection Configuration Configuration:
 * - \b materialName (mandatory, string): the name of the associated Ogre material
 * - \b parameter (mandatory, string): name of the shader parameter to set
 * - \b technique (optional, string, default=""): name of the technique, default to the first in the material
 * - \b shaderType (optional, vertex/geometry/fragment, default=vertex): the type of the shader.
 */
class VISUOGREADAPTOR_CLASS_API SShaderParameter final : public ::fwRenderOgre::IParameter
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SShaderParameter, ::fwRenderOgre::IParameter)

    /// Creates the adaptor.
    VISUOGREADAPTOR_API SShaderParameter() noexcept;

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API ~SShaderParameter() noexcept override;

protected:

    /// Configures the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Finds the material on which this service works.
    VISUOGREADAPTOR_API void starting() override;

    /// Updates the shader parameter with the input.
    VISUOGREADAPTOR_API void updating() override;

    /// Does nothing.
    VISUOGREADAPTOR_API void swapping() override;

    /// Clears resources.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Defines the material name.
    std::string m_materialName;
};

} // namespace visuOgreAdaptor.
