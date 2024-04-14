/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <viz/scene3d/parameter_adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor sends a Sight data as a shader parameter.
 *
 * Send parameters to vertex and fragment shaders
 *
 * @section Slots Slots
 *  - \b set_bool_parameter(bool, std::string): Set the uniform from an integer value.
 *  - \b set_color_parameter(std::array<std::uint8_t, 4>, std::string): Set the uniform from a color value.
 *  - \b set_int_parameter(int, std::string): Set the uniform from an integer value.
 *  - \b set_int2_parameter(int, int, std::string): Set the uniform from two integer values.
 *  - \b set_int3_parameter(int, int, int, std::string): Set the uniform from three integer values.
 *  - \b setFloatParameter(float, std::string): Set the uniform from an float value.
 *  - \b set_double_parameter(double, std::string): Set the uniform from an double value.
 *  - \b set_double2_parameter(double, double, std::string): Set the uniform from two double values.
 *  - \b set_double3_parameter(double, double, double, std::string): Set the uniform from three double values.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="paramAdaptor" type="sight::module::viz::scene3d::adaptor::shader_parameter">
        <inout key="parameter" uid="collimation" />
        <config materialName="mat" parameter="u_value" shaderType="fragment" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b parameter [sight::data::object]: parameter containing the data to upload.
 *
 * @subsection Configuration Configuration:
 * - \b materialName (mandatory, string): the name of the associated Ogre material
 * - \b parameter (mandatory, string): name of the shader parameter to set
 * - \b technique (optional, string, default=""): name of the technique, default to the first in the material
 * - \b shaderType (optional, vertex/geometry/fragment, default=vertex): the type of the shader.
 */
class shader_parameter final : public sight::viz::scene3d::parameter_adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(shader_parameter, sight::viz::scene3d::parameter_adaptor);

    /// Creates the adaptor.
    shader_parameter() noexcept = default;

    /// Destroys the adaptor.
    ~shader_parameter() noexcept final = default;

protected:

    /// Configures the adaptor.
    void configuring() final;

    /// Finds the material on which this service works.
    void starting() final;

    /// Updates the shader parameter with the input.
    void updating() final;

    /// Clears resources.
    void stopping() final;

private:

    /// Defines the material name.
    std::string m_material_name;
};

} // namespace sight::module::viz::scene3d::adaptor.
