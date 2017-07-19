/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwItkIO/helper/Transform.hpp"

namespace fwItkIO
{
namespace helper
{

//-----------------------------------------------------------------------------

Transform::MatrixType Transform::convertToITK(const ::fwData::TransformationMatrix3D::csptr& _inTrf)
{
    MatrixType itkMat;

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; ++j)
        {
            itkMat(i, j) = _inTrf->getCoefficient(i, j);
        }
    }

    return itkMat;
}

//-----------------------------------------------------------------------------

void Transform::convertFromITK(const MatrixType& _inTrf, ::fwData::TransformationMatrix3D::sptr& _outTrf)
{
    SLM_ASSERT("Input itk Matrix should be 4x4", _inTrf.ColumnDimensions == 4 && _inTrf.RowDimensions == 4);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; ++j)
        {
            _outTrf->setCoefficient(i, j, _inTrf(i, j));
        }
    }
}

//-----------------------------------------------------------------------------

}// namespace helper
}// namespace fwItkIO
