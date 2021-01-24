/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "fwItkIO/config.hpp"

#include <data/TransformationMatrix3D.hpp>

#include <itkMatrix.h>

namespace fwItkIO
{
namespace helper
{
/**
 * @brief Helper class to convert Sight TransformationMatrix3D to itk Matrix and the other way around.
 */
class FWITKIO_CLASS_API Transform
{
public:
    /// Typedef to a itk Transform <double, 4,4>
    typedef ::itk::Matrix< double, 4, 4> MatrixType;
    /**
     * @brief convert a Sight transformation to itk
     * @param _inTrf input data::TransformationMatrix3D
     * @return Pointer to a itk::Matrix<double, 4, 4>
     */
    FWITKIO_API static MatrixType convertToITK(const data::TransformationMatrix3D::csptr& _inTrf);

    /**
     * @brief convert an itk::Transform<double, 4, 4> to a Sight one
     * @param _inTrf Const Pointer to a itk::Transform<double, 4,4>
     * @param _outTrf output data::TransformationMatrix3D (needs to be allocated before)
     */
    FWITKIO_API static void convertFromITK(const MatrixType& _inTrf,
                                           data::TransformationMatrix3D::sptr& _outTrf );
};

}// namespace helper
}// namespace fwItkIO
