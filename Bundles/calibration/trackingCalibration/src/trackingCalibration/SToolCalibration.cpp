/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "trackingCalibration/SToolCalibration.hpp"

#include <calibration3d/helper.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <Eigen/Core>

namespace trackingCalibration
{

static const ::fwServices::IService::KeyType s_MATRIX_CENTER_OUTPUT      = "matrixCenter";
static const ::fwServices::IService::KeyType s_MATRIX_CALIBRATION_OUTPUT = "matrixCalibration";
static const ::fwServices::IService::KeyType s_MATRICES_VECTOR_INPUT     = "matricesVector";

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
    for (auto it = outputs.first; it != outputs.second; ++it)
    {
        const std::string key = it->second.get<std::string>("<xmlattr>.key");
        if (key == s_MATRIX_CENTER_OUTPUT)
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
    if (m_hasOutputCenter)
    {
        this->setOutput(s_MATRIX_CENTER_OUTPUT, nullptr);
    }
}

// -----------------------------------------------------------------------------

void SToolCalibration::updating()
{

}

// -----------------------------------------------------------------------------

void SToolCalibration::computeRegistration(::fwCore::HiResClock::HiResClockType)
{
    ::fwData::Vector::csptr matricesVector = this->getInput< ::fwData::Vector >(s_MATRICES_VECTOR_INPUT);

    ::fwData::mt::ObjectReadLock lock(matricesVector);

    ::fwData::TransformationMatrix3D::sptr calibrationMatrix = ::fwData::TransformationMatrix3D::New();
    this->setOutput(s_MATRIX_CALIBRATION_OUTPUT, calibrationMatrix);

    ::fwData::TransformationMatrix3D::sptr centerMatrixNoRot = ::fwData::TransformationMatrix3D::New();

    ::calibration3d::helper::calibratePointingTool(matricesVector, calibrationMatrix, centerMatrixNoRot);

    if (m_hasOutputCenter)
    {
        this->setOutput(s_MATRIX_CENTER_OUTPUT, centerMatrixNoRot);
    }
}

} // trackingCalibration
