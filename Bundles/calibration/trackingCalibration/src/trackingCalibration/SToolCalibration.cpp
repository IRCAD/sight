/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackingCalibration/SToolCalibration.hpp"

#include <eigenTools/helper.hpp>

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

    const ::fwData::Vector::ContainerType matrices = matricesVector->getContainer();
    const size_t nbrMatrices                       = matrices.size();

    if (nbrMatrices < 4)
    {
        SLM_WARN("Number of points when computing the tool calibration should be more than 5.");
        return;
    }

    ::eigenTools::helper::EigenMatrix matrixSum;
    matrixSum.fill(0.);
    ::Eigen::Vector4d vectorSum;

    for (size_t i = 0; i < nbrMatrices; ++i)
    {
        ::fwData::TransformationMatrix3D::csptr m1 = ::fwData::TransformationMatrix3D::dynamicCast(matrices.at(i));
        ::eigenTools::helper::EigenMatrix xyz1;
        xyz1.fill(0.);
        xyz1(0, 0) = m1->getCoefficient(0, 3);
        xyz1(0, 1) = m1->getCoefficient(1, 3);
        xyz1(0, 2) = m1->getCoefficient(2, 3);
        xyz1(0, 3) = 1.0;

        matrixSum = matrixSum + xyz1.transpose() * xyz1;
        vectorSum = vectorSum + xyz1.squaredNorm() * ::Eigen::Vector4d(xyz1(0, 0), xyz1(0, 1), xyz1(0, 2), xyz1(0, 3));
    }

    ::eigenTools::helper::EigenMatrix tempMatrix;
    tempMatrix.fill(0.);
    tempMatrix(0, 0) = vectorSum[0];
    tempMatrix(0, 1) = vectorSum[1];
    tempMatrix(0, 2) = vectorSum[2];
    tempMatrix(0, 3) = vectorSum[3];
    tempMatrix       = -tempMatrix* matrixSum.inverse();

    const double a = -1. * tempMatrix(0, 0) / 2.;
    const double b = -1. * tempMatrix(0, 1) / 2.;
    const double c = -1. * tempMatrix(0, 2) / 2.;

    ::Eigen::Vector3d translation;
    translation.fill(0);
    for (size_t i = 0; i < nbrMatrices; ++i)
    {
        ::fwData::TransformationMatrix3D::csptr m1 = ::fwData::TransformationMatrix3D::dynamicCast(matrices.at(i));
        const ::eigenTools::helper::EigenMatrix pointMatrix = ::eigenTools::helper::toEigen(m1->getCoefficients());
        ::eigenTools::helper::EigenMatrix centerMatrix(pointMatrix);
        const ::eigenTools::helper::EigenMatrix pointMatrixInverse = pointMatrix.inverse();

        centerMatrix(0, 3) = a;
        centerMatrix(1, 3) = b;
        centerMatrix(2, 3) = c;

        const ::eigenTools::helper::EigenMatrix calibrationMatrix = pointMatrixInverse * centerMatrix;
        translation(0) += calibrationMatrix(0, 3);
        translation(1) += calibrationMatrix(1, 3);
        translation(2) += calibrationMatrix(2, 3);
    }

    translation /= static_cast<double>(nbrMatrices);

    ::fwData::TransformationMatrix3D::sptr matrixCalibration = ::fwData::TransformationMatrix3D::New();
    matrixCalibration->setCoefficient(0, 3, translation(0));
    matrixCalibration->setCoefficient(1, 3, translation(1));
    matrixCalibration->setCoefficient(2, 3, translation(2));
    this->setOutput(s_MATRIX_CALIBRATION_OUTPUT, matrixCalibration);

    if (m_hasOutputCenter)
    {
        ::fwData::TransformationMatrix3D::sptr centerMatrixNoRot = ::fwData::TransformationMatrix3D::New();
        centerMatrixNoRot->setCoefficient(0, 3, a);
        centerMatrixNoRot->setCoefficient(1, 3, b);
        centerMatrixNoRot->setCoefficient(2, 3, c);
        this->setOutput(s_MATRIX_CENTER_OUTPUT, centerMatrixNoRot);
    }
}

} // trackingCalibration
