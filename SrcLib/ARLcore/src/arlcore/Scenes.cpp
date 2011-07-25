/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Scenes.h>

#include <arlcore/Camera.h>
#include <arlcore/Misc.h>
#include <arlcore/SmartPointsList.h>
#include <vnl/vnl_math.h>
/**
 * This scene is used for :
 * arlCore::testRegistration3D3D
 * arlCore::testProjectiveRegistration
 * arlCore::testHomographyRegistration
 * arlCore::testReconstructionPolyscopic
 * arlCore::testEpipolarMatching
 * arlCore::testReconstructionPolyscopicUncertainty
 *
 * It creates a scene with nbCamerasMax camera, specific intrinsic camera parameter defined below and nbPoints2DMax points
 * The number of points is randomly chosen so that it is comprised between 4 and nbPoints2DMax+4. This is necessary for most
 * of the unit test. For some test (arlCore::testReconstructionPolyscopicUncertainty), we choose to create only 1 point
 * to avoid correlation within the statistical tests. This can be done by setting nbPoints2DMax to 1.
 */
arlCore::SceneUnitTest::SceneUnitTest( arlCore::PlaneSystem &universe, double worldPointsSize, unsigned int nbPoints2DMax):
Scene(universe)
{
    const unsigned int NbCamerasMax = 10;
    const double size_rand_mire=300, size_cam=500, distance_mire_cam_min = 1000;//, WorldPointsSize=200;
    arlCore::Point ZERO_MONDE(0.0,0.0,0.0);

    std::vector<double> int_param, int_range;
    int_param.push_back(1000);//fx
    int_param.push_back(1000);//fy
    int_param.push_back(400);//cx
    int_param.push_back(200);//cy
    int_param.push_back(0.0);// int_param.push_back(1.0);//with distortion
    int_param.push_back(0.0);// int_param.push_back(1.0);//with distortion
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);

//  int_range.push_back(200.0);
//  int_range.push_back(200.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(100.0);
    int_range.push_back(50.0);
//  int_range.push_back(0.7);
//  int_range.push_back(1.0);
//  int_range.push_back(0.01);
//  int_range.push_back(0.01);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);

    arlCore::Point tirage_camera(1);
    tirage_camera.set(0, 0.0);
    tirage_camera.addUniformNoise(0,(double)NbCamerasMax);
    tirage_camera.set(0, (tirage_camera[0]+(double)NbCamerasMax )/2 );

    unsigned int nbCameras = (unsigned int)floor(tirage_camera[0]);
    if( nbCameras<2 ) nbCameras=2;
//  std::cerr<< "nbCameras = " << nbCameras << std::endl;

    // tirage du point de mire des cameras dans un cube de dimension size_rand_mire
    m_Centre.shapeRandom(arlCore::ARLCORE_SHAPE_CUBE, size_rand_mire);
//  m_Centre.shapeRandom(arlCore::ARLCORE_SHAPE_CUBE, 0.01);
//  std::cerr<< "point_mire = " << m_Centre.getString() << std::endl;
//  addCameras(2, m_Centre, 0.1, distance_mire_cam_min, int_param, int_range);
    addCameras(nbCameras, m_Centre, size_cam, distance_mire_cam_min, int_param, int_range);


    /////////////////////////////////////////////////////////////////////////////////////////////////
    // creation de points 3D autour du point mire
    // point_monde = point dans un volume pour tester le recalage 3D/3D et ISPPC OSPPC et EPPC
    // point_monde_plan = point dans le plan xOy pour tester le recalage homographique
    // FONCTION(point_mire, nombre de points max, parametre de tirage, points tires, points tires bruites, param bruit)
    arlCore::Point tirage_point(1);

    // il faut au moins 3 points pour le recalage 3D3D et 4 pour le calcul homographique
    tirage_point.addUniformNoise(0,nbPoints2DMax/2);

    unsigned int nbPoints = (unsigned int)floor(tirage_point[0] + nbPoints2DMax/2 + 4);
    if(nbPoints2DMax ==1) nbPoints = 1;
    addTag(nbPoints, arlCore::ARLCORE_SHAPE_SPHERE, m_Centre, worldPointsSize);
    addTag(nbPoints, arlCore::ARLCORE_SHAPE_PLAINSQUARE, ZERO_MONDE, worldPointsSize);
}

arlCore::SceneUnitTest::~SceneUnitTest()
{}

