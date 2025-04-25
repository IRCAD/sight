/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include <core/macros.hpp>

#include <data/mesh.hpp>

#include <service/generator.hpp>

#include <boost/multi_array.hpp>

namespace sight::data
{

class mesh;

} // namespace sight::data

namespace sight::module::filter::mesh::generator
{

/**
 * @brief Generates the mesh used to display the ultrasound image.
 *
 * @section Slots Slots
 * - \b set_int_parameter(int, std::string): Called to modify the simulation parameters.
 *   - depth: Sets the depth of the US plane.
 *   - angle: Sets angle of the US beam.
 *   - width: Sets the width of the US plane.
 *   - deltaDepth: Sets the delta depth of the US plane.
 * - \b set_bool_parameter(bool, std::string): Called to modify the simulation parameters.
 *   - shape: rectangle or US beam.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::filter::mesh::generator::ultrasound_mesh">
       <inout key="mesh" uid="..." />
        <config deltaDepth="50" depth="132" width="90" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b mesh [sight::data::mesh] : the generated US plane.
 *
 * @subsection Configuration Configuration:
 * - \b resolutionX (optional, default=25u) : resolution in X of the US plane.
 * - \b resolutionY (optional, default=25u) : resolution in Y of the US plane.
 * - \b depth (optional, default=132) : depth of the US plane.
 * - \b width (optional, default=90) : width of the US plane.
 * - \b angle (optional, default=60) : angle of the US beam.
 * - \b deltaDepth (optional, default=50) : delta depth of the US plane.
 */
class ultrasound_mesh final : public service::generator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(ultrasound_mesh, service::base);

    /// Create slots.
    ultrasound_mesh() noexcept;

    /// Destroys the service.
    ~ultrasound_mesh() override;

protected:

    /// Configures the service.
    void configuring(const config_t& _config) override;

    /// Generate the mesh.
    void starting() override;

    /// Generate and update the mesh.
    void updating() override;

    /// Does nothing.
    void stopping() override;

private:

    /// Updates the mesh points position.
    void update_mesh_position();

    /**
     * @brief Creates the mesh.
     * @param _mesh the data used to store information.
     */
    void create_quad_mesh(const data::mesh::sptr& _mesh) const;

    /**
     * @brief Updates the mesh.
     * @param _mesh the data used to store information.
     */
    void update_quad_mesh(const data::mesh::sptr& _mesh);

    /**
     * @brief Sets an integer parameter.
     * @param _val value to set.
     * @param _key the key used to select the member to change.
     */
    void set_int_parameter(int _val, std::string _key);

    /**
     * @brief Sets a boolean parameter.
     * @param _val value to set.
     * @param _key the key used to select the member to change.
     */
    void set_bool_parameter(bool _val, std::string _key);

    /// Defines the resolution in X.
    std::size_t m_resolution_x {25U};

    /// Defines the resolution in Y.
    std::size_t m_resolution_y {25U};

    /// Defines the depth of the US plane.
    int m_depth {132};

    /// Defines the angle of the US plane.
    int m_angle {60};

    /// Defines the width of the US plane.
    int m_width {90};

    /// Defines the delta depth of the US plane.
    int m_delta_depth {50};

    /// Defines the shape of the US plane, rectangle or US beam.
    bool m_shape {true};

    /// Stores mesh positions.
    boost::multi_array<float, 3> m_mesh_position_array;

    static constexpr std::string_view MESH_INOUT = "mesh";

    sight::data::ptr<sight::data::mesh, sight::data::access::inout> m_mesh {this, MESH_INOUT};
};

} // namespace sight::module::filter::mesh::generator
