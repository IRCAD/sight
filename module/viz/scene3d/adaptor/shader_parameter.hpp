/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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
 * @brief This adaptor sends a Sight data as a shader parameter. It can bind
 *
 * Send parameters to vertex and fragment shaders.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="paramAdaptor" type="sight::module::viz::scene3d::adaptor::shader_parameter">
        <inout key="parameter" uid="..." />
        <config materialName="mat" parameter="u_value" shader_type="fragment" />
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
 * - \b shader_type (optional, vertex/geometry/fragment, default=vertex): the type of the shader.
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

    /// Clears resources.
    void stopping() final;

private:

    /// Defines the material name.
    std::string m_material_name;
};

} // namespace sight::module::viz::scene3d::adaptor.
