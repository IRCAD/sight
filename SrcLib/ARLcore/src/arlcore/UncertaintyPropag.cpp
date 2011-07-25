/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/UncertaintyPropag.h>
#include <stdlib.h>

#include <stdlib.h> 
#include <cstdio>

#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_transpose.h>

#include <arlcore/Point.h>
#include <arlcore/Misc.h>
#include <arlcore/vnl_covariance_matrix.h>

bool arlCore::propagateCovarianceToReconst3D(const std::vector<arlCore::Point::csptr>&list2D, const std::vector<const arlCore::Camera*> &camsList,
     arlCore::Point &point3D, arlCore::ARLCORE_RECONSTRUCTION3D methode, std::vector<double> &log, const bool pixelFrame)
{
    unsigned int i, l, perfect_reproj2D_nb=0;;
    if(methode == arlCore::ARLCORE_R3D_TWO_LINES_PERFECT || methode == arlCore::ARLCORE_R3D_MULTI_LINES_APPROX || methode == arlCore::ARLCORE_R3D_MULTI_LINES_PERFECT)
    {
        const bool verbose=false;
        if(verbose){std::cerr<<"point3D cov matrix= \n"<< point3D.getCovMatrix() <<std::endl;}

        for(l=0;l<camsList.size();++l)
            if( list2D[l]->getCovMatrix().is_zero() )
                perfect_reproj2D_nb++;
                
        if(perfect_reproj2D_nb > 1)// it means that 2 lines are perfect, then the cov matrix is zero
        {
            arlCore::vnl_covariance_matrix &cov_mat = point3D.getCovMatrix();
            for(i=0;i<3;++i)
                for(l=0;l<3;++l)
                    cov_mat.put(i,l,0.0);// = Sigma_Point_Rec;
            return true;
        }

        arlCore::vnl_covariance_matrix &cov_mat = point3D.getCovMatrix();
        for(i=0;i<3;++i)
            for(l=0;l<3;++l)
                cov_mat.put(i,l,0.0);// = Sigma_Point_Rec;
        return true;
    }
    if(methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION || methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY)
    {
        for(l=0;l<camsList.size();++l)
            if( list2D[l]->getCovMatrix().is_zero() )
                perfect_reproj2D_nb++;
                
        if(perfect_reproj2D_nb > 1)// it means that 2 lines are perfect, then the cov matrix is zero
        {
            arlCore::vnl_covariance_matrix &cov_mat = point3D.getCovMatrix();
            for(i=0;i<3;++i)
                for(l=0;l<3;++l)
                    cov_mat.put(i,l,0.0);// = Sigma_Point_Rec;
            return true;
        }
    const bool verbose = false;
    std::vector< vnl_matrix_fixed<double,2,3> > dml_dM(camsList.size());
    std::vector< vnl_matrix_fixed<double,3,2> > dPhi_dml(camsList.size());
    vnl_matrix_fixed<double,3,3> H, Hinv, Gamma, Sigma_Point_Rec;
    H.fill(0.0);
    Gamma.fill(0.0);
    
    vnl_matrix_fixed<double,1,3> mat_Cint;
    vnl_matrix_fixed<double,2,1> tmp_mat_2_1;
    vnl_matrix_fixed<double,1,2> tilde_mi_sous_mi;
    arlCore::Point tmp_arl_pt(3), p2D_undistorted(2);
    double inner_product_Cl_Pil_M;

    for(l=0;l<camsList.size();++l)
    {
        camsList[l]->undistort2DPoint(*list2D[l], p2D_undistorted);
        camsList[l]->getExtrinsic().trf(point3D,tmp_arl_pt);
        inner_product_Cl_Pil_M = camsList[l]->getCint()[0] * tmp_arl_pt[0]
                                +camsList[l]->getCint()[1] * tmp_arl_pt[1]
                                +camsList[l]->getCint()[2] * tmp_arl_pt[2];
        if(verbose){std::cerr<<"inner= "<<inner_product_Cl_Pil_M<<std::endl;}
        mat_Cint.set_row(0,camsList[l]->getCint());
        tmp_mat_2_1.set_column(0,p2D_undistorted.getCoordinates());
        if(verbose){std::cerr<<"mat_Cint= "<<mat_Cint<<std::endl;
        std::cerr<<"tmp_mat_2_1= "<<tmp_mat_2_1<<std::endl;         
        std::cerr<<"tmp_arl_pt= "<<tmp_arl_pt.print()<<std::endl;}          
        if(verbose){std::cerr<<"Qint = "<<camsList[l]->getQint().as_matrix()<<std::endl;
        std::cerr<<"tmp_mat_2_1 * mat_Cint "<<tmp_mat_2_1 * mat_Cint<<std::endl;}
        //dml_dM[l] = (camsList[l]->getQint().as_matrix() + toto.as_matrix());// * camsList[l]->getExtrinsic().getRotation().as_matrix();
        dml_dM[l] = (camsList[l]->getQint().as_matrix() - tmp_mat_2_1 * mat_Cint) * camsList[l]->getExtrinsic().getRotation();
        dml_dM[l] /= inner_product_Cl_Pil_M;
        if(verbose){std::cerr<<"dml_dM[l]= "<<dml_dM[l]<<std::endl;}    

        vnl_matrix_fixed<double,3,3> U;
        arlCore::Point point3Dreproj(2);
        camsList[l]->project3DPoint(point3D,point3Dreproj,false);
        tilde_mi_sous_mi.set_row(0, p2D_undistorted.getCoordinates() - point3Dreproj.getCoordinates() );
        if(methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION)
        {
            double sigma2D_2 = list2D[l]->getCovMatrix()(0,0);
            if(verbose){std::cerr<<"mat cov="<<  list2D[l]->getCovMatrix()(0,0) <<" "<<list2D[l]->getCovMatrix()(1,1)<<"\n";}
            // calcul de U + U.transpose NEGLIGEABLE !!
            U = (camsList[l]->getExtrinsic().getRotation()).transpose().as_matrix() * (mat_Cint.transpose().as_matrix()) * tilde_mi_sous_mi.as_matrix() * dml_dM[l];
            //std::cerr<<"U + U^t/sigma2D/<Cl|Pil*M>= "<<1/sigma2D_2/inner_product_Cl_Pil_M *(U+U.transpose())<<std::endl;
            dPhi_dml[l] = -1/sigma2D_2 * dml_dM[l].transpose();
            H += 1/sigma2D_2 * dml_dM[l].transpose() * dml_dM[l] + 1/sigma2D_2/inner_product_Cl_Pil_M *(U+U.transpose());
            //std::cerr<<"H^l="<<H<<std::endl;
            Gamma += sigma2D_2 * dPhi_dml[l] * dPhi_dml[l].transpose();
        }
        if(methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY)
        {
            vnl_matrix<double> sigma2D_inv = vnl_matrix_inverse<double>(list2D[l]->getCovMatrix());
            // calcul de U + U.transpose NEGLIGEABLE !!
            U = (camsList[l]->getExtrinsic().getRotation()).transpose().as_matrix() * (mat_Cint.transpose().as_matrix()) * tilde_mi_sous_mi.as_matrix() * sigma2D_inv * dml_dM[l];
            //std::cerr<<"U + U^t/sigma2D/<Cl|Pil*M>= "<<1/sigma2D_2/inner_product_Cl_Pil_M *(U+U.transpose())<<std::endl;
            dPhi_dml[l] = - dml_dM[l].transpose() * sigma2D_inv;
            H += dml_dM[l].transpose()* sigma2D_inv * dml_dM[l] + 1/inner_product_Cl_Pil_M *(U+U.transpose());
            //std::cerr<<"H^l="<<H<<std::endl;
            Gamma += dPhi_dml[l] * list2D[l]->getCovMatrix() *dPhi_dml[l].transpose();
        }       
    }
    if(verbose){std::cerr<< "Hessian matrix ="<< H<<std::endl;}
    Hinv = vnl_matrix_inverse<double>(H);
    if(verbose){std::cerr<< "Hessian inv ="<< Hinv <<std::endl;
    std::cerr<< "Gamma="<< Gamma <<std::endl;}
    Sigma_Point_Rec  = Hinv * Gamma.as_matrix() * Hinv.transpose();
    if(verbose){std::cerr<< "matrice covariance 3D = "<< Sigma_Point_Rec << std::endl;}
    arlCore::vnl_covariance_matrix &cov_mat = point3D.getCovMatrix();
    for(i=0;i<3;++i)
        for(l=0;l<3;++l)
            cov_mat.put(i,l,Sigma_Point_Rec[i][l]);// = Sigma_Point_Rec;
    return true;
    }//if(methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION || methode == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY)

    if(methode == arlCore::ARLCORE_R3D_TWO_LINES_APPROX)
    {
        const bool verbose = false;
        const bool verbose_cov = false;
        //vnl_vector_fixed<double,3> I, J, m1C1, m2C2, C1C2, v, n1, n2;
        vnl_vector_fixed<double,4> var, C2, m2;
        arlCore::vnl_rigid_matrix P;//,PassageMatrix = camsList[1]->getExtrinsic() * camsList[0]->getInvExtrinsic();
        //double num, denom, landa1, landa2;
        unsigned int i, j;
//      P =  vnl_matrix_inverse<double>(PassageMatrix.as_matrix() );
        P = camsList[0]->getExtrinsic() * camsList[1]->getInvExtrinsic();
        if(verbose_cov){std::cerr<<"Passage matrix =\n"<< P;}
            arlCore::Point focal_m1(3), focal_m2(3);
        camsList[0]->pixelPlaneToUnitFocalPlane( *(list2D[0]), focal_m1, false );
        camsList[1]->pixelPlaneToUnitFocalPlane( *(list2D[1]), focal_m2, false );
    
        arlCore::vnl_covariance_matrix &cov_mat_focal_m1 = focal_m1.getCovMatrix();
        arlCore::vnl_covariance_matrix &cov_mat_focal_m2 = focal_m2.getCovMatrix();
        vnl_matrix_fixed<double,3,3> tmp;

        if(verbose_cov){std::cerr<<"cov_mat_m1 =\n"<<list2D[0]->getCovMatrix()<<"\n";
        std::cerr<<"cov_mat_m2 =\n"<<list2D[1]->getCovMatrix()<<"\n";}

        cov_mat_focal_m1.fill(0);
        cov_mat_focal_m2.fill(0);tmp.fill(0);
        cov_mat_focal_m1.put(0,0,(list2D[0]->getCovMatrix()[0][0]) / (camsList[0]->getfx()*camsList[0]->getfx()));
        cov_mat_focal_m1.put(1,1,(list2D[0]->getCovMatrix()[1][1]) / (camsList[0]->getfy()*camsList[0]->getfy()));
        tmp.put(0,0,(list2D[1]->getCovMatrix()[0][0]) / (camsList[1]->getfx()*camsList[1]->getfx()));
        tmp.put(1,1,(list2D[1]->getCovMatrix()[1][1]) / (camsList[1]->getfy()*camsList[1]->getfy()));
        if(verbose){std::cerr<<"tmp ="<< tmp <<"\n";}
        
//      tmp = vnl_transpose(P.getRotation()) * tmp * P.getRotation();
        tmp = P.getRotation() * tmp * vnl_transpose(P.getRotation() );
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_focal_m2.put(i,j,tmp[i][j]);// = Sigma_Point_Rec;
        
        if(verbose_cov){std::cerr<<"cov_mat_focal_m1 =\n"<<cov_mat_focal_m1<<"\n";
        std::cerr<<"cov_mat_focal_m2 =\n"<<cov_mat_focal_m2<<"\n";}
        
        var.put(0, 0.0); var.put(1, 0.0); var.put(2, 0.0); var.put(3, 1.0);
        C2 = P.as_matrix() * var.as_vector();
        if(verbose){std::cerr<<"C2 ="<<C2<<"\n";}
        // maintenant C2 est exprimée dans le repere liée à la caméra 1
        var.put(0, focal_m2[0]);
        var.put(1, focal_m2[1]);
        var.put(2, focal_m2[2]);
        var.put(3, 1.0);
        m2 = P.as_matrix() *var.as_vector();
        if(verbose){std::cerr<<"m2 ="<<m2<<"\n";}
        // maintenant m2 est exprimée dans le repere liée à la caméra 1
        arlCore::Point m1C1((-1.0)*focal_m1[0], (-1.0)*focal_m1[1], (-1.0)*focal_m1[2]);
        if(verbose){std::cerr<<"m1C1 ="<<m1C1.print()<<"\n";}
        arlCore::vnl_covariance_matrix &cov_mat_m1C1 = m1C1.getCovMatrix();
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_m1C1.put(i,j,cov_mat_focal_m1[i][j]);
                
        if(verbose_cov){std::cerr<<"cov_mat_focal_m1C1 =\n"<<cov_mat_m1C1<<"\n";}
                
        var  = C2 - m2;
        arlCore::Point m2C2( C2[0] - m2[0], C2[1] - m2[1], C2[2] - m2[2]);
        if(verbose){std::cerr<<"m2C2 ="<<m2C2.print()<<"\n";}
        arlCore::vnl_covariance_matrix &cov_mat_m2C2 = m2C2.getCovMatrix();
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_m2C2.put(i,j,cov_mat_focal_m2[i][j]);

        if(verbose_cov){std::cerr<<"cov_mat_focal_m2C2 =\n"<<cov_mat_m2C2<<"\n";}
        //m2C2.put(0, var(0)); m2C2.put(1, var(1)); m2C2.put(2, var(2));
        arlCore::Point C1C2( C2[0], C2[1], C2[2]);
        arlCore::vnl_covariance_matrix &cov_mat_C1C2 = C1C2.getCovMatrix();
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_C1C2.put(i,j,0.0);

        if(verbose_cov){std::cerr<<"cov_mat_C1C2 =\n"<<cov_mat_C1C2<<"\n";}
        //C1C2.put(0, C2(0)); C1C2.put(1, C2(1)); C1C2.put(2, C2(2));
        // Determination du premier point I sur la droite de reprojection
        //v = vnl_cross_3d(m1C1.as_vector() , m2C2.as_vector() );
        arlCore::Point v(3), n1(3), n2(3);
        v.cross_3D(m1C1 , m2C2);
        if(verbose_cov){std::cerr<<"cov_mat_v =\n"<<v.getCovMatrix()<<"\n";}
        //n1= vnl_cross_3d(v.as_vector(), m1C1.as_vector() );
        n1.cross_3D(v, m1C1);
        if(verbose){std::cerr<<"v ="<<v.print()<<"\n";
        std::cerr<<"n1 ="<<n1.print()<<"\n";}
        if(verbose_cov){std::cerr<<"cov_mat_n1 =\n"<<n1.getCovMatrix()<<"\n";}
        //double dot1=0, dot2=0;
        arlCore::Point dot1(1), dot2(1);
        dot1.dotProduct(C1C2, n1);
        dot2.dotProduct(m2C2, n1);
        if(verbose){std::cerr<<"dot1="<<dot1[0]<<"\n";std::cerr<<"cov_mat_dot1 =\n"<<dot1.getCovMatrix()<<"\n";
        std::cerr<<"dot2="<<dot2[0]<<"\n";std::cerr<<"cov_mat_dot2 =\n"<<dot2.getCovMatrix()<<"\n";}

        assert(dot2[0]!=0);
        //landa1 = num/denom;
        arlCore::Point landa1(1);
        landa1.divide(dot1,dot2);
        
        arlCore::Point I(3),J(3), int_I(3), int_J(3);
        int_I.multiply(landa1, m2C2);

        I.set(0, C2(0) - landa1[0]* m2C2[0] );  
        I.set(1, C2(1) - landa1[0]* m2C2[1] );
        I.set(2, C2(2) - landa1[0]* m2C2[2] );
        arlCore::vnl_covariance_matrix &cov_mat_I = I.getCovMatrix();
        
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_I.put(i,j, int_I.getCovMatrix()[i][j]);
//      for(i=0;i<3;++i)
//          for(j=0;j<3;++j)
//              cov_mat_I.put(i,j, landa1[0]*landa1[0]*m2C2.getCovMatrix()[i][j]);
        
        if(verbose_cov){std::cerr<<"cov_mat_int_I =\n"<<int_I.getCovMatrix()<<"\n";
        std::cerr<<"cov_mat_I =\n"<<I.getCovMatrix()<<"\n";}
        // Determination du deuxieme point J sur la droite de reprojection
        //n2 = vnl_cross_3d(v, m2C2);
        n2.cross_3D(v, m2C2);
        if(verbose){std::cerr<<"n2="<<n2.print()<<"\n";}
//      dot1 = dot2 =0;
//      for(i=0;i<3;++i)
//      {
//          dot1 +=  C1C2(i) * n2(i);
//          dot2 +=  m1C1(i) * n2(i);
//      }
        dot1.dotProduct(C1C2, n2);
        dot2.dotProduct(m1C1, n2);  
        //dot2.set(0, -1*dot2[0]);
        if(verbose){std::cerr<<"dot1="<<dot1[0]<<"\n";
        std::cerr<<"dot2="<<dot2[0]<<"\n";}
        assert(dot2[0]!=0);
        arlCore::Point landa2(1);
//      landa2 = num/denom;
        landa2.divide(dot1,dot2);
        if(verbose){std::cerr<<"landa2="<<landa2[0]<<"\n";}
        int_J.multiply(landa2, m1C1);
        
        J.set(0,  landa2[0]* m1C1[0] );  
        J.set(1,  landa2[0]* m1C1[1] );
        J.set(2,  landa2[0]* m1C1[2] );
        arlCore::vnl_covariance_matrix &cov_mat_J = J.getCovMatrix();
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_J.put(i,j, int_J.getCovMatrix()[i][j]);
//      for(i=0;i<3;++i)
//          for(j=0;j<3;++j)
//              cov_mat_J.put(i,j, landa2[0]*landa2[0]*m1C1.getCovMatrix()[i][j]);
                
        if(verbose_cov){std::cerr<<"cov_mat_J_int =\n"<<int_J.getCovMatrix()<<"\n";
        std::cerr<<"cov_mat_J =\n"<<J.getCovMatrix()<<"\n";}
        // On prend le barycentre de I et J comme estimation de la reconstruction
        arlCore::Point point3D_rep_cam(3);
        point3D_rep_cam.x(0.5*I[0] + 0.5*J[0]);
        point3D_rep_cam.y(0.5*I[1] + 0.5*J[1]);
        point3D_rep_cam.z(0.5*I[2] + 0.5*J[2]);
        
        if(verbose){std::cerr<<"point3D_rep_cam="<<point3D_rep_cam.print()<<"\n";}
        
        arlCore::vnl_covariance_matrix &cov_mat_point3D = point3D.getCovMatrix();
        vnl_matrix<double> toto(3,3);
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                toto.put(i,j, 0.5*0.5*I.getCovMatrix()[i][j] +  0.5*0.5*J.getCovMatrix()[i][j] );
    
        if(verbose_cov){std::cerr<<"cov_mat_toto =\n"<< toto <<"\n";}

        toto = camsList[0]->getInvExtrinsic().getRotation() * toto * camsList[0]->getExtrinsic().getRotation();
//      toto = camsList[0]->getInvExtrinsic().getRotation() * toto;
    
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                cov_mat_point3D.put(i,j, toto[i][j]);
        if(verbose_cov){std::cerr<<"cov_mat_point3D =\n"<< cov_mat_point3D <<"\n";}
        
        if( cov_mat_point3D[0][0] < 0.0 || cov_mat_point3D[1][1] < 0.0 || cov_mat_point3D[2][2] < 0.0)
            {std::cerr<<"Covariance Matrix has negative values "<<std::endl;assert(true);}
    }
    return true;    
}

