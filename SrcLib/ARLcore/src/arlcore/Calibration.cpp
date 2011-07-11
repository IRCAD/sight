/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Calibration.h>

#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/vnl_cross.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <arlcore/Reconst3D.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Optimization.h>
#include <arlcore/vnl_rigid_vector.h>

/** 
 * @brief This method of intrinsic parameter initilisation is taken from the paper of Z. Zhang:
 * A Flexible New Technique for Camera Calibration in IEEE Transactions on Pattern Analysis
 * and Machine Intelligence Volume 22 ,  Issue 11  (November 2000) pages: 1330 - 1334
 *
 * The notations of the paper have been keeped
 */   
bool arlCore::initIntrinsicCalibration(const std::vector<arlCore::PointList> &model3DList, const std::vector <arlCore::PointList> &points2DList, arlCore::Camera &camera , std::vector< vnl_rigid_matrix> &liste_extrinsic, const std::vector<double> &optimiserParameters, std::vector<double> &log)
{
    assert(model3DList.size()>0 && points2DList.size()>0);
    assert(model3DList.size()==1 || model3DList.size()==points2DList.size());
    const bool Verbose = false;
    std::vector< vnl_matrix_fixed<double,3,3> > liste_H(points2DList.size());
    const unsigned int NbPoses=(unsigned int)points2DList.size();
    unsigned int i, j;
    std::vector< double> optimiserParameters_tmp;// to get the reprojection error of the homography
    optimiserParameters_tmp.push_back(1.0);
    log.push_back(model3DList.size());//log[0] contains the number of pose
    for( i=0 ; i<NbPoses ; ++i )
    {
        if(model3DList.size()==1) j=0; else j=i;
        std::vector<double> log_tmp;
        planarHomographyUnknownIntrinsic( points2DList[i], model3DList[j], liste_H[i], optimiserParameters_tmp, log_tmp, true);
        if(Verbose)
        {
            std::cerr<<"H["<<i<<"]=" << liste_H[i]/liste_H[i](2,2)<<std::endl;
            std::cerr<<"Homography reprojection error="<< log_tmp[0] <<std::endl;
        }
        log.push_back(log_tmp[0]/model3DList[i].size()); //log reprojection error after homography computation
    }   
    ///////////////////////// v_12 , v_11 et v_22 are created /////////////////////////
    std::vector< vnl_vector_fixed<double, 6> > v_12(NbPoses), v_11(NbPoses), v_22(NbPoses);
    for( i=0 ; i<NbPoses ; ++i )
    {// Vector are filled like in the Zhang's paper 
        v_11[i](0) = liste_H[i](0,0)*liste_H[i](0,0);
        v_11[i](1) = liste_H[i](0,0)*liste_H[i](1,0) + liste_H[i](1,0)*liste_H[i](0,0);
        v_11[i](2) = liste_H[i](1,0)*liste_H[i](1,0);
        v_11[i](3) = liste_H[i](2,0)*liste_H[i](0,0) + liste_H[i](0,0)*liste_H[i](2,0);
        v_11[i](4) = liste_H[i](2,0)*liste_H[i](1,0) + liste_H[i](1,0)*liste_H[i](2,0);
        v_11[i](5) = liste_H[i](2,0)*liste_H[i](2,0);
        
        v_12[i](0) = liste_H[i](0,0)*liste_H[i](0,1);
        v_12[i](1) = liste_H[i](0,0)*liste_H[i](1,1) + liste_H[i](1,0)*liste_H[i](0,1);
        v_12[i](2) = liste_H[i](1,0)*liste_H[i](1,1);
        v_12[i](3) = liste_H[i](2,0)*liste_H[i](0,1) + liste_H[i](0,0)*liste_H[i](2,1);
        v_12[i](4) = liste_H[i](2,0)*liste_H[i](1,1) + liste_H[i](1,0)*liste_H[i](2,1);
        v_12[i](5) = liste_H[i](2,0)*liste_H[i](2,1);
        
        v_22[i](0) = liste_H[i](0,1)*liste_H[i](0,1);
        v_22[i](1) = liste_H[i](0,1)*liste_H[i](1,1) + liste_H[i](1,1)*liste_H[i](0,1);
        v_22[i](2) = liste_H[i](1,1)*liste_H[i](1,1);
        v_22[i](3) = liste_H[i](2,1)*liste_H[i](0,1) + liste_H[i](0,1)*liste_H[i](2,1);
        v_22[i](4) = liste_H[i](2,1)*liste_H[i](1,1) + liste_H[i](1,1)*liste_H[i](2,1);
        v_22[i](5) = liste_H[i](2,1)*liste_H[i](2,1);
    }
    vnl_matrix<double> V(2*NbPoses, 6);
    for( i=0 ; i<NbPoses ; ++i )
    {
        V.set_row(2*i,v_12[i].as_vector() );
        V.set_row(2*i+1, (v_11[i] - v_22[i]).as_vector() );
    }   
    vnl_svd<double> svd(V);
    vnl_vector_fixed<double, 6> b = svd.nullvector();
    //Extraction des parametres intrinseques tjs selon le papier zhang b=[B11 B12 B22 B13 B23 B33]
    //                                                                    b(0)b(1)b(2)b(3)b(4)b(5)
    const double NearZero = 0.0; //FIXME
    assert(fabs(b(0)*b(2)-b(1)*b(1))>NearZero);
    if(fabs(b(0)*b(2)-b(1)*b(1))<=NearZero) return false;
    const double cy = (b(1)*b(3) - b(0)*b(4))/ ( b(0)*b(2)-b(1)*b(1) );
    assert(fabs(b(0))>NearZero);
    if((fabs(b(0))<=NearZero)) return false;
    const double landa = b(5)-(b(3)*b(3)+cy*( b(1)*b(3)-b(0)*b(4)) )/b(0);
    assert(landa/b(0)>=0.0);
    if(landa/b(0)<0.0) return false;
    const double fx = sqrt(landa/b(0));
    assert(landa*b(0)/( b(0)*b(2)-b(1)*b(1))>=0.0);
    if(landa*b(0)/( b(0)*b(2)-b(1)*b(1))<0.0) return false;
    const double fy = sqrt(landa*b(0)/( b(0)*b(2)-b(1)*b(1)));
    assert(fabs(landa)>NearZero);
    if(fabs(landa)<=NearZero) return false;
    const double skew = -b(1)*fx*fx*fy/landa;
    assert(fabs(fx)>NearZero);
    if(fabs(fx)<=NearZero) return false;
    const double cx = skew*cy/fx - b(3)*fx*fx/landa;
    if(Verbose)
    {
        std::cerr << "solution intrinseque !"<<std::endl;
        std::cerr << fx << "  " << skew << "  "<< cx <<std::endl;
        std::cerr << " 0 " << fy << "  " << cy <<std::endl;
        std::cerr << " 0   0  1 " <<std::endl;
    }
    camera.setfx(fx);
    camera.setfy(fy);
    camera.setcx(cx);
    camera.setcy(cy);
    if(liste_extrinsic.size()<NbPoses) liste_extrinsic.resize(NbPoses);
    for( i=0; i<NbPoses; ++i )
    {// Extraction des matrices extrinseques
        vnl_matrix<double> camInv = vnl_matrix_inverse<double>(camera.getIntrinsicMatrix());        
        double new_landa = 1 /(camInv*liste_H[i].get_column(0)).two_norm() ;
        vnl_vector<double> r1 = new_landa * ( camInv*liste_H[i].get_column(0) );
        vnl_vector<double> r2 = new_landa * ( camInv*liste_H[i].get_column(1) );
        vnl_vector<double> r3 = vnl_cross_3d(r1, r2);
        vnl_vector<double> t  = new_landa * ( camInv*liste_H[i].get_column(2) );
        vnl_matrix_fixed<double,3,3> rot;
        rot.set_column(0, r1); rot.set_column(1, r2); rot.set_column(2, r3);
        if(Verbose) std::cerr<<" vnl_matrix_fixed rot ="<< std::endl <<rot<<std::endl;
        vnl_rotation3d_matrix true_rot(rot);
        true_rot.closest_rotation(); 
        if(Verbose) std::cerr<<" vnl_rotation3d_matrix rot ="<< std::endl <<true_rot<<std::endl;
        liste_extrinsic[i].setRotation(true_rot);
        liste_extrinsic[i].setTranslation(t(0), t(1), t(2) );
    }
    return true;
}

