/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "handEyeCalibration/HandEyeApi.hpp"

#include "handEyeCalibration/camodocal/HandEyeCalibration.h"

#include <eigenTools/helper.hpp>

namespace handEyeCalibration
{

//-------------------------------------------------------------------------------------------------

HandEyeApi::HandEyeApi()
{

}

//-------------------------------------------------------------------------------------------------

HandEyeApi::~HandEyeApi()
{
    //clear the vector
    this->clearData();
}

//-------------------------------------------------------------------------------------------------

void HandEyeApi::setTransformLists(const std::vector< ::fwData::TransformationMatrix3D::csptr >& _m1,
                                   const std::vector< ::fwData::TransformationMatrix3D::csptr >& _m2)
{
    SLM_ASSERT("Input vectors must have the same size", _m1.size() == _m2.size());
    this->clearData();

    EigenMatrixListType transfoList1, transfoList2;
    EigenMat4Type mat1, mat2;

    for(size_t i = 0; i < _m1.size(); ++i)
    {
        mat1 = ::eigenTools::helper::toEigen<double>(_m1.at(i));
        mat2 = ::eigenTools::helper::toEigen<double>(_m2.at(i));

        transfoList1.push_back(mat1);
        transfoList2.push_back(mat2);
    }

    this->initializeData(transfoList1, transfoList2);
}

//-------------------------------------------------------------------------------------------------

::fwData::TransformationMatrix3D::sptr HandEyeApi::computeHandEye()
{
    ::Eigen::Matrix4d result;
    ::camodocal::HandEyeCalibration::setVerbose( false );
    ::camodocal::HandEyeCalibration::estimateHandEyeScrew(m_rvecs1, m_tvecs1, m_rvecs2, m_tvecs2, result);

    ::fwData::TransformationMatrix3D::sptr mat = ::eigenTools::helper::toF4s(result);

    return mat;
}

//-------------------------------------------------------------------------------------------------

void HandEyeApi::initializeData(const EigenMatrixListType& _transfoList1, const EigenMatrixListType& _transforList2)
{
    const size_t nbMatrix = _transfoList1.size();
    SLM_ASSERT("Size of transformation list should be the same", nbMatrix == _transforList2.size());

    m_rvecs1.reserve(nbMatrix);
    m_tvecs1.reserve(nbMatrix);
    m_rvecs2.reserve(nbMatrix);
    m_tvecs2.reserve(nbMatrix);

    for(size_t i = 0; i < nbMatrix; ++i)
    {
        ::eigenTools::helper::RvecTvecType RvecTvec1 =
            ::eigenTools::helper::eigenMatToRvecTvec(_transfoList1[i]);
        m_rvecs1.push_back(RvecTvec1.first);
        m_tvecs1.push_back(RvecTvec1.second);

        ::eigenTools::helper::RvecTvecType RvecTvec2 =
            ::eigenTools::helper::eigenMatToRvecTvec(_transforList2[i]);
        m_rvecs2.push_back(RvecTvec2.first);
        m_tvecs2.push_back(RvecTvec2.second);
    }
}

//-------------------------------------------------------------------------------------------------

void HandEyeApi::clearData()
{
    m_rvecs1.clear();
    m_rvecs2.clear();

    m_tvecs1.clear();
    m_tvecs2.clear();
}

}// namespace handEyeCalibration
