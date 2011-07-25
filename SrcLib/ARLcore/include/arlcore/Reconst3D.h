/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_RECONST3D_H
#define _ARLCORE_RECONST3D_H
#include <arlcore/Common.h>

#include <vector>
#include <string>

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

#include <arlcore/MatrixR.h>
#include <arlcore/Camera.h>
#include <arlcore/PointsList.h>

namespace arlCore
{
    /**
    * @brief 3D triangulation methods that are available 
    */
    enum ARLCORE_RECONSTRUCTION3D { ARLCORE_R3D_UNKNOWN,
        ARLCORE_R3D_HORAUD_APPROX, ARLCORE_R3D_HORAUD_PERFECT,
        ARLCORE_R3D_TWO_LINES_APPROX, ARLCORE_R3D_TWO_LINES_PERFECT,
        ARLCORE_R3D_MULTI_LINES_APPROX, ARLCORE_R3D_MULTI_LINES_PERFECT,
        ARLCORE_R3D_REPROJECTION_OPTIMIZATION, ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY,
        ARLCORE_R3D_NBTYPE };

    static std::string ARLCORE_RECONSTRUCTION3D_NAMES[ARLCORE_R3D_NBTYPE]={ "ARLCORE_R3D_UNKNOWN",
        "ARLCORE_R3D_HORAUD_APPROX", "ARLCORE_R3D_HORAUD_PERFECT",
        "ARLCORE_R3D_TWO_LINES_APPROX", "ARLCORE_R3D_TWO_LINES_PERFECT",
        "ARLCORE_R3D_MULTI_LINES_APPROX", "ARLCORE_R3D_MULTI_LINES_PERFECT",
        "ARLCORE_R3D_REPROJECTION_OPTIMIZATION", "ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY"};

    /**
    * @brief Compute the 3D position in the camera frame of a point M detected by N cameras
    * The fonction uses 2 cameras or more. It depends on the method you choose.
    * Put pixelFrame to true if the 2D points are given in the pixel frame (pixel coordinate)
    * Put pixelFrame to false if the 2D points are given in the focal plane (3D real coordinate
    * that are generally computed by multiplying the pixel coordinates with the inverse 
    * of the camera intrinsic matrix).
    * 
    * @param[in] points2D : arlCore::Point list that corresponds to the image coordinates
    * detected in each camera of the point we want to triangulate
    * @param[in] point3D : triangulation result
    * @param[in] camsList : camera list that is used for the reconstruction. The camera SHOULD 
    * be jointly calibrated. If not you will get absurd result. If a camera have not been 
    * intrinsically calibrated, you will get also absurd result. Consequently you should
    * test your camera before using them (using method Camera::isIntrinsicCalibrated() )  
    * 
    * m_1,....,m_i,....,m_N are the points detected in the ith camera image (pixel coordinates)
    * Using the distortion parameters of the camera we can compute the coordinates in pixel of 
    * the point M reprojections if there is no distortion. In that case, calling
    * mc_1,....,mc_i,....,mc_N  the distortion-free point coordinates (in pixel), it is 
    * possible to accurately estimate the M position by computing the intersection of the back projected
    * lines C_im_i (where C_i is the optical center of the ith camera). 
    * 
    * Almost all our algorithms ends by APPROX or PERFECT. APPROX means that the distortion
    * correction has been performed using a simple and quick iterative algorithm (that
    * genrally works when distortion is not too important). PERFECT corresponds to a more
    * specialized algorithm (coming from VNL) that finds the roots of a polynome. The latter
    * method is much slower (50 times and more...)
    * 
    * ARLCORE_R3D_HORAUD : compute a triangulation from an analytic analysis that
    * takes into account 2 cameras only (see R. Horaud. 
    * Vision par ordinateur: outils fondamentaux. Ed. Hermes. 1995)
    * ARLCORE_R3D_TWO_LINES : compute a triangulation from the back projection of
    * the lines C_1m_1 and C_2m_2 (the triangulated point is the point at an equal
    * distance from C_1m_1 and C_2m_2).
    * ARLCORE_R3D_MULTI_LINES : compute a triangulation from the back projection of
    * all the lines C_im_i. The triangulated point P is the point that optimize
    * the distance to all lines
    * ARLCORE_R3D_REPROJECTION_OPTIMIZATION : compute a triangulation from the reprojection 
    * error in each image. The triangulated point P is the point that optimize
    * the following criterion sum_i || P_i(P) - m_i||^2 where P_i is the projective
    * function of the ith camera.
    * ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY : same as before but the 
    * criterion takes into account the uncertainty on the m_i. We optimize:
    * sum_i transpose(P_i(P) - m_i) * cov_m_i^(-1) * (P_i(P) - m_i) where
    * cov_m_i is the covariance matrix of m_i
    * 
    * @param[out] log Provide different values that depends on the chosen method :
    *
    * ARLCORE_R3D_TWO_LINES_APPROX and ARLCORE_R3D_TWO_LINES_PERFECT:
    * log[0] : distance between the two reprojection lines (in mm)
    * 
    * ARLCORE_R3D_MULTI_LINES_APPROX et ARLCORE_R3D_MULTI_LINES_PERFECT 
    * log[0] = average distance moyenne between the reconstructed point and all reprojected lines (in mm)
    * log[1] = average distance moyenne between the initial point (before optimization) and all reprojected lines (in mm)
    * 
    * ARLCORE_R3D_REPROJECTION_OPTIMIZATION and ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY :
    * log[0] = average distance between the reconstructed point that is reprojected in the image and its detection in all images (pix)
    * log[1] = average distance between the initial point that is reprojected in the image and its detection in all images (pix)
    */
    ARLCORE_API bool reconst3D( const std::vector<Point::csptr> &points2D, const std::vector< const Camera*> &camsList, Point &point3D,
        ARLCORE_RECONSTRUCTION3D, unsigned int &plane, std::vector<double> &log, double errorMax=-1,  bool propagateCov=false, const bool pixelFrame=true);

