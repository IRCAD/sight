/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VNL_RIGID_VECTOR_H
#define _VNL_RIGID_VECTOR_H
#include <arlcore/Common.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_vector.h>

namespace arlCore
{
    class vnl_rigid_matrix;
    class vnl_rotation3d_vector;
    class vnl_rigid_vector : public vnl_vector_fixed<double,6>
    {
    /**
     * @class   vnl_rigid_vector
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Rigid transformation vector
     * Vector representation, useful for optimization
     * [0,3] : Rotation vector
     * [4,6] : Translation vector
     */
    public:
        //! @brief Default constructor
        ARLCORE_API vnl_rigid_vector( void );

        //! @brief Constructor
        ARLCORE_API vnl_rigid_vector( const vnl_vector<double>& );

        //! @brief Constructor by copy
        ARLCORE_API vnl_rigid_vector( const vnl_rigid_vector& );

        //! @brief Constructor
        ARLCORE_API vnl_rigid_vector( const vnl_rigid_matrix& );

        //! @brief Destructor
        ARLCORE_API ~vnl_rigid_vector( void );

        //! @brief Affectation
        ARLCORE_API vnl_rigid_vector& operator=( const vnl_vector<double>& );

        //! @brief Affectation
        ARLCORE_API vnl_rigid_vector& operator=( const vnl_rigid_vector& );

        //! @brief Affectation
        ARLCORE_API vnl_rigid_vector& operator=( const vnl_rigid_matrix& );

        //! @brief add a gaussian noise on each composant of vector
        void addGaussianNoise( double std );

        //! @return Rotation vector
        ARLCORE_API vnl_rotation3d_vector getRotation3dVector( void ) const;

        //! @return Translation vector
        ARLCORE_API vnl_vector_fixed<double,3> getTranslation( void ) const;

    private:
        //! @brief Private copy
        void copy_in( const vnl_vector<double>& );

        //! @brief Private copy
        void copy_in( const vnl_rigid_vector& );

        //! @brief Private copy
        void copy_in( const vnl_rigid_matrix& );
    };
} // namespace arlCore
#endif // _VNL_RIGID_VECTOR_H