bool arlCore::refineIntrinsicCalibration(const std::vector<arlCore::PointList> &model3DList, const std::vector <arlCore::PointList> &points2DList, unsigned int nbParameters, arlCore::Camera &camera, std::vector< vnl_rigid_matrix> &liste_extrinsic, const std::vector<double> &optimiserParameters, std::vector<double> &log)
{
    assert(liste_extrinsic.size()==(unsigned int)points2DList.size());
    if(liste_extrinsic.size()!=(unsigned int)points2DList.size()) return false;
    bool Verbose_1 = false, Verbose_2 = false; // default
    if(optimiserParameters.size()>1) 
        if(optimiserParameters[1] == 1)
            Verbose_1 = true;
        else if(optimiserParameters[1] == 2)
        {
            Verbose_1 = true;
            Verbose_2 = true;
        }
    if(Verbose_1) std::cerr<<"debut refineIntrinsicCalibration "<<std::endl;
    double Method = 0; // Default
    if(optimiserParameters.size()>0) Method = optimiserParameters[0];
    
    assert(nbParameters>0 && nbParameters<=8);
    const unsigned int NbPoses=(unsigned int)points2DList.size();
    assert(NbPoses>0);
    assert(model3DList.size()==1 || model3DList.size()==points2DList.size());
    unsigned int nbPointsTotal=0, i, j; 
    vnl_vector<double> init (nbParameters+6*(NbPoses));
    init[0] = camera.getfx();
    init[1] = camera.getfy();
    init[2] = camera.getcx();
    init[3] = camera.getcy();
    init[4] = 0.0;
    init[5] = 0.0;
    init[6] = 0.0;
    init[7] = 0.0;  
    for( i=0 ; i<NbPoses ; ++i )
    {// On considere que le modèle peut avoir un nb de points différents sur chaque pose
        if(model3DList.size()==1) j=0; else j=i;
        assert(points2DList[i].size()==model3DList[j].size());
        nbPointsTotal += model3DList[j].size();
        vnl_rigid_vector tmp(liste_extrinsic[i]);
        for( j=0 ; j<6 ; ++j )
            init[nbParameters + i*6 + j]=tmp[j];
    }
    if(Method == 0) // OPTIMIZATION WITH LEVENBERG-MARQUARDT
    {
        const unsigned int Unknowns = NbPoses*6+nbParameters;
        const unsigned int Residuals = nbPointsTotal;
        if(Residuals<Unknowns) return false;
        arlCore::IntrinsicLS_cost_function LS_reprojection_f(Unknowns, Residuals, vnl_least_squares_function::use_gradient);
        LS_reprojection_f.setVerbose(Verbose_2);
        for( i=0 ; i<NbPoses ; ++i )
        {
            if(model3DList.size()==1) j=0; else j=i;
            LS_reprojection_f.addPattern( points2DList[i].getList(), model3DList[j].getList() );
        }
        vnl_levenberg_marquardt refineIntrinsic(LS_reprojection_f);
        //LS_reprojection_f.setObserver(true);
        refineIntrinsic.set_trace(true);
        //refineIntrinsic.set_check_derivatives(1);
        if(optimiserParameters.size()>2) refineIntrinsic.set_f_tolerance(optimiserParameters[2]);
        else refineIntrinsic.set_f_tolerance(1e-6); // Erreur de reprojection stable (sqrt(m_error)) à 1e-3 près
        if(optimiserParameters.size()>3)
            if(optimiserParameters[3] == 0){}//1e-8 default
            else {refineIntrinsic.set_x_tolerance(optimiserParameters[3]);} 
        if(optimiserParameters.size()>4) 
            if(optimiserParameters[4] == 0){}//1e-5 default
            else {refineIntrinsic.set_g_tolerance(optimiserParameters[4]);}
        if(Verbose_1)
        {
            std::cerr<<"f tolerance ="<<refineIntrinsic.get_f_tolerance()<<std::endl;
            std::cerr<<"x tolerance ="<<refineIntrinsic.get_x_tolerance()<<std::endl;
            std::cerr<<"g tolerance ="<<refineIntrinsic.get_g_tolerance()<<std::endl;
            std::cerr<<"check derivative="<<refineIntrinsic.get_check_derivatives()<<std::endl;
        }
        refineIntrinsic.minimize_using_gradient(init);
        if(Verbose_1)
        {
            //LS_reprojection_f.plot();
            std::cerr <<"Average reprojection error RMS before optimization = ="<<refineIntrinsic.get_start_error()<<std::endl;
            std::cerr <<"Average reprojection error RMS after optimization = ="<<refineIntrinsic.get_end_error()<<std::endl;
            std::cerr <<"get_num_evaluations ="<<refineIntrinsic.get_num_evaluations()<<std::endl;
            std::cerr <<"get_num_iterations ="<<refineIntrinsic.get_num_iterations()<<std::endl;
        }
        log.push_back(refineIntrinsic.get_end_error());//log
        log.push_back(refineIntrinsic.get_start_error());//log
        log.push_back(refineIntrinsic.get_num_iterations());//log
        log.push_back(NbPoses);//log
        for( i=0 ; i<NbPoses ; ++i )
        {
            log.push_back(model3DList[i].size()); //log number of points for each pose
            vnl_vector<double> vec(6);
            for( j=0 ; j<6 ; ++j )
                vec[j] = init[nbParameters + i*6 + j];
            vnl_rigid_vector rigid_vec(vec);
            liste_extrinsic[i] = vnl_rigid_matrix(rigid_vec);
        }
        vnl_vector< double> reprojection_error(nbPointsTotal);
        LS_reprojection_f.getReprojectionError(init, reprojection_error);
        //double sum=0;
        log.push_back(nbPointsTotal);//log
        for( i=0 ; i<nbPointsTotal ; ++i )
            {
                if(Verbose_1){std::cerr<<"reproj ("<<i<<")= "<<reprojection_error[i]<<std::endl;}           
                //sum+=reprojection_error[i]*reprojection_error[i];
                log.push_back(reprojection_error[i]);//log
            }
        //std::cerr<<"rms ="<< sqrt(sum/ nbPointsTotal)<<std::endl;
        camera.setfx(init[0]);
        camera.setfy(init[1]);
        camera.setcx(init[2]);
        camera.setcy(init[3]);
        camera.setAlphaC( 0.0 );
        for( i=0 ; i<4 ; ++i )
            camera.setkc( i, init[4+i] );
    }
    if(Method == 1) // OPTIMIZATION WITH POWELL 
    {   
        arlCore::Intrinsic_cost_function reprojection_f(NbPoses*6+nbParameters);
        reprojection_f.setVerbose(Verbose_2);
        for( i=0 ; i<NbPoses ; ++i )
        {
            if(model3DList.size()==1) j=0; else j=i;
            reprojection_f.addPattern( points2DList[i].getList(), model3DList[i].getList() );
        }
        vnl_powell refineIntrinsic(&reprojection_f);
        if(optimiserParameters.size()>2) refineIntrinsic.set_f_tolerance(optimiserParameters[2]);
        else refineIntrinsic.set_f_tolerance(1e-10);
        if(optimiserParameters.size()>3)
            if(optimiserParameters[3] == 0){}//1e-8 default
            else {refineIntrinsic.set_x_tolerance(optimiserParameters[3]);} 
        if(optimiserParameters.size()>4) 
            if(optimiserParameters[4] == 0){}//1e-5 default
            else {refineIntrinsic.set_g_tolerance(optimiserParameters[4]);}
        if(Verbose_1)
        {
            std::cerr<<"f tolerance ="<<refineIntrinsic.get_f_tolerance()<<std::endl;
            std::cerr<<"x tolerance ="<<refineIntrinsic.get_x_tolerance()<<std::endl;
            std::cerr<<"g tolerance ="<<refineIntrinsic.get_g_tolerance()<<std::endl;
            std::cerr<<"check derivative="<<refineIntrinsic.get_check_derivatives()<<std::endl;
        }
        refineIntrinsic.minimize(init);
        if(Verbose_1)
        {
            std::cerr <<"Average reprojection error RMS before optimization ="<<sqrt(refineIntrinsic.get_start_error())<<std::endl;
            std::cerr <<"Average reprojection error RMS after optimization ="<<sqrt(refineIntrinsic.get_end_error())<<std::endl;
            //reprojection_f.plot();
            std::cerr <<"get_num_evaluations ="<<refineIntrinsic.get_num_evaluations()<<std::endl;
            std::cerr <<"get_num_iterations ="<<refineIntrinsic.get_num_iterations()<<std::endl;
        }
        log.push_back(sqrt(refineIntrinsic.get_end_error()));//log
        log.push_back(sqrt(refineIntrinsic.get_start_error()));//log
        log.push_back(refineIntrinsic.get_num_iterations());//log
        log.push_back(NbPoses);//log
        for( i=0 ; i<NbPoses ; ++i )
        {
            log.push_back(model3DList[i].size()); //log number of points for each pose
            vnl_vector<double> vec(6);
            for( j=0 ; j<6 ; ++j )
                vec[j] = init[nbParameters + i*6 + j];
            vnl_rigid_vector rigid_vec(vec);
            liste_extrinsic[i] = vnl_rigid_matrix(rigid_vec);
        }
        std::vector< double> reprojection_error = reprojection_f.getReprojectionError(init);
        //double sum=0;
        log.push_back(nbPointsTotal);//log
        for( i=0 ; i<nbPointsTotal ; ++i )
            {
                if(Verbose_1){std::cerr<<"reproj ("<<i<<")= "<<reprojection_error[i]<<std::endl;}           
                //sum+=reprojection_error[i]*reprojection_error[i];
                log.push_back(reprojection_error[i]);
            }
        //std::cerr<<"rms ="<< sqrt(sum/ nbPointsTotal)<<std::endl;
        camera.setfx(init[0]);
        camera.setfy(init[1]);
        camera.setcx(init[2]);
        camera.setcy(init[3]);
        camera.setAlphaC( 0.0 );
        for( i=0 ; i<4 ; ++i )
            camera.setkc( i, init[4+i] );
    }   
    return true;
}

