/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VNL_ROTATION3D_MATRIX_H
#define _VNL_ROTATION3D_MATRIX_H
#include <arlcore/Common.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vector>

namespace arlCore
{
    class vnl_rotation3d_vector;
    class vnl_rotation3d_matrix : public vnl_matrix_fixed<double,3,3>
    {
    /**
     * @class   vnl_rotation3d_matrix
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Matrice de rotation 3x3
     * vnl_rotation3d_matrix spécialise une matrice quelconque 3x3
     * Des méthodes permettent de tester ses propriétés ou de trouver la matrice de rotation exacte la plus proche
     */
    public:
        //! @brief Default constructor
        ARLCORE_API vnl_rotation3d_matrix( void );

        //! @brief Constructor
        ARLCORE_API vnl_rotation3d_matrix( const vnl_rotation3d_vector& );

        //! @brief Constructor
        ARLCORE_API vnl_rotation3d_matrix( const vnl_matrix_fixed<double,3,3>& );

        /**
         * @Brief Constructor : Initialization by Euler angles
         * Notations : cf http://mathworld.wolfram.com/EulerAngles.html
         * @param Phi = rotation about the z-axis
         * @param Theta = rotation about the x-axis [0,Pi]
         * @param Psy = rotation about z-axis(again)
         */
        ARLCORE_API vnl_rotation3d_matrix( double phi, double theta, double psy );

        //! @brief Destructor
        ARLCORE_API ~vnl_rotation3d_matrix( void );

        //! @brief Affectation
        ARLCORE_API vnl_rotation3d_matrix& operator=( const vnl_rotation3d_vector& );

        //! @brief Affectation
        ARLCORE_API vnl_rotation3d_matrix& operator=( const vnl_matrix_fixed<double,3,3>& );

        //! @brief Conversion
        ARLCORE_API vnl_matrix<double> as_matrix( void );

        //! @brief Is it a correct rotation matrix ?
        ARLCORE_API bool is_rotation( void ) const;

        //! @brief If it's possible, current rotation matrix takes the closest real rotation in sense of Frobenius norm
        ARLCORE_API bool closest_rotation( void );
        
        //! @brief compute the distance in riemannian sense between the current matrix and B
        ARLCORE_API double sq_rieman_dist( const vnl_rotation3d_matrix &B );

        //! @brief compute the gravity center of the rotation matrix list (in riemannian sense)
        //  @out distance between the gravity center and the matrix list (TODO not implemented) 
        ARLCORE_API double average_rotation3d ( const std::vector< vnl_rotation3d_matrix * > &);

    private:
        //! @brief Private copy
        void copy_in( const vnl_rotation3d_vector& );

        //! @brief Private copy
        void copy_in( const vnl_matrix_fixed<double,3,3>& );
    };
} // namespace arlCore
#endif // _VNL_ROTATION3D_MATRIX_H
