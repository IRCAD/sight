/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Reconst3D.h>

#include <vcl_vector.h>
#include <vnl/vnl_real_npolynomial.h>
#include <vnl/vnl_cross.h>
#include <vnl/algo/vnl_rnpoly_solve.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_conjugate_gradient.h>

#include <arlcore/Optimization.h>
#include <arlcore/PointsList.h>
#include <arlcore/Misc.h>
#include <arlcore/vnl_rigid_vector.h>
#include <arlcore/MatrixR.h>
#include <arlcore/UncertaintyPropag.h>

using namespace std;

bool reconstruction3D( const vector<arlCore::Point::csptr> &points2D, const vector<const arlCore::Camera*> &cameras, arlCore::Point &point3D, arlCore::ARLCORE_RECONSTRUCTION3D methode, vector<double> &log, const bool pixelFrame );

/**
 * @brief 3D reconstruction of a point that is seen by several cameras
 */
bool arlCore::reconst3D(const std::vector<arlCore::Point::csptr>&list2D, const std::vector<arlCore::Camera> &cameras, arlCore::Point &point3D,
            arlCore::ARLCORE_RECONSTRUCTION3D methode, unsigned int &plane, std::vector<double> &log, double errorMax, bool propagateCov, const bool pixelFrame)
{
    std::vector<const arlCore::Camera*> cams;
    unsigned int i;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return reconst3D( list2D, cams, point3D, methode, plane, log, errorMax, propagateCov, pixelFrame);
}

bool arlCore::reconst3D(const std::vector<arlCore::Point::csptr>&list2D, const std::vector<const arlCore::Camera*> &camsList, arlCore::Point &point3D,
            arlCore::ARLCORE_RECONSTRUCTION3D methode, unsigned int &plane, std::vector<double> &log, double errorMax, bool propagateCov, const bool pixelFrame)
{
    assert(point3D.size()==3);
    assert(camsList.size()>1);
    plane = camsList[0]->getPlane();
    arlCore::Point p3D(3);
    //FIXME : Adapt code with mobile cameras : Set calibration parameters in the same plane
    if(reconstruction3D(list2D, camsList, p3D, methode, log, pixelFrame))
        if(errorMax<0 || errorMax>=log[0])
        {
            point3D.set(p3D);
            point3D.setVisible(true);
            point3D.setColour(1,2,3);
            point3D.setError(p3D.getError());
            if(propagateCov) propagateCovarianceToReconst3D(list2D, camsList, point3D, methode, log, pixelFrame);
            else
            {
                arlCore::vnl_covariance_matrix &cov = point3D.getCovMatrix();
                cov = p3D.getCovMatrix();
            }
    /*      unsigned int i;
            std::cout<<"RECONST3D ERROR ("<<methode<<") : ";
            for( i=0 ; i<log.size() ; ++i )
                std::cout<<log[i];
            std::cout<<"\n";*/
            return true;
        }
    point3D.setError(-1.0);
    return false;
}

/**
 *  @brief : Triangulation of a 3D point from 2D points detected in 2 video images (and only 2)
 * which coordinates are written in the optical frames of the cameras. These coordinates
 * are obtained with the function arlCore::pixelPlaneToUnitFocalPlane
 * 
 * param[in] : focalPts2D1 = 3D coordinates in the optical unit frame associated to camera 1 
 * of the 2D point detected in cam 1
 * param[in] : focalPts2D1 = 3D coordinates in the optical unit frame associated to camera 2
 * of the 2D point detected in cam 2
 * param[in] : PassageMatrix 4x4 matrix between cam 1 and cam 2
 * param[in] : methode = trinagulation method choice (Horaud or Lines)
 * 
 */
bool simpleRecons3D( arlCore::Point &point3D, const vnl_vector_fixed<double,3>& focalPt2D1, const vnl_vector_fixed<double,3>& focalPt2D2,
                    vnl_matrix_fixed<double,4,4>& PassageMatrix, arlCore::ARLCORE_RECONSTRUCTION3D methode, double &error )
{
    switch(methode)
    {
        case arlCore::ARLCORE_R3D_HORAUD_APPROX :
        case arlCore::ARLCORE_R3D_HORAUD_PERFECT :
        {// METHODE ANALYTIQUE DE HORAUD (la plus simple et la plus rapide)
            const double &x  = focalPt2D1(0);
            const double &y  = focalPt2D1(1);
            const double &x_ = focalPt2D2(0);
            const double &bx = PassageMatrix(0,3);
            const double &bz = PassageMatrix(2,3);
            long double alpha = x*PassageMatrix(0,0) + y*PassageMatrix(0,1) + PassageMatrix(0,2);
            long double beta  = x*PassageMatrix(2,0) + y*PassageMatrix(2,1) + PassageMatrix(2,2);
            assert(beta*x_-alpha!=0);
            if(beta*x_-alpha==0) return false;
            long double z = (bx - bz*x_) /(beta*x_ -alpha);
            //FIXME : Instabilit� num�rique quand (beta*x_-alpha) tend vers 0
            //Probl�me constat� � partir de 0.1
            error=fabs(beta*x_-alpha)/z; // FIXME error grand quand le denominateur tend vers 0
            point3D.x(x*z);
            point3D.y(y*z);
            point3D.z(z);
            point3D.setError(error);
            return true;
        }
        case arlCore::ARLCORE_R3D_TWO_LINES_PERFECT :
        case arlCore::ARLCORE_R3D_TWO_LINES_APPROX :
        {// METHODE ANALYTIQUE DU MILIEU DES DEUX DROITES (tres rapide)
            vnl_vector_fixed<double,3> I, J, v, n1, n2, m1C1, m2C2, C1C2;
            vnl_vector_fixed<double,4> var, C2, m2;
            vnl_matrix_fixed<double,4,4> P;
            double num, denom, landa1, landa2;
            unsigned int i;
            P =  vnl_matrix_inverse<double>(PassageMatrix.as_matrix() );
            var.put(0, 0.0); var.put(1, 0.0); var.put(2, 0.0); var.put(3, 1.0);
            C2 = P.as_matrix() * var.as_vector();
            // C2 est exprimee dans le repere liée à la caméra 1
            var.put(0, focalPt2D2(0));
            var.put(1, focalPt2D2(1));
            var.put(2, focalPt2D2(2));
            var.put(3, 1.0);
            m2 = P.as_matrix() *var.as_vector();
            // m2 est exprimée dans le repère liee à la caméra 1
            m1C1 = (-1.0)*focalPt2D1;
            var  = C2 - m2;
            m2C2.put(0, var(0)); m2C2.put(1, var(1)); m2C2.put(2, var(2));
            C1C2.put(0, C2(0)); C1C2.put(1, C2(1)); C1C2.put(2, C2(2));
            // Determination du premier point I sur la droite de reprojection
            v = vnl_cross_3d(m1C1.as_vector() , m2C2.as_vector() );
            n1= vnl_cross_3d(v.as_vector(), m1C1.as_vector() );
            double dot1=0, dot2=0;
            for(i=0;i<3;++i)
            {
                dot1 +=  C1C2(i) * n1(i);
                dot2 +=  m2C2(i) * n1(i);
            }
            num   = -1*dot1;
            denom = -1*dot2;
            assert(denom!=0);
            if(denom==0) return false;
            landa1 = num/denom;
            I.put(0, C2(0) - landa1* m2C2(0) );  
            I.put(1, C2(1) - landa1* m2C2(1) );
            I.put(2, C2(2) - landa1* m2C2(2) );
            // Determination du deuxieme point J sur la droite de reprojection
            n2 = vnl_cross_3d(v, m2C2);
            dot1 = dot2 =0;
            for(i=0;i<3;++i)
            {
                dot1 +=  C1C2(i) * n2(i);
                dot2 +=  m1C1(i) * n2(i);
            }
            num   = dot1;
            denom = -1*dot2;
            assert(denom!=0);
            landa2 = num/denom;
            J.put(0, -1* landa2* m1C1(0) );  
            J.put(1, -1* landa2* m1C1(1) );
            J.put(2, -1* landa2* m1C1(2) );
            // On prend le barycentre de I et J comme estimation de la reconstruction
            point3D.x(0.5*I(0) + 0.5*J(0));
            point3D.y(0.5*I(1) + 0.5*J(1));
            point3D.z(0.5*I(2) + 0.5*J(2));
            // Calcul des distances entre les droites par deux méthodes différentes
            // Cette valeur permet de savoir si les deux droites correspondent bien a un point a reconstruire
//          v.normalize();
//          error = 0;
//          for( i=0 ; i<3 ; ++i )
//              error +=  v(i) * C1C2(i);
            error = sqrt (fabs (vnl_vector_ssd(I,J) ));
            point3D.setError(error);
            return true;
        }
    }
    return false;
}

