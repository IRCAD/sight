/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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
