/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "SToolCalibration.hpp"

#include <geometry/vision/helper.hpp>

#include <service/macros.hpp>

namespace sight::module::navigation::calibration
{

static const service::IService::KeyType s_MATRIX_CENTER_OUTPUT      = "matrixCenter";
static const service::IService::KeyType s_MATRIX_CALIBRATION_OUTPUT = "matrixCalibration";
static const service::IService::KeyType s_MATRICES_VECTOR_INPUT     = "matricesVector";

// -----------------------------------------------------------------------------

SToolCalibration::SToolCalibration() noexcept =
    default;

// -----------------------------------------------------------------------------

SToolCalibration::~SToolCalibration() noexcept =
    default;

// -----------------------------------------------------------------------------

void SToolCalibration::configuring()
{
    const auto configTree = this->getConfiguration();
    const auto outputs    = configTree.equal_range("out");
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

void SToolCalibration::starting()
{
}

// -----------------------------------------------------------------------------

void SToolCalibration::stopping()
{
    m_matrixCalibration = nullptr;
    if(m_hasOutputCenter)
    {
        m_matrixCenter = nullptr;
    }
}

// -----------------------------------------------------------------------------

void SToolCalibration::updating()
{
}

// -----------------------------------------------------------------------------

void SToolCalibration::computeRegistration(core::HiResClock::HiResClockType /*timestamp*/)
{
    const auto matricesVector = m_matricesVector.lock();

    data::Matrix4::sptr calibrationMatrix = data::Matrix4::New();

    data::Matrix4::sptr centerMatrixNoRot = data::Matrix4::New();

    geometry::vision::helper::calibratePointingTool(matricesVector.get_shared(), calibrationMatrix, centerMatrixNoRot);

    m_matrixCalibration = calibrationMatrix;

    if(m_hasOutputCenter)
    {
        m_matrixCenter = centerMatrixNoRot;
    }
}

} // namespace sight::module::navigation::calibration