bool reconsReproj(const vector<arlCore::Point::csptr> &points2D, const vector<const arlCore::Camera*> &cameras, arlCore::Point &point3D, arlCore::ARLCORE_RECONSTRUCTION3D methode, vector<double> &log);
bool reconstruction3D( const vector<arlCore::Point::csptr> &points2D, const vector<const arlCore::Camera*> &cameras,
        arlCore::Point &point3D, arlCore::ARLCORE_RECONSTRUCTION3D methode, vector<double> &log, const bool pixelFrame )
{
    log.clear();
    if( points2D.size() != cameras.size() ) return false;
    if( cameras.size() < 2 ) return false;
//  for(i=0; i< points2D.size(); ++i)
//    if(points2D[i]->size() != 2)
//      cerr <<"Pb : tous les points video n'ont pas la bonne dimension i.e. 2" << endl;
//  if(point3D.size() != 3)
//    cerr <<"Pb : le point resultat reconstruit n'a pas la bonne dimension i.e. 3" << endl;
    std::vector< vnl_vector_fixed<double,3> >focalPts2D;
    unsigned int i;
    double error;
    switch(methode)
    {
    case arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION :
    case arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY :
        assert(pixelFrame); // FIXME
        return reconsReproj(points2D, cameras, point3D, methode, log);
    case arlCore::ARLCORE_R3D_HORAUD_APPROX :
    case arlCore::ARLCORE_R3D_TWO_LINES_APPROX :
    case arlCore::ARLCORE_R3D_HORAUD_PERFECT :
    case arlCore::ARLCORE_R3D_TWO_LINES_PERFECT :
    {
        bool perfect = (methode==arlCore::ARLCORE_R3D_HORAUD_PERFECT || methode==arlCore::ARLCORE_R3D_TWO_LINES_PERFECT);
        focalPts2D.resize(2);
        for( i=0 ; i<focalPts2D.size() ; ++i )
            if(pixelFrame)
            {
                assert(points2D[i]->size()==2);
                cameras[i]->pixelPlaneToUnitFocalPlane( *(points2D[i]), focalPts2D[i], perfect );
            }
            else
            {
                focalPts2D[i].put(0,points2D[i]->x());
                focalPts2D[i].put(1,points2D[i]->y());
                focalPts2D[i].put(2,1.0);
            }
        vnl_matrix_fixed<double,4,4> PM1_2 = cameras[1]->getExtrinsic() * cameras[0]->getInvExtrinsic();
        simpleRecons3D ( point3D, focalPts2D[0], focalPts2D[1], PM1_2, methode, error);
        cameras[0]->getInvExtrinsic().trf(point3D);
        log.push_back(error); // 1/denom pour Horaud ou la distance entre les 2 droites
        return true;
    }
    case arlCore::ARLCORE_R3D_MULTI_LINES_APPROX :
    case arlCore::ARLCORE_R3D_MULTI_LINES_PERFECT :
    {
        std::vector< vgl_line_3d_2_points <double> >lines;
        vgl_line_3d_2_points <double>line;
        focalPts2D.resize(points2D.size());
        for( i=0 ; i<focalPts2D.size() ; ++i )
        {
            if(pixelFrame)
            {
                assert(points2D[i]->size()==2);
                cameras[i]->pixelPlaneToUnitFocalPlane( *(points2D[i]), focalPts2D[i], methode==arlCore::ARLCORE_R3D_MULTI_LINES_PERFECT );
            }
            else
            {
                focalPts2D[i].put(0,points2D[i]->x());
                focalPts2D[i].put(1,points2D[i]->y());
                focalPts2D[i].put(2,1.0);
            }
            if(cameras[i]->projectiveLine(focalPts2D[i], line))
                lines.push_back(line);
        } // Initialisation
        vnl_matrix_fixed<double,4,4> PM1_2 = cameras[1]->getExtrinsic() * cameras[0]->getInvExtrinsic();
        if(!simpleRecons3D ( point3D, focalPts2D[0], focalPts2D[1], PM1_2, arlCore::ARLCORE_R3D_TWO_LINES_APPROX, error)) return false;;
        cameras[0]->getInvExtrinsic().trf(point3D);
        vnl_vector<double> point3DSolution = point3D.getCoordinates();
        arlCore::OptimiseLineIntersection reconstruction(lines);
        vnl_powell compute_reconstruction(&reconstruction);
        compute_reconstruction.minimize(point3DSolution);
        for( i=0; i< 3 ; ++i )
            point3D.set(i,point3DSolution(i));
        log.push_back(compute_reconstruction.get_end_error() / points2D.size()); // Distance moyenne aux droites
        point3D.setError(log.back());
        log.push_back(compute_reconstruction.get_start_error() / points2D.size());
    return true;
    }
    }
    return false;
}

/**
 * @brief 
 * Rigid registration of a 3D point set so that its reprojection on one image fits a 2D point set
 * 
 * This function compute the rigid transformation that registers a 3D rigid object (defined 
 * in its own frame)  in the world frame of one calibrated camera (we call here
 * world frame the frame in which the camera were calibrated). In fact, this function corresponds
 * to arlCore::projectivePointsRegistration_3D_2D used with only one camera, but we add 
 * an initialization procedure before launching it (arlCore::planarHomographyRegistration_3D_2D).
 * TODO This function works if the 3D model is planar (because of the initialization) !
 * TODO We have to retrieve this constraint 
 */
bool arlCore::monoViewPointRegistration3D2D( const arlCore::Camera &cam , const std::vector<arlCore::Point::csptr > &points2D,
        const PointList &model3D, arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION method,
        const std::vector<double> &optimiserParameters , std::vector<double> &log, bool verbose )
{
    return monoViewPointRegistration3D2D( cam, points2D, model3D.getList(), T, method, optimiserParameters , log, verbose );
}

