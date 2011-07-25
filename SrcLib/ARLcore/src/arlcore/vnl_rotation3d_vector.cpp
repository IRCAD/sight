/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/vnl_rotation3d_vector.h>

#include <math.h>
#include <iostream>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>

#include <arlcore/vnl_rotation3d_matrix.h>
#include <arlcore/Misc.h>

arlCore::vnl_rotation3d_vector::vnl_rotation3d_vector( void ):
vnl_vector_fixed<double,3>()
{}

arlCore::vnl_rotation3d_vector::vnl_rotation3d_vector( double a, double b, double c ):
vnl_vector_fixed<double,3>(a,b,c)
{}

arlCore::vnl_rotation3d_vector::vnl_rotation3d_vector( const vnl_rotation3d_matrix& rm )
{
    copy_in(rm);
}

arlCore::vnl_rotation3d_vector::~vnl_rotation3d_vector( void )
{}

arlCore::vnl_rotation3d_vector& arlCore::vnl_rotation3d_vector::operator=( const vnl_rotation3d_matrix& rm )
{
    copy_in(rm);
    return *this;
}

void arlCore::vnl_rotation3d_vector::modulo()
{
    double theta, var=0, k, a;
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        var+=get(i)*get(i);
    var = sqrt(var);
    theta = ( var/(2*vnl_math::pi) - (int)(var/(2*vnl_math::pi)) ) * 2*vnl_math::pi;
    if(var == 0) return;
    else if(theta >= vnl_math::pi && theta < 2*vnl_math::pi)
    {
        theta -= 2*vnl_math::pi;
        k = theta/var;
    }
    else k = theta/var;
    for( i=0 ; i<3 ; ++i )
    {
        a=get(i);
        put(i,k*a);
    }
}

void arlCore::vnl_rotation3d_vector::uniform_random( void )
{
    double coef, norme, vec[3];
    unsigned int i;
    do
    { 
        norme = 0;
        for( i=0 ; i<3 ; ++i )
        {
            vec[i] = arlRandom::Random::uniformDoubleRnd(-1.0, 1.0);
            norme += vec[i]*vec[i];
        }
    }while( norme > 1 || norme < 0.05 );
    coef = arlRandom::Random::uniformDoubleRnd(-vnl_math::pi, vnl_math::pi)/sqrt(norme);
    for ( i=0 ; i<3 ; ++i ) 
        put( i, vec[i]*coef );
}

double arlCore::vnl_rotation3d_vector::getTheta( void )
{
    if(get(0)!=0) return atan(get(1)/get(0));
    std::cerr<<"x nul : Theta indéterminé";
    return 0;
}

double arlCore::vnl_rotation3d_vector::getPhi( void )
{
    double r=sqrt(get(0)*get(0)+get(1)*get(1)+get(2)*get(2));
    if(r!=0) return acos(get(2)/r);
    std::cerr<<"Le vecteur rotation est de norme nulle : Phi indéterminé";
    return 0;
}

void arlCore::vnl_rotation3d_vector::add_gaussian_noise( double std )
{
    //TODO Appliquer au vecteur rotation un bruit gaussien d'écart type std
    // sur la direction et sur l'angle(norme)
}

bool arlCore::vnl_rotation3d_vector::copy_in ( const vnl_rotation3d_matrix& rm )
{   // Rotation Matrix -> Rotation Vector
    int i,k;
    double d, c, sq, theta, fact, sin_r[3];
    c = 0.5 * (rm[0][0] + rm[1][1] + rm[2][2] - 1.0); // cos(theta)
    d = fabs(c);
    if(d>1.0) c /= d;
    theta = acos(c);
    if(theta<0.0) return false;
    // E_ERRN( theta < 0.0, E_ERRNULL,"RM2RV","negative theta");
    if(fabs(theta) <= 1e-5)
    {   // R close to identity : DL around theta = 0
        fact = 0.5 + theta*theta/12.0;
        (*this)[0] = (rm[2][1] - rm[1][2]) * fact;
        (*this)[1] = (rm[0][2] - rm[2][0]) * fact;
        (*this)[2] = (rm[1][0] - rm[0][1]) * fact;
    } 
    else 
        if(fabs(theta - vnl_math::pi) <= 1e-5) 
        {   // R is close to a symetry : DL around theta = Pi
            // set values with diag terms of n.nt
            for( i=0 ; i<3 ; ++i )
            {   // approx order 0
                // c = (rm[i][i] - 1.0)*0.5;
                sq=1.0+(rm[i][i]-1.0)/(1-c);
                // 0 <=  [ sq = (n[i])^2 ] <= 1
                if(sq<0.0) sq=0.0;
                if(sq>1.0) sq=1.0;
                (*this)[i]=sqrt(sq);
            }
            d = sqrt((*this)[0]*(*this)[0] + (*this)[1]*(*this)[1] +(*this)[2]*(*this)[2]); // K_norm((*this),3);
            if(d==0) return false;
            // K_mulscalvect(theta/d,(*this),(*this),3);
            (*this)[0] *= theta/d;
            (*this)[1] *= theta/d;
            (*this)[2] *= theta/d;
            if((*this)[0]<0.0) return false;
            // E_ERRN( (*this)[0] < 0.0, E_ERRNULL,"RM2RV","negative square root");
            // set signs with off-diag terms of n.nt
            if((rm[0][1] + rm[1][0]) < 0.0) (*this)[1] = -(*this)[1];
            if((rm[0][2] + rm[2][0]) < 0.0) (*this)[2] = -(*this)[2];
            // determine wether r = +/- theta*n
            sin_r[0] = (rm[2][1] - rm[1][2]);
            sin_r[1] = (rm[0][2] - rm[2][0]);
            sin_r[2] = (rm[1][0] - rm[0][1]);
            // determine the most significant term
            k=0;
            if( fabs(sin_r[1]) > fabs(sin_r[k])) k=1;
            if( fabs(sin_r[2]) > fabs(sin_r[k])) k=2;
            // choose the sign
            if( sin_r[k] * (*this)[k] < 0.0 )
                // They do not have the same signs : r = -(*this)
                for( k=0;k<3;++k)
                    (*this)[k] = -(*this)[k];
        }
        else 
        {   // At last no numerical problem
            fact = 0.5 * theta / sin(theta);
            (*this)[0] = (rm[2][1] - rm[1][2]) * fact;
            (*this)[1] = (rm[0][2] - rm[2][0]) * fact;
            (*this)[2] = (rm[1][0] - rm[0][1]) * fact;
        }
    return true;
}
