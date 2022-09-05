/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/geometry/base/config.hpp"

#include <core/base.hpp>
#include <core/com/helper/SigSlotConnection.hpp>

#include <data/Matrix4.hpp>

#include <service/IController.hpp>

namespace sight::module::geometry::base
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
        <service uid="..." type="sight::module::geometry::base::SConcatenateMatrices">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." inverse="true"/>
                <key uid="..." autoConnect="true"/>
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [sight::data::Matrix4]: List of matrix keys to concatenate. For each input matrix, it is
 * possible to invert it before multiplying with it by specifying \b inverse="true".
 * The autoConnect is connected to the update slot, thus is will trigger a new concatenation.
 * @subsection In-Out In-Out:
 * - \b output [sight::data::Matrix4]: Output matrix.
 */

class MODULE_GEOMETRY_BASE_CLASS_API SConcatenateMatrices : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SConcatenateMatrices, sight::service::IController);

    /**
     * @brief Constructor.
     */
    MODULE_GEOMETRY_BASE_API SConcatenateMatrices() noexcept;

    /**
     * @brief Destructor.
     */
    ~SConcatenateMatrices() noexcept override =
        default;

protected:

    /// This method is used to configure the service.
    MODULE_GEOMETRY_BASE_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void stopping() override;

    /// Concatenates the matrices
    MODULE_GEOMETRY_BASE_API void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Matrix4::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_GEOMETRY_BASE_API KeyConnectionsMap getAutoConnections() const override;

private:

    typedef std::vector<bool> InvertVectorType;

    /// Vector to specify if matrix must be inverted.
    InvertVectorType m_invertVector;

    static constexpr std::string_view s_MATRIX_GROUP_INPUT = "matrix";
    data::ptr_vector<data::Matrix4, sight::data::Access::in> m_matrices {this, s_MATRIX_GROUP_INPUT, true};

    static constexpr std::string_view s_OUTPUT = "output";
    data::ptr<data::Matrix4, sight::data::Access::inout> m_output {this, s_OUTPUT};
};

} //namespace sight::module::geometry::base
