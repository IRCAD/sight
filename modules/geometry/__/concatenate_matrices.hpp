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

#include <core/base.hpp>

#include <data/matrix4.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry
{

/**
 * @brief   This service concatenates N matrices together.
 *
 * The computation of the concatenation is triggered when updating the service.
 * Auto-connections to the matrices can also be used to force the service to recompute the output matrix.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::concatenate_matrices">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." inverse="true"/>
                <key uid="..." auto_connect="true"/>
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [sight::data::matrix4]: List of matrix keys to concatenate. For each input matrix, it is
 * possible to invert it before multiplying with it by specifying \b inverse="true".
 * The auto_connect is connected to the update slot, thus is will trigger a new concatenation.
 * @subsection In-Out In-Out:
 * - \b output [sight::data::matrix4]: Output matrix.
 */

class concatenate_matrices : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(concatenate_matrices, sight::service::controller);

    concatenate_matrices() noexcept = default;
    ~concatenate_matrices() noexcept override = default;

protected:

    /// This method is used to configure the service.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Concatenates the matrices
    void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Matrix4::MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    using invert_vector_t = std::vector<bool>;

    /// Vector to specify if matrix must be inverted.
    invert_vector_t m_invert_vector;

    static constexpr std::string_view MATRIX_GROUP_INPUT = "matrix";
    data::ptr_vector<data::matrix4, sight::data::access::in> m_matrices {this, MATRIX_GROUP_INPUT};

    static constexpr std::string_view OUTPUT = "output";
    data::ptr<data::matrix4, sight::data::access::inout> m_output {this, OUTPUT};
};

} //namespace sight::module::geometry