    ARLCORE_API bool reconst3D( const std::vector<Point::csptr> &list2D, const std::vector<Camera> &camsList, Point &point3D,
        ARLCORE_RECONSTRUCTION3D, unsigned int &plane, std::vector<double> &log, double errorMax=-1,  bool propagateCov=false, const bool pixelFrame=true);
    /**
    * @brief This function computes the rigid transformation from the 3D model frame
    * to the camera frame. In fact, the function firstly compute an homography that
    * fits the N points of the 3D model to the N corresponding points detected in the 
    * video image. Since we compute an homography, this means that the 3D model has to
    * be (almost) a plane and has to contain at least 4 points.
    * Of course, 3D and 2D points must be perfectly matched:
    * no NULL pointer and vector of same size
    * @param[in] Camera : camera that looks to the planar object
    * @param[in] points2D : list of 2D coordinates of the 3D model points detected in the video image
    * @param[in] T : rigid transformation that corresponds to the passage matrix from the 3D model frame
    * to the camera frame - (T * P_i = M_i where P_i are coordinates in the 3D model frame
    * and M_i in the camera frame)
    * @param[in] optimiser_parameters :  optimiser_parameters[0] and optimiser_parameters[1] must
    * be defined before using the function. If optimiser_parameters[0] = 1, then, log[0] contains
    * the average reprojection error and log[1] the norm of the first column of H (which should be equal to 1 !!).
    * To apply the prenormalisation (which ensure a good calculation of the svd) 
    * optimiserParameters[1] must be set to 1.
    *  
    */
    ARLCORE_API bool planarHomographyRegistration_3D_2D( const Camera &, const std::vector< Point::csptr > &points2D, const std::vector< Point* > &model3D,
         vnl_rigid_matrix &T, const std::vector<double> &optimiser_parameters, std::vector<double> &log, bool verbose=false);

//  ARLCORE_API bool planarHomographyRegistration_3D_2D( const Camera &, const std::vector< Point* > &points2D, const PointList &model3D,
//       vnl_rigid_matrix &T, const std::vector<double> &optimiser_parameters, std::vector<double> &log, bool verbose=false);

