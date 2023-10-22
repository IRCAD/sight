/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "tool_calibration.hpp"

#include <geometry/vision/helper.hpp>

#include <service/macros.hpp>

namespace sight::module::navigation::calibration
{

static const service::base::key_t s_MATRIX_CALIBRATION_OUTPUT = "matrixCalibration";

// -----------------------------------------------------------------------------

tool_calibration::tool_calibration() noexcept =
    default;

// -----------------------------------------------------------------------------

tool_calibration::~tool_calibration() noexcept =
    default;

// -----------------------------------------------------------------------------

void tool_calibration::configuring()
{
    const auto config_tree = this->get_config();
    const auto outputs     = config_tree.equal_range("out");
    for(auto it = outputs.first ; it != outputs.second ; ++it)
    {
        const auto key = it->second.get<std::string>("<xmlattr>.key");
        if(key == s_MATRIX_CENTER_OUTPUT)
        {
            m_hasOutputCenter = true;
        }
    }
}

// -----------------------------------------------------------------------------

void tool_calibration::starting()
{
}

// -----------------------------------------------------------------------------

void tool_calibration::stopping()
{
    m_matrixCalibration = nullptr;
    if(m_hasOutputCenter)
    {
        m_matrixCenter = nullptr;
    }
}

// -----------------------------------------------------------------------------

void tool_calibration::updating()
{
}

// -----------------------------------------------------------------------------

void tool_calibration::computeRegistration(core::hires_clock::type /*timestamp*/)
{
    const auto matrices_vector = m_matricesVector.lock();

    data::matrix4::sptr calibration_matrix = std::make_shared<data::matrix4>();

    data::matrix4::sptr center_matrix_no_rot = std::make_shared<data::matrix4>();

    geometry::vision::helper::calibrate_pointing_tool(
        matrices_vector.get_shared(),
        calibration_matrix,
        center_matrix_no_rot
    );

    m_matrixCalibration = calibration_matrix;

    if(m_hasOutputCenter)
    {
        m_matrixCenter = center_matrix_no_rot;
    }
}

} // namespace sight::module::navigation::calibration
