/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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
#include <data/vector.hpp>

#include <service/registerer.hpp>

namespace sight::module::navigation::calibration
{

/**
 * @brief Service that computes a pivot calibration
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::navigation::calibration::tool_calibration" >
        <in key="matricesVector" uid="..." />
        <out key="matrixCalibration" uid="..." />
        <out key="matrixCenter" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b matricesVector [sight::data::vector]: vector of matrices corresponding to points on a sphere
 *
 * @subsection Output Output
 * - \b matrixCalibration [sight::data::matrix4]: tool calibration
 * - \b matrixCenter [sight::data::matrix4] (optional): matrix representing the point around which the
 * tool
 * is moved
 *
 */
class tool_calibration : public service::registerer
{
public:

    SIGHT_DECLARE_SERVICE(tool_calibration, sight::service::registerer);

    /**
     * @brief Constructor.
     */
    tool_calibration() noexcept;

    /**
     * @brief Destructor.
     */
    ~tool_calibration() noexcept override;

protected:

    /// Configures the service
    void configuring() override;

    /// Does nothing
    void starting() override;

    /// Does nothing
    void updating() override;

    /// Does nothing
    void stopping() override;

    /// Registers matrix slot
    void compute_registration(core::clock::type _timestamp)
    override;

private:

    bool m_has_output_center {false};

    static constexpr std::string_view MATRIX_CENTER_OUTPUT      = "matrixCenter";
    static constexpr std::string_view MATRIX_CALIBRATION_OUTPUT = "matrixCalibration";
    static constexpr std::string_view MATRICES_VECTOR_INPUT     = "matricesVector";

    sight::data::ptr<sight::data::matrix4, sight::data::access::out> m_matrix_center {this, MATRIX_CENTER_OUTPUT,
                                                                                      false, true
    };
    sight::data::ptr<sight::data::matrix4, sight::data::access::out> m_matrix_calibration {this,
                                                                                           MATRIX_CALIBRATION_OUTPUT
    };
    sight::data::ptr<sight::data::vector, sight::data::access::in> m_matrices_vector {this, MATRICES_VECTOR_INPUT};
};

} // namespace sight::module::navigation::calibration
