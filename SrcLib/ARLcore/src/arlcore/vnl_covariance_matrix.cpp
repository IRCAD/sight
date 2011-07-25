/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/vnl_covariance_matrix.h>

arlCore::vnl_covariance_matrix::vnl_covariance_matrix( unsigned int n ):
vnl_matrix<double>(n,n)
{}

arlCore::vnl_covariance_matrix::vnl_covariance_matrix( void ):
vnl_matrix<double>(0,0)
{}

arlCore::vnl_covariance_matrix::vnl_covariance_matrix( const vnl_covariance_matrix& rv )
{
    copy_in(rv);
}

arlCore::vnl_covariance_matrix::~vnl_covariance_matrix( void )
{}

arlCore::vnl_covariance_matrix& arlCore::vnl_covariance_matrix::operator=( const arlCore::vnl_covariance_matrix& rv )
{
    copy_in(rv);
    return *this;
}

void arlCore::vnl_covariance_matrix::copy_in( const vnl_covariance_matrix& rv )
{
    if(this==&rv) return;
    unsigned int i,j;
    for( i=0 ; i<rv.rows() ; ++i )
        for( j=0 ; j<rv.columns() ; ++j )
            put(i,j,rv.get(i,j));
}

bool arlCore::vnl_covariance_matrix::errorEllipse( double &a, double &b, double &theta ) const
{   // http://ieeexplore.ieee.org/iel5/7695/21048/00977046.pdf
    a=0,b=0,theta=0;
    if(is_zero()) return true;
    if(size()!=4) return false;
    double sigma1P2 = get(0,0);
    double sigma2P2 = get(1,1);
    double sigma2 = sqrt(sigma2P2);
    double sigma12P2 = get(0,1);
    double sigma12 = sqrt(sigma12P2);
    double sigma2P3  = sigma2P2*sigma2;
    double x0 = sigma1P2-sigma2P2;
    double x1 = 2*(sigma1P2*sigma2P2-sigma12P2);
    double x2 = sigma1P2 + sigma2P2;
    double x3 = sqrt(x0*x0+4*sigma12P2);
    //! todo Fix cases of division by zero
    a = sqrt(x1/(x2-x3));
    b = sqrt(x1/(x2+x3));
    theta = 0.5*atan((2*sigma12)/(sigma1P2-sigma2P3));
    return true;
}