bool arlCore::intrinsicCalibration(const std::vector<arlCore::PointList> &model3DList, const std::vector <arlCore::PointList> &points2DList, unsigned int nbParameters, arlCore::Camera &camera, std::vector< vnl_rigid_matrix> &liste_extrinsic, const std::vector<double> &optimiserParameters, std::vector<double> &log)
{
    assert(model3DList.size()==points2DList.size());
    //! @todo Assert( Au moins 4 points par pose )
    std::vector<double> optimiserParametersInit, optimiserParametersRefine;// TODO the optimiserParameters should be filled independently
    unsigned int i;
    for( i=0 ; i<(unsigned int)optimiserParameters.size(); ++i )
        optimiserParametersRefine.push_back(optimiserParameters[i]);

    // FIXME : Initialisation possible uniquement si model3D plan et z==0
    //! @todo Utiliser une autre initialisation si model3D non plan
    //! @todo Si model3D plan et z!=0, calculer T le ramenant dans le plan oxy
    if(initIntrinsicCalibration(model3DList, points2DList, camera, liste_extrinsic, optimiserParametersInit, log))
    {
        if(refineIntrinsicCalibration(model3DList, points2DList, nbParameters, camera, liste_extrinsic, optimiserParametersRefine, log))
            return true;
    }else
    {
        //! @todo Traiter le cas où l'initialisation ne fonctionne pas
    }
    return false;
}

