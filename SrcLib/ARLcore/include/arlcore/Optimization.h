/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_OPTIMIZATION_H
#define _ARLCORE_OPTIMIZATION_H
#include <arlcore/Common.h>

#include <vector>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_least_squares_cost_function.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

#include <arlcore/Point.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Camera.h>
#include <arlcore/PointsList.h>
#include <arlcore/PlaneSystem.h>
#include <arlcore/ICP.h>

namespace arlCore
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Mother Class for observation
     * @warning the observer provides informations on the optimisation only if it has 
     * been activated using setObserver()
     */
    class CostFunction
    {
    public:
        //! @brief Constructor
        ARLCORE_API CostFunction( void );

        //! @brief Compute and return the criterion
        ARLCORE_API virtual double getCriterion( const vnl_vector<double> &x );

        //! @brief Activation of the observer on the optimiser
        ARLCORE_API void setObserver( bool b );

        //! @return Iteration Number
        ARLCORE_API unsigned int getNbIterations( void ) const;

        /**
         * @brief This function will stack in a PointList ALL the criterion value computed
         * in f . The points are 2 dimensional, x coordinates corresponds to the time
         * in ms and y coordinates correponds to the criterion value
         */
        ARLCORE_API const PointList& getObservation( void ) const;

        //! @brief Tool to visualize the criterion evolution
        ARLCORE_API void plot( void ) const;

        //! @return Last error
        ARLCORE_API double getError( void ) const;

        //! @brief Verify if returned solution & end error are correct
        ARLCORE_API bool verify( const vnl_vector<double>& x, double endError );

        // TODO Be private
        double m_errorMin;
        vnl_vector<double> m_solution;
        bool m_firstIteration;

    protected:
        //! @brief This method has to be written at the end of f(x) of child classes (return observe(); ) to make work the observer
        ARLCORE_API double observe( const vnl_vector<double> &x );

        //! @brief Use this method as observe( void ), if criterion can't be evaluated
        ARLCORE_API double error( const vnl_vector<double> &x );

        bool m_observer;

        //! @brief Point list that contains the criterion value along time (if observer is set to true)
        PointList m_errorList;

        //! @brief Iteration number
        long int m_iterations;

        long int m_time;

        //! @brief Criterion value
        double m_error;

        static double m_errorMax;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Computation of the best plane P that fits a 3D point cloud M_i
     * The class seeks P so that sum || dist(M_i , P) ||^2 is minimal
     */
    class OptimisePlane : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimisePlane();
        ARLCORE_API ~OptimisePlane();
        ARLCORE_API double f(vnl_vector< double > const &x);  
        
        ARLCORE_API void addPoint( const vgl_point_3d< double >&point );
        ARLCORE_API void delPoint( unsigned int );
        ARLCORE_API unsigned int size( void );

        ARLCORE_API const std::vector< vgl_point_3d< double >* > &getPoints( void );

    private:
        std::vector< vgl_point_3d< double >* > m_points;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    04/2009
     * @brief   Optimization of ICP criterion
     */
    class OptimiseICP : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimiseICP( arlCore::ICP& );
        ARLCORE_API ~OptimiseICP();
        double f(vnl_vector< double > const &x);  
        
    private:
        arlCore::ICP& m_ICP;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    04/2009
     * @brief   Optimization of ICP criterion with Least Squares method
     */
    class OptimiseICP_LS : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API OptimiseICP_LS( arlCore::ICP&, UseGradient g );
        ARLCORE_API ~OptimiseICP_LS();
        void f(vnl_vector< double > const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector< double > const &x, vnl_matrix< double > &g);
        
    private:
        arlCore::ICP& m_ICP;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2008
     * @brief   Computation of the best line L that fits a 3D point cloud
     * The class seeks L so that sum || dist(M_i , L) ||^2 is minimal
     */
    class Optimise3DLine : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API Optimise3DLine( const std::vector< vgl_point_3d<double> >& );
        ARLCORE_API ~Optimise3DLine();
        ARLCORE_API double f(vnl_vector< double > const &x);

        ARLCORE_API unsigned int size( void );

    private:
        const std::vector< vgl_point_3d<double> > &m_points;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Being a point list P that lies on a sphere (circle), this class computes an estimation
     * of the sphere (circle) center C. The function that is minimized is:
     * 
     * sum || dist( estimated_radius , CP_i ) ||^2
     * 
     * where radius is the average of the CP_i
     */
    class OptimisePivot : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimisePivot( const arlCore::PointList& );
        ARLCORE_API ~OptimisePivot();
        ARLCORE_API double f(vnl_vector< double > const &x);  
        ARLCORE_API double getRadius( void );

    private:
        const arlCore::PointList &m_points;
        double m_radius;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    08/2008
     * @brief   Détermine T en minimisant la variation de d
     * Soit une pointe tenue par l'organe terminal et observée par des caméras stéréo
     * Soit M06i, une liste de transformations entre la base du robot et l'organe terminal
     * Soit TIPi(x,y), une liste de coordonnées d'une pointe dans le repère Video
     * Soit l'inconnue T, transformation entre le repère video et la base du robot
     * Critère d, la distance entre l'origine de l'organe terminal et la pointe
     * Minimisation de Delta d avec di = || M06i.T.TIPi ||
     */
    class OptimiseVideoRobot1 : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimiseVideoRobot1( const std::vector< arlCore::vnl_rigid_matrix > &M06, const arlCore::PointList &Tip );
        ARLCORE_API ~OptimiseVideoRobot1();
        ARLCORE_API double f(vnl_vector< double > const &x);  
        ARLCORE_API double getDistance( void ); // d
        ARLCORE_API double getStdDev( void ); // Standard deviation

    private:
        const arlCore::PointList &m_tip;
        const std::vector< arlCore::vnl_rigid_matrix > &m_M06;
        double m_distance;
        double m_stdDev;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    03/2009
     * @brief   Base class for End effector->Chessboard (X) & Robot base->Video (Z) calibration
     * Soit une liste de cameras calibrees (cam)
     * Soit un modele 3D de mire
     * Soit une liste de tranformations M06 entre la base du robot et l'organe terminal
     * Soit des listes de points detectes sur chaque camera
     * Critère Somme des distances au carre entre les points 3D reprojetes et detectes
     * Points 3D dans le repere des cameras apres transformation Modele->cameras = inv(X)*inv(A)*inv(Z)
     */
    class OptimiseVideoRobot : public CostFunction
    {
    public:
        ARLCORE_API OptimiseVideoRobot( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList );
        ARLCORE_API ~OptimiseVideoRobot();

        ARLCORE_API double criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z );

    protected:
        bool criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &invZ, vnl_vector<double> &fx );

    private:
        const std::vector< arlCore::vnl_rigid_matrix > &m_M06;
        const std::vector< std::vector< arlCore::PointList > > &m_chessboards; // [cam][pose][point]
        const std::vector<const arlCore::Camera*> &m_cameras;
        const std::vector< std::vector<arlCore::PointList> > &m_points2DList; // [cam][pose][point]
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    02/2009
     * @brief   Détermine X en minimisant l'erreur de reprojection
     * Soit une liste de cameras calibrees (cam)
     * Soit un modele 3D de mire
     * Soit une liste de tranformations M06 entre la base du robot et l'organe terminal
     * Soit des listes de points detectes sur chaque camera
     * Soit l'inconnue X, transformation entre l'organe terminal et la mire
     * Soit l'inconnue Z, transformation entre le repère video et la base du robot
     * Critère Somme des distances au carre entre les points 3D reprojetes et detectes
     * Points 3D dans le repere des cameras apres transformation Modele->cameras = inv(X)*inv(A)*inv(Z)
     */
    class OptimiseVideoRobotX : public vnl_cost_function, public OptimiseVideoRobot
    {
    public:
        ARLCORE_API OptimiseVideoRobotX( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const arlCore::vnl_rigid_matrix &Z,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList );
        ARLCORE_API ~OptimiseVideoRobotX();
        double f(vnl_vector< double > const &x);  

    private:
        const arlCore::vnl_rigid_matrix &m_Z;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    08/2009
     * @brief   OptimiseVideoRobotX with Least Squares method
     */
    class OptimiseVideoRobotX_LS : public vnl_least_squares_function, public OptimiseVideoRobot
    {
    public:
        ARLCORE_API OptimiseVideoRobotX_LS( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const arlCore::vnl_rigid_matrix &Z,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList,
                             unsigned int number_of_residuals=1, UseGradient g=use_gradient);
        ARLCORE_API ~OptimiseVideoRobotX_LS();

        void f(vnl_vector< double > const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector< double > const &x, vnl_matrix< double > &g);

    private:
        const arlCore::vnl_rigid_matrix &m_Z;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    02/2009
     * @brief   Détermine Z en minimisant l'erreur de reprojection
     * Soit une liste de cameras calibrees (cam)
     * Soit un modele 3D de mire
     * Soit une liste de tranformations M06 entre la base du robot et l'organe terminal
     * Soit des listes de points detectes sur chaque camera
     * Soit l'inconnue X, transformation entre l'organe terminal et la mire
     * Soit l'inconnue Z, transformation entre le repère video et la base du robot
     * Critère Somme des distances au carre entre les points 3D reprojetes et detectes
     * Points 3D dans le repere des cameras apres transformation Modele->cameras = inv(X)*inv(A)*inv(Z)
     */
    class OptimiseVideoRobotZ : public vnl_cost_function, public OptimiseVideoRobot
    {
    public:
        ARLCORE_API OptimiseVideoRobotZ( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const arlCore::vnl_rigid_matrix &X,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList );
        ARLCORE_API ~OptimiseVideoRobotZ();
        double f(vnl_vector< double > const &x);

    private:
        const arlCore::vnl_rigid_matrix &m_X;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    08/2009
     * @brief   OptimiseVideoRobotZ with Least Squares method
     */
    class OptimiseVideoRobotZ_LS : public vnl_least_squares_function, public OptimiseVideoRobot
    {
    public:
        ARLCORE_API OptimiseVideoRobotZ_LS( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const arlCore::vnl_rigid_matrix &X,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList,
                             unsigned int number_of_residuals=1, UseGradient g=use_gradient);
        ARLCORE_API ~OptimiseVideoRobotZ_LS();

        void f(vnl_vector< double > const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector< double > const &x, vnl_matrix< double > &g);

    private:
        const arlCore::vnl_rigid_matrix &m_X;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    03/2009
     * @brief   Détermine X et Z en minimisant l'erreur de reprojection
     * Soit une liste de cameras calibrees (cam)
     * Soit un modele 3D de mire
     * Soit une liste de tranformations M06 entre la base du robot et l'organe terminal
     * Soit des listes de points detectes sur chaque camera
     * Soit l'inconnue X, transformation entre l'organe terminal et la mire
     * Soit l'inconnue Z, transformation entre le repère video et la base du robot
     * Critère Somme des distances au carre entre les points 3D reprojetes et detectes
     * Points 3D dans le repere des cameras apres transformation Modele->cameras = inv(X)*inv(A)*inv(Z)
     */
    class OptimiseVideoRobotXZ : public vnl_cost_function, public OptimiseVideoRobot
    {
    public:
        ARLCORE_API OptimiseVideoRobotXZ( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList );
        ARLCORE_API ~OptimiseVideoRobotXZ();
        double f(vnl_vector< double > const &x);  
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    03/2009
     * @brief   OptimiseVideoRobotXZ with Least Squares method
     */
    class OptimiseVideoRobotXZ_LS : public vnl_least_squares_function, public OptimiseVideoRobot
    {
    public:
        ARLCORE_API OptimiseVideoRobotXZ_LS( const std::vector< arlCore::vnl_rigid_matrix > &M06,
                             const std::vector< std::vector< arlCore::PointList > > &models3D,
                             const std::vector<const arlCore::Camera*> &cams,
                             const std::vector< std::vector< arlCore::PointList > > &points2DList,
                             unsigned int number_of_residuals=1, UseGradient g=use_gradient);
        ARLCORE_API ~OptimiseVideoRobotXZ_LS();

        void f(vnl_vector< double > const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector< double > const &x, vnl_matrix< double > &g);
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   This class is used to perform a triangulation from N 2D visible points 
     * in N video images. Of course, the cameras need to be calibrated in the same frame.
     * The criterion that is optimized is:
     * sum || P_i(M) - m_i ||^2
     * where P_i is the projective function of the i th camera, m_i the 2d coordinates
     * of the point extracted in the i th image.
     */
    class OptimiseReprojection : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimiseReprojection(const std::vector<const Camera*> &cameras, const std::vector<Point::csptr> &points2D);
        ARLCORE_API ~OptimiseReprojection();

        double f(vnl_vector< double > const &point3D);

    private:
        const std::vector<const Camera*> &m_cameras;
        const std::vector<Point::csptr>  &m_points2D;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    12/2008
     * @brief   Solve Ax=xB equation by minimization of the reprojection
     */
/*  class AxxBReproj_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        AxxBReproj_cost_function(
            const std::vector< vnl_rigid_matrix > &A,
            const std::vector< vnl_rigid_matrix > &B,
            const std::vector<const arlCore::Camera*> &videoCams,
            const std::vector< PointList > &models3DList, //[pose]
            const std::vector< std::vector<PointList> > &points2DList); //[cam][pose]

        ~AxxBReproj_cost_function();

        double f(vnl_vector< double > const &x);

    private:
        const std::vector< arlCore::vnl_rigid_matrix > &m_A_matrix;
        const std::vector< arlCore::vnl_rigid_matrix > &m_B_matrix;
        const std::vector<const arlCore::Camera*> &m_videoCams;
        const std::vector< arlCore::PointList > &m_models3DList;
        const std::vector< std::vector<arlCore::PointList> > &m_points2DList;
    };*/

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    09/2008
     * @brief   AX=XB resolution by optimization of Dornaika method
     */
    class Dornaika_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API Dornaika_cost_function(const std::vector< vnl_rigid_matrix >&A, const std::vector< vnl_rigid_matrix >&B);
        ARLCORE_API ~Dornaika_cost_function();

        double f(vnl_vector< double > const &x);

    private:
        const std::vector< vnl_rigid_matrix > &m_A_matrix;
        const std::vector< vnl_rigid_matrix > &m_B_matrix;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    09/2008
     * @brief   AX=XB resolution by optimization of Dornaika method
     */
    class Dornaika_LS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API Dornaika_LS_cost_function(const std::vector< vnl_rigid_matrix >&A, const std::vector< vnl_rigid_matrix >&B,
        unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g=use_gradient);
        ARLCORE_API ~Dornaika_LS_cost_function();

        void f(vnl_vector< double > const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector< double > const &x, vnl_matrix< double > &g);

    private:
        const std::vector< vnl_rigid_matrix > &m_A_matrix;
        const std::vector< vnl_rigid_matrix > &m_B_matrix;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    09/2008
     * @brief   Evaluation by polynomial method of a field of deformation
     */
    class Polynomial_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API Polynomial_cost_function( const arlCore::PointList &real, const arlCore::PointList &distorded, unsigned int degree );
        ARLCORE_API ~Polynomial_cost_function();
        double f(vnl_vector<double> const &x);

        ARLCORE_API unsigned int getNbParameters();

    private:
        const arlCore::PointList &m_real;
        const arlCore::PointList &m_distorded;
        unsigned int m_nbEquations;
        unsigned int m_degree;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   This class is used to perform a triangulation from N 2D visible points 
     * in N video images. Of course, the cameras need to be calibrated in the same frame.
     * The criterion that is optimized is different from the one in the class 
     * arlcore::OptimiseReprojection, since in this case it takes into account the uncertainty
     * of each 2D point. The uncertainty is given in the covariance matrix associated to
     * each 2D point. If the covariance matrix are not filled, they are supposed to be identity.
     * The criterion that is optimized is
     * sum transpose( P_i(M) - m_i ) * cov_matrix_m_i^(-1) * ( P_i(M) - m_i )
     * where P_i is the projective function of the i th camera, m_i the 2d coordinates
     * of the point extracted in the i th image and cov_matrix_m_i the covariance matrix
     * of the point m_i extracted in the i th video image.
     * (Note: the covariance matrix is often anisotropic when the point extracted in the video 
     * image is the gravity center of an ellipsoid) 
     */
    class OptimiseReprojectionUncertainty : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimiseReprojectionUncertainty(const std::vector<const Camera*> &cameras, const std::vector<Point::csptr> &points2D);
        ARLCORE_API ~OptimiseReprojectionUncertainty();

        double f (vnl_vector< double > const &point3D);

    private:
        const std::vector<const Camera*> &m_cameras;
        const std::vector<Point::csptr>  &m_points2D;
        std::vector< vnl_matrix_fixed<double,2,2> > m_inv_cov_matrix;
        
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Given a set of lines that should intersect in a unique point, but 
     * which are in reality very close, this class finds the closest point to
     * each line. We optimize here the following criterion:
     * sum dist(L_i, M)^2
     * 
     * where M is the unknown 3D point, L_i are the lines which should intersect in a
     * unique point and dist(L_i, M) compute the euclidean distance between the line L_i and M.
     * This function is used for example in the triangulation method called 
     * ARLCORE_R3D_MULTI_LINES_APPROX and ARLCORE_R3D_MULTI_LINES_PERFECT
     */
    class OptimiseLineIntersection : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OptimiseLineIntersection(const std::vector< vgl_line_3d_2_points <double> >&lines);
        ARLCORE_API ~OptimiseLineIntersection();
        
        double f (vnl_vector< double > const &x);

    private:
        const std::vector< vgl_line_3d_2_points <double> >&m_lineList;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   This class perform a 3D/2D projective point registration called
     * EPPC (see. S. Nicolau, X. Pennec, L. Soler, N. Ayache, An accuracy
     * certified augmented reality system for therapy guidance. In ECCV04,
     * LNCS 3023, 2004 P 79-91)
     * 
     * Context of use:
     * You have a rigid 3D point set \tilde{M_i} that are seen by N camera. The 3D
     * point set coordinates are know in its own frame. This 3D point set is visible
     * in N video images that are jointly calibrated. In each video 
     * image you extract the projection of \tilde{M_i} and obtain \tilde{m_j_i} where j indicates
     * the camera number. Your purpose is to compute the rigid transformation
     * between the camera frame and the 3D point own frame.
     * 
     * Typically, this function is useful in augmented rality application (but not only).
     * You are looking with several cameras at a patient on which several radio-opaque 
     * markers were stuck. The patient has been CT-scanned and markers are visible both 
     * in the CT image and in the video images. To superimpose the 3D model segmented 
     * in the CT scan, you need to estimate the rigid transformation between the CT 
     * frame and the camera frame.
     *  
     * For this criterion to be interesting, the 3D point set must come
     * from a measurement that is accurate but not too much. You need also to
     * have an idea on your measure uncertainty (3D and 2D) and 
     * that they are different between the 3D points and the 2D points.
     * 
     * We assume you know for each point \tilde{M_i} and \tilde{m_j_i} its covariance matrix. 
     * The criterion to be optimized is:
     * sum transpose( M_i - \tilde{M_i)} ) * cov_matrix_M_i ^(-1) * ( M_i - \tilde{M_i)} ) 
     * + sum_j sum_i transpose( P_j(T*M_i) - \tilde{m_i}) *cov_matrix_m_j_i ^(-1)*( P_j(T*M_i) - \tilde{m_i}) 
     * 
     * where M_i are the PERFECT position of the 3D points M_i (which is not known 
     * but is estimated when optimizing the criterion). In this criterion, T and the M_i
     * are the unknown. The optimization on the unknown is done iteratively.
     * 
     * Since we are not (at first sight) interested by the 3D points estimation, the
     * class provides only the transformation T at the end of the optimization.
     * The function get3Derror provides the first term value during the optimization
     * The function get2Derror provides the second term value during the optimization
     * You can also ask for them at the end of the optimization.
     * If you need the estimation of the 3D points, just add a function that
     * gives back m_model3D.
     */
    class EPPC_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API EPPC_cost_function(const std::vector < const Camera *> &c,
            const std::vector<Point*> &model3D,
            const std::vector< std::vector<Point::csptr> >&points2DList,
            vnl_matrix_fixed<double,4,4> &trsf);
        ARLCORE_API ~EPPC_cost_function(){}
        double f (vnl_vector< double > const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);
        ARLCORE_API double get3Derror() const {return m_3Derror(0,0);};
        ARLCORE_API double get2Derror() const {return m_2Derror(0,0);};

    private:
        std::vector< vnl_matrix_fixed<double,3,3> > m_invCovMat3D;
        std::vector< std::vector<vnl_matrix_fixed<double,2,2> > > m_invCovMat2D;
        const std::vector<const Camera*> &m_cameras;
        const std::vector< Point* > &m_model3D;
        const std::vector< std::vector<Point::csptr> > &m_points2DList;
        const vnl_matrix_fixed<double,4,4>  &m_trsf;
        vnl_matrix_fixed<double,1,1> m_2Derror;
        vnl_matrix_fixed<double,1,1> m_3Derror;
    };
        
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   EPPC cost function with Least Square method
     */
    class EPPC_LS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API EPPC_LS_cost_function(const std::vector < const Camera *> &c,
            const std::vector<Point*> &model3D,
            const std::vector< std::vector<Point::csptr> >&points2DList,
            vnl_matrix_fixed<double,4,4> &trsf,
            unsigned int number_of_unknowns,
            unsigned int number_of_residuals, UseGradient g=use_gradient);

        ARLCORE_API ~EPPC_LS_cost_function(){}
        void f(vnl_vector<double> const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector<double> const &x, vnl_matrix<double>& g);
        ARLCORE_API double get3Derror() const {return m_3Derror(0,0);};
        ARLCORE_API double get2Derror() const {return m_2Derror(0,0);};

    private:
        std::vector< vnl_matrix_fixed<double,3,3> > m_invCovMat3D;
        std::vector< std::vector<vnl_matrix_fixed<double,2,2> > > m_invCovMat2D;
        const std::vector<const Camera*> &m_cameras;
        const std::vector< Point* > &m_model3D;
        const std::vector< std::vector<Point::csptr> > &m_points2DList;
        const vnl_matrix_fixed<double,4,4>  &m_trsf;
        vnl_matrix_fixed<double,1,1> m_2Derror;
        vnl_matrix_fixed<double,1,1> m_3Derror;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   ISPPC cost function
     * The context of use of this class is the same than for EPPC, but in that case
     * the noise on your 3D point is negligible with respect to the noise on your 
     * 2D points.
     * If you have isotropic noise on the 2D points extraction, or if you want to
     * neglect it, m_noise_feature must be set to false. Then you optimise the
     * following criterion:
     * sum_j sum_i ||P_j(T*M_i) - m_i||^2
     * 
     * where M_i are the position of the 3D points, m_j_i is the projection of
     * M_i in the j th camera, P_j is the projective function of the j th camera and
     * T is the unknown (rigid transformation). This criterion typically corresponds
     * to a 2D reprojection error (in pixel).
     * 
     * If you have anisotropic noise, you optimize the following criterion:
     * sum_j sum_i transpose( P_j(T*M_i) - m_i) *cov_matrix_m_j_i ^(-1)*( P_j(T*M_i) - m_i)
     * 
     * where  cov_matrix_m_j_i is the covariance matrix of the i th point in the
     * j th camera.
     * 
     * TODO : verbose mode so that we can easily print out the criterion evolution
     * during the minimization
     */
    class ISPPC_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API ISPPC_cost_function(const std::vector<const Camera *>&a,
            const std::vector< vnl_vector_fixed<double,4> > &points3D,
            const std::vector< std::vector<Point::csptr> > &points2D, bool noise_feature=false);  

        ARLCORE_API ~ISPPC_cost_function(){}
        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);
        ARLCORE_API double getNoiseFeature() const {return m_noise_feature;};
        ARLCORE_API void   setNoiseFeature(bool feat) {m_noise_feature = feat;};

    private:
        const std::vector< vnl_vector_fixed<double,4> > &m_points3D;
        const std::vector< std::vector<Point::csptr> > &m_points2D;
        const std::vector<const Camera*> &m_cameras; 
        std::vector< std::vector<vnl_matrix_fixed<double,2,2> > > m_invCovMat2D;
        bool m_noise_feature;//if the noise is isotropic, this bool is true and m_invCovMat2D is empty
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   ISPPC cost function with Least Square method
     */     
    class ISPPC_LS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API ISPPC_LS_cost_function(const std::vector<const Camera *>&a,
            const std::vector< vnl_vector_fixed<double,4> > &points3D,
            const std::vector< std::vector<Point::csptr> > &points2D,
            unsigned int number_of_unknowns,
            unsigned int number_of_residuals, UseGradient g=use_gradient, bool noise_feature = false);

        ~ISPPC_LS_cost_function(){}
        void f(vnl_vector<double> const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector<double> const &x, vnl_matrix<double>& g);
        ARLCORE_API double getNoiseFeature() const {return m_noise_feature;};
        ARLCORE_API void setNoiseFeature(bool feat) {m_noise_feature = feat;};

    private:
        const std::vector< vnl_vector_fixed<double,4> > &m_points3D;
        const std::vector< std::vector<Point::csptr> > &m_points2D;
        const std::vector<const Camera*> &m_cameras; 
        std::vector< std::vector<vnl_matrix_fixed<double,2,2> > > m_invCovMat2D;
        bool m_noise_feature;//if the noise is isotropic, this bool is true and m_invCovMat2D is empty
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   OSPPC cost function
     * The context of use of this class is the same than for EPPC, but in that case
     * the noise on your 3D point is negligible with respect to the noise on your 
     * 2D points. This criterion was introduced by Lu, C. Fast and Globally Convergent 
     * Pose Estimation from Video Images in Trans on PAMI 2000 vol 22(6) pp 610--622,
     * and minimize the sum of the squared 3D distance between the 3D points and the 
     * half lines beginning from the optical center C and passing through
     * the 2D points extracted from the video images.
     * 
     * You optimise the following criterion:
     * sum_j sum_i  || 1/{C_j m_j_i} ||^2 * || C_j(T * M_i) vect {C_j m_j_i} || ^2
     * 
     * where M_i are the position of the 3D points, C_j is the optical center of 
     * the j th camera, m_j_i is the projection of M_i in the j th camera, C_j(T * M_i)
     * is a vector between C_j and T*M_i, C_j m_j_i is a vector between C_j and M_j_i ,
     * vect is the cross product between two vectors and T is the unknown (rigid 
     * transformation). This criterion typically corresponds to a 3D projection error
     * in mm.
     * 
     * TODO : verbose mode so that we can easily print out the criterion evolution
     * during the minimization
     */
    class OSPPC_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API OSPPC_cost_function(const std::vector<const Camera *>&a,
            const std::vector< Point* > &points3D,
            const std::vector< std::vector<Point::csptr> > &points2D);
        ARLCORE_API ~OSPPC_cost_function(){};
        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);

    private:
        const std::vector<Point*> &m_points3D;
        const std::vector< std::vector<Point::csptr> > &m_points2D;
        const std::vector<const Camera*> &m_cameras;
        std::vector< std::vector< vnl_vector_fixed<double,3> > > m_extPoints2D;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   OSPPC cost function with Least Square method
     */
    class OSPPC_LS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API OSPPC_LS_cost_function(const std::vector<const Camera*>&,
            const std::vector<Point*> &points3D,
            const std::vector< std::vector<Point::csptr> > &points2D,
            unsigned int number_of_unknowns,
            unsigned int number_of_residuals, UseGradient g=use_gradient);
        ARLCORE_API ~OSPPC_LS_cost_function(){};
        void f(vnl_vector<double> const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector<double> const &x, vnl_matrix<double>& g);

    private:
        const std::vector<Point*> &m_points3D;
        const std::vector< std::vector<Point::csptr> > &m_points2D;
        const std::vector<const Camera*> &m_cameras;
        std::vector< std::vector< vnl_vector_fixed<double,3> > > m_extPoints2D;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization intrinsic parameters of a camera according to the 
     * method of Z. Zhang. Flexible Camera Calibration by Viewing a Plane from 
     * Unknown Orientations in ICCV 99.
     * The criterion that is optimised is Sum( Sum( ||Project(Tj * Mi) - mij||² ) )
     * and the unknown are the intrinsic parameter (hidden in the function Project) :
     * fx;fy;cx;cy;k1;k2;p1;p2;[k3;alphaC] : k3 and alphaC are not used
     * For each pose, the pattern can be different (number of points, 3D position of points)
     * It is assumed that all the points of each 3D model is visible in the video image
     * Therefore, if a point of the 3D model is not seen in the video image, it has to be
     * removed from the 3D model given to the function 
     */
    class Intrinsic_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        Intrinsic_cost_function(unsigned int nbParameters=0);
        ~Intrinsic_cost_function();
        ARLCORE_API unsigned int getNbParameters( void ) const;
        ARLCORE_API bool save( const std::string &fileName ) const;
        ARLCORE_API unsigned int addPattern( const std::vector<Point*>& points2D, const std::vector<Point*>& model3D );
        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);
        ARLCORE_API void setVerbose( bool verbose = true );
        ARLCORE_API std::vector<double> getReprojectionError( vnl_vector< double > const &x );

    private:
        unsigned int m_nbParameters;
        PlaneSystem m_universe;
        Camera *m_camera;
        std::vector< PointList* > m_2DpatternsList;
        std::vector< PointList* > m_3DpatternsList;
        bool m_verbose;
        bool m_available_reprojection_error;
        std::vector<double> m_point_reprojection_error;
        
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization intrinsic parameters with Levenberg optimization
     * see Intrinsic_cost_function for more explanation
     */
    class IntrinsicLS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API IntrinsicLS_cost_function(unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g=use_gradient);
        ARLCORE_API ~IntrinsicLS_cost_function();
        ARLCORE_API unsigned int getNbParameters( void ) const;
        ARLCORE_API bool save( const std::string &fileName ) const;
        ARLCORE_API unsigned int addPattern( const std::vector<Point*>& points2D, const std::vector<Point*>& model3D );
        void f(vnl_vector<double> const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector<double> const &x, vnl_matrix<double>& j);
        ARLCORE_API void setVerbose( bool verbose = true );
        ARLCORE_API void getReprojectionError( vnl_vector< double > const &x, vnl_vector< double > &fx );
 

    private:
        unsigned int m_nbParameters;
        PlaneSystem m_universe;
        Camera *m_camera;
        std::vector< PointList* > m_2DpatternsList;
        std::vector< PointList* > m_3DpatternsList;
        bool m_verbose;     
