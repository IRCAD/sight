/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#pragma once

#include "io/itk/config.hpp"

#include <data/matrix4.hpp>

#include <itkMatrix.h>

namespace sight::io::itk::helper
{

/**
 * @brief Helper class to convert Sight Matrix4 to itk Matrix and the other way around.
 */
class IO_ITK_CLASS_API Transform
{
public:

    /// Typedef to a itk Transform <double, 4,4>
    typedef ::itk::Matrix<double, 4, 4> MatrixType;
    /**
     * @brief convert a Sight transformation to itk
     * @param _inTrf input data::matrix4
     * @return Pointer to a itk::Matrix<double, 4, 4>
     */
    IO_ITK_API static MatrixType convertToITK(const data::matrix4::csptr& _inTrf);

    /**
     * @brief convert an itk::Transform<double, 4, 4> to a Sight one
     * @param _inTrf Const Pointer to a itk::Transform<double, 4,4>
     * @param _outTrf output data::matrix4 (needs to be allocated before)
     */
    IO_ITK_API static void convertFromITK(
        const MatrixType& _inTrf,
        data::matrix4::sptr& _outTrf
    );
};

} // namespace sight::io::itk::helper
