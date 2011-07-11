/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VNL_COVARIANCE_VECTOR_H
#define _VNL_COVARIANCE_VECTOR_H
#include <arlcore/Common.h>

#include <vnl/vnl_matrix.h>

namespace arlCore
{
    class vnl_covariance_matrix : public vnl_matrix<double>
    {
    /**
     * @class   vnl_covariance_vector
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Covariance matrix
     */
    public:
        //! @brief Constructor of a n x n covariance matrix filled with 0.0
        ARLCORE_API vnl_covariance_matrix( unsigned int n );

        //! @brief Default constructor of a 0 x 0 covariance matrix
        ARLCORE_API vnl_covariance_matrix( void );

        //! @brief Copy constructor
        ARLCORE_API vnl_covariance_matrix( const vnl_covariance_matrix& );

        //! @brief Destructor
        ARLCORE_API ~vnl_covariance_matrix( void );

        //! @brief Affectation (Get the new size)
        ARLCORE_API vnl_covariance_matrix& operator=( const vnl_covariance_matrix& );

        /**
         * @brief Confidence ellipse for 2 x 2 matrix
         * @result False if the matrix isn't a 2 x 2 matrix
         */
        ARLCORE_API bool errorEllipse( double &a2, double &b2, double &theta ) const;

    private:
        //! @brief Private copy
        void copy_in( const vnl_covariance_matrix& );
    };
} // namespace arlCore
#endif // _VNL_COVARIANCE_VECTOR_H