/**
 * @brief  Initialization of the extrinsic parameter of a group of camera
 * A 3D model (chessboard...) is looked by ALL cameras and several poses of
 * the 3D model are recorded 
 * We estimate here :
 * 1) the extrinsic transformations Te_k with respect to the first camera frame so that
 * sum ||P1(Te_k*Mk_i) - mk_i ||^2 is minimal. P1 is the projective function
 * of the first camera with extrinsic parameters AT IDENTITY !! 
 * Mk_i are the 3D points of the model in his own frame for the k_th pose
 * mk_i are the 2D point reprojection which can be visible or not (then they are NULL) for the k_th pose
 * In the function, points which are not visible in the image are sorted and not considered when given to arlcore::planarHomography
 *
 * 2) transformations Ts_j between each camera frame with respect to the 1st camera frame, so that
 * sum ||Pj(Ts_j*Te_0*M0_i) - m0_j_i ||^2 is minimal. Pj corresponds to the projective function
 * of the j th camera with extrinsic parameters set to IDENTITY.
 * M0_i are the 3D points of the model in his own frame for the 0 th pose
 * m0_j_i are the 2D point reprojection of the 3D points for the 0 th pose of the camera j
 *
 * The estimation is realized from the pose nb 0. We could do better by averaging
 * the estimations on all poses but this seems not necessary.
 * c
 * In the algorithm the computed homography is then refined with ISPPC
 * TODO the optimisation criterion of refineExtrinsicCalibration allows to calibrate a polyscopic
 * system which field of view are not common. Nevertheless, the initilization supposes that 3D models 3D
 * are always visible in the 1st camera. This is a constraint we could remove
 */