void arlCore::WriteTableau ( char *nom, std::vector<double> tab, unsigned int n)
{
    unsigned int  i;
    FILE *f = fopen (nom, "w"); 
    for( i=0 ; i<n ; ++i )
        if(tab[i] != 0)
    fprintf (f,"%1.8f\n", tab[i]);
    fclose(f);
}

double arlCore::CumulativeChi2_3(double mu2)
{   // Validation : KS test, mean and covar
    return arlCore::IncompleteGammaP(3.0/2., mu2/2.);
}

double* arlCore::KSValidation3D(char* index_file)
{
    double I,*Itab,I2,ksp,ksdiff;
    int N, i;
    FILE *fd;
    double *Result;
    Result = new double[3];
    /**************  ouverture du fichier passe en argument **********/
    if((fd = fopen (index_file, "r")) == NULL)
    {
        fprintf (stderr, "no file %s\n", index_file);
        assert(true);
    }
    N=0;
    while( fscanf(fd,"%lg\n",&I) != EOF ) N++;
        rewind(fd);
    if( N==0)
    {
        fprintf (stderr, "le fichier est vide %s!!\n", index_file);
        assert(true);
    }
    Itab = new double[N];
    //    E_ISERR(FALSE,"Validation", "Allocation problem");
    I = 0.0;
    for(i=0;i<N;i++)
    {
        fscanf(fd,"%lg\n",Itab+i);
        I += Itab[i];
    }
    I /= ((double) N);
    I2 = 0;
    for(i=0;i<N;i++)
        I2 += (I-Itab[i])*(I-Itab[i]);
    if( N > 1)
        I2 /= ((double) N -1.);
    /* KS test */
    ksp = arlCore::SimpleKSTest( Itab, N, CumulativeChi2_3, &ksdiff);
//  printf("*****************************************************************\n");
//  printf("   Mean Val Index     -  Variance Val Index   - K-S significance \n");
//  printf("   Expected: 3.0      -     %12.6g    -     0.01 / 1.0\n",sqrt(6.0));
//  printf("       %12.6g    -     %12.6g   -    %12.6g\n",I,sqrt(I2),ksp);
//  printf("*****************************************************************\n\n");
    Result[0] = I;
    Result[1] = sqrt(I2);
    Result[2] = ksp;
    /* fclose(fd); */
    delete [] Itab;
    //    K_Free( (void **) &Itab );
    fclose(fd);
    return(Result);
}

