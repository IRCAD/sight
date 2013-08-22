/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
 
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "fwDataTools/TransformationMatrix3D.hpp"

namespace fwDataTools
{

bool TransformationMatrix3D::invert(::fwData::TransformationMatrix3D::sptr trf_input,
        ::fwData::TransformationMatrix3D::sptr trf_output)
{
    OSLM_INFO("Inverting matrix. Input:" << std::endl << *trf_input);
    ::boost::numeric::ublas::matrix< ::fwData::TransformationMatrix3D::TM3DType, ::boost::numeric::ublas::row_major,
            ::fwData::TransformationMatrix3D::TMCoefArray > mx_input(4, 4, trf_input->getCRefCoefficients()), mx_output(4,
            4, trf_output->getCRefCoefficients());

    // create a permutation matrix for the LU-factorization
    ::boost::numeric::ublas::permutation_matrix< std::size_t > mx_perm(mx_input.size1());

    // perform LU-factorization
    int res = ::boost::numeric::ublas::lu_factorize(mx_input, mx_perm);
    if (res != 0)
    {
        SLM_INFO("Cannot compute.");
        return false;
    }

    // create identity matrix of "inverse"
    mx_output.assign(
            ::boost::numeric::ublas::identity_matrix< ::fwData::TransformationMatrix3D::TM3DType >(mx_input.size1()));

    // backsubstitute to get the inverse
    ::boost::numeric::ublas::lu_substitute(mx_input, mx_perm, mx_output);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            trf_output->setCoefficient(i, j, mx_output(i, j));
        }
    }
    OSLM_INFO("Output:" << std::endl << *trf_output);
    return true;
}

// ----------------------------------------------------------------------------

void TransformationMatrix3D::multiply(::fwData::TransformationMatrix3D::sptr fTrf_A,
        ::fwData::TransformationMatrix3D::sptr fTrf_B, ::fwData::TransformationMatrix3D::sptr fTrf_C)
{
    ::boost::numeric::ublas::matrix< ::fwData::TransformationMatrix3D::TM3DType, ::boost::numeric::ublas::row_major,
            ::fwData::TransformationMatrix3D::TMCoefArray > mx_a(4, 4, fTrf_A->getCRefCoefficients()), mx_b(4, 4,
            fTrf_B->getCRefCoefficients()), mx_c(4, 4);

    mx_c = ::boost::numeric::ublas::prod(mx_a, mx_b);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            fTrf_C->setCoefficient(i, j, mx_c(i, j));
        }
    }
}

// ----------------------------------------------------------------------------

void TransformationMatrix3D::identity(::fwData::TransformationMatrix3D::sptr trf)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            trf->setCoefficient(i, j, i == j ? 1 : 0);
        }
    }
}

// ----------------------------------------------------------------------------

void TransformationMatrix3D::multiply(::fwData::TransformationMatrix3D::sptr trf,
            ::fwData::Point::sptr input, ::fwData::Point::sptr output)
{
    ::boost::numeric::ublas::matrix< ::fwData::TransformationMatrix3D::TM3DType, ::boost::numeric::ublas::row_major,
            ::fwData::TransformationMatrix3D::TMCoefArray > mx(4, 4, trf->getCRefCoefficients());

    ::boost::array<double, 3> inCoord = input->getCRefCoord();
    ::boost::numeric::ublas::vector< double > in(4), out(4);
    in[0] = inCoord[0];
    in[1] = inCoord[1];
    in[2] = inCoord[2];
    in[3] = 1;

    out = ::boost::numeric::ublas::prod(mx, in);
    ::boost::array<double, 3> res = {{ out[0], out[1], out[2] }};
    output->setCoord(res);
}

} // namespace fwDataTools