bool arlCore::initExtrinsicCalibration(const std::vector<PointList>& models3D, const std::vector<std::vector<std::vector<arlCore::Point*> > >& liste_points2D, const std::vector<arlCore::Camera> &cameras, std::vector< arlCore::vnl_rigid_matrix> &init_rigid_trsf, const std::vector<double> &optimiserParameters, std::vector<double> &log )
{
    unsigned int i;
    std::vector<const arlCore::Camera*> cams;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return initExtrinsicCalibration( models3D, liste_points2D, cams, init_rigid_trsf, optimiserParameters, log );
}

bool arlCore::initExtrinsicCalibration(const std::vector<PointList>& models3D, const std::vector<std::vector<std::vector<arlCore::Point*> > >& liste_points2D, const std::vector<const arlCore::Camera*> &cameras, std::vector< arlCore::vnl_rigid_matrix> &init_rigid_trsf, const std::vector<double> &optimiserParameters, std::vector<double> &log )
{
    const bool Verbose = false;
    const unsigned int NbPoses = (unsigned int)models3D.size(), NbCameras = (unsigned int)liste_points2D[0].size();
    log.push_back(NbPoses);
    std::vector<double> log_homography, log_ISPPC;
    // estimation of Te_i
    std::vector<std::vector<arlCore::Point::csptr> > point_2D_visible_cam0(NbPoses), point_2D_visible_pose0(NbCameras-1);
    std::vector<std::vector<arlCore::Point*> > point_3D_visible_cam0(NbPoses), point_3D_visible_pose0(NbCameras-1);
    std::vector<double> optimiserParametersISPPC;//not used here and have to be parameterized in the function (which is not good)
    unsigned int i,j;   
    for( i=0 ; i<NbPoses ; ++i )//creation of the 2D point list and 3D point list without the invisible points
        for( j=0 ; j<models3D[i].size(); j++ )
            if(liste_points2D[i][0][j]!= 0)
            {
                point_3D_visible_cam0[i].push_back((Point*)models3D[i][j]);
                point_2D_visible_cam0[i].push_back(liste_points2D[i][0][j]);
            }
    for( i=1 ; i<NbCameras ; ++i )
        for( j=0 ; j<models3D[0].size() ; j++ )
            if(liste_points2D[0][i][j]!= 0)
            {
                //std::cerr<<"numero camera "<< i << "   numero point "<<j<<std::endl;
                //std::cerr<<"models3D[0][j] " << models3D[0][j]->getCoordinates() <<std::endl;
                //std::cerr<<"liste_points2D[0][i][j] " << liste_points2D[0][i][j]->getCoordinates() <<std::endl;
                // point_3D_visible_pose0 contains a list of 3D model for the camera 2 -> N
                // points that are not visible in the i th camera are not taken into account because of the if test
                point_3D_visible_pose0[i-1].push_back((Point*)models3D[0][j]);
                // point_2D_visible_pose0 contains a list of 2D point reprojection of the model for the camera 2 -> N
                point_2D_visible_pose0[i-1].push_back(liste_points2D[0][i][j]);
            }
    init_rigid_trsf.resize(NbCameras-1+NbPoses);
    for( i=0 ; i<NbPoses ; ++i )// computation of the Te_k (contained in init_rigid_trsf)
    {
        arlCore::planarHomographyRegistration_3D_2D( *(cameras[0]), point_2D_visible_cam0[i], point_3D_visible_cam0[i], init_rigid_trsf[i], optimiserParameters, log_homography, false);
        std::vector<const arlCore::Camera*> tmp_cam;
        tmp_cam.push_back(cameras[0]);
        std::vector<std::vector<arlCore::Point::csptr> > tmp_pts_2D;
        tmp_pts_2D.push_back(point_2D_visible_cam0[i]);
        arlCore::multiViewPointRegistration3D2D( tmp_cam, tmp_pts_2D, point_3D_visible_cam0[i], init_rigid_trsf[i], arlCore::ARLCORE_PR_ISPPC, optimiserParameters, log_ISPPC, false);
    }           
    for( i=1 ; i<NbCameras ; ++i )
    {
        arlCore::planarHomographyRegistration_3D_2D( *(cameras[i]), point_2D_visible_pose0[i-1], point_3D_visible_pose0[i-1], init_rigid_trsf[NbPoses +i-1], optimiserParameters, log_homography, false);
        //The transformation  T_calc_j (here init_rigid_trsf[NbPoses +i-1]) computed in the last step for cam i minimize the following criterion
        //sum ||Pj(T_calc_j*M0_i) - m0_j_i ||^2 . On doit rendre Ts_j et Ts_j*Te_0 =T_calc_j
        //std::cerr<< "HOMOGRAPHY init_rigid_trsf[NbPoses +j-1]="<<init_rigid_trsf[NbPoses +j-1]<< std::endl;
        std::vector<const arlCore::Camera*> tmp_cam;
        tmp_cam.push_back(cameras[i]);
        std::vector<std::vector<arlCore::Point::csptr> > tmp_pts_2D;
        tmp_pts_2D.push_back(point_2D_visible_pose0[i-1]);
        arlCore::multiViewPointRegistration3D2D( tmp_cam, tmp_pts_2D, point_3D_visible_pose0[i-1], init_rigid_trsf[NbPoses +i-1], arlCore::ARLCORE_PR_ISPPC, optimiserParameters, log_ISPPC, false);
        // now the transformation init_rigid_trsf[NbPoses +i-1] is refined
        //std::cerr<< "ISPPC 3D2D init_rigid_trsf[NbPoses +i-1]="<<init_rigid_trsf[NbPoses +i-1]<< std::endl;
        arlCore::vnl_rigid_matrix var;
        var.invert( init_rigid_trsf[0] );
        // Ts_j is computed as T_calc_j * Te_0 ^(-1) 
        init_rigid_trsf[NbPoses +i-1] = init_rigid_trsf[NbPoses +i-1] * var;
    }
    // TODO fill the log with log_homography and log_ISPPC
    if(Verbose) std::cerr<<"Exit initExtrinsicCalibration "<<std::endl;
    return true;
}
/**
 * @brief Refinement of the extrinsic calibration of several cameras
 * In:
 * liste_points2D : for each pose, for each camera, there is a 2D point list
 * models3D : 3D models point list (for each pose the 3D model can be different, but all points have to be seen by all camera, this has to be changed in the future)
 * cameras : intrinsic parameter list of all cameras (extrinsic parameter have to be identity)
 * init_rigid_trsf : estimated rigid transformation between each camera frame to the 3D model frame
 * 
 * optimiserParameter : this works for powell and LM minimization
 * [0] = number of the optimization method (0 for LM, 1 for powell)
 * [1] = verbose mode on the reprojection error : if 1 then the reprojection error is displayed during optimization 
 * [2] = f_tolerance : if 0 or nothing is given, default value is 1e-6
 * [3] = x_tolerance : if 0 or nothing is given, default value is 1e-8
 * [4] = g_tolerance : if 0 or nothing is given, default value is 1e-5
 * 
 * Optimised criterion definition:
 * sum(pose k) sum(camera 1) sum(points i) ||P_1 (T_ek*Mi) - m_k_1_i ||^2
 * + sum(pose k) sum(camera j>1) sum(points i) ||P_j (T_j*T_ek*Mi) - m_k_j_i ||^2
 * where T_ek is the extrinsic transformation from 1 st camera to the 3D model in the k th pose
 * and T_j represents the transformation between the camera j optical center frame to the 1 st frame
 * WARNING : it is assumed that for each pose, the same number of point is seen by all camera
 * This constraint should be relaxed in the future
 * 
 * Log[0] : final RMS reprojection error sqrt(mean squared residual)
 * Log[1] : begin RMS reprojection error sqrt(mean squared residual)
 * Log[2] ... Log[total number of residual+2] : list or reprojection error of ALL points
 * They are organized in the following way:
 * pose 0 camera 0 pt 1
 * pose 0 camera 0 pt 2
 * ..
 * pose 0 camera 0 pt nbmax of pattern 0
 * pose 0 camera 1 pt 1
 * pose 0 camera 1 pt 2
 * ..
 * pose 0 camera 1 pt nbmax of pattern 0
 * ..
 * pose 0 camera nb_total pt nbmax of pattern 0
 * pose 1 camera 0 pt 1 ...
 * pose 1 camera 0 pt nbmax of pattern 1
 * pose 1 camera 1 pt 1 ...
 * pose 1 camera 1 pt nbmax of pattern 1 ...
 * ..
 * pose 1 camera nb_total pt nbmax of pattern 1 ... etc 
 */