    /**
    * @brief From a general point of view, this function provides "the best" homography between 
    * two point sets in 2D. More specifically, it is used for camera calibration
    * in order to find an estimation of the intrinsic parameters using several poses 
    * of a planar chessboard.
    * The function finds H so that : 
    *       [u_i]       [X_i] 
    * m_i = [v_i] = H x [Y_i] 
    *       [ 1 ]       [ 1 ] 
    * 
    * where:
    * @param[in] points2D : list of 2D coordinates of the point list 1
    * @param[in] model3D  : list of 2D coordinates of the point list 1 (it can be also the list
    * of 3D coordinates of the 3D model points from the chessboard with z=0)
    * @param[in] H : homography 3x3 matrix
    * @param[in] optimiser_parameters :  optimiser_parameters[0] and optimiser_parameters[1] must
    * be defined before using the function. If optimiser_parameters[0] = 1, then, log[0] contains
    * the average reprojection error
    * To apply the prenormalisation (which ensure a good calculation of the svd) 
    * optimiserParameters[1] must be set to 1.
    * Of course, the 2 point list must be perfectly matched:
    * no NULL pointer and vector of same size
    *  
    */
    ARLCORE_API bool planarHomographyUnknownIntrinsic( const std::vector<Point*> &points2D, const std::vector< Point* > &model3D, vnl_matrix_fixed<double,3,3> &H, const std::vector<double> &optimiserParameters, std::vector<double> &log, bool verbose);

    ARLCORE_API bool planarHomographyUnknownIntrinsic( const PointList &points2D, const PointList &model3D, vnl_matrix_fixed<double,3,3> &H, const std::vector<double> &optimiserParameters, std::vector<double> &log, bool verbose);
         
    /**
     * @brief 3D/2D registration method
     * more informations about the theory can be found in 
     */
    enum ARLCORE_PROJECTIVE_REGISTRATION { ARLCORE_PR_UNKNOWN,
        ARLCORE_PR_ISPPC, ARLCORE_PR_ISPPC_LM, ARLCORE_PR_ISPPC_CG,
        ARLCORE_PR_ISPPC_ANISOTROP, ARLCORE_PR_ISPPC_ANISOTROP_LM,
        ARLCORE_PR_OSPPC, ARLCORE_PR_OSPPC_LM, ARLCORE_PR_OSPPC_CG,
        ARLCORE_PR_EPPC, ARLCORE_PR_EPPC_LM,  ARLCORE_PR_EPPC_CG, 
        ARLCORE_PR_NBTYPES };
    static std::string ARLCORE_PROJECTIVE_REGISTRATION_NAMES[ARLCORE_PR_NBTYPES]={ "ARLCORE_PR_UNKNOWN",
        "ARLCORE_PR_ISPPC", "ARLCORE_PR_ISPPC_LM", "ARLCORE_PR_ISPPC_CG",
        "ARLCORE_PR_ISPPC_ANISOTROP", "ARLCORE_PR_ISPPC_ANISOTROP_LM",
        "ARLCORE_PR_OSPPC", "ARLCORE_PR_OSPPC_LM", "ARLCORE_PR_OSPPC_CG",
        "ARLCORE_PR_EPPC", "ARLCORE_PR_EPPC_LM", "ARLCORE_PR_EPPC_CG" };

    /**
     * @brief 
     * Rigid registration of a 3D point set so that its reprojection on one image fits a 2D point set
     *
     * @param[in] Camera : camera that looks to the planar object.
     * 
     * @param[in] model3D  : list of 3D coordinates of the 3D model
     * @param[in] points2D : list of 2D reprojection of the 3D points on the video image
     * @param[out] T : rigid transformation that contains the solution after computation 
     * 
     * TODO This function works if the 3D model is planar (because of the initialization) !
     * TODO We have to retrieve this constraint 
     * TODO This function needs optimiserParameters and log
     */
    ARLCORE_API bool monoViewPointRegistration3D2D( const arlCore::Camera & ,
        const std::vector<Point::csptr> &points2D, const std::vector<Point*> &model3D,
        arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION method,
        const std::vector<double> &optimiserParameters , std::vector<double> &log, bool verbose );

