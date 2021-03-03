/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "modules/filter/mesh/config.hpp"

#include <core/macros.hpp>

#include <data/Mesh.hpp>

#include <service/ISimulator.hpp>

#include <boost/multi_array.hpp>

namespace sight::data
{
class Mesh;
}

namespace sight::module::filter::mesh
{
namespace generator
{
/**
 * @brief Generate the mesh used to display the ultrasound image.
 *
 * @section Slots Slots
 * - \b setIntParameter(int, std::string): Called to modify the simulation parameters.
 *   - depth: Set the depth of the US plane.
 *   - angle: Set angle of the US beam.
 *   - width: Set the width of the US plane.
 *   - deltaDepth: Set the delta depth of the US plane.
 * - \b setBoolParameter(bool, std::string): Called to modify the simulation parameters.
 *   - shape: Use a rectangle or an US beam.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::filter::mesh::generator::SUltrasoundMesh">
       <inout key="mesh" uid="..." />
        <config deltaDepth="50" depth="132" width="90" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b mesh [sight::data::Mesh] : the generate US plane.
 *
 * @subsection Configuration Configuration:
 * - \b resolutionX (optional, default=25u) : resolution in X of the US plane.
 * - \b resolutionY (optional, default=25u) : resolution in Y of the US plane.
 * - \b depth (optional, default=132) : depth of the US plane.
 * - \b width (optional, default=90) : width of the US plane.
 * - \b angle (optional, default=60) : angle of the US beam.
 * - \b deltaDepth (optional, default=50) : delta depth of the US plane.
 */
class MODULE_FILTER_MESH_CLASS_API SUltrasoundMesh final : public service::ISimulator
{

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SUltrasoundMesh, service::IService)

    /// Create slots.
    MODULE_FILTER_MESH_API SUltrasoundMesh() noexcept;

    /// Destroys the service.
    MODULE_FILTER_MESH_API ~SUltrasoundMesh() override;

protected:

    /// Configures the service.
    MODULE_FILTER_MESH_API void configuring() override;

    /// Generate the mesh.
    MODULE_FILTER_MESH_API void starting() override;

    /// Generate and update the mesh.
    MODULE_FILTER_MESH_API void updating() override;

    /// Does nothing.
    MODULE_FILTER_MESH_API void stopping() override;

private:

    /// Updates the mesh points position.
    void updateMeshPosition();

    /**
     * @brief Creates the mesh.
     * @param _mesh the data used to store information.
     */
    void createQuadMesh(const data::Mesh::sptr& _mesh) const;

    /**
     * @brief Updates the mesh.
     * @param _mesh the data used to store information.
     */
    void updateQuadMesh(const data::Mesh::sptr& _mesh);

    /**
     * @brief Sets an integer parameter.
     * @param _val value to set.
     * @param _key the key used to select the member to change.
     */
    void setIntParameter(int _val, std::string _key);

    /**
     * @brief Sets a boolean parameter.
     * @param _val value to set.
     * @param _key the key used to select the member to change.
     */
    void setBoolParameter(bool _val, std::string _key);

    /// Defines the resolution in X.
    size_t m_resolutionX {25u};

    /// Defines the resolution in Y.
    size_t m_resolutionY {25u};

    /// Defines the depth of the US plane.
    int m_depth {132};

    /// Defines the angle of the US plane.
    int m_angle {60};

    /// Defines the width of the US plane.
    int m_width {90};

    /// Defines the delta depth of the US plane.
    int m_deltaDepth {50};

    /// Defines the shape of the US plane, rectangle or US beam.
    bool m_shape {false};

    /// Stores mesh positions.
    ::boost::multi_array<float, 3> m_meshPositionArray;

};
} // namespace generator.
} // namespace sight::module::filter::mesh.