//      double m_reproj_error;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization of extrinsic parameters of several cameras
     * This class optimize a criterion that allows to calibrate in a same
     * frame several cameras that are looking at the same 3D model object in
     * several positions.
     * For more explanation on the criterion, cf. arlCore::refineExtrinsicCalibration
     */
    class Extrinsic_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        Extrinsic_cost_function(const std::vector<const Camera *>&a, unsigned int nou);
        ~Extrinsic_cost_function();
        double f(vnl_vector<double> const &x);
        ARLCORE_API unsigned int addPattern( const std::vector<std::vector<Point*> >& points2D, const std::vector<Point*>& model3D );
        //void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);
        ARLCORE_API void setVerbose( bool verbose = true );
        ARLCORE_API std::vector<double> getReprojectionError( vnl_vector< double > const &x );

    private:
        unsigned int m_nb3Dpoints;
        std::vector< std::vector< std::vector< Point* > > > m_2DpatternsList;
        std::vector< std::vector< Point* > >  m_3DpatternsList;
        const std::vector<const Camera*> &m_cameras;
        bool m_verbose;
        bool m_available_reprojection_error;
        std::vector<double> m_point_reprojection_error;
        
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization extrinsic parameters with Levenberg Marquart optimization
     * Note that the step variable in fgradf plays an important role in the convergence
     * speed. 1e-8 seems to be an optimal value. The reason is still unknown. Setting
     * this variable to 1e-12 or 1e-6 multiply the convergence time by a factor of 3
     * or of 4 !  
     */
    class ExtrinsicLS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API ExtrinsicLS_cost_function(const std::vector<const Camera *>&a,unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g=use_gradient);
        ARLCORE_API ~ExtrinsicLS_cost_function();
        void f(vnl_vector<double> const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector<double> const &x, vnl_matrix<double>& j);
        ARLCORE_API unsigned int addPattern( const std::vector<std::vector<Point*> >& points2D, const std::vector<Point*>& model3D );
        ARLCORE_API void setVerbose( bool verbose = true );
        ARLCORE_API void getReprojectionError( vnl_vector< double > const &x, vnl_vector< double > &fx );
    private:
        unsigned int m_nb3Dpoints;

        std::vector< std::vector< std::vector< Point* > > > m_2DpatternsList;
        std::vector< std::vector< Point* > >  m_3DpatternsList;
        const std::vector<const Camera*> &m_cameras;
        bool m_verbose;
    };
    
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization of the Sylvester equation AX=XB. To understand what is solved
     * please refer to arlcore::vnl_rigid_matrix::solve_AXXB
     * 2 criterions :
     * 1st criterion that is minimized is sum_i || A_iX-XB_i ||^2 where the norm is the
     * frobenius norm 
     * 2nd criterion : Strobl-Hirzinger
     */
    class AX_XB_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API AX_XB_cost_function(
            const std::vector< vnl_rigid_matrix >&A,
            const std::vector< vnl_rigid_matrix >&B,
            double alpha = 1.0):
        vnl_cost_function(6),
        m_A_matrix(A),
        m_B_matrix(B),
        m_alpha(alpha){}

        ARLCORE_API ~AX_XB_cost_function(){}

        ARLCORE_API double criterion( const arlCore::vnl_rigid_matrix &X );
        ARLCORE_API double getAlpha( const arlCore::vnl_rigid_matrix &X );
        ARLCORE_API void setAlpha( double );

        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);

    private:
        const std::vector< vnl_rigid_matrix > m_A_matrix;
        const std::vector< vnl_rigid_matrix > m_B_matrix;
        double m_alpha;
    };
    
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization of the Sylvester equation AX=XB. To understand what is solved
     * please refer to arlcore::vnl_rigid_matrix::solve_AXXB
     * Criterion of Strobl-Hirzinger with BXA = Z
     */
