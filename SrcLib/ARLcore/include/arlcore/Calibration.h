/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_CALIBRATION_H
#define _ARLCORE_CALIBRATION_H
#include <arlcore/Common.h>

#include <arlcore/Camera.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Point.h>
#include <arlcore/PointsList.h>

namespace arlCore
{
    /**
    * @brief Intrinsic camera parameter initialisation
    * @param[in] models3D List of 3D models, one for each pose (the object has to be a plane) 
    * If the 3D model does not change in all the pose you can provide only one PointList that corresponds
    * to the calibration object 3D points
    * If your calibration object changes in each pose (for example because some points are occluded), you have to
    * give for each pose the PointList corresponding to the calibration object that is used
    * @param[in] points2D : PointList vector of 2D points extracted for each pose
    * @param[in,out] camera : result of the calibration is written in arlcore::camera
    * @param[out] extrinsic : this vector vnl_rigid_matrix contains for each object pose (after calibration)
    * the rigid transformation between the camera frame and the object frame
    * @param[in] optimiserParameters : optional parameter list not used here
    * @param[out] log[0] gives the number of pose used in the calibration
    * @param[out] log[i+1] provides the average reprojection error for the ith pose (sum_j euclidean_dist(reproj_j - extracted_j))
    * @warning : this initialization fonction works for 3D objects which Z coordinates are 0 only.
    */
    ARLCORE_API bool initIntrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector <PointList>& points2D, Camera &, std::vector< vnl_rigid_matrix> &extrinsic,
        const std::vector<double> &optimiserParameters , std::vector<double> &log );

    /**
    * @brief Refinement of the camera intrinsic parameters
    * @param[in] models3D List of 3D models, one for each pose
    * If the 3D model does not change in all the N poses you can provide only one PointList that corresponds
    * to the calibration object 3D points
    * If your calibration object changes in each pose (for example because some points are occluded), you have to
    * give for each pose the PointList corresponding to the calibration object that is used
    * @param[in] points2D : PointList vector of 2D points extracted for each pose
    * @param[in,out] camera : contains the initialization parameters and is modified after the calibration
    * @param[in] nbParameters : corresponds to the model of camera that is used for the optimization
    * if nbParameters = 8, the camera has the following parameters : fc1, fc2, cc1, cc2, k1, k2, k3, k4 are optimized
    * if nbParameters = 6, the camera has the following parameters : fc1, fc2, cc1, cc2, k1, k2  are optimized (TODO not implemented)
    * if nbParameters = 5, the camera has the following parameters : fc1, fc2, cc1, cc2, k1  are optimized (TODO not implemented)
    * if nbParameters = 4, the camera has the following parameters : fc1, fc2, cc1, cc2  are optimized (TODO not implemented)
    * @param[out] extrinsic : : this vector vnl_rigid_matrix contains for each object pose (after calibration)
    * the rigid transformation between the camera frame and the object frame
    * @param[in] optimiserParameters optional parameter for the optimiser
    *               [0]=Optimization method (0=LEVENBERG-MARQUARDT ; 1=POWELL) Default=0
    *               [1]=Verbose (0=false else true) Default=0
    *               [2]=f tolerance Default=1e-6
    *               [3]=x tolerance Default=1e-8
    *               [4]=g tolerance Default=1e-5
    * @param[out] log List 
    *               [0]=Average reprojection error RMS at the end of the optimization (sqrt(mean(sum_i reprojection_error)))
    *               [1]=Average reprojection error RMS at the beginning of the optimization
    *               [2]=Number of iteration to converge
    *               [3]=Number of pose
    *               [3+1]=Number of point for the first pose
    *               [3+i]=Number of point for the ith pose
    *               [3+N]=Number of point for the last pose
    *               [3+N+1]=Number total of point
    *               [3+N+2 -> 3+N+??]= Reprojection error for each of the points
    * 
    */
    ARLCORE_API bool refineIntrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector <PointList>& points2D, unsigned int nbParameters,
        Camera &, std::vector< vnl_rigid_matrix> &, const std::vector<double> &optimiserParameters,
        std::vector<double> &log );

    /**
    * @brief Compute the intrinsic parameters of a camera : Initialisation + Refinement
    * @param[in] models3D List of 3D models, one for each pose
    * If the 3D model does not change in all the pose you can provide only one PointList that corresponds
    * to the calibration object 3D points
    * If your calibration object changes in each pose (for example because some points are occluded), you have to
    * give for each pose the PointList corresponding to the calibration object that is used
    * @param[in] points2D : PointList vector of 2D points extracted for each pose
    * @param[in,out] camera : contains the initialization parameters and is modified after the calibration
    * @param[in] nbParameters : corresponds to the model of camera that is used for the optimization
    * if nbParameters = 8, the camera has the following parameters : fc1, fc2, cc1, cc2, k1, k2, k3, k4 are optimized
    * if nbParameters = 6, the camera has the following parameters : fc1, fc2, cc1, cc2, k1, k2  are optimized (TODO not implemented)
    * if nbParameters = 5, the camera has the following parameters : fc1, fc2, cc1, cc2, k1  are optimized (TODO not implemented)
    * if nbParameters = 4, the camera has the following parameters : fc1, fc2, cc1, cc2  are optimized (TODO not implemented)
    * @param[out] extrinsic : : this vector vnl_rigid_matrix contains for each object pose (after calibration)
    * the rigid transformation between the camera frame and the object frame
    * @param[in] optimiserParameters optional parameter list that is used for the refinement method (see refineIntrinsicCalibration
    * to understand how to fill it)
    * log will contains the log of arlcore::initIntrinsicCalibration and arlcore::refineIntrinsicCalibration
    * @param[out] log[0] gives the number of pose N used in the calibration
    * @param[out] log[1 -> N] provides the average reprojection error for the ith pose during the initialization process
    * @param[out] log[N+1] = Average reprojection error RMS at the end of the optimization
    * @param[out] log[N+2] = Average reprojection error RMS at the beginning of the optimization
    * @param[out] log[N+3] = Iteration number to converge
    * @param[out] log[N+4] gives the number of pose N used in the calibration
    * @param[out] log[N+4+i] gives the number of point of the ith pose
    * @param[out] log[N+4+N] gives the number of point of the Nth pose
    * @param[out] log[N+4+N+1 -> N+4+N+1 +?? ] Reprojection error for each of the points
    */
    ARLCORE_API bool intrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector <PointList>& points2D, unsigned int nbParameters,
        Camera &, std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double>& optimiserParameters,
        std::vector<double>& log );

    /**
    * @brief Initialisation of the extrinsic calibration between several cameras
    * The parameter cameras is not changed !
    * The rigid transformation from the calibration grid (for the ith pose) to the camera 0 
    * is given in extrinsics[i-1]
    * The rigid transformation from the camera 0 frame to the jth camera frame is given in 
    * extrinsic[nbPose+j-1]
    * @param[in] models3D List of 3D models points for each pose
    * @param[in] listsPoints2D List of 2D points (reprojection of th 3D model points) detected for each pose for each camera
    * @param[in] cameras List of cameras that we want to calibrate
    * @param[out] extrinsics contains the transformation matrix from the camera 0 frame to the 3D model for each pose
    * and is followed by the the transformation matrix from the camera 0 frame to camera j frame (computed using the pose 0 only)
    * cf. Calibration.cpp to understand the criteria
    * @param[in] optimiserParameters optional list of parameters for the planarHomography function used in the function
    * @param[out] log[0] gives the number of pose used in the calibration
    * @param[out] log List of results to check the estimation quality (TODO still not used now)
    */
    ARLCORE_API bool initExtrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector<std::vector<std::vector<Point*> > >& listsPoints2D, const std::vector<const Camera*>& cameras,
        std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double> &optimiserParameters, std::vector<double> &log );

    ARLCORE_API bool initExtrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector<std::vector<std::vector<Point*> > >& listsPoints2D, const std::vector<Camera>& cameras,
        std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double> &optimiserParameters, std::vector<double> &log );

    /**
    * @brief Refinement of the extrinsic calibration of several cameras
    * @param[in] models3D List of 3D models, one for each pose
    * @param[in] listsPoints2D List of 2D points (reprojection of th 3D model points) detected for each pose for each camera
    * @param[in] cameras List of cameras that we want to calibrate (extrinsic parameters have to be identity)
    * @param[out] extrinsics contains the transformation matrix from the camera 0 frame to the 3D model for each pose
    * and is followed by the transformation matrix T_j from the camera 0 frame to camera j frame ( M_i^j = T_j * M_i^0)
    * cf. Calibration.cpp to understand the criteria
    * @param[in] optimiserParameters optional parameter for the optimiser
    *               [0]=Optimization method (0=LEVENBERG-MARQUARDT ; 1=POWELL) Default=0
    *               [1]=Verbose (0=false else true) Default=0
    *               [2]=f tolerance Default=1e-6
    *               [3]=x tolerance Default=1e-8
    *               [4]=g tolerance Default=1e-5
    * @param[out] log List 
    *               [0]=End error
    *               [1]=Start error
    *               [2,n+1]=Reprojection error for each of the n points
    */
    ARLCORE_API bool refineExtrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector<std::vector<std::vector<Point*> > >& listsPoints2D, const std::vector<const Camera*>& cameras,
        std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double> &optimiserParameters, std::vector<double> &log );

    ARLCORE_API bool refineExtrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector<std::vector<std::vector<Point*> > >& listsPoints2D, const std::vector<Camera>& cameras,
        std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double> &optimiserParameters, std::vector<double> &log );

    /**
    * @brief Extrinsic calibration of several cameras
    * @param[in] models3D List of 3D models, one for each pose
    * @param[in] listsPoints2D List of 2D points (reprojection of th 3D model points) detected for each pose for each camera
    * @param[in] cameras List of cameras that we want to calibrate (extrinsic parameters have to be identity)
    * @param[out] extrinsics contains the transformation matrix from the camera 0 frame to the 3D model for each pose
    * and is followed by the transformation matrix from the camera 0 frame to camera j frame (computed using the pose 0 only)
    * cf. Calibration.cpp to understand the criteria
    * @param[in] optimiserParameters optional parameter list that is used for the refinement method (see refineExtrinsicCalibration
    * to understand how to fill it)
    * @param[out] log[0] gives the number of pose used in the calibration
    * @param[out] log List (identical to the arlcore::refineExtrinsicCalibration log)
    *               [1]=End error
    *               [2]=Start error
    *               [3,n+1]=Reprojection error for each of the n points
    */
    ARLCORE_API bool extrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector<std::vector<std::vector<Point*> > >& listsPoints2D, const std::vector<const Camera*>& cameras,
        std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double> &optimiserParameters, std::vector<double> &log );

    ARLCORE_API bool extrinsicCalibration(const std::vector<PointList>& models3D,
        const std::vector<std::vector<std::vector<Point*> > >& listsPoints2D, const std::vector<Camera>& cameras,
        std::vector< vnl_rigid_matrix>& extrinsics, const std::vector<double> &optimiserParameters, std::vector<double> &log );

} // namespace arlCore
#endif // _ARLCORE_CALIBRATION_H
