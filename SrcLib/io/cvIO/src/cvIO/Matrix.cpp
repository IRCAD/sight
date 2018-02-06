/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvIO/Matrix.hpp"

namespace cvIO
{

//-----------------------------------------------------------------------------

void Matrix::copyFromCv(const ::cv::Matx44f& _src, ::fwData::TransformationMatrix3D::sptr& _dst)
{
    SLM_ASSERT("::fwData::TransformationMatrix3D is null", _dst);

    for (std::uint8_t i = 0; i < 4; ++i)
    {
        for (std::uint8_t j = 0; j < 4; ++j)
        {
            _dst->setCoefficient(i, j, static_cast<double>( _src(i, j) ));
        }
    }
}

//-----------------------------------------------------------------------------

void Matrix::copyToCv(const fwData::TransformationMatrix3D::csptr& _src, cv::Matx44f& _dst)
{
    SLM_ASSERT("::fwData::TransformationMatrix3D is null", _src);

    for (std::uint8_t i = 0; i < 4; ++i)
    {
        for (std::uint8_t j = 0; j < 4; ++j)
        {
            _dst(i, j) = static_cast<float>(_src->getCoefficient(i, j));
        }
    }

}

//-----------------------------------------------------------------------------

} //namespace cvIO
