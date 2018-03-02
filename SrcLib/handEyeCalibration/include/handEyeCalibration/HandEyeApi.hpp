/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "handEyeCalibration/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/StdVector>

namespace handEyeCalibration
{
/**
 * @brief The HandEyeApi class is used to abstract handEyeCalibration from camodocal.
 * @note For the moment 'camodocal' is part of fw4spl but it can be externalized in the futur
 *
 */
class HANDEYECALIBRATION_CLASS_API HandEyeApi
{
public:
    /// Constructor
    HANDEYECALIBRATION_API HandEyeApi();

    /// Destructor
    HANDEYECALIBRATION_API ~HandEyeApi();

    HANDEYECALIBRATION_API void pushData(const std::array<float, 16> _m1,
                                         const std::array<float, 16> _m2);

    HANDEYECALIBRATION_API void pushData(const std::array<double, 16> _m1,
                                         const std::array<double, 16> _m2);
    /**
     * @brief Push two synchronized ::fwData::TransformationMatrix3D
     **/
    HANDEYECALIBRATION_API void pushMatrix(const ::fwData::TransformationMatrix3D::csptr _m1,
                                           const ::fwData::TransformationMatrix3D::csptr _m2);
    /**
     * @brief Set two synchronized vector of ::fwData::TransformationMatrix3D
     **/
    HANDEYECALIBRATION_API void setTransformLists(const std::vector< ::fwData::TransformationMatrix3D::csptr >& _m1,
                                                  const std::vector< ::fwData::TransformationMatrix3D::csptr >& _m2);

    /**
     * @brief computeHandEye will first prepare the datas and put it into 'camodocal' hand eye calibration
     * @return the fwData::TransformationMatrix3D corresponding to the hand eye calibration
     */
    HANDEYECALIBRATION_API ::fwData::TransformationMatrix3D::sptr computeHandEye();

    /**
     * @brief Reset all data
     *
     **/
    HANDEYECALIBRATION_API void clearData();

private:

    void initializeData();

    typedef ::Eigen::Matrix<double, 4, 4, ::Eigen::RowMajor> eigenMat4Type;
    std::vector< eigenMat4Type, ::Eigen::aligned_allocator< eigenMat4Type > > m_transfoList1;
    std::vector< eigenMat4Type, ::Eigen::aligned_allocator< eigenMat4Type > > m_transfoList2;

    /// List of rvec tvec (eigen style)
    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_rvecs1;
    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_tvecs1;

    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_rvecs2;
    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_tvecs2;

};

} //namespace handEyeCalibration