/*  class AXB_Z_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API AXB_Z_cost_function(
            const std::vector< vnl_rigid_matrix >&A,
            const std::vector< vnl_rigid_matrix >&B,
            double alpha = 1.0):
        vnl_cost_function(12),
        m_A_matrix(A),
        m_B_matrix(B),
        m_alpha(alpha){}

        ARLCORE_API ~AXB_Z_cost_function(){}

        ARLCORE_API double criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z );
        ARLCORE_API double getAlpha( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z );
        ARLCORE_API void setAlpha( double );

        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);

    private:
        const std::vector< vnl_rigid_matrix > m_A_matrix;
        const std::vector< vnl_rigid_matrix > m_B_matrix;
        double m_alpha;
    };*/

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Optimization of the AXBZ = 0 equation
     * 4 Criterions
     * 1) Minimization of sum_i || vnl_rigid_vector(A_iXB_iZ) ||^2 where the norm is the
     * two norm. AXBZ should be identity so the rigid vector should be [0 0 0 0 0 0]
     * 4) Criterion of Strobl-Hirzinger
     */
    class AXB_Z_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API AXB_Z_cost_function(
            const std::vector< vnl_rigid_matrix >&A,
            const std::vector< vnl_rigid_matrix >&B,
            double alpha=1.0):
        vnl_cost_function(12),
        m_A_matrix(A),
        m_B_matrix(B),
        m_alpha(alpha){}

        ARLCORE_API ~AXB_Z_cost_function(){}

        ARLCORE_API double criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z );
        ARLCORE_API double getAlpha( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z );
        ARLCORE_API void setAlpha( double );

        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);

    private:
        const std::vector< vnl_rigid_matrix > m_A_matrix;
        const std::vector< vnl_rigid_matrix > m_B_matrix;
        double m_alpha;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   The purpose of this class is to register 2 lists of matched
     * 3D points. It minimizes the following 3D/3D registration criterion:
     * 
     * sum_i  transpose(T* M_i - N_i) * Cov_M_i ^(-1) * (T* M_i - N_i)
     * 
     * where M_i are the points to register on the N_i points, T the rigid transformation
     * that is seeked, and Cov_M_i the covariance matrix that contains the uncertainty
     * on M_i. This means that we consider that the N_i points are perfect and that the M_i
     * observation are corrupted with a noise of covariance matrix Cov_M_i .
     */     
    class register3D3DUncertainty_cost_function : public vnl_cost_function, public CostFunction
    {
    public:
        ARLCORE_API register3D3DUncertainty_cost_function(const arlCore::PointList &points3D_source,
            const arlCore::PointList &points3D_cible);

        ARLCORE_API ~register3D3DUncertainty_cost_function(){}
        double f(vnl_vector<double> const &x);
        void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);

    private:
        const arlCore::PointList &m_points3D_source, m_points3D_cible;
        std::vector< vnl_matrix_fixed<double,3,3> > m_inv_cov_matrix;
    };
        
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   The purpose of this class is to register 2 lists of matched
     * 3D points. See register3D3DUncertainty_cost_function for all informations.
     * In this class we optimize with the Levenberg Marquardt method.
     */     
    class register3D3DUncertainty_LS_cost_function : public vnl_least_squares_function, public CostFunction
    {
    public:
        ARLCORE_API register3D3DUncertainty_LS_cost_function(const arlCore::PointList &points3D_source,
            const arlCore::PointList &points3D_cible,
            unsigned int number_of_unknowns,
            unsigned int number_of_residuals, UseGradient g=use_gradient);

        ARLCORE_API ~register3D3DUncertainty_LS_cost_function(){}
        void f(vnl_vector<double> const &x, vnl_vector< double > &fx);
        void gradf(vnl_vector<double> const &x, vnl_matrix<double>& g);

    private:
        const arlCore::PointList &m_points3D_source, m_points3D_cible;
        std::vector< vnl_matrix_fixed<double,3,3> > m_inv_cov_matrix;
    };

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Compute the average of a rotation list using a geodesic norm on SO(3)
     * It minimizes sum_i ||R_i - R_average ||^2  where R_average is the unknown 
     * and ||.|| is a riemannian distance
     * 
     * TODO still not include in arlcore::vnl_rigid_matrix
     */     
    class averageRotation_cost_function : public vnl_cost_function, public CostFunction
    {
     public:
      ARLCORE_API averageRotation_cost_function(const std::vector< arlCore::vnl_rotation3d_matrix* > list):vnl_cost_function(6),
      m_rotation_list(list){}
      ARLCORE_API ~averageRotation_cost_function(){}
      ARLCORE_API inline std::vector< arlCore::vnl_rotation3d_matrix* > get_matrix_list() const {return this->m_rotation_list;};
      ARLCORE_API inline int get_nou() const {return this->number_of_unknowns;};
      ARLCORE_API inline void set_nou(int a) {this->number_of_unknowns = a;};
      ARLCORE_API inline void set_matrix_list(std::vector< arlCore::vnl_rotation3d_matrix* > a) {this->m_rotation_list = a;};
      
      double f (vnl_vector< double > const &x);
      void gradf(vnl_vector<double> const &x, vnl_vector<double>& g);
    
      friend double sq_rieman_dist(arlCore::vnl_rotation3d_matrix, arlCore::vnl_rotation3d_matrix);

    private:
      std::vector< arlCore::vnl_rotation3d_matrix* > m_rotation_list;
      int number_of_unknowns;
    };
    
} // namespace arlCore
#endif // _ARLCORE_OPTIMIZATION_H
