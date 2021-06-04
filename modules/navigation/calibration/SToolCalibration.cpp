/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include <data/Matrix4.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/Vector.hpp>

#include <geometry/vision/helper.hpp>

#include <service/macros.hpp>

namespace sight::module::navigation::calibration
{

static const service::IService::KeyType s_MATRIX_CENTER_OUTPUT      = "matrixCenter";
static const service::IService::KeyType s_MATRIX_CALIBRATION_OUTPUT = "matrixCalibration";
static const service::IService::KeyType s_MATRICES_VECTOR_INPUT     = "matricesVector";

// -----------------------------------------------------------------------------

SToolCalibration::SToolCalibration() noexcept
{
}

// -----------------------------------------------------------------------------

SToolCalibration::~SToolCalibration() noexcept
{
}

// -----------------------------------------------------------------------------

void SToolCalibration::configuring()
{
    const auto configTree = this->getConfigTree();
    const auto outputs    = configTree.equal_range("out");
    for(auto it = outputs.first ; it != outputs.second ; ++it)
    {
        const std::string key = it->second.get<std::string>("<xmlattr>.key");
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
    this->setOutput(s_MATRIX_CALIBRATION_OUTPUT, nullptr);
    if(m_hasOutputCenter)
    {
        this->setOutput(s_MATRIX_CENTER_OUTPUT, nullptr);
    }
}

// -----------------------------------------------------------------------------

void SToolCalibration::updating()
{
}

// -----------------------------------------------------------------------------

void SToolCalibration::computeRegistration(core::HiResClock::HiResClockType)
{
    data::Vector::csptr matricesVector = this->getInput<data::Vector>(s_MATRICES_VECTOR_INPUT);

    data::mt::ObjectReadLock lock(matricesVector);

    data::Matrix4::sptr calibrationMatrix = data::Matrix4::New();
    this->setOutput(s_MATRIX_CALIBRATION_OUTPUT, calibrationMatrix);

    data::Matrix4::sptr centerMatrixNoRot = data::Matrix4::New();

    geometry::vision::helper::calibratePointingTool(matricesVector, calibrationMatrix, centerMatrixNoRot);

    if(m_hasOutputCenter)
    {
        this->setOutput(s_MATRIX_CENTER_OUTPUT, centerMatrixNoRot);
    }
}

} // trackingCalibration
