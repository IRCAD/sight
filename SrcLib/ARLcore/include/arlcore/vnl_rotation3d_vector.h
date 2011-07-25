/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VNL_ROTATION3D_VECTOR_H
#define _VNL_ROTATION3D_VECTOR_H
#include <arlcore/Common.h>

#include <vnl/vnl_vector_fixed.h>

namespace arlCore
{
    class vnl_rotation3d_matrix;
    class vnl_rotation3d_vector : public vnl_vector_fixed<double,3>
    {
    /**
     * @class   vnl_rotation3d_matrix
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Vecteur rotation
     * L'axe de rotation est déterminé par la direction du vecteur
     * L'angle est déterminé par la norme du vecteur modulo 2Pi dans le sens direct
     */
    public:
        //! @brief Default constructor
        ARLCORE_API vnl_rotation3d_vector( void );

        //! @brief Constructor
        ARLCORE_API vnl_rotation3d_vector( double, double, double );

        //! @brief Constructor
        ARLCORE_API vnl_rotation3d_vector( const vnl_rotation3d_matrix& );

        //! @brief Destructor
        ARLCORE_API ~vnl_rotation3d_vector( void );

        //! @brief Copy
        ARLCORE_API vnl_rotation3d_vector& operator=( const vnl_rotation3d_matrix& );

        /**
        * @brief modulo
        * Normalise la norme du vecteur dans l'intervalle [-Pi,Pi[
        */
        ARLCORE_API void modulo();

        /**
        * @brief Applique un bruit gaussien sur la direction et l'angle
        * Gaussienne centrée sur le vecteur rotation et d'écart type std
        */
        ARLCORE_API void add_gaussian_noise( double std );

        /**
        * @brief Retourne un vecteur rotation aléatoire
        * Direction et angle aléatoires et indépendants
        */
        ARLCORE_API void uniform_random( void );

        /**
        * @brief Retourne Theta en coordonnées sphériques
        * In curvilinear coordinates, Theta is the azimuthal angle in the xy-plane from the x-axis in [0,2Pi[
        * Ref : http://mathworld.wolfram.com/SphericalCoordinates.html
        */
        ARLCORE_API double getTheta( void );

        /**
        * @brief Retourne Theta en coordonnées sphériques
        * In curvilinear coordinates, Phi is the polar angle in the z-axis in [0,Pi]
        * Ref : http://mathworld.wolfram.com/SphericalCoordinates.html
        */
        ARLCORE_API double getPhi( void );

    private:
        //! @brief Private copy
        bool copy_in ( const vnl_rotation3d_matrix& );
    };
} // namespace arlCore
#endif // _VNL_ROTATION3D_VECTOR_H
