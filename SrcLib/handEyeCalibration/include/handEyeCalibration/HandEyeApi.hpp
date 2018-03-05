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
 * @brief The HandEyeApi class wraps the camodocal hand-eye API.
 * @note For the moment 'camodocal' is part of fw4spl but it can be externalized in the future
 */
class HANDEYECALIBRATION_CLASS_API HandEyeApi
{
public:
    /// Constructor.
    HANDEYECALIBRATION_API HandEyeApi();

    /// Destructor, clears the input data.
    HANDEYECALIBRATION_API ~HandEyeApi();

    /**
     * @brief Sets two synchronized vector of input matrices.
     * @pre both lists must have the same size.
     */
    HANDEYECALIBRATION_API void setTransformLists(const std::vector< ::fwData::TransformationMatrix3D::csptr >& _m1,
                                                  const std::vector< ::fwData::TransformationMatrix3D::csptr >& _m2);

    /**
     * @brief computeHandEye will first prepare the data and pass them to the 'camodocal' hand eye calibration
     * @return the hand eye calibration matrix
     */
    HANDEYECALIBRATION_API ::fwData::TransformationMatrix3D::sptr computeHandEye();

    /// Resets the input data.
    HANDEYECALIBRATION_API void clearData();

private:

    typedef ::Eigen::Matrix<double, 4, 4, ::Eigen::RowMajor> EigenMat4Type;
    typedef std::vector< EigenMat4Type, ::Eigen::aligned_allocator< EigenMat4Type > > EigenMatrixListType;

    /// Decomposes matrices into rvec/tvec pairs.
    void initializeData(const EigenMatrixListType& _transfoList1, const EigenMatrixListType& _transforList2);

//    std::vector< eigenMat4Type, ::Eigen::aligned_allocator< eigenMat4Type > > m_transfoList1;
//    std::vector< eigenMat4Type, ::Eigen::aligned_allocator< eigenMat4Type > > m_transfoList2;

    /// List of rvec tvec (eigen style)
    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_rvecs1;
    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_tvecs1;

    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_rvecs2;
    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d > > m_tvecs2;

};

} //namespace handEyeCalibration
