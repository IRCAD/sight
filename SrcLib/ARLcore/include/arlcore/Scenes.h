/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_SCENES_H
#define _ARLCORE_SCENES_H
#include <arlcore/Common.h>

#include <arlcore/Scene.h>

namespace arlCore
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Scenes virtuelles specifiques
     */

    /**
     * @brief : Scene for several unit test
     * This scene is used for :
     * arlCore::testRegistration3D3D 
     * arlCore::testProjectiveRegistration 
     * arlCore::testHomographyRegistration
     * arlCore::testReconstructionPolyscopic
     * arlCore::testEpipolarMatching
     * arlCore::testReconstructionPolyscopicUncertainty
     * 
     * It creates a scene with nbCamerasMax camera focusing at a point m_Centre randomly chosen in a cube of 300 mm edge
     * (size_rand_mire = 300). The intrinsic camera parameter are randomly chosen as well and are defined in the Function.
     * The number of camera cannot be below 2. The camera position are randomly chosen in a cube of 500 mm edge and
     * the minimal distance between the camera and the focused point is 1000 (distance_mire_cam_min).
     * The number of points "nbPoints2D" is randomly chosen so that it is comprised between 4 and nbPoints2DMax+4. This
     * is necessary for most of the unit test. For some test (arlCore::testReconstructionPolyscopicUncertainty), 
     * we choose to create only 1 point to avoid correlation within the statistical tests. This can be done by 
     * setting nbPoints2DMax to 1.
     * 
     * Two tags are generated: the first one contains "nbPoints2D" points randomly generated in a sphere 
     * centered in m_Centre and of radius "worldPointsSize". The second tag (used essentially for homography
     * evaluation) is a flat cloud of "nbPoints2D" points generated in a square centered in (0,0,0). The edge
     * of the square has a dimension equal to "worldPointsSize".
     */
    class SceneUnitTest : public Scene
    {
    public:
        ARLCORE_API SceneUnitTest( PlaneSystem&, double worldPointsSize=200, unsigned int nbPoints2DMax=40 );
        ARLCORE_API ~SceneUnitTest( void );
        ARLCORE_API const Point& getCentre( void ) const;
        
    private:
        Point m_Centre;
    };
    
    /**
     * @brief : Scene for several unit test
     * This scene is used for :
     * arlCore::testInitIntrinsicCalibration
     * arlCore::testIntrinsicCalibration
     * arlCore::testInitExtrinsicCalibration
     * arlCore::testExtrinsicCalibration
     * 
     * @param[in] = PlaneSystem
     * @param[in] = k_range (std dev on the distortion values that are randomly chosen)
     * @param[in] = nbCameras_MAX
     * @param[in] = nbPoses_MAX
     * 
     * This scene generates one or several cameras which look(s) at a calibration object in several 
     * positions. It is used to validate the calibration algorithm implemented from Z. Zhang paper's
     * (Flexible Camera Calibration by Viewing a Plane from Unknown Orientations in ICCV 99) for one
     * camera and the calibration algorithm described in the PhD thesis of S. Nicolau (see Optimization.h
     * to find the calibration criterion).
     * 
     * The camera is randomly positioned in a sphere of radius "SphereRadius", looking at the point 
     * "Origin", with a minimal distance to "Origin" of "DistanceMin". The point "Origin" is randomly chosen
     * in a cube centered in (0,0,0) withan edge of 100. The intrinsic parameters are chosen as follows:
     * fx = 1000 + GaussianNoise(mean = 0, std dev = 200)
     * fy = 1000 + GaussianNoise(mean = 0, std dev = 200)
     * cx = 400 + GaussianNoise(mean = 0, std dev = 100)
     * cy = 200 + GaussianNoise(mean = 0, std dev = 50)
     * k1 = 0 + GaussianNoise(mean = 0, std dev = k_range[0])
     * k2 = 0 + GaussianNoise(mean = 0, std dev = k_range[1])
     * k3 = 0 + GaussianNoise(mean = 0, std dev = k_range[2])
     * k4 = 0 + GaussianNoise(mean = 0, std dev = k_range[3])
     * 
     * The number of pose is randomly chosen between 4 and nbPoses_MAX + 4
     * The number of camera is randomly chosen between 1 and nbCameras_MAX +1 (if nbCameras_MAX = 1, then
     * only one camera is created). Consequently, you have to avoid negative value for nbCameras_MAX (it
     * is not tested).
     * 
     * For each pose i the calibration object is different and the number of point is equal to
     * ChessNbPoints[i]). The calibration object is flat (Z=0), and its points are randomly chosen in 
     * a square centered in (0,0,0) with an edge of "ChessSize"=120. Each pose corresponds to a Tag
     * that is created in the scene.
     */
    class SceneUnitTestInitIntrinsicCalib : public Scene
    {
    public:
        ARLCORE_API SceneUnitTestInitIntrinsicCalib( PlaneSystem& , std::vector<double> k_range, const unsigned int nbCameras_MAX, const unsigned int nbPoses_MAX);
        ARLCORE_API ~SceneUnitTestInitIntrinsicCalib( void );
        ARLCORE_API const Point& getCentre( void ) const;
        ARLCORE_API const unsigned int getNbPoses( void ) const;
        ARLCORE_API const unsigned int getChessNbPoints( unsigned int ) const;
        
    private:
        Point m_Centre;
        unsigned int m_nbPoses;
        std::vector<unsigned int> m_ChessNbPoints;
    };
    
    
    /**
     * @brief : Scene for several unit test
     * This scene is used to compare the performance of the following registration criterion :
     * EPPC
     * Isotropic and anisotropic ISPPC
     * OSPPC
     * Isotropic and anisotropic 3D/3D registration
     * 
     * @param[in] = PlaneSystem
     * @param[in] = parameters (allows to specify parameters of the generated scene)
     * @param[in] = staticStatus (allows to specify which parameters of the scene is chosen once for all)
     * @param[in] = noiseValues
     * @param[out] = RSB_3D
     * @param[out] = RSB_2D
     * 
     * A set of cameras is created and they look at m_Centre (which is randomly chosen in an cube of 
     * size_rand_point_de_mire_camera and centered in (0,0,0) ). Their intrinsic parameters can 
     * be configured statically in the .cpp file using int_param and int_range.
     * A 3D object is created in the world and a set of control point as well (2 Tags are
     * then generated, the parameters of which are defined in the param[in] parameters). 
     * 
     * -The object to register contains parameters[0] points, but if staticStatus[0] == 0 the number
     * of points is randomly chosen between 4 and parameters[0].
     * -The scene contains parameters[1] camera, but if staticStatus[1] == 0 the number
     * of points is randomly chosen between 2 and parameters[1].
     * -The solid angle in which the camera are positioned is parameters[2] steradian (value should
     * be comprised between 0 and 720), but if staticStatus[2] == 0, it is equal to 720.
     * -The size of the shape in which the object points are randomly chosen is equal to 
     * parameters[3], but if staticStatus[3] == 0 it is randomly chosen between 20 and parameters[3].
     * -The shape of the object is given by parameters[4] (see Point.h to learn the available shape and
     * their associated number), but if staticStatus[4] == 0 the shape is randomly chosen between 
     * those available in [1...parameters[4] ]
     * -The number of control points is given in parameters[5] points, but if staticStatus[0] == 0 the number
     * of points is randomly chosen between 1 and parameters[5].
     * -The size of the shape in which the control points are randomly chosen is equal to 
     * parameters[6], but if staticStatus[6] == 0 it is randomly chosen between 1 and parameters[6].
     * -The distance between the registration and the control point is equal to 
     * parameters[7], but if staticStatus[7] == 0 it is randomly chosen between 50 and parameters[7].
     * -The shape of the control points is given by parameters[8], but if staticStatus[8] == 0 
     * the shape is randomly chosen between those available in [1...parameters[8] ]
     * 
     * noiseValues correspond to the std dev of the gaussian error that will be added: 
     * -to the x,y,z coordinates of the object 
     * -to the x,y coordinates of their reprojection in the video images
     * RSB_3D and RSB_2D are the signal/noise ratio of the 3D object generated and of the
     * 2D points projected in the video images. These values can only be computed after the
     * scene generation and knowing the std dev that has corrupted the 3D and 2D data.  
     */
    class SceneCriterionComparison : public Scene
    {
    public:
        ARLCORE_API SceneCriterionComparison( PlaneSystem& , std::vector<double> parameters, std::vector<double> staticStatus,
         std::vector<double> noiseValues, double RSB_3D, double RSB_2D  );
        ARLCORE_API ~SceneCriterionComparison( void );
        ARLCORE_API const Point& getCentre( void ) const;
        
    private:
        Point m_Centre;
        Point m_ControlCentre;
    };

    /**
     * @brief : Scene for Solve AXXB UnitTests
     * 2 cameras with angle = 
     * Square of ...mm with 150 points at the distance of 1000 mm from cameras
     */
    class SceneAXXB : public Scene
    {
    public:
        ARLCORE_API SceneAXXB( PlaneSystem&, unsigned int NbCameras=2 );
        ARLCORE_API ~SceneAXXB( void );
        ARLCORE_API const Point& getCentre( void ) const;
        
    private:
        Point m_Centre;
    };  
} // namespace ArlCore
#endif // _ARLCORE_SCENES_H