bool arlCore::monoViewPointRegistration3D2D( const arlCore::Camera &cam , const std::vector<arlCore::Point::csptr > &points2D,
        const std::vector< arlCore::Point* > &model3D, arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION method,
        const std::vector<double> &optimiserParameters , std::vector<double> &log, bool verbose )
{
    const bool Refinement = method!=arlCore::ARLCORE_PR_UNKNOWN;
    assert(model3D.size()>=points2D.size());
//  plane=cam.getPlane(); // FIXME Parametre redondant ?
    unsigned int i;
    std::vector<arlCore::Point::csptr> p2D;
    std::vector<arlCore::Point*> p3D;
    for( i=0 ; i<points2D.size() ; ++i )
        if(points2D[i])
            if(points2D[i]->isVisible())
            {
                p2D.push_back(points2D[i]);
                p3D.push_back(model3D[i]);
            }
//  std::vector<double> optimiserParameters, log;
    // TODO : Injecter une tolerance dans multiViewPointRegistration3D2D et sortir si elle n'est pas respectée : tol
//  tol=0; // FIXME
    // FIXME Tester la planeite du modele. Si inf a une certaine tolerance return false;
    if(arlCore::planarHomographyRegistration_3D_2D(cam, p2D, p3D, T, optimiserParameters, log, verbose))
    {
        if(Refinement)
        {
            vector<const arlCore::Camera*>cams;
            cams.push_back(&cam);
            std::vector< std::vector<arlCore::Point::csptr> > p_2D;
            p_2D.push_back(p2D);
            arlCore::multiViewPointRegistration3D2D(cams, p_2D, p3D, T, method, optimiserParameters, log, verbose);
        }
        return true;
    }
    return false;
}

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
 * param[in] : cameras = camera vector used for the registration (it works also with one camera only)
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
 * ISPPC corrsponds to the minimization of the reprojection criterion in the camera : sum i ||P(T*Mi) - mi ||^2
 * This function can be launched for on eor several cameras. Obviously, an initialization is better than nothing
 * but it generally works correctly without one (then it is identity).
 * ISPPC and OSPPC assumes an isotropic noise for 2D detection in all images
 * ISPPC_ANISOTROP and EPPC use the point covariance matrix in the criterion formula 
 * 
 * The detailed formula of each criterion is given in Optimization.h
 * 
 */
bool  arlCore::multiViewPointRegistration3D2D( const vector<arlCore::Camera> &cameras, const vector< vector< arlCore::Point::csptr > > &points2D, const PointList &model3D, arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION methode, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    unsigned int i;
    std::vector<const arlCore::Camera*> cams;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return multiViewPointRegistration3D2D( cams, points2D, model3D, T, methode, optimiserParameters, log, verbose);
}

bool  arlCore::multiViewPointRegistration3D2D( const vector<const arlCore::Camera*> &cameras, const vector< vector< arlCore::Point::csptr > > &points2D, const PointList &model3D, arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION methode, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    return multiViewPointRegistration3D2D( cameras, points2D, model3D.getList(), T, methode, optimiserParameters, log, verbose);
}

bool  arlCore::multiViewPointRegistration3D2D( const vector<arlCore::Camera> &cameras, const vector< vector< arlCore::Point::csptr > > &points2D, const std::vector< arlCore::Point* > &model3D, arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION methode, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    unsigned int i;
    std::vector<const arlCore::Camera*> cams;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return multiViewPointRegistration3D2D( cams, points2D, model3D, T, methode, optimiserParameters, log, verbose);
}

