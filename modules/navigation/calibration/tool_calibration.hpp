/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/navigation/calibration/config.hpp"

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
class MODULE_NAVIGATION_CALIBRATION_CLASS_API tool_calibration : public service::registerer
{
public:

    SIGHT_DECLARE_SERVICE(tool_calibration, sight::service::registerer);

    /**
     * @brief Constructor.
     */
    MODULE_NAVIGATION_CALIBRATION_API tool_calibration() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_NAVIGATION_CALIBRATION_API ~tool_calibration() noexcept override;

protected:

    /// Configures the service
    MODULE_NAVIGATION_CALIBRATION_API void configuring() override;

    /// Does nothing
    MODULE_NAVIGATION_CALIBRATION_API void starting() override;

    /// Does nothing
    MODULE_NAVIGATION_CALIBRATION_API void updating() override;

    /// Does nothing
    MODULE_NAVIGATION_CALIBRATION_API void stopping() override;

    /// Registers matrix slot
    MODULE_NAVIGATION_CALIBRATION_API void computeRegistration(core::hires_clock::type _timestamp)
    override;

private:

    bool m_hasOutputCenter {false};

    static constexpr std::string_view s_MATRIX_CENTER_OUTPUT      = "matrixCenter";
    static constexpr std::string_view s_MATRIX_CALIBRATION_OUTPUT = "matrixCalibration";
    static constexpr std::string_view s_MATRICES_VECTOR_INPUT     = "matricesVector";

    sight::data::ptr<sight::data::matrix4, sight::data::Access::out> m_matrixCenter {this, s_MATRIX_CENTER_OUTPUT,
                                                                                     false, true
    };
    sight::data::ptr<sight::data::matrix4, sight::data::Access::out> m_matrixCalibration {this,
                                                                                          s_MATRIX_CALIBRATION_OUTPUT
    };
    sight::data::ptr<sight::data::vector, sight::data::Access::in> m_matricesVector {this, s_MATRICES_VECTOR_INPUT};
};

} // namespace sight::module::navigation::calibration
