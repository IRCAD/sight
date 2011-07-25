/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/vnl_rotation3d_matrix.h>

#include <iostream>
#include <math.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_trace.h>

#include <arlcore/vnl_rotation3d_vector.h>
#include <arlcore/Optimization.h>

arlCore::vnl_rotation3d_matrix::vnl_rotation3d_matrix( void ):
vnl_matrix_fixed<double,3,3>()
{}

arlCore::vnl_rotation3d_matrix::vnl_rotation3d_matrix( const vnl_rotation3d_vector& rv )
{
    copy_in(rv);
}

arlCore::vnl_rotation3d_matrix::vnl_rotation3d_matrix( double phi, double theta, double psy )
{   // http://mathworld.wolfram.com/EulerAngles.html
    put(0,0,cos(psy)*cos(phi)-cos(theta)*sin(phi)*sin(psy));
    put(0,1,cos(psy)*sin(phi)+cos(theta)*cos(phi)*sin(psy));
    put(0,2,sin(psy)*sin(theta));
    put(1,0,-sin(psy)*cos(phi)-cos(theta)*sin(phi)*cos(psy));
    put(1,1,-sin(psy)*sin(phi)+cos(theta)*cos(phi)*cos(psy));
    put(1,2,cos(psy)*sin(theta));
    put(2,0,sin(theta)*sin(phi));
    put(2,1,-sin(theta)*cos(phi));
    put(2,2,cos(theta)*1.0);
}

arlCore::vnl_rotation3d_matrix::~vnl_rotation3d_matrix( void )
{}

arlCore::vnl_rotation3d_matrix& arlCore::vnl_rotation3d_matrix::operator=( const vnl_rotation3d_vector& rv )
{
    copy_in(rv);
    return *this;
}

arlCore::vnl_rotation3d_matrix::vnl_rotation3d_matrix( const vnl_matrix_fixed<double,3,3>& mat)
{
    copy_in(mat);
}

arlCore::vnl_rotation3d_matrix& arlCore::vnl_rotation3d_matrix::operator=( const vnl_matrix_fixed<double,3,3>& mat)
{
    copy_in(mat);
    return *this;
}

bool arlCore::vnl_rotation3d_matrix::is_rotation( void ) const
{   
    vnl_matrix_fixed<double,3,3> test_Identity;
    test_Identity = (*this) * this->transpose(); //on verifie d'abord pour le produit
    double det = vnl_det( *this ); //calcul du determinant qui doit etre egal a 1 et pas -1
    if( test_Identity.is_identity(1e-8) || fabs(vnl_det( *this ) -1) < 1e-6  ) return true;
    else return false;
}

vnl_matrix<double> arlCore::vnl_rotation3d_matrix::as_matrix( void )
{
    vnl_matrix<double> tmp(3,3);
    unsigned int i, j;
    for(i=0; i<3; i++) 
        for(j=0; j<3; j++)
             tmp(i,j) = (*this)(i,j);
    return tmp;
}