//////////////////////////////
/************************************************************************/
/* Probabilistic tests and densities */
/************************************************************************/
double arlCore::ProbKSTest(double alam)
{   // Kolmogorov-Smirnov probability
    const double EPS1 = 0.001;
    const double EPS2 = 1.0e-12;
    unsigned int i;
    double fac=2.0,sum=0.0,term,termbf=0.0;
    const double a2 = -2.0*alam*alam;
    for( i=1 ; i<=100 ; ++i )
    {
        term=fac*exp(a2*i*i);
        sum += term;
        if( fabs(term)<=EPS1*termbf || fabs(term)<=EPS2*sum )
            return sum;
        fac = -fac;
        termbf=fabs(term);
    }
    return 1.0;
}
double arlCore::LnGamma( double xx )
{   // Log of Gamma Function (NRC : gammln)
    double x,y,tmp,ser;
    static double cof[6]=
        {76.18009172947146, -86.50532032941677, 24.01409824083091,
        -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5};
    unsigned int i;  
    y=x=xx;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    for( i=0 ; i<6 ; ++i )
        ser += cof[i]/++y;
    return -tmp+log(2.5066282746310005*ser/x);
}

double IGammaCFraction(double a, double x)
{   // Incomplete Gamma Function Continuous fraction estimation
    const double FPMIN = 1.0e-30;
    const double EPS = 3.0e-7;
    const unsigned int ITMAX = 100;
    int i;
    double an,b,c,d,del,h;
    const double gln = arlCore::LnGamma(a);
    b=x+1.0-a;
    c=1.0/FPMIN;
    d=1.0/b;
    h=d;
    for( i=1 ; i<ITMAX ; ++i )
    {
        an = -i*(i-a);
        b += 2.0;
        d=an*d+b;
        if(fabs(d) < FPMIN) d=FPMIN;
        c=b+an/c;
        if(fabs(c) < FPMIN) c=FPMIN;
        d=1.0/d;
        del=d*c;
        h *= del;
        if(fabs(del-1.0) < EPS) break;
    }
    //  E_ERR(i >= ITMAX, E_ERRNULL, -2.0, "IGammaCFraction(a,x)",   "a too large, ITMAX too small");
    return(exp(-x+a*log(x)-(gln))*h);
}