const arlCore::Point& arlCore::SceneUnitTest::getCentre( void ) const
{
    return m_Centre;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// French information that can be found in Scenes.h :
// Creation d'une scene avec une camera regardant nbPoses mires planes differentes en nb de points (m_ChessNbPoints[i])
// le point regarde par la camera n'est pas exactement l'origine : c'est un pt aleatoire dans un cube de 100x100x100 autour
// de l'origine.
// Le parametre k_range correspond a l'etendu de l'erreur que l'on souhaite sur les parametres de distortion de la
// camera creee. Ce parametre supplementaire permet de se servir de cette scene pour testIntrinsicCalibration
// et pour testInitIntrinsicCalibration
// Apres-coup j'ai rajoute un parametre supplementaire a la fonction: le nbre de camera MAX. Cela permet de se
// servir de cette scene a la fois pour le calibrage intrinseque d'une camera et le calibrage extrinseque de
// plusieurs cameras
// Attention le nb de pose MAX est un nbre entier tire dans [0, nbPoses_MAX] + 4
arlCore::SceneUnitTestInitIntrinsicCalib::SceneUnitTestInitIntrinsicCalib( arlCore::PlaneSystem &universe, std::vector<double> k_range, const unsigned int nbCameras_MAX, const unsigned int nbPoses_MAX):
Scene(universe)
{
    const double ChessSize = 120;
    const double cubicSize = 1000;
    const unsigned int nbParameters = 8;
    arlCore::Point Origin(0.0,0.0,0.0), zero_monde(0.0,0.0,0.0);

//  Origin.cubicRandom(100);
    Origin.shapeRandom(ARLCORE_SHAPE_CUBE, 100);
    m_nbPoses = (unsigned int)floor( arlRandom::Random::uniformDoubleRnd(0.0, nbPoses_MAX) + 4);//
    unsigned int nbCameras = (unsigned int)floor( arlRandom::Random::uniformDoubleRnd(0.0, nbCameras_MAX) + 1);
    if(nbCameras_MAX == 1) nbCameras = 1;

    arlCore::Point ChessNbPoints_tmp(m_nbPoses);
    unsigned int i,k;

    //ChessNbPoints_tmp.cubicRandom(ChessSize);//ChessNbPoints[i] contient le nb de points 3D de la ieme mire
    ChessNbPoints_tmp.shapeRandom(ARLCORE_SHAPE_CUBE, ChessSize);//ChessNbPoints[i] contient le nb de points 3D de la ieme mire
    for( i=0 ; i< m_nbPoses; ++i )
    {
        m_ChessNbPoints.push_back((unsigned int)(floor(ChessNbPoints_tmp[i] + ChessSize/2+20)));
        //std::cerr<<"ChessNbPoints ="<<m_ChessNbPoints[i] <<std::endl;
    }
    const double SphereRadius = 200, DistanceMin = 1000;
    std::vector<double> int_param, int_range;
    int_param.push_back(1000);
    int_param.push_back(1000);
    int_param.push_back(400);
    int_param.push_back(200);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);

    int_range.push_back(200.0);
    int_range.push_back(200.0);
    int_range.push_back(100.0);
    int_range.push_back(50.0);
    int_range.push_back(k_range[0]);
    int_range.push_back(k_range[1]);
    int_range.push_back(k_range[2]);
    int_range.push_back(k_range[3]);
    int_range.push_back(0.0);
    int_range.push_back(0.0);

    //creation d'une camera sans distortion dont on recherche les parametres pour tester initIntrinsicCalibration
    //Elle est orientee vers le point Origin(0,0,0)
    addCameras(nbCameras, Origin, SphereRadius, DistanceMin, int_param, int_range);
    //creation de m_nbPoses Tags qui contiennent ChessNbPoints[k] pts3D
    for( k=0 ; k<m_nbPoses ; ++k )
        addTag(m_ChessNbPoints[k], arlCore::ARLCORE_SHAPE_PLAINSQUARE, zero_monde, ChessSize);
}

arlCore::SceneUnitTestInitIntrinsicCalib::~SceneUnitTestInitIntrinsicCalib()
{}

const arlCore::Point& arlCore::SceneUnitTestInitIntrinsicCalib::getCentre( void ) const
{
    return m_Centre;
}
const unsigned int arlCore::SceneUnitTestInitIntrinsicCalib::getNbPoses(void) const
{
    return m_nbPoses;
}

const unsigned int arlCore::SceneUnitTestInitIntrinsicCalib::getChessNbPoints( unsigned int i) const
{
    return m_ChessNbPoints[i];
}

