/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "eigenTools/helper.hpp"

namespace eigenTools
{

namespace helper
{

//-------------------------------------------------------------------------------------------------

::fwData::TransformationMatrix3D::sptr toF4s( const ::Eigen::Matrix4f& _mat)
{
    ::fwData::TransformationMatrix3D::sptr trf = ::fwData::TransformationMatrix3D::New();

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            trf->setCoefficient(r, c, static_cast<double>(_mat(r, c)));
        }
    }

    return trf;
}

//-------------------------------------------------------------------------------------------------

::fwData::TransformationMatrix3D::sptr toF4s( const ::Eigen::Matrix4d& _mat)
{
    ::fwData::TransformationMatrix3D::sptr trf = ::fwData::TransformationMatrix3D::New();

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            trf->setCoefficient(r, c, _mat(r, c));
        }
    }

    return trf;
}

//-------------------------------------------------------------------------------------------------

RvecTvecType eigenMatToRvecTvec(const Eigen::Matrix4d& _mat)
{
    ::Eigen::AngleAxisd angleAxis;

    ::Eigen::Matrix3d rotation;

    for(unsigned int r = 0; r < 3; ++r)
    {
        for(unsigned int c = 0; c < 3; ++c)
        {
            rotation(r, c) = _mat(r, c);
        }
    }

    angleAxis.fromRotationMatrix(rotation);

    ::Eigen::Vector3d rvec = angleAxis.angle() * angleAxis.axis();
    ::Eigen::Vector3d tvec = _mat.block<3, 1>(0, 3);

    return std::make_pair(rvec, tvec);
}

//-------------------------------------------------------------------------------------------------

RvecTvecType f4sMatToRvecTvec(const fwData::TransformationMatrix3D::csptr _mat)
{
    ::Eigen::Matrix4d eigenMat = toEigen< double >(_mat);
    return eigenMatToRvecTvec(eigenMat);
}

//-------------------------------------------------------------------------------------------------

EigenMatrix toEigen(const std::array<float, 16>& _farray)
{
    ::Eigen::Matrix< double, 4, 4, ::Eigen::RowMajor > mat;

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            mat(r, c) = static_cast<double>(_farray[4*r+c]);
        }
    }

    return mat;
}

//-------------------------------------------------------------------------------------------------

EigenMatrix toEigen(const std::array<double, 16>& _farray)
{
    ::Eigen::Matrix< double, 4, 4, ::Eigen::RowMajor > mat;

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            mat(r, c) = _farray[4*r+c];
        }
    }

    return mat;
}

//-------------------------------------------------------------------------------------------------

RvecTvecType float16ToRvecTvec(const std::array<float, 16>& _farray)
{
    ::Eigen::Matrix4d eigenMat = toEigen(_farray);
    return eigenMatToRvecTvec(eigenMat);
}

//-------------------------------------------------------------------------------------------------

} //namespace helper
} //namespace eigenTools