double IGammaSeries(double a, double x)
{   // Incomplete Gamma Function Series representation estimation
    const double EPS = 3.0e-7;
    const unsigned int ITMAX = 100;
    unsigned int i;
    double sum,del,ap, gamser;
    if( x<=0.0 ) 
    {
        std::cerr<<"Error in IGammaSeries : x==0.0\n";
        return 0.0;
    }
    //     {
    //       E_ERR(x < 0.0, E_ERRNULL, -2.0,"IGammaSeries",
    //      "x less than 0 in routine gser");
    //       return 0.0;
    //     } 
    else
    {
        const double gln = arlCore::LnGamma(a);
        ap=a;
        del=sum=1.0/a;
        for( i=0 ; i<ITMAX ; ++i )
        {
            ++ap;
            del *= x/ap;
            sum += del;
            if(fabs(del) < fabs(sum)*EPS)
            {
                gamser=sum*exp(-x+a*log(x)-(gln));
                return gamser;
            }
        }
    //     E_XERR(E_ERRNULL, -2.0, "IGammaSeries(a,x)",
    //      "a too large, ITMAX too small");
    }
    return -2.0;
}

double arlCore::IncompleteGammaP(double a, double x)
{   // Incomplete Gamma Function P(a,x)
    // Cumulative pdf of chi^2_d : IncompleteGammaP(d/2, mu2/2)
//   E_ERR (x < 0.0 || a <= 0.0, E_ERRNULL,-1., 
//   "IncompleteGammaP", "Invalid arguments (must be positive)");
    if(x < (a+1.0)) return IGammaSeries(a,x);
    else return 1.0-IGammaCFraction(a,x);
}