// In randomParam you should find in the following order:
// number of point
// number of camera
// angle between the camera
// size of the shape in which the 3D points are randomly chosen
// shape of the random 3D points
// number of control point
// distance between the control point center and the registration point
// size of the shape in which the control points are randomly chosen
// shape of the random control points
arlCore::SceneCriterionComparison::SceneCriterionComparison( arlCore::PlaneSystem &universe, std::vector<double> parameters,
 std::vector<double> staticStatus, std::vector<double> noiseValues, double RSB_3D, double RSB_2D ):
Scene(universe)
{
    bool calibration_endo = true;

    unsigned int nbCameras, nbPoints, nbControlPoints;
    double angleCameras, registrationPointsSize, controlPointsSize, distanceRegControlPoints;
    const double size_rand_point_de_mire_camera=300, size_cam=500, distance_mire_cam_min = 1000;
    arlCore::Point ZERO_MONDE(0.0,0.0,0.0);
    arlCore::ARLCORE_SHAPE regPointShape, controlPointShape;

    /////////////////////////////////////////////////////////////
    // Determination of the number of point /////////////////////
    if( parameters[0] < 3 && parameters[0] > 10000)
    {
        nbPoints = 4;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison nbPoints3DMax < 4 or >10000"<<std::endl;
    }
    else
        if(staticStatus[0] != 0) nbPoints = (unsigned int)parameters[0];
        else
        {   // il faut au moins 3 points pour le recalage 3D3D et 4 pour le calcul homographique
            nbPoints = (unsigned int)floor(arlRandom::Random::uniformDoubleRnd(4.0,  parameters[0]) );
        }
    std::cerr<<"Nombre de points du modele ="<<nbPoints<<std::endl;

    ///////////////////////////////////////////////////////////////
    //  Determination of the number of camera /////////////////////
    if( parameters[1] < 2 && parameters[1] > 100)
    {
        nbCameras = 2;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison nbCameras < 2 or >100"<<std::endl;
    }
    else
        if(staticStatus[1] != 0) nbCameras = (unsigned int)parameters[1];
        else
            nbCameras = (unsigned int)floor(arlRandom::Random::uniformDoubleRnd(2.0,  parameters[1]) );
    std::cerr<<"Nombre de cameras ="<<nbCameras<<std::endl;

    ////////////////////////////////////////////////////////////////////////
    //  Determination of the angle between the cameras /////////////////////
    if( parameters[2] < 0 && parameters[2] > 720)
    {
        angleCameras = 2*vnl_math::pi;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison angleCamera < 0 or >720"<<std::endl;
    }
    else
        if(staticStatus[2] != 0) angleCameras = parameters[2]*vnl_math::pi/180;
        else
            angleCameras = 4*vnl_math::pi;//(unsigned int)floor(arlRandom::Random::uniformDoubleRnd(10.0,  parameters[2]) );
    std::cerr<<"Angle entre les cameras ="<<angleCameras*180/vnl_math::pi<<std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Determination of the size of the shape in which the 3D points are randomly chosen /////////////////////
    if( parameters[3] < 0 && parameters[3] > 1500)
    {
        registrationPointsSize = 500;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison registrationPointsSize < 0 or >1500"<<std::endl;
    }
    else
        if(staticStatus[3] != 0) registrationPointsSize = parameters[3];
        else
            registrationPointsSize = (unsigned int)floor(arlRandom::Random::uniformDoubleRnd(20.0,  parameters[3]) );
    std::cerr<<"Dimension du volume de tirage des points 3D modele ="<<registrationPointsSize<<std::endl;

    /////////////////////////////////////////////////////////////////////
    //  Determination of the shape of the 3D points /////////////////////
    if( parameters[4] < 1 && parameters[4] > (double)ARLCORE_SHAPE_NBSHAPES)
    {
        regPointShape = arlCore::ARLCORE_SHAPE_SPHERE;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison regPointShape < 1 or >ARLCORE_SHAPE_NBSHAPES"<<std::endl;
    }
    else
        if(staticStatus[4] != 0) regPointShape = (arlCore::ARLCORE_SHAPE)(unsigned int) parameters[4];
        else
            regPointShape = (arlCore::ARLCORE_SHAPE)(unsigned int)floor(arlRandom::Random::uniformDoubleRnd(1.0,  parameters[4]) );
    std::cerr<<"Forme du volume de tirage des points 3D modele ="<<arlCore::ARLCORE_SHAPE_NAMES[(int)parameters[4]]<<std::endl;

    /////////////////////////////////////////////////////////////////////
    // Determination of the number of control point /////////////////////
    if( parameters[5] < 0 && parameters[5] > 50)
    {
        nbControlPoints = 10;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison nbControlPoints < 0 or >50"<<std::endl;
    }
    else
        if(staticStatus[5] != 0) nbControlPoints = (unsigned int)parameters[5];
        else
            nbControlPoints = (unsigned int)floor(arlRandom::Random::uniformDoubleRnd(4,  parameters[5]) );
    std::cerr<<"Nombre de points de controle ="<<nbControlPoints<<std::endl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Determination of the size of the shape in which the control points are randomly chosen /////////////////////
    if( parameters[6] < 0 && parameters[6] > 1500)
    {
        controlPointsSize = 500;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison controlPointsSize < 0 or >1500"<<std::endl;
    }
    else
        if(staticStatus[6] != 0) controlPointsSize = parameters[6];
        else
            controlPointsSize =  (double)floor(arlRandom::Random::uniformDoubleRnd(20,  parameters[6]) );
    std::cerr<<"Dimension du volume de tirage des points 3D controle ="<<controlPointsSize<<std::endl;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Determination of the distance between the registration and the control point /////////////////////
    if( parameters[7] < 0 && parameters[7] > 1500)
    {
        distanceRegControlPoints = 500;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison distanceRegControlPoints < 0 or >1500"<<std::endl;
    }
    else
        if(staticStatus[7] != 0) distanceRegControlPoints = parameters[7];
        else
            distanceRegControlPoints =  (double)floor(arlRandom::Random::uniformDoubleRnd(50,  parameters[7]) );
    std::cerr<<"Distance entre les points modele et les points controle ="<<distanceRegControlPoints<<std::endl;

    //////////////////////////////////////////////////////////////////////////
    //  Determination of the shape of the control points /////////////////////
    if( parameters[8] < 1 && parameters[8] > (double)ARLCORE_SHAPE_NBSHAPES)
    {
        controlPointShape = arlCore::ARLCORE_SHAPE_SPHERE;
        std::cerr<<"In SceneCriterionComparison::SceneCriterionComparison controlPointShape < 1 or >ARLCORE_SHAPE_NBSHAPES"<<std::endl;
    }
    else
        if(staticStatus[8] != 0) controlPointShape = (arlCore::ARLCORE_SHAPE)(unsigned int) parameters[8];
        else
            controlPointShape = (arlCore::ARLCORE_SHAPE)(unsigned int)floor(arlRandom::Random::uniformDoubleRnd(1.0,  parameters[8]) );
    std::cerr<<"Forme du volume de tirage des points 3D controle ="<<arlCore::ARLCORE_SHAPE_NAMES[(int)parameters[8]]<<std::endl;

    std::vector<double> int_param, int_range;
    int_param.push_back(1000);
    int_param.push_back(1000);
    int_param.push_back(400);
    int_param.push_back(200);
//  int_param.push_back(1.0);
//  int_param.push_back(1.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);

//  int_range.push_back(200.0);
//  int_range.push_back(200.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);//100
    int_range.push_back(0.0);//50
//  int_range.push_back(0.7);
//  int_range.push_back(1.0);
//  int_range.push_back(0.01);
//  int_range.push_back(0.01);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);

//  std::cerr<< "nbCameras = " << nbCameras << std::endl;

    // tirage du point de mire des cameras
    m_Centre.shapeRandom(arlCore::ARLCORE_SHAPE_CUBE, size_rand_point_de_mire_camera);
    m_ControlCentre = m_Centre;
    m_ControlCentre.shapeRandom(arlCore::ARLCORE_SHAPE_SPHERESURFACE, distanceRegControlPoints);

    if(calibration_endo == true)
    {
        m_Centre = ZERO_MONDE;
        m_ControlCentre = ZERO_MONDE;
    }
//  std::cerr<< "point_mire = " << m_Centre.getString() << std::endl;
    if(staticStatus[2] != 0)
        addCameras(nbCameras, m_Centre, size_cam, distance_mire_cam_min, int_param, int_range, angleCameras);
    else
        addCameras(nbCameras, m_Centre, size_cam, distance_mire_cam_min, int_param, int_range);

    if(calibration_endo == true)
    {
        unsigned int nbEndoscopePose = 10;
        double angleVisuEndocam = 200;
        double distance_chessbd_endocam_min = 60;
        double size_endocam = 50;
        int_param[0] = 250;
        int_param[1] = 250;
        int_param[2] = 300;
        int_param[3] = 150;
        addCameras(nbEndoscopePose, m_Centre, size_endocam, distance_chessbd_endocam_min, int_param, int_range, angleVisuEndocam);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // creation de points 3D autour du point mire
    // creation des points de controle
    addTag(nbPoints, regPointShape, m_Centre, registrationPointsSize);
    addTag(nbControlPoints, controlPointShape, m_ControlCentre, controlPointsSize);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // calcul des RSB 3D et 2D
    const arlCore::PointList &regPoints     = this->getTags().getTag(0)->getGeometry();
    std::vector< arlCore::PointList >reprojection2D(nbCameras);
    arlCore::SmartPointList  splMonde;
    unsigned int i;
    for( i=0 ; i<nbCameras ; ++i )
    {
        this->detection( i+1, 0, splMonde, 0.0 );
        splMonde.getPoints(reprojection2D[i], i+1, (void*) this->getTags().getTag(0));
    }
    for( i=0 ; i<5 ; ++i )
    std::cerr<<"Noise values ="<<noiseValues[i]<<std::endl;

    double c,d, std_3D=0, std_2D=0;
    arlCore::Point gravity_3D(3), gravity_2D(2), a,b;
    regPoints.properties(gravity_3D, a, b, c, d, std_3D);
    RSB_3D = 10*log10( (std_3D/noiseValues[0] +std_3D/noiseValues[1]+std_3D/noiseValues[2])/3 );
    std::cerr<<"RSB 3D = "<<10*log10(std_3D/noiseValues[0] +std_3D/noiseValues[1]+std_3D/noiseValues[2])<<std::endl;
    for( i=0 ; i<nbCameras ; ++i)
    {
        double std_2D_ind;
        reprojection2D[i].properties(gravity_2D, a, b, c, d, std_2D_ind);
        std_2D = sqrt( std_2D*std_2D + std_2D_ind*std_2D_ind );
        std::cerr<<"RSB 2D["<<i<<"] = "<<10*log10( (std_2D/noiseValues[3]+std_2D/noiseValues[4])/2/nbCameras )<<std::endl;
    }
    RSB_2D = 10*log10( (std_2D/noiseValues[3]+std_2D/noiseValues[4])/2/nbCameras );
}

const arlCore::Point& arlCore::SceneCriterionComparison::getCentre( void ) const
{
    return m_Centre;
}

arlCore::SceneCriterionComparison::~SceneCriterionComparison()
{}

// SceneAXXB
arlCore::SceneAXXB::SceneAXXB( arlCore::PlaneSystem &universe, unsigned int NbCameras ):
Scene(universe)
{
    // Chessboard
    const unsigned int NbPoints = 13; // Number of points by side of the square
    const double SquareEdgeSize = 200; // mm
    // Cameras
    const double size_rand_mire = 300; // mm
    const double size_cam = 500; // mm
    const double distance_mire_cam_min = 1000; // mm

    arlCore::Point ZERO_MONDE( 0.0, 0.0, 0.0 );

    std::vector<double> int_param, int_range;
    int_param.push_back(1000);//fx
    int_param.push_back(1000);//fy
    int_param.push_back(400);//cx
    int_param.push_back(200);//cy
    int_param.push_back(0.0);// int_param.push_back(1.0);//with distortion
    int_param.push_back(0.0);// int_param.push_back(1.0);//with distortion
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);
    int_param.push_back(0.0);

//  int_range.push_back(200.0);
//  int_range.push_back(200.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(100.0);
    int_range.push_back(50.0);
//  int_range.push_back(0.7);
//  int_range.push_back(1.0);
//  int_range.push_back(0.01);
//  int_range.push_back(0.01);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);
    int_range.push_back(0.0);

    // tirage du point de mire des cameras dans un cube de dimension size_rand_mire
    m_Centre.shapeRandom(arlCore::ARLCORE_SHAPE_CUBE, size_rand_mire);
    addCameras( NbCameras, m_Centre, size_cam, distance_mire_cam_min, int_param, int_range);

    arlCore::Tag* tag = addTag( NbPoints, arlCore::ARLCORE_SHAPE_CHESSBOARD, ZERO_MONDE, SquareEdgeSize);
    tag->getGeometry().save("c:/Chess.vtk",arlCore::ARLCORE_POINT_SAVE_VTK);
    //addTag( 150, arlCore::ARLCORE_SHAPE_PLAINSQUARE, ZERO_MONDE, SquareEdgeSize);
}

arlCore::SceneAXXB::~SceneAXXB()
{}

const arlCore::Point& arlCore::SceneAXXB::getCentre( void ) const
{
    return m_Centre;
}