bool arlCore::refineExtrinsicCalibration( const std::vector<PointList>& models3D, const std::vector<std::vector<std::vector<arlCore::Point*> > >& liste_points2D, const std::vector<arlCore::Camera> &cameras, std::vector< arlCore::vnl_rigid_matrix> &refine_rigid_trsf, const std::vector<double> &optimiserParameters, std::vector<double> &log )
{
    unsigned int i;
    std::vector<const arlCore::Camera*> cams;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return refineExtrinsicCalibration( models3D, liste_points2D, cams, refine_rigid_trsf, optimiserParameters, log );
}

bool arlCore::refineExtrinsicCalibration( const std::vector<PointList>& models3D, const std::vector<std::vector<std::vector<arlCore::Point*> > >& liste_points2D, const std::vector<const arlCore::Camera*> &cameras, std::vector< arlCore::vnl_rigid_matrix> &refine_rigid_trsf, const std::vector<double> &optimiserParameters, std::vector<double> &log )
{
    unsigned int method = 0; // Default
    bool Verbose = false, Verbose_optim = false; // default
    if(optimiserParameters.size()>1) Verbose_optim = (optimiserParameters[1]!=0.0);
    if(Verbose) std::cerr<<"debut refineExtrinsicCalibration "<<std::endl;
    if(optimiserParameters.size()>0) method = (unsigned int)optimiserParameters[0];
    const unsigned int NbCameras = (unsigned int)cameras.size();
    const unsigned int NbPoses = (unsigned int)models3D.size();
    unsigned int i, j;
    vnl_vector<double> init((NbCameras-1)*6+6*NbPoses);
    std::vector<arlCore::vnl_rigid_matrix> solution((NbCameras-1)+NbPoses);
    std::vector<arlCore::vnl_rigid_matrix> estimation((NbCameras-1)+NbPoses);
    long unsigned int nbTotal2Dpoints = 0;
    for( i=0 ; i<NbPoses ; ++i )
        nbTotal2Dpoints+=models3D[i].size()*NbCameras;
    refine_rigid_trsf.resize(NbCameras-1+NbPoses);
    for( i=0 ; i<NbCameras-1+NbPoses ; ++i )
    {
        arlCore::vnl_rigid_vector vec(refine_rigid_trsf[i]);
        for( j=0 ; j<6 ; ++j )
            init[6*i+j] = vec[j];
    }
    if(method == 0) // OPTIMIZATION WITH LEVENBERG-MARQUARDT
    {
        arlCore::ExtrinsicLS_cost_function Extrinsic_cost_func(cameras, (NbCameras-1)*6+6*NbPoses, nbTotal2Dpoints, vnl_least_squares_function::use_gradient);
        Extrinsic_cost_func.setVerbose(Verbose_optim);
        for( i=0 ; i<NbPoses ; ++i)
            Extrinsic_cost_func.addPattern( liste_points2D[i], models3D[i].getList() );
        vnl_levenberg_marquardt refineExtrinsic(Extrinsic_cost_func);
        refineExtrinsic.set_trace(true);
        //refineExtrinsic.set_check_derivatives(1);
        if(optimiserParameters.size()>2) refineExtrinsic.set_f_tolerance(optimiserParameters[2]);
        else refineExtrinsic.set_f_tolerance(1e-6); // Erreur de reprojection stable (sqrt(m_error)) à 1e-3 près
        if(optimiserParameters.size()>3)
            if(optimiserParameters[3] == 0){}//1e-8 default
            else {refineExtrinsic.set_x_tolerance(optimiserParameters[3]);} 
        if(optimiserParameters.size()>4) 
            if(optimiserParameters[4] == 0){}//1e-5 default
            else {refineExtrinsic.set_g_tolerance(optimiserParameters[4]);}
        Extrinsic_cost_func.setObserver(true);
        if(Verbose) std::cerr<<"debut minimize "<<std::endl;
        refineExtrinsic.minimize_using_gradient(init);
        if(Verbose) std::cerr<<"fin minimize "<<std::endl;
        vnl_vector<double> reprojection_error(nbTotal2Dpoints);
        Extrinsic_cost_func.getReprojectionError(init,reprojection_error);
        log.push_back(refineExtrinsic.get_end_error());
        log.push_back(refineExtrinsic.get_start_error());
        for( i=0 ; i<nbTotal2Dpoints ; ++i )
            log.push_back(reprojection_error[i]);
    }
    if(method == 1) // OPTIMIZATION WITH POWELL 
    {   
        arlCore::Extrinsic_cost_function Extrinsic_cost_func(cameras, (NbCameras-1)*6+6*NbPoses);
        Extrinsic_cost_func.setVerbose(Verbose_optim);
        for( i=0 ; i<NbPoses ; ++i)
            Extrinsic_cost_func.addPattern( liste_points2D[i], models3D[i].getList() );
        vnl_powell refineExtrinsic(&Extrinsic_cost_func);
        if(optimiserParameters.size()>2) refineExtrinsic.set_f_tolerance(optimiserParameters[2]);
        else refineExtrinsic.set_f_tolerance(1e-6); // Erreur de reprojection stable (sqrt(m_error)) à 1e-3 près
        if(optimiserParameters.size()>3)
            if(optimiserParameters[3] == 0){}//1e-8 default
            else {refineExtrinsic.set_x_tolerance(optimiserParameters[3]);} 
        if(optimiserParameters.size()>4) 
            if(optimiserParameters[4] == 0){}//1e-5 default
            else {refineExtrinsic.set_g_tolerance(optimiserParameters[4]);}
        Extrinsic_cost_func.setObserver(true);
        if(Verbose) std::cerr<<"debut minimize "<<std::endl;
        refineExtrinsic.minimize(init);
        if(Verbose) std::cerr<<"fin minimize "<<std::endl;
        std::vector< double> reprojection_error = Extrinsic_cost_func.getReprojectionError(init);
        log.push_back(refineExtrinsic.get_end_error());
        log.push_back(refineExtrinsic.get_start_error());
        for( i=0 ; i<nbTotal2Dpoints ; ++i )
            log.push_back(reprojection_error[i]);
    }   
//  Extrinsic_cost_func.plot();
    for( i=0 ; i<NbCameras + NbPoses-1 ; ++i )
    {
        arlCore::vnl_rigid_vector vec;
        for( j=0 ; j<6 ; ++j )
            vec[j] = init[6*i+j];
        arlCore::vnl_rigid_matrix var(vec);
        refine_rigid_trsf[i] = var;
    }
    return true;
}