double arlCore::CumulativeChi2(double mu2, int n)
{   // Cumulative distribution of chi2_n(mu2)
    return arlCore::IncompleteGammaP(n/2., mu2/2.);
}

double IBetaSeries(double a, double b, double x)
{   // Continued fraction for Incomplete Beta Function   (NRC : betacf)
    const unsigned int MAXIT = 100;
    const double EPS = 3.0e-7;
    const double FPMIN = 1.0e-30;
    unsigned int m,m2;
    double aa,c,d,del,h,qab,qam,qap;

    qab=a+b;
    qap=a+1.0;
    qam=a-1.0;
    c=1.0;
    d=1.0-qab*x/qap;
    if(fabs(d) < FPMIN) d=FPMIN;
    d=1.0/d;
    h=d;
    for (m=1;m<=MAXIT;m++)
    {
        m2=2*m;
        aa=m*(b-m)*x/((qam+m2)*(a+m2));
        d=1.0+aa*d;
        if(fabs(d) < FPMIN) d=FPMIN;
        c=1.0+aa/c;
        if(fabs(c) < FPMIN) c=FPMIN;
        d=1.0/d;
        h *= d*c;
        aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
        d=1.0+aa*d;
        if(fabs(d) < FPMIN) d=FPMIN;
        c=1.0+aa/c;
        if(fabs(c) < FPMIN) c=FPMIN;
        d=1.0/d;
        del=d*c;
        h *= del;
        if(fabs(del-1.0) < EPS) break;
    }
//   E_ERR(m > MAXIT, E_ERRNULL, -1,
//  "IBetaSeries","a or b too big, or MAXIT too small in IBetaSeries");
  return h;
}