    ARLCORE_API bool monoViewPointRegistration3D2D( const arlCore::Camera & ,
        const std::vector<Point::csptr> &points2D, const PointList &model3D,
        arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION method,
        const std::vector<double> &optimiserParameters , std::vector<double> &log, bool verbose );

/**
 * @brief 
 * Rigid registration of a 3D point set so that its reprojection on several images fits 2D point sets
 * 
 * This function compute the rigid transformation that registers a 3D rigid object (defined 
 * in its own frame)  in the world frame of several jointly calibrated camera (we call here
 * world frame the frame in which the camera were calibrated). Practically, this frame
 * corresponds to the position of a chessboard that has been used during the simultaneous
 * calibration of cameras. 
 *
 * param[in] : cameras = camera vector used for the registration (it works also with one camera only). The camera SHOULD 
 * be jointly calibrated. If not you will get absurd result. If a camera have not been 
 * intrinsically calibrated, you will get also absurd result. Consequently you should
 * test your camera before using them (using method Camera::isIntrinsicCalibrated() )  
 * param[in] : points2D = vector of vector of 2D points [j] detected in the camera [i]
 * param[in] : T = initialisation of the iterative registration
 * param[in] : methode = choice of the optimization criterion and method (more informations on each 
 * criterion can be found in Optimization.h and in the following paper : S.Nicolau et.al. An Accuracy 
 * Certified Augmented Reality System for Therapy Guidance. In Proc. of the 8th European Conference 
 * on Computer Vision (ECCV 04), Part III, volume LNCS 3023, Prague, pages 79-91, May 2004)
 * param[in] : optimiserParameters = parameters for optimization function used (TODO still not used)
 * 
 * param[out] : log
 *      EPPC:
 *           log[0] = criterion value at the end of optimization divided by the point number
 *           log[1] = criterion value at the beginning of optimization divided by the point number
 *           log[2] = part of the criterion value that corresponds to the 2D contribution (TODO not divided by the point number)
 *           log[3] = part of the criterion value that corresponds to the 3D contribution (TODO not divided by the point number)
 *           log[4] = stop criterion of the EPPC, corresponds to the distance between the 2 last transformations
 *      ISPPC & OSPPC:
 *           log[0] = criterion value at the end of optimization divided by the point number
 *           log[1] = criterion value at the beginning of optimization divided by the point number
 * 
 * More information are given in Reconst3D.cpp
 * 
 */

    ARLCORE_API bool multiViewPointRegistration3D2D ( const std::vector< const Camera* >& ,
        const std::vector< std::vector<Point::csptr> >& points2D, const std::vector<Point*> &model3D,
        arlCore::vnl_rigid_matrix &T, ARLCORE_PROJECTIVE_REGISTRATION,
        const std::vector<double> &optimiserParameters, std::vector<double> &log, bool verbose);

    ARLCORE_API bool multiViewPointRegistration3D2D ( const std::vector< Camera >& ,
        const std::vector< std::vector<Point::csptr> >& points2D, const std::vector<Point*> &model3D,
        arlCore::vnl_rigid_matrix &T, ARLCORE_PROJECTIVE_REGISTRATION,
        const std::vector<double> &optimiserParameters, std::vector<double> &log, bool verbose);

    ARLCORE_API bool multiViewPointRegistration3D2D ( const std::vector< const Camera* >& ,
        const std::vector< std::vector<Point::csptr> >& points2D, const PointList &model3D,
        arlCore::vnl_rigid_matrix &T, ARLCORE_PROJECTIVE_REGISTRATION,
        const std::vector<double> &optimiserParameters , std::vector<double> &log, bool verbose);

    ARLCORE_API bool multiViewPointRegistration3D2D ( const std::vector< Camera >& ,
        const std::vector< std::vector<Point::csptr> >& points2D, const PointList &model3D,
        arlCore::vnl_rigid_matrix &T, ARLCORE_PROJECTIVE_REGISTRATION,
        const std::vector<double> &optimiserParameters , std::vector<double> &log, bool verbose);

} // namespace arlCore
#endif // _ARLCORE_RECONST3D_H
