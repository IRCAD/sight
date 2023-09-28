/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/geometry/__/config.hpp"

#include <data/Matrix4.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry
{

/**
 * @brief  Switches between several matrices.
 *
 * @section Slots Slots
 * - \b switchMatrix() : Each time the slot switchMatrix() is called the next matrix given in the configuration is
 * copied on the matrix bound
 * to the service
 * - \b switchToMatrix(int index) : switch to the matrix at the given index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::SSwitchMatrices" autoConnect="true">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [sight::data::Matrix4]: List of two matrices keys to switch.
 * @subsection In-Out In-Out:
 * - \b output [sight::data::Matrix4]: Output matrix.
 *
 */
class MODULE_GEOMETRY_CLASS_API SSwitchMatrices : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(SSwitchMatrices, service::controller);

    /**
     * @brief Constructor.
     */
    MODULE_GEOMETRY_API SSwitchMatrices() noexcept;

    /**
     * @brief Destructor.
     */
    ~SSwitchMatrices() noexcept override =
        default;

    MODULE_GEOMETRY_API static const core::com::slots::key_t SWITCH_SLOT;
    typedef core::com::slot<void ()> SwitchSlotType;

    MODULE_GEOMETRY_API static const core::com::slots::key_t SWITCH_TO_SLOT;
    typedef core::com::slot<void (int)> SwitchToSlotType;

protected:

    /// This method is used to configure the service.
    MODULE_GEOMETRY_API void configuring() override;

    /// This method is used to initialize the service.
    MODULE_GEOMETRY_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_API void stopping() override;

    /// Does nothing.
    MODULE_GEOMETRY_API void updating() override;

    /// Switch to next Matrix
    MODULE_GEOMETRY_API void switchMatrix();

    /// Switch to Matrix "index"
    MODULE_GEOMETRY_API void switchToMatrix(int index);

    MODULE_GEOMETRY_API connections_t getAutoConnections() const override;

private:

    std::size_t m_indexOfDesiredMatrix {0};

    static constexpr std::string_view s_MATRIX_INPUT  = "matrix";
    static constexpr std::string_view s_MATRIX_OUTPUT = "output";

    data::ptr_vector<data::Matrix4, data::Access::in> m_matrix {this, s_MATRIX_INPUT, true};
    data::ptr<data::Matrix4, data::Access::inout> m_output {this, s_MATRIX_OUTPUT};
};

} //namespace sight::module::geometry