double arlCore::IncompleteBetaI(register double a, register double b,register  double x)
{   // Incomplete Beta Function Ix(a,b)  (NRC : betai)
  register double bt;
//   E_ERR (x < 0.0 || x > 1, E_ERRNULL,-1.,
//   "IncompleteBetaI","Invalid x argument (must be between 0 and 1)");
  if(x == 0.0 || x == 1.0) 
    bt=0.0;
  else
    bt=exp(LnGamma(a+b)-LnGamma(a)-LnGamma(b)+a*log(x)+b*log(1.0-x));
  if(x < (a+1.0)/(a+b+2.0))
    return bt*IBetaSeries(a,b,x)/a;
  else
    return 1.0-bt*IBetaSeries(b,a,1.0-x)/b;
}

double arlCore::CumulativeFisher(double a, double b, double x)
{   // Cumulative distribution of Fisher-Snedecor F(a,b)(x)
    return(1.0 - IncompleteBetaI(b/2., a/2., b/(b+a*x) ));
}

double CumulativeHotelling(double p, double n, double x)
{   // Cumulative distribution for Hotelling's test :  F(p,n-p)(x)
    return(1.0 - arlCore::IncompleteBetaI((n-p)/2., p/2., (n-p)/(n+p*(x-1.)) ));
}

