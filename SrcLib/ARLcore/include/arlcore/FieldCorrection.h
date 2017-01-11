/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_FIELDCORRECTION_H__
#define __ARLCORE_FIELDCORRECTION_H__
#include <arlcore/Common.h>

#include <string>

#include <vnl/vnl_vector.h>

#include <arlcore/MatrixR.h>
#include <arlcore/PointsList.h>
#include <arlcore/Point.h>

namespace arlCore
{
/**
 * @brief   Correction d'un champs de vecteurs
 */
class FieldCorrector
{
public:
    //! @brief Default constructor
    ARLCORE_API FieldCorrector( void );

    //! @brief Constructor for particular degree
    ARLCORE_API FieldCorrector( unsigned int degree );

    //! @brief Copy constructor
    ARLCORE_API FieldCorrector( const FieldCorrector& );

    //! @brief Destructor
    ARLCORE_API ~FieldCorrector( void );

    //! @return Description of the field corrector
    ARLCORE_API std::string getString( void ) const;

    //! @brief Set degree of the polynomial and initialize polynomial
    ARLCORE_API bool setDegree( unsigned int degree );

    //! @brief Get degree of the polynomial
    ARLCORE_API unsigned int getDegree( void );

    //! @brief Get number of parameters
    ARLCORE_API unsigned int getNbParameters( void );

    //! @brief Set parameters
    ARLCORE_API bool setParameters( const vnl_vector<double>& parameters );

    //! @brief Load parameters from a file
    ARLCORE_API bool load( const std::string& fileName );

    //! @brief Activ or deactiv correction
    ARLCORE_API bool activCorrection( bool );

    //! @brief Copy
    ARLCORE_API bool copy( const FieldCorrector& );

    //! @brief T2 receive T1 corrected. T1=distorded transformation T2=undistorded
    ARLCORE_API bool correct( const vnl_rigid_matrix& T1, vnl_rigid_matrix& T2 ) const;

    //! @brief P2 receive P1 corrected. P1=distorded point P2=undistorded
    ARLCORE_API bool correct( CSPTR( Point ) P1, SPTR( Point ) P2 ) const;

    //! @brief Correct the transformation T
    ARLCORE_API bool correct( vnl_rigid_matrix& T ) const;

private:
    vnl_vector<double> m_parameters;
    unsigned int m_degree;
    bool m_correction;
};

/**
 * @brief Calibration of field of deformations
 * @param[in] real Point list at the correct position
 * @param[in] distorded Point list at the distorded position
 * @param[in] degree Interpolation degree
 * @param[out] parameters parameters of the polynomial
 * @param[out] RMS Error of the optimization
 */
ARLCORE_API bool fieldCalibration( CSPTR( PointList ) real, CSPTR(
                                       PointList ) distorded, unsigned int degree, vnl_vector<double> &parameters,
                                   double& RMS );

} // namespace arlCore
#endif // __ARLCORE_FIELDCORRECTION_H__
