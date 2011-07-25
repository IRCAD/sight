/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_ICP_H
#define _ARLCORE_ICP_H
#include <arlcore/Common.h>

#include <vnl/vnl_vector.h>

#include <arlcore/MatrixR.h>
#include <arlcore/Mesh.h>
#include <arlcore/PointsList.h>

namespace arlCore
{
    class Point;
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    04/2009
     * @brief   Iterativ Closest Point registration between two point's clouds
     */
    class ICP
    {
    public:
        /**
        * @brief Constructor
        * @param[in] model
        * @param[in] cloud
        * @param[in] justVisible If true, use only visible points
        */
        ARLCORE_API ICP( const arlCore::PointList &model, const arlCore::PointList &cloud, bool justVisible=true );

        /**
        * @brief Constructor
        * @param[in] model
        * @param[in] cloud
        * @param[in] justVisible If true, use only visible cloud's points
        */
        ARLCORE_API ICP( const arlCore::Mesh &model, const arlCore::PointList &cloud, bool justVisible=true );

        //! @brief Copy constructor
        ARLCORE_API ICP( const ICP& ); // TODO

        //! @brief Destructor
        ARLCORE_API ~ICP( void );

        //! @brief Optimise solution with Powell method
        ARLCORE_API double optimisePowell( void );

        //! @brief Optimise solution with Least Squares method
        ARLCORE_API double optimiseLS( void );

        //! @brief Initialize solution
        ARLCORE_API void initSolution( const arlCore::vnl_rigid_matrix &M );

        //! @return Reference on solution
        ARLCORE_API const arlCore::vnl_rigid_matrix& getSolution( void ) const;

        //! @brief Set option : Number max of iterations
        ARLCORE_API bool setMaxIterations( unsigned int=500 );

        //! @return Number of points of model
        ARLCORE_API unsigned int getModelSize( void ) const;

        //! @return Number of points of cloud
        ARLCORE_API unsigned int getCloudSize( void ) const;

        //! @return Number of iteration of the last calculation
        ARLCORE_API unsigned int getNbIterations( void ) const;

        //! @return Start error
        ARLCORE_API double getStartError( void ) const;

        //! @return End error
        ARLCORE_API double getEndError( void ) const;

        //! @brief Solve the registration
        ARLCORE_API bool solve( void );

        //! @return RMS
        ARLCORE_API double computeError( void );

        //! @brief Compute distance for each point of the cloud (visible or not depending on the option)
        ARLCORE_API unsigned int computeDistances( std::vector<double> &distances );

        //! @brief Set points with points of cloud and distance to the model in point's error
        ARLCORE_API unsigned int computeDistances( arlCore::PointList &points );

        //! @brief Compute RMS with M as solution (used by optimizer)
        ARLCORE_API double computeCriterion( const arlCore::vnl_rigid_matrix &M );

        //! @brief Compute RMS with M as solution (used by optimizer)
        ARLCORE_API double computeCriterion( const arlCore::vnl_rigid_matrix &M, vnl_vector< double > &fx );

    protected:

    private:
        bool initialization( void );

        // 2 modes : Point to point & Point to Mesh
        bool m_point2PointMode;
        const arlCore::Mesh *m_modelMesh;
        const arlCore::PointList *m_cloud;
        bool m_initialization;
        bool m_justVisible;
        unsigned int m_dimension;
        vnl_vector<double> m_modelGravity, m_cloudGravity;
        unsigned int m_modelSize, m_cloudSize;
#ifdef ANN
        ANNpointArray m_modelPoints, m_cloudPoints;
        ANNpointArray m_Pk, m_Yk, m_Pi;
        ANNkd_tree* m_ANNtree;
        int m_nbNN; // number of nearest neighbors
        ANNidxArray m_nn_idx; // near neighbor indices
        ANNdistArray m_squaredDists; // near neighbor distances
#endif // ANN
        vnl_matrix_fixed<double,3,3> m_rotInit;
        vnl_vector_fixed<double,3> m_traInit;
        arlCore::vnl_rigid_matrix m_solution;

        unsigned int m_maxIterations;
        unsigned int m_nbIterations;

        double m_startError, m_endError;
    };
} // namespace arlCore
#endif // _ARLCORE_ICP_H
