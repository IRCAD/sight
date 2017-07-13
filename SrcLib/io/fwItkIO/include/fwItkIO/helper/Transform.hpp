/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWITKIO_HELPER_TRANSFORM_HPP__
#define __FWITKIO_HELPER_TRANSFORM_HPP__

#include "fwItkIO/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <itkMatrix.h>

namespace fwItkIO
{
namespace helper
{
/**
 * @brief Helper class to convert F4s TransformationMatrix3D to itk Matrix and the other way around.
 */
class FWITKIO_CLASS_API Transform
{
public:
    /// Typedef to a itk Transform <double, 4,4>
    typedef ::itk::Matrix< double, 4, 4> MatrixType;
    /**
     * @brief convert a F4S transformation to itk
     * @param _inTrf input fwData::TransformationMatrix3D
     * @return Pointer to a itk::Matrix<double, 4, 4>
     */
    FWITKIO_API static MatrixType convertToITK(const ::fwData::TransformationMatrix3D::csptr& _inTrf);

    /**
     * @brief convert an itk::Transform<double, 4, 4> to a F4S one
     * @param _inTrf Const Pointer to a itk::Transform<double, 4,4>
     * @param _outTrf output fwData::TransformationMatrix3D (needs to be allocated before)
     */
    FWITKIO_API static void convertFromITK(const MatrixType& _inTrf,
                                           ::fwData::TransformationMatrix3D::sptr& _outTrf );
};

}// namespace helper
}// namespace fwItkIO

#endif //  __FWITKIO_HELPER_TRANSFORM_HPP__