bool arlCore::extrinsicCalibration( const std::vector<PointList>& models3D, const std::vector<std::vector<std::vector<arlCore::Point*> > >& liste_points2D, const std::vector<arlCore::Camera> &cameras, std::vector< arlCore::vnl_rigid_matrix> &refine_rigid_trsf, const std::vector<double> &optimiserParameters, std::vector<double> &log )
{
    unsigned int i;
    std::vector<const arlCore::Camera*> cams;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return extrinsicCalibration( models3D, liste_points2D, cams, refine_rigid_trsf, optimiserParameters, log );
}

bool arlCore::extrinsicCalibration( const std::vector<PointList>& models3D, const std::vector<std::vector<std::vector<arlCore::Point*> > >& liste_points2D, const std::vector<const arlCore::Camera*> &cameras, std::vector< arlCore::vnl_rigid_matrix> &refine_rigid_trsf, const std::vector<double> &optimiserParameters, std::vector<double> &log )
{
    std::vector<double> optimiserParametersInit, optimiserParametersRefine;// TODO the optimiserParameters should be filled independently
    unsigned int i;
    for( i=0 ; i<(unsigned int)optimiserParameters.size(); ++i )
        optimiserParametersRefine.push_back(optimiserParameters[i]);
    if(initExtrinsicCalibration(models3D, liste_points2D, cameras, refine_rigid_trsf, optimiserParametersInit, log ))
    {
        if(refineExtrinsicCalibration(models3D, liste_points2D, cameras, refine_rigid_trsf, optimiserParametersRefine, log))
            return true;
    }else
    {
        // TODO : Traiter le cas où l'initialisation ne fonctionne pas
    }
    return false;
}
