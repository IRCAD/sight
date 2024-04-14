/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "io/itk/helper/transform.hpp"

namespace sight::io::itk::helper
{

//-----------------------------------------------------------------------------

transform::matrix_t transform::convert_to_itk(const data::matrix4::csptr& _in_trf)
{
    matrix_t itk_mat;

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            itk_mat(i, j) = (*_in_trf)(i, j);
        }
    }

    return itk_mat;
}

//-----------------------------------------------------------------------------

void transform::convert_from_itk(const matrix_t& _in_trf, data::matrix4::sptr& _out_trf)
{
    SIGHT_ASSERT("Input itk Matrix should be 4x4", _in_trf.ColumnDimensions == 4 && _in_trf.RowDimensions == 4);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            (*_out_trf)(i, j) = _in_trf(i, j);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::io::itk::helper