bool  arlCore::multiViewPointRegistration3D2D( const vector<const arlCore::Camera*> &cameras, const vector< vector< arlCore::Point::csptr > > &points2D, const std::vector< arlCore::Point* > &model3D, arlCore::vnl_rigid_matrix &T, arlCore::ARLCORE_PROJECTIVE_REGISTRATION methode, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    //if(cameras.size()<2) return false;
    const unsigned int Nb2DPointsMin = 3;
    const double EPPCfTolerance = 1e-6;
    const double EPPCxTolerance = 1e-6;
    const unsigned int Model3DSize = (unsigned int)model3D.size();
    bool b = false;
    log.clear();
    unsigned int i,j;
    double RMS = -1.0;
    vnl_vector< double > init(6);
    double nbVisiblePoints = 0.0;
    for( i=0 ; i<cameras.size() ; ++i )
    {
        assert(cameras[i]);
        if(!cameras[i]->isIntrinsicCalibrated()) return b;
//      assert(Model3DSize==points2D[i].size());
        if(Model3DSize!=points2D[i].size()) return b;
        for( j=0 ; j<points2D[i].size() ; ++j )
            if( points2D[i][j]!=0 )
                if(points2D[i][j]->isVisible())
                    ++nbVisiblePoints;
    }
    if(nbVisiblePoints<Nb2DPointsMin) return b;
    if(methode==arlCore::ARLCORE_PR_EPPC)
    {
        const double RMSTrsf = 1e-4;
        const unsigned int nbIterationsMax = 10;
        vnl_vector< double > init_pts3D(3*Model3DSize), init_pts3D_precedent(3*Model3DSize), init_precedent(6);
        init = arlCore::vnl_rigid_vector (T);
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i ) // TODO : Factoriser dans toute la fonction (attention OSPPC)
        {
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
            for( j=0 ; j<3 ; ++j )
                init_pts3D[3*i+j] = (*model3D[i])[j];
        }
        arlCore::ISPPC_cost_function ISPPC(cameras,estime3D,points2D, true);
        vnl_powell computeISPPC(&ISPPC);
        computeISPPC.minimize(init);
        arlCore::EPPC_cost_function EPPC(cameras,model3D,points2D,T);
        vnl_powell computeEPPC(&EPPC);
        computeEPPC.set_f_tolerance(EPPCfTolerance);
        computeEPPC.set_x_tolerance(EPPCxTolerance);
        computeISPPC.set_f_tolerance(EPPCfTolerance);
        computeISPPC.set_x_tolerance(EPPCxTolerance);
        unsigned int nbIterations=0;
        log.resize(5);
        do
        {           
            T=arlCore::vnl_rigid_matrix (arlCore::vnl_rigid_vector(init)); 
//          std::cerr<<"ITERATION POW EPPC : "<<nbIterations<<std::endl;
//          std::cerr<<"transfo ="<<std::endl<<T<<std::endl;
//          std::cerr<<"erreur debut = "<<computeISPPC.get_start_error()<<std::endl;
//          std::cerr<<"erreur fin = "<<computeISPPC.get_end_error()<<std::endl;
            //Estimation des points 3D parfaits avec T(k+1)
            computeEPPC.minimize(init_pts3D);
            if(nbIterations==0) log[1]=sqrt(computeEPPC.get_start_error()/nbVisiblePoints);
            init_precedent = init;
            for(i=0; i<Model3DSize; ++i)
                for( j=0 ; j<3 ; ++j )
                    estime3D[i].put(j, init_pts3D[3*i+j]);
            computeISPPC.minimize(init);
            ++nbIterations;
        }while( (init-init_precedent).rms()>RMSTrsf && nbIterations<nbIterationsMax );
        RMS = sqrt(computeEPPC.get_end_error()/nbVisiblePoints);
        log[0] = RMS;
        // FIXME Ponderer les erreurs suivantes
        log[2] = EPPC.get2Derror();
        log[3] = EPPC.get3Derror();
        log[4] = RMSTrsf;
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_EPPC_LM)
    {
        const double RMSTrsf = 1e-4;
        const unsigned int nbIterationsMax = 10;
        vnl_vector< double > init_pts3D(3*Model3DSize), init_pts3D_precedent(3*Model3DSize), init_precedent(6);
        init = arlCore::vnl_rigid_vector (T);
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i )
        {
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
            for( j=0 ; j<3 ; ++j )
                init_pts3D[3*i+j] = (*model3D[i])[j];
        }
        arlCore::ISPPC_LS_cost_function ISPPC(cameras,estime3D,points2D, 6, nbVisiblePoints, vnl_least_squares_function::use_gradient, true);
        vnl_levenberg_marquardt computeISPPC(ISPPC);
        computeISPPC.minimize_using_gradient(init);
        arlCore::EPPC_LS_cost_function EPPC(cameras,model3D,points2D,T, 3*Model3DSize, nbVisiblePoints+Model3DSize, vnl_least_squares_function::use_gradient);
        vnl_levenberg_marquardt computeEPPC(EPPC);
        computeEPPC.set_f_tolerance(EPPCfTolerance);
        computeEPPC.set_x_tolerance(EPPCxTolerance);
        unsigned int nbIterations=0;
        log.resize(5);
        do
        {
            //std::cerr<<"ITERATION LM EPPC : "<<nbIterations<<std::endl;           
            T=arlCore::vnl_rigid_matrix (arlCore::vnl_rigid_vector(init)); 
            //Estimation des points 3D parfaits avec T(k+1)
            computeEPPC.minimize_using_gradient(init_pts3D);
            if(nbIterations==0) log[1]=sqrt(computeEPPC.get_start_error()/nbVisiblePoints);
            init_precedent = init;
            for(i=0; i<Model3DSize; ++i)
                for( j=0 ; j<3 ; ++j )
                    estime3D[i].put(j, init_pts3D[3*i+j]);
            computeISPPC.minimize_using_gradient(init);
            ++nbIterations;
        }while( (init-init_precedent).rms()>RMSTrsf && nbIterations<nbIterationsMax );
        RMS = computeEPPC.get_end_error();
        log[0] = RMS;
        // FIXME Ponderer les erreurs suivantes
        log[2] = EPPC.get2Derror();
        log[3] = EPPC.get3Derror();
        log[4] = RMSTrsf;
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_EPPC_CG)
    {
        const double RMSTrsf = 1e-4;
        const unsigned int nbIterationsMax = 5;
        vnl_vector< double > init_pts3D(3*Model3DSize), init_pts3D_precedent(3*Model3DSize), init_precedent(6);
        init = arlCore::vnl_rigid_vector (T);
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i )
        {
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
            for( j=0 ; j<3 ; ++j )
                init_pts3D[3*i+j] = (*model3D[i])[j];
        }
        arlCore::ISPPC_cost_function ISPPC(cameras,estime3D,points2D);
        vnl_conjugate_gradient computeISPPC(ISPPC);
        computeISPPC.minimize(init);
        arlCore::EPPC_cost_function EPPC(cameras,model3D,points2D,T);
        vnl_conjugate_gradient computeEPPC(EPPC);
        computeEPPC.set_f_tolerance(EPPCfTolerance);
        computeEPPC.set_x_tolerance(EPPCxTolerance);
        unsigned int nbIterations=0;
        do
        {           
            T=arlCore::vnl_rigid_matrix (arlCore::vnl_rigid_vector(init)); 
            //Estimation des points 3D parfaits avec T(k+1)
            computeEPPC.minimize(init_pts3D);
            init_precedent = init;
            for( i=0 ; i<Model3DSize ; ++i )
                for( j=0 ; j<3 ; ++j )
                    estime3D[i].put(j, init_pts3D[3*i+j]);
            computeISPPC.minimize(init);
            ++nbIterations;
        }while( (init-init_precedent).rms()>RMSTrsf && nbIterations<nbIterationsMax );
        RMS = computeEPPC.get_end_error();
        log.push_back(RMS);
        log.push_back(sqrt(computeEPPC.get_start_error()/nbVisiblePoints));
        // FIXME Ponderer les erreurs suivantes
        log.push_back(EPPC.get2Derror());
        log.push_back(EPPC.get3Derror());
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_ISPPC)
    {
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i )
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
        arlCore::vnl_rigid_vector vec(T);
        arlCore::ISPPC_cost_function ISPPC(cameras,estime3D,points2D, false);
        init = vec;
        vnl_powell computeISPPC(&ISPPC);
        computeISPPC.minimize(init);
        RMS = sqrt(computeISPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeISPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_ISPPC_LM)
    {
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        unsigned int nb_visible_point=0;
        for( i=0 ; i<Model3DSize ; ++i )
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
        arlCore::vnl_rigid_vector vec(T);
        arlCore::ISPPC_LS_cost_function ISPPC(cameras,estime3D,points2D, 6, nbVisiblePoints, vnl_least_squares_function::use_gradient, false);
        init = vec;
        vnl_levenberg_marquardt computeISPPC(ISPPC);
        computeISPPC.minimize_using_gradient(init);
        RMS = sqrt(computeISPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeISPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_ISPPC_ANISOTROP)
    {
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i )
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
        arlCore::vnl_rigid_vector vec(T);
        arlCore::ISPPC_cost_function ISPPC(cameras,estime3D,points2D, true);
        init = vec;
        vnl_powell computeISPPC(&ISPPC);
        computeISPPC.minimize(init);
        RMS = sqrt(computeISPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeISPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_ISPPC_ANISOTROP_LM)
    {
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i )
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
        arlCore::vnl_rigid_vector vec(T);
        arlCore::ISPPC_LS_cost_function ISPPC(cameras,estime3D,points2D, 6, nbVisiblePoints, vnl_least_squares_function::use_gradient, true);
        init = vec;
        vnl_levenberg_marquardt computeISPPC(ISPPC);
        computeISPPC.minimize_using_gradient(init);
        RMS = sqrt(computeISPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeISPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }
    if(methode==arlCore::ARLCORE_PR_ISPPC_CG)
    {
        std::vector< vnl_vector_fixed<double,4> > estime3D(Model3DSize);
        for( i=0 ; i<Model3DSize ; ++i )
            estime3D[i] = vnl_vector_fixed<double,4>((*model3D[i])[0], (*model3D[i])[1], (*model3D[i])[2], 1.0);
        arlCore::vnl_rigid_vector vec(T);
        arlCore::ISPPC_cost_function ISPPC(cameras,estime3D,points2D);
        init = vec;
        vnl_conjugate_gradient computeISPPC(ISPPC);
        computeISPPC.minimize(init);
        RMS = sqrt(computeISPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeISPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }
    
    if(methode==arlCore::ARLCORE_PR_OSPPC)
    {
        arlCore::vnl_rigid_vector vec(T);
        arlCore::OSPPC_cost_function OSPPC(cameras,model3D,points2D);
        init = vec;
        vnl_powell computeOSPPC(&OSPPC);
        computeOSPPC.minimize(init);
        RMS = sqrt(computeOSPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeOSPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }   
    if(methode==arlCore::ARLCORE_PR_OSPPC_LM)
    {
        arlCore::vnl_rigid_vector vec(T);
        arlCore::OSPPC_LS_cost_function OSPPC(cameras,model3D,points2D, 6, nbVisiblePoints, vnl_least_squares_function::use_gradient);
        init = vec;
        vnl_levenberg_marquardt computeOSPPC(OSPPC);
        computeOSPPC.minimize_using_gradient(init);
        RMS = sqrt(computeOSPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeOSPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }   
    if(methode==arlCore::ARLCORE_PR_OSPPC_CG)
    {
        arlCore::vnl_rigid_vector vec(T);
        arlCore::OSPPC_cost_function OSPPC(cameras,model3D,points2D);
        init = vec;
        vnl_conjugate_gradient computeOSPPC(OSPPC);
        computeOSPPC.minimize(init);
        RMS = sqrt(computeOSPPC.get_end_error()/nbVisiblePoints);
        log.push_back(RMS);
        log.push_back(sqrt(computeOSPPC.get_start_error()/nbVisiblePoints));
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        b= true;
    }
    T.setRMS(RMS);
    T.setStdDev(0.0);
    return b;
}

/**
 * @brief   HOMOGRAPHY WHEN THE INTRINSIC PARAMETERS ARE UNKNOWN
 * *****************************************************************************************
 * This function is necessary for camera calibration (see Zhang. Z. A flexible new technique for camera
 * calibration). The notations have been kept from his paper. The idea is that the point of the 
 * chessboard model are projectively equivalent to the points detected in the video image.
 * Then it exist an homography 3x3 H so that (assuming points on the chessboard have a Z coordinate 
 * equal to 0) :
 * 
 *       [u_i]       [X_i] 
 * m_i = [v_i] = H x [Y_i] 
 *       [w_i]       [ 1 ] 
 * 
 *
 * From a general point of view, this function provides "the best" homography between 
 * two point sets in 2D.
 * Considering the camera calibration problem, the H matrix corresponds to the multiplication of
 * the intrinsic matrix and the extrinsic matrix:
 * [u]   [ -  h1  - ]  [X]        [ | |  |] [X]                                                 [fc1 0   cc1]
 * [v] = [ -  h2  - ]x [Y] =A x   [r1 r2 t]x[Y]  where A is the intrinsic parameter matrix =    [0   fc2 cc2]
 * [w]   [ -  h3  - ]  [1]        [ | |  |] [1]                                                 [0   0    1 ]
 * 
 * where [u v w]' are PIXEL coordinates and not REAL. To find A and the extrinsic matrix, several poses
 * of the chessboard are necessary.
 * 
 * To find H, we seek the solution that minimize the following system:
 * [X1 Y1 1 0 0 0 -u1X1 -u1Y1 -u1 ]   [ | ]
 * [.                          .  ]   [ h1 ]
 * [Xi Yi 1 0 0 0 -uiXi -uiYi -ui ]   [ | ]
 * [.                          .  ]   [ | ]
 * [XN YN 1 0 0 0 -uNXN -uNYN -uN ]  x[ h2 ]  = 0
 * [0 0 0 X1 Y1 1 -v1X1 -v1Y1 -v1 ]   [ | ]
 * [.                          .  ]   [ | ]
 * [0 0 0 Xi Yi 1 -viXi -viYi -vi ]   [ h3 ]
 * [.                          .  ]   [ | ]
 * [0 0 0 XN YN 1 -vNXN -vNYN -vN ]   [ | ]
 *                                                                            [ - h1 -]
 * where the h vector corresponds to the homography matrix that fulfils m_i = [ - h2 -]M_i
 *                                                                            [ - h3 -]
 * M are the coordinates of the pattern points [X Y 1.0] in the pattern frame (note the pattern MUST be planar)
 * m are the coordinates of its correspoding 3D points in the video image
 * Since the homography matrix is defined up to a scale factor, the solution is given by a svd
 * of the big matrix. Then we obtain the eigen vector associated to the smallest eigen value.
 * 
 * To obtain an evaluation of the homography estimation quality, put optimiserParameters[0] to 1.
 * In that case, log[0] contains the reprojection error.
 * To apply the prenormalisation (that ensure a good computation of the svd) detailed in R. Hartely.
 * "In defence of the 8 points algorithm", put optimiserParameters[1] to 1. In practice, this did not
 *  change our results...
 * ******************************************************************************************/
template <typename Type>
bool internPlanarHomographyUnknownIntrinsic( const Type &points2D, const Type &model3D, vnl_matrix_fixed<double,3,3> &H, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{   
    bool normalize =false;
    bool error_log=false;   
    //ATTENTION : Points2D and  model3D doivent �tre parfaitement appareill�s :
    // Tailles identiques, pas de pointeur null
    if(points2D.size()<4) return false;
    assert(points2D.size()==model3D.size());
    unsigned int i, size=(unsigned int)model3D.size();
    vnl_matrix<double> P(3,3);
    vnl_matrix_fixed<double,4,4> T_tmp;
    vnl_matrix<double> a_matrix(size*2, 9);
    if(optimiserParameters.size() > 0)
        if(optimiserParameters[0] == 1)
            error_log = true;
    if(optimiserParameters.size() > 1)
        if(optimiserParameters[1] == 1)
            normalize = true;   
    // PRENORMALISATION DES DONNEES cf "In defence of the 8 points algorithm" et "Pose reconstruction with 
    // uncalibrated CT imaging device"
    double meanX_model3D=0, meanY_model3D=0, scale_model3D=0;
    double meanX_points2D=0, meanY_points2D=0, scale_points2D=0;
    vnl_matrix_fixed<double,3,3> norm_mat_model3D, norm_mat_points2D;
    if(normalize)
    {
        for( i=0 ; i<size ; ++i )// calcul des moyennes et des ecart-types
        {
            meanX_model3D += model3D[i]->x();
            meanY_model3D += model3D[i]->y();
            meanX_points2D += points2D[i]->x();
            meanY_points2D += points2D[i]->y();
        }
        meanX_model3D /= size; meanY_model3D /= size;  meanX_points2D/=size ;  meanY_points2D/=size ;
        for( i=0 ; i<size ; ++i )
        {
            scale_model3D += sqrt( (model3D[i]->x() - meanX_model3D)*(model3D[i]->x() - meanX_model3D) + (model3D[i]->y() - meanY_model3D)*(model3D[i]->y() - meanY_model3D));
            scale_points2D += sqrt( (points2D[i]->x() - meanX_points2D)*(points2D[i]->x() - meanX_points2D) + (points2D[i]->y() - meanY_points2D)*(points2D[i]->y() - meanY_points2D));
        }
        scale_model3D /= (size * 1.414213562373);//1.41421356237310 = sqrt(2)
        scale_points2D /= (size * 1.414213562373);
        norm_mat_model3D.set_identity();   norm_mat_points2D.set_identity();
        norm_mat_model3D(0,0) = norm_mat_model3D(1,1) = 1/scale_model3D;
        norm_mat_points2D(0,0) = norm_mat_points2D(1,1) = 1/scale_points2D;
        norm_mat_model3D(0,2) = -meanX_model3D/scale_model3D;
        norm_mat_model3D(1,2) = -meanY_model3D/scale_model3D;
        norm_mat_points2D(0,2) = -meanX_points2D/scale_points2D;
        norm_mat_points2D(1,2) = -meanY_points2D/scale_points2D;
    }           
    for( i=0 ; i<size ; ++i )
    {
        if(normalize)
        {
            a_matrix.put(i,0, (model3D[i]->x() - meanX_model3D)/scale_model3D);
            a_matrix.put(i,1, (model3D[i]->y() - meanY_model3D)/scale_model3D);
        }
        else
        {
            a_matrix.put(i,0, model3D[i]->x());
            a_matrix.put(i,1, model3D[i]->y());
        }
        a_matrix.put(i,2, 1.0);
        a_matrix.put(i,3, 0.0);
        a_matrix.put(i,4, 0.0);
        a_matrix.put(i,5, 0.0);
        if(normalize)
        {
            double scaled_pt2D = (points2D[i]->x() - meanX_points2D)/scale_points2D;
            a_matrix.put(i,6, (model3D[i]->x() - meanX_model3D)/scale_model3D*(-1)* scaled_pt2D);
            a_matrix.put(i,7, (model3D[i]->y() - meanY_model3D)/scale_model3D*(-1)* scaled_pt2D);
            a_matrix.put(i,8, (-1)*scaled_pt2D);
        }
        else
        {
            a_matrix.put(i,6, model3D[i]->x()*(-1)* points2D[i]->x());
            a_matrix.put(i,7, model3D[i]->y()*(-1)* points2D[i]->x());
            a_matrix.put(i,8, (-1)*points2D[i]->x());
        }
    }
    for( i=size ; i<2*size ; ++i )
    {
        a_matrix.put(i,0, 0.0);
        a_matrix.put(i,1, 0.0);
        a_matrix.put(i,2, 0.0);
        if(normalize)
        {
            a_matrix.put(i,3, (model3D[i-size]->x() - meanX_model3D)/scale_model3D);
            a_matrix.put(i,4, (model3D[i-size]->y() - meanY_model3D)/scale_model3D);
        }
        else
        {
            a_matrix.put(i,3, model3D[i-size]->x());
            a_matrix.put(i,4, model3D[i-size]->y());
        }
        a_matrix.put(i,5, 1.0);
        if(normalize)
        {
            double scaled_pt2D = (points2D[i-size]->y() - meanY_points2D)/scale_points2D;
            a_matrix.put(i,6, (model3D[i-size]->x() - meanX_model3D)/scale_model3D*(-1)*scaled_pt2D);
            a_matrix.put(i,7, (model3D[i-size]->y() - meanY_model3D)/scale_model3D*(-1)*scaled_pt2D);
            a_matrix.put(i,8, (-1)*scaled_pt2D);
        }
        else
        {
            a_matrix.put(i,6, model3D[i-size]->x()*(-1)*points2D[i-size]->y());
            a_matrix.put(i,7, model3D[i-size]->y()*(-1)*points2D[i-size]->y());
            a_matrix.put(i,8, (-1)*points2D[i-size]->y());
        }
    }
//  TODO tester si c plus rapide de passer par vnl_symmetric_eigensystem plutot que vnl_svd
//  vnl_symmetric_eigensystem<double> objet(a_matrix.transpose()*a_matrix); 
//  cerr << "null vector = "<<objet.nullvector() << endl;
//  vnl_matrix<double> PP(3,3);
//  PP.set(objet.nullvector().data_block());
//  PP/=PP.get_column(0).two_norm();
//  cerr << "Matrice PP estimee " << endl << PP << endl;
//  
    //a_matrix.normalize_rows();
    vnl_svd<double> svd(a_matrix);
    //cerr << "svd(a_matrix) U =" <<endl << svd.U() <<endl;cerr << "svd(a_matrix) V =" <<endl << svd.V() <<endl;cerr << "svd(a_matrix) W =" <<endl << svd.W() <<endl;
    //cerr << "Vecteur propre associe a valeur singuliere minimale " << endl << svd.nullvector() << endl;
    P.set(svd.nullvector().data_block());
//  cerr << "Matrice P estimee " << endl << P << endl;  
    if(normalize){
        vnl_matrix<double> tmp = vnl_matrix_inverse<double>(norm_mat_points2D);
        P = tmp * P * norm_mat_model3D; 
    }
    P/=P.get_column(0).two_norm();//TODO cette normalisation est-elle necessaire ?
    // on teste si l'homographie estimee recale l'objet derriere la camera ou pas
    // si c'est le cas alors on multiplie P par -1;
    //cerr << "Matrice P estimee apres normalisation" << endl << P << endl; 
    vnl_vector_fixed<double,3> var_3D, test;
    var_3D(0) = model3D[0]->x() ;
    var_3D(1) = model3D[0]->y();
    var_3D(2) = 1.0;
    test = P * var_3D;
    if(test[2] < 0.0) P*=-1.0;
    //cerr << "Matrice P estimee " << endl << P << endl;
    //cerr << "norme colonne 1=" <<P.get_column(0).two_norm() << endl;
    //cerr << "norme colonne 2=" <<P.get_column(1).two_norm() << endl;
//  if(fabs(P.get_column(1).two_norm() - 1.0) > 1e-2)//TODO evaluer la bonne valeur d'erreur (empirique pour l'instant)
//  {
//      T.set_identity();
//      return false;
//  }
    H = P;
    if(error_log)
    {
        double erreur_reprojection = 0;
        log.clear();
        for( i=0 ; i<size ; ++i )
        {
            arlCore::Point tmp2D(2);
            vnl_vector_fixed<double,3> model3D_tmp, tmp3D;

            model3D_tmp.put(0,model3D[i]->x());model3D_tmp.put(1,model3D[i]->y());model3D_tmp.put(2,1.0);
            //std::cerr<<"model3D_tmp =" <<model3D_tmp<<std::endl;
            tmp3D = H * model3D_tmp.as_vector();
            tmp2D.set(0, tmp3D(0)/tmp3D(2));
            tmp2D.set(1, tmp3D(1)/tmp3D(2));
            //std::cerr<<"tmp2D =" <<tmp2D.getString()<<std::endl;
            //std::cerr<<"points2D[i] =" <<points2D[i]->getString()<<std::endl;
            erreur_reprojection += points2D[i]->distance(tmp2D);
        }
        log.push_back(erreur_reprojection/size);
        //log.push_back(P.get_column(1).two_norm());
    }
    return true;
}

bool arlCore::planarHomographyUnknownIntrinsic( const vector<arlCore::Point*> &points2D, const vector< arlCore::Point* > &model3D, vnl_matrix_fixed<double,3,3> &H, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    return internPlanarHomographyUnknownIntrinsic( points2D, model3D, H, optimiserParameters, log, verbose);
}

bool arlCore::planarHomographyUnknownIntrinsic( const arlCore::PointList &points2D, const arlCore::PointList &model3D, vnl_matrix_fixed<double,3,3> &H, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    return internPlanarHomographyUnknownIntrinsic( points2D, model3D, H, optimiserParameters, log, verbose);
}

/**
 * 
 * @brief PLANAR HOMOGRAPHY REGISTRATION METHOD
 *******************************************************************************************
 * We are looking for the rigid matrix R,t so that :
 * [u]   [ [     ][  ] ] [ [     ][ ] ] [X]  [ | |  |] [X]
 * [v] = [ [  Rw ][tw] ]x[ [  R  ][t] ] [Y] =[r1 r2 t]x[Y]
 * [w]   [ [     ][  ] ] [ [     ][ ] ] [0]  [ | |  |] [1]
 *       [ [0 0 0][1 ] ] [ [0 0 0][1] ] [1]
 * 
 * where [u v w]' are the REAL coordinates (in the unit focal plane) - and not pixellic
 *                                                                  [ - h1 -]
 * we write the matrix [r1 r2 t] under the homographic matrix    H =[ - h2 -] with 8 degrees of freedom
 *                                                                  [ - h3 -]
 * then we seek the solution that minimize the systeme :
 * [X1 Y1 1 0 0 0 -u1X1 -u1Y1 -u1 ]   [ | ]
 * [.                          .  ]   [ h1 ]
 * [Xi Yi 1 0 0 0 -uiXi -uiYi -ui ]   [ | ]
 * [.                          .  ]   [ | ]
 * [XN YN 1 0 0 0 -uNXN -uNYN -uN ]  x[ h2 ]  = 0
 * [0 0 0 X1 Y1 1 -v1X1 -v1Y1 -v1 ]   [ | ]
 * [.                          .  ]   [ | ]
 * [0 0 0 Xi Yi 1 -viXi -viYi -vi ]   [ h3 ]
 * [.                          .  ]   [ | ]
 * [0 0 0 XN YN 1 -vNXN -vNYN -vN ]   [ | ]
 *                                                                            [ - h1 -]
 * where the h vector corresponds to the homographic matrix that fulfills m = [ - h2 -]M
 *                                                                            [ - h3 -]
 * and M are the coordinates of the points of the PLANAR pattern [X Y 1.0] in the pattern frame
 * and m are the coordinates of its match in the video image in the REAL WORLD (in the unit focal plane)
 * Since the homographic matrix is defined up to a scale factor , the solution is given by the svd 
 * of the big matrix. Then, we get the eigen vector associated to the smallest eigen value.
 * Once H is computed, we multiply this matrix so that the first column is of unit norm.
 * we compute r1 x r2 to find the third vector of the pseudo rotation matrix
 * then we seek the closest rotation matrix of this matrix using an svd.
 * Finally, we multiply left the found matrix with the inverse extrinsic matrix of the camera 
 * To obtain a quality evaluation of the homography estimation, optimiserParameters[0]
 * must be set to 1. In that case, log[0] contains the reprojection error and log[1]
 * the norm of the first column of H (which should be equal to 1 !!).
 * 
 * To apply the prenormalisation (which ensure a good calculation of the svd), proposed in 
 * "In defence of the 8 points algorithm" (R. Hartley), optimiserParameters[1] must be set to 1.
 * 
 */
template <typename Type>
bool internPlanarHomographyRegistration_3D_2D( const arlCore::Camera &camera, const vector<arlCore::Point::csptr> &points2D, const Type &model3D, arlCore::vnl_rigid_matrix &T, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    //return true;
    if(!camera.isIntrinsicCalibrated()) return false;
    //ATTENTION : Points2D and  model3D doivent être parfaitement appareillés :
    // Tailles identiques, pas de pointeur null
    bool normalize =true;
    bool error_log=false;
    if(points2D.size()<4) return false;
    assert(points2D.size()==model3D.size());
    unsigned int i, size=(unsigned int)model3D.size();
    std::vector<vnl_vector_fixed<double,3> > focalPoints(size);
    vnl_matrix_fixed<double,3,3> P, mat_tmp;
    arlCore::vnl_rigid_matrix T_tmp;
    vnl_matrix<double> a_matrix(size*2, 9);
    if(optimiserParameters.size() > 0)
        if(optimiserParameters[0] == 1)
            error_log = true;
    if(optimiserParameters.size() > 1)
        if(optimiserParameters[1] == 1)
            normalize = true;
    // PRENORMALISATION DES DONNEES cf "In defence of the 8 points algorithm" et "Pose reconstruction with 
    // uncalibrated CT imaging device"
    double meanX_model3D=0, meanY_model3D=0, scale_model3D=0;
    double meanX_points2D=0, meanY_points2D=0, scale_points2D=0;
    vnl_matrix_fixed<double,3,3> norm_mat_model3D, norm_mat_points2D;
    if(normalize)
    {
        for( i=0 ; i<size ; ++i )// calcul des moyennes et des ecart-types
        {
            assert(model3D[i]);
            assert(points2D[i]);
            meanX_model3D += model3D[i]->x();
            meanY_model3D += model3D[i]->y();
            meanX_points2D += points2D[i]->x();
            meanY_points2D += points2D[i]->y();
        }
        meanX_model3D /= size; meanY_model3D /= size;  meanX_points2D/=size ;  meanY_points2D/=size ;
        for( i=0 ; i<size ; ++i )
        {
            scale_model3D += sqrt( (model3D[i]->x() - meanX_model3D)*(model3D[i]->x() - meanX_model3D) + (model3D[i]->y() - meanY_model3D)*(model3D[i]->y() - meanY_model3D));
            scale_points2D += sqrt( (points2D[i]->x() - meanX_points2D)*(points2D[i]->x() - meanX_points2D) + (points2D[i]->y() - meanY_points2D)*(points2D[i]->y() - meanY_points2D));
        }
        scale_model3D /= (size * 1.414213562373);//1.41421356237310 = sqrt(2)
        scale_points2D /= (size * 1.414213562373);
        norm_mat_model3D.set_identity();   norm_mat_points2D.set_identity();
        norm_mat_model3D(0,0) = norm_mat_model3D(1,1) = 1/scale_model3D;
        norm_mat_points2D(0,0) = norm_mat_points2D(1,1) = 1/scale_points2D;
        norm_mat_model3D(0,2) = -meanX_model3D/scale_model3D;
        norm_mat_model3D(1,2) = -meanY_model3D/scale_model3D;
        norm_mat_points2D(0,2) = -meanX_points2D/scale_points2D;
        norm_mat_points2D(1,2) = -meanY_points2D/scale_points2D;
    }
    for( i=0 ; i<size ; ++i )
    {
        assert(points2D[i]);
        assert(model3D[i]);
        camera.pixelPlaneToUnitFocalPlane( *(points2D[i]), focalPoints[i], false );
        if(normalize)
        {
            a_matrix.put(i,0, (model3D[i]->x() - meanX_model3D)/scale_model3D);
            a_matrix.put(i,1, (model3D[i]->y() - meanY_model3D)/scale_model3D);
        }
        else
        {
            a_matrix.put(i,0, model3D[i]->x());
            a_matrix.put(i,1, model3D[i]->y());
        }
        a_matrix.put(i,2, 1.0);
        a_matrix.put(i,3, 0.0);
        a_matrix.put(i,4, 0.0);
        a_matrix.put(i,5, 0.0);
        if(normalize)
        {
            double scaled_pt2D = (focalPoints[i](0) - meanX_points2D)/scale_points2D;
            a_matrix.put(i,6, (model3D[i]->x() - meanX_model3D)/scale_model3D*(-1)* scaled_pt2D);
            a_matrix.put(i,7, (model3D[i]->y() - meanY_model3D)/scale_model3D*(-1)* scaled_pt2D);
            a_matrix.put(i,8, (-1)*scaled_pt2D);
        }
        else
        {
            a_matrix.put(i,6, model3D[i]->x()*(-1)*focalPoints[i](0));
            a_matrix.put(i,7, model3D[i]->y()*(-1)*focalPoints[i](0));
            a_matrix.put(i,8, (-1)*focalPoints[i](0));
        }
    }
    for( i=size ; i<2*size ; ++i )
    {
        assert(model3D[i-size]);
        a_matrix.put(i,0, 0.0);
        a_matrix.put(i,1, 0.0);
        a_matrix.put(i,2, 0.0);
        if(normalize)
        {
            a_matrix.put(i,3, (model3D[i-size]->x() - meanX_model3D)/scale_model3D);
            a_matrix.put(i,4, (model3D[i-size]->y() - meanY_model3D)/scale_model3D);
        }
        else
        {
            a_matrix.put(i,3, model3D[i-size]->x());
            a_matrix.put(i,4, model3D[i-size]->y());
        }
        a_matrix.put(i,5, 1.0);
        if(normalize)
        {
            double scaled_pt2D = (focalPoints[i-size](1) - meanY_points2D)/scale_points2D;
            a_matrix.put(i,6, (model3D[i-size]->x() - meanX_model3D)/scale_model3D *(-1)* scaled_pt2D);
            a_matrix.put(i,7, (model3D[i-size]->y() - meanY_model3D)/scale_model3D *(-1)* scaled_pt2D);
            a_matrix.put(i,8, (-1)*scaled_pt2D);
        }
        else
        {
            a_matrix.put(i,6, model3D[i-size]->x()*(-1)*focalPoints[i-size](1));
            a_matrix.put(i,7, model3D[i-size]->y()*(-1)*focalPoints[i-size](1));
            a_matrix.put(i,8, (-1)*focalPoints[i-size](1));
        }
    }
//  TODO tester si c plus rapide de passer par vnl_symmetric_eigensystem plutot que vnl_svd
//  vnl_symmetric_eigensystem<double> objet(a_matrix.transpose()*a_matrix); 
//  cerr << "null vector = "<<objet.nullvector() << endl;
//  cerr << "tAA = "<<a_matrix.transpose()*a_matrix << endl;
//  vnl_matrix<double> PP(3,3);
//  PP.set(objet.nullvector().data_block());
//  PP/=PP.get_column(0).two_norm();
//  cerr << "Matrice PP estimee " << endl << PP << endl;
//  
    //a_matrix.normalize_rows();
    vnl_svd<double> svd(a_matrix);
    //cerr << "svd(a_matrix) U =" <<endl << svd.U() <<endl;cerr << "svd(a_matrix) V =" <<endl << svd.V() <<endl;cerr << "svd(a_matrix) W =" <<endl << svd.W() <<endl;
    //cerr << "Vecteur propre associe a valeur singuliere minimale " << endl << svd.nullvector() << endl;
    P.set(svd.nullvector().data_block());
    if(normalize){
        vnl_matrix<double> tmp = vnl_matrix_inverse<double>(norm_mat_points2D);
        P = tmp * P.as_matrix() * norm_mat_model3D;
    }   
    P/=P.get_column(0).two_norm();
    // on teste si l'homographie estimee recale l'objet derriere la camera ou pas
    // si c'est le cas alors on multiplie P par -1;
    vnl_vector_fixed<double,3> var_3D, test;
    var_3D(0) = model3D[0]->x() ;
    var_3D(1) = model3D[0]->y();
    var_3D(2) = 1.0;
    test = P * var_3D;
    if(test[2] < 0.0) P*=-1.0;
    //cerr << "Matrice P estimee " << endl << P << endl;
    //cerr << "norme colonne 1=" <<P.get_column(0).two_norm() << endl;
    //cerr << "norme colonne 2=" <<P.get_column(1).two_norm() << endl;
//  if(fabs(P.get_column(1).two_norm() - 1.0) > 1e-2)//TODO evaluer la bonne valeur d'erreur (empirique pour l'instant)
//  {
//      T.set_identity();
//      return false;
//  }
    mat_tmp.set_column(0, P.get_column(0));
    mat_tmp.set_column(1, P.get_column(1));
    mat_tmp.set_column(2, vnl_cross_3d(P.get_column(0), P.get_column(1)));
    arlCore::vnl_rotation3d_matrix rot_mat(mat_tmp);
    rot_mat.closest_rotation();
    T_tmp.set_identity();
    T_tmp.setRotation(rot_mat);
    for( i=0 ; i<3 ; ++i )
        T_tmp.put(i,3, P(i,2));
    T = camera.getInvExtrinsic() * T_tmp.as_matrix();
    if(error_log)
    {
        double erreur_reprojection = 0;
        log.clear();
        for( i=0 ; i<size ; ++i )
        {
            arlCore::Point tmp3D(3), tmp2D(2);
            T.trf(*(model3D[i]), tmp3D );
            camera.project3DPoint( tmp3D, tmp2D );
            erreur_reprojection += points2D[i]->distance(tmp2D);
        }
        log.push_back(erreur_reprojection/size);
        log.push_back(P.get_column(1).two_norm());
    }
    return true;
}

bool arlCore::planarHomographyRegistration_3D_2D( const arlCore::Camera &camera, const vector<arlCore::Point::csptr> &points2D, const std::vector< arlCore::Point* > &model3D, arlCore::vnl_rigid_matrix &T, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    return internPlanarHomographyRegistration_3D_2D( camera, points2D, model3D, T, optimiserParameters, log, verbose);
}
/*
bool arlCore::planarHomographyRegistration_3D_2D( const arlCore::Camera &camera, const vector<arlCore::Point*> &points2D, const PointList &model3D, arlCore::vnl_rigid_matrix &T, const vector<double> &optimiserParameters, vector<double> &log, bool verbose)
{
    return internPlanarHomographyRegistration_3D_2D( camera, points2D, model3D, T, optimiserParameters, log, verbose);
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////
// en entree :
// points2D : liste de arlCore::Point correspond aux coordonnees image (pts pixel) dans toutes les
// cameras du point qu'on veut reconstruire
// point3D : resultat de la reconstruction
// methode : choix de la methode parmi (reprojection_optimisation ou reprojection_optimisation_uncertainty
// log :qques valeurs importantes pour la qualite de la reconstruction
//
// reprojection_optimisation : methode iterative de reconstruction sans prise en compte de la
// covariance sur les points 2D. Initialisation avec two_lines_approx
// reprojection_optimisation_uncertainty : methode iterative de reconstruction avec prise en compte de la
// covariance sur les points 2D. Initialisation avec two_lines_approx
// En principe, methode la plus lente
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool reconsReproj(const vector<arlCore::Point::csptr> &points2D, const vector<const arlCore::Camera*> &cameras, arlCore::Point &point3D, arlCore::ARLCORE_RECONSTRUCTION3D methode, vector<double> &log)
{
    unsigned int i;
    bool b=false;
    vector<double> log_tmp;
    reconstruction3D(points2D, cameras, point3D, arlCore::ARLCORE_R3D_TWO_LINES_APPROX, log_tmp, true);
    vnl_vector<double> point3DSolution = point3D.getCoordinates();
    // Methode iterative de reprojection optimale avec correction de la distorsion
    if( methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION )
    {
        arlCore::OptimiseReprojection reconstruction(cameras, points2D);
        vnl_powell compute_reconstruction(&reconstruction);
        compute_reconstruction.minimize(point3DSolution);
        log.push_back(compute_reconstruction.get_end_error() / points2D.size());
        log.push_back(compute_reconstruction.get_start_error() / points2D.size() );
        b=true;
    }
    // Methode iterative de reprojection optimale avec correction de la distorsion
    // et prise en compte de la covariance
    if( methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY )
    {
        arlCore::OptimiseReprojectionUncertainty reconstruction(cameras, points2D);
        vnl_powell compute_reconstruction(&reconstruction);
        compute_reconstruction.minimize(point3DSolution);
        log.push_back(compute_reconstruction.get_end_error() / points2D.size() );
        log.push_back(compute_reconstruction.get_start_error() / points2D.size() );
        b=true;
    }
    for( i=0 ; i<3 ; ++i )
        point3D.set(i,point3DSolution(i));
    if(log.size()>0) point3D.setError(log[0]);
    return b;
}