int dcmp(double *a, double *b)
{
    if( *a<*b ) return(-1);
    else if(*a > *b) return(+1); 
        else return(0);
}

//! @return maximum of 2 variables
template <typename T> inline T max( const T& a, const T& b ){if(a>b) return a; else return b;}

//! @return minimum of 2 variables
template <typename T> inline T min( const T& a, const T& b ){if(a<b) return a; else return b;}

double arlCore::SimpleKSTest( double *data, int n, double (*pdf)(double), double *d)
{
/*----------------------------------------------------------------------*/
/* Simple Kolmogorov-Smirnov test : compare a set of n samples */
/*  (stored in array data) with the (cumulative) pdf  */
/* Output : d = maxdiff between distributions */
/* Returned value = corresponding probability */
/*----------------------------------------------------------------------*/
    unsigned int i;
    double dt, en, ff, fn, fo=0.0;
    qsort( (void *) data,  n, sizeof(double), (int(*)(const void *, const void *)) dcmp);
    en=n; // Pourquoi cette nouvelle variable?
    *d=0.0;
    for( i=0 ; i<n ; ++i )
    {
        fn = i/en; // Test Division par zero
        ff = (*pdf)(data[i]);
        dt = max(fabs(fo-ff),fabs(fn-ff));
        if( dt>*d ) *d=dt;
        fo=fn;
    }
    /* en=sqrt(en); */
    /*return ProbKSTest((en+0.12+0.11/en)*(*d)); */
    return ProbKSTest(sqrt(en)*(*d));
}

double arlCore::DoubleKSTest( double *data1, int n1, double *data2, int n2, double *d)
{
/*----------------------------------------------------------------------*/
/* Double Kolmogorov-Smirnov test : compare the set of n1 samples data1 */
/*   with the set of n2 samples data2 */
/* Output : d = maxdiff between distributions */
/* Returned value = corresponding probability */
/*----------------------------------------------------------------------*/
    int j1=0,j2=0;
    double d1,d2,dt,en1,en2,en,fn1=0.0,fn2=0.0;
    qsort( (void *) data1,  n1, sizeof(double),  
        (int(*)(const void *, const void *)) dcmp);
    qsort( (void *) data2,  n2, sizeof(double),  
        (int(*)(const void *, const void *)) dcmp);
    en1=n1;
    en2=n2;
    *d=0.0;
    while(j1 < n1 && j2 < n2)
    {
        if((d1=data1[j1]) <= (d2=data2[j2])) fn1=j1++/en1;
        if(d2 <= d1) fn2=j2++/en2;
        if((dt=fabs(fn2-fn1)) > *d) *d=dt;
    }
    en=sqrt(en1*en2/(en1+en2));
    return arlCore::ProbKSTest((en+0.12+0.11/en)*(*d));
}
