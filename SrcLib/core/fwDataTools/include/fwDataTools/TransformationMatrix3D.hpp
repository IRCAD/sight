/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
 
#ifndef __FWDATATOOLS_TRANSFORMATIONMATRIX3D_HPP__
#define __FWDATATOOLS_TRANSFORMATIONMATRIX3D_HPP__

#include <fwCore/base.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Point.hpp>

#include "fwDataTools/export.hpp"

namespace fwDataTools
{
/**
 * @brief   TransformationMatrix3D performs matrices computations.
 * @class   TransformationMatrix3D
 */
class TransformationMatrix3D
{
public:

    /**
     * @brief Invert a matrix.
     * @param[in] trf_input Input matrix
     * @param[out] trf_output Inverted matrix.
     */
    FWDATATOOLS_API static bool invert(::fwData::TransformationMatrix3D::sptr trf_input,
            ::fwData::TransformationMatrix3D::sptr trf_output);

    /**
     * @brief Multiply two matrices.
     * @param[in] ftrf_A First matrix.
     * @param[in] fTrf_B Second matrix.
     * @param[out] fTrf_C Output matrix.
     */
    FWDATATOOLS_API static void multiply(::fwData::TransformationMatrix3D::sptr fTrf_A,
            ::fwData::TransformationMatrix3D::sptr fTrf_B, ::fwData::TransformationMatrix3D::sptr fTrf_C);

    /**
     * @brief Set the matrix to identity.
     * @param[in/out] trf Matrix we want to set to identity.
     */
    FWDATATOOLS_API static void identity(::fwData::TransformationMatrix3D::sptr trf);

    /**
     * @brief Multiply a point and a matrix.
     * @param[in] trf Matrix.
     * @param[in] input Input point.
     * @param[out] output Output point.
     */
    FWDATATOOLS_API static void multiply(::fwData::TransformationMatrix3D::sptr trf,
            ::fwData::Point::sptr input, ::fwData::Point::sptr output);

};
} // namespace fwDataTools

#endif // __FWDATATOOLS_TRANSFORMATIONMATRIX3D_HPP__
