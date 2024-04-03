/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <data/matrix4.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry
{

/**
 * @brief  Switches between several matrices.
 *
 * @section Slots Slots
 * - \b switch_matrix() : Each time the slot switchMatrix() is called the next matrix given in the configuration is
 * copied on the matrix bound
 * to the service
 * - \b switch_to_matrix(int index) : switch to the matrix at the given index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::switch_matrices" auto_connect="true">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [sight::data::matrix4]: List of two matrices keys to switch.
 * @subsection In-Out In-Out:
 * - \b output [sight::data::matrix4]: Output matrix.
 *
 */
class switch_matrices : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(switch_matrices, service::controller);

    /**
     * @brief Constructor.
     */
    switch_matrices() noexcept;

    /**
     * @brief Destructor.
     */
    ~switch_matrices() noexcept override =
        default;

    static const core::com::slots::key_t SWITCH_SLOT;
    using switch_slot_t = core::com::slot<void ()>;

    static const core::com::slots::key_t SWITCH_TO_SLOT;
    using switch_to_slot_t = core::com::slot<void (int)>;

protected:

    /// This method is used to configure the service.
    void configuring() override;

    /// This method is used to initialize the service.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Does nothing.
    void updating() override;

    /// Switch to next Matrix
    void switch_matrix();

    /// Switch to Matrix "index"
    void switch_to_matrix(int _index);

    connections_t auto_connections() const override;

private:

    std::size_t m_index_of_desired_matrix {0};

    static constexpr std::string_view MATRIX_INPUT  = "matrix";
    static constexpr std::string_view MATRIX_OUTPUT = "output";

    data::ptr_vector<data::matrix4, data::access::in> m_matrix {this, MATRIX_INPUT, true};
    data::ptr<data::matrix4, data::access::inout> m_output {this, MATRIX_OUTPUT};
};

} //namespace sight::module::geometry