bool arlCore::vnl_rotation3d_matrix::closest_rotation( void )
{   
    unsigned int i,j, dim = 3;
    double small_val = 1e-12; //TODO c'est 1e-6 en float, il faut trouver la relation et la generaliser
    vnl_matrix_fixed<double,3,3> V, tmp2;
    vnl_matrix<double> tmp(3,3);
    tmp = this->as_matrix();    
    vnl_svd<double> svd_T_matrix( tmp );    
    V = svd_T_matrix.V();
//  V.transpose();
    tmp2 = svd_T_matrix.U() * V.transpose();
//  this->Mul(U,V);  // R = U.Vtrp
  double det = vnl_det( tmp2 );
  double det_test = fabs( fabs ( det ) - 1 );
  // If s = det(U.Vtrp) id not +1 or -1 : error
//  if( det_test >  small_val )
//      {
//      std::cout << "det = " << svd_T_matrix.determinant_magnitude() << "      |det|-1 = " << det_test << std::endl;
//      std::cout << "IsValid : non unit determinant" <<std::endl;
        //throw RotMat3D::ErrNonRotMat("LSQRot : non unit determinant");
//      }
  // If s = -1, we should multiply by s = -1 both the smallest
  // singular value of diag and the corresponding ligne in R
    if(det < 0.0)
        {
    // find smallest singular value
    //    s = w(0);
    //    Index i_dmax = 0;
    //    for(i=1;i<dim;i++)
    //      if( w(i) < s ) {
    //        s = w(i);
    //        i_dmax = i;
    //      }

    // in vxl : singular value are ranked from the largest to the smallest in svd_T_matrix.W() 
    // Multiply the sing. val and the corresponding row in V by -1
//    w(i_dmax) *= -1.0;
        for(i=0;i<dim;i++) 
            V(i,2) *= -1.0;
        // and recompute the rotation
        //this->Mul( U, V);  // R = U.Vtrp
        tmp2 = svd_T_matrix.U() * V.transpose();
    }
    for(i=0; i<3; i++) 
        for(j=0; j<3; j++)
             (*this)(i,j) = tmp2(i,j);
  // Grep for the number of singular (0) values
  {
    // Find the max : dans la vxl smw = svd.W(3,3)
    double smax = svd_T_matrix.W(0,0);
//    for(i=0;i<dim;i++)
//      if(w(i)>smax) smax = w(i);
    // counting small singular values
    int num = 0;
    if( svd_T_matrix.W(0,0) < small_val )
      num = dim;
    else
      for(i=0;i<dim;i++)
        if( ( (double) svd_T_matrix.W(i,i)/100. + smax) == ((double) smax)) num++;
//    if( num > 1)
//      std::cout << "*** WARNING: Matrix has " << num << " null singular values" << std::endl;
  }
/* TODO Using residue
  // compute the residue
  {
    double residue =0.0;
    for(i=0;i<dim;i++)
      residue += svd_T_matrix.W(i,i);
    return residue;
  }*/       
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Cette fonction calcule la distance géodésique entre deux rotations
// d(R1, R2) = 1/sqrt(2) || Log(R1^T * R2) || (norme de frobenius)
// In : A matrice 3x3 de rotation
// In : B matrice 3x3 de rotation
// Out : Distance entre les deux matrices de rotation
///////////////////////////////////////////////////////////////////////////////////////////////
double arlCore::vnl_rotation3d_matrix::sq_rieman_dist ( const vnl_rotation3d_matrix &B )
{
//  const bool Verbose = false;
    vnl_matrix<double> tmp(3,3), logAB(3,3);
//  unsigned int i;
//  if(Verbose)
//  {
//      for( i=0 ; i<3 ; ++i )
//          fprintf(stderr," B %f %f %f \n",  B(i,0), B(i,1), B(i,2));
//      fprintf(stderr,"\n Dans fonction rieman_dist \n");
//  }
    tmp = (this->as_matrix()).transpose() * B;
//  if(Verbose)
//      for( i=0 ; i<3 ; ++i )
//          fprintf(stderr," tmp %f %f %f \n",  tmp(i,0), tmp(i,1), tmp(i,2));
    double theta = 0.0;
    if( fabs ( ( vnl_trace(tmp) - 1.0 )/2.0 ) <= 1.0)
        theta = acos( ( vnl_trace(tmp) - 1.0 )/2.0 );
//  else
//  {
//      if(Verbose) fprintf(stderr,"ATTENTION : dans sq_rieman_dist l'angle entre deux matrices de rotation est tellement proche de 0, qu'une erreur d'arrondi est peut-etre survenue (lié à la fonction acos qui n'est défini que dans [-1,1]). Par défaut, toute valeur en dehors de cet intervalle est fixé à 1\n");
//  }
//  if(Verbose)
//      fprintf(stderr,"trace = %1.14f ; theta = %g \n",vnl_trace(tmp), theta);

    if(theta > 1e-6)
    {
        logAB = theta/(2*sin(theta)) * (tmp -tmp.transpose());
//      if(Verbose)
//          for( i=0 ; i<3 ; ++i )
//              fprintf(stderr,"logAB %f %f %f \n",  logAB(i,0), logAB(i,1), logAB(i,2));

        double dist_r = 0.5 *logAB.frobenius_norm() * logAB.frobenius_norm();
//      if(Verbose)
//      {
//          fprintf(stderr,"dist_r = %f \n", dist_r);
//          fprintf(stderr,"frobenius_norm = %f \n",logAB.frobenius_norm());
//      }
        return dist_r;
    }
    return 0.0;
}

double arlCore::vnl_rotation3d_matrix::average_rotation3d (const std::vector<vnl_rotation3d_matrix*> &rotation_list)
{
    
    arlCore::averageRotation_cost_function average_dist(rotation_list);
    average_dist.set_matrix_list(rotation_list);
    vnl_powell compute_average(&average_dist);
    vnl_vector<double> vrot_solution(3);
    vrot_solution(0)  = 0.0;  vrot_solution(1)  = 0.0;  vrot_solution(2)  = 0.0;
    compute_average.minimize(vrot_solution);

    std::cerr<<"Vecteur rotation moyen = "<<vrot_solution<<std::endl;
    std::cerr<<"Erreur de début = "<< compute_average.get_start_error()<<std::endl;
    std::cerr<<"Erreur de fin = "<< compute_average.get_end_error()<<std::endl;

    arlCore::vnl_rotation3d_vector vec(vrot_solution(0),vrot_solution(1),vrot_solution(2));
    vec.modulo();
    arlCore::vnl_rotation3d_matrix average_rot_matrix(vec);
    std::cerr << "Matrice de rotation moyenne" << std::endl << average_rot_matrix << std::endl;
    return 0;
}

void arlCore::vnl_rotation3d_matrix::copy_in( const vnl_rotation3d_vector& rv )
{   // Rotation Vector (a en radian) -> Rotation Matrix
    double c, s, theta, k1, k2, theta2, aux;
    unsigned int i, j;
    theta2 = rv[0] * rv[0] + rv[1] * rv[1] + rv[2] * rv[2];
    theta = sqrt(theta2);
    if(theta > 1e-2 )
    {
        c = cos(theta);
        s = sin(theta);
        k1 = s / theta;
        k2 = (1 - c) / theta2;
    }
    else 
    {   // Lim. Dev around theta = 0
        k2 = 1.0/2.0 - theta2/24.0;
        c = 1.0 - theta2*k2;
        k1 = 1.0 - theta2/6;
    }   // I + M*Mt
    for ( i=0 ; i<3 ; ++i )
        for ( j=0 ; j<=i ; ++j )
        {
            (*this)[i][j] = k2 * rv[i] * rv[j];
            if(i != j) (*this)[j][i] = (*this)[i][j];
            else (*this)[i][i] += c;
        }
    aux = k1 * rv[2];
    (*this)[0][1] -= aux;
    (*this)[1][0] += aux;
    aux = k1 * rv[1];
    (*this)[0][2] += aux;
    (*this)[2][0] -= aux;
    aux = k1 * rv[0];
    (*this)[1][2] -= aux;
    (*this)[2][1] += aux;
}

void arlCore::vnl_rotation3d_matrix::copy_in( const vnl_matrix_fixed<double,3,3>& mat )
{
    unsigned int i, j;
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            put(i,j, mat(i,j));
}
