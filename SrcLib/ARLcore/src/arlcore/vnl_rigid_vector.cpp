/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/vnl_rigid_vector.h>

#include <arlcore/vnl_rotation3d_vector.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Misc.h>

arlCore::vnl_rigid_vector::vnl_rigid_vector( void ):
vnl_vector_fixed<double,6>()
{}

arlCore::vnl_rigid_vector::vnl_rigid_vector( const vnl_rigid_vector& rv ):
vnl_vector_fixed<double,6>()
{
    copy_in(rv);
}

arlCore::vnl_rigid_vector::vnl_rigid_vector( const vnl_vector<double>& v ):
vnl_vector_fixed<double,6>()
{
    copy_in(v);
}

arlCore::vnl_rigid_vector::vnl_rigid_vector( const vnl_rigid_matrix& rm ):
vnl_vector_fixed<double,6>()
{
    copy_in(rm);
}

arlCore::vnl_rigid_vector::~vnl_rigid_vector( void )
{}

arlCore::vnl_rigid_vector& arlCore::vnl_rigid_vector::operator=( const vnl_vector<double>& v )
{
    copy_in(v);
    return *this;
}

arlCore::vnl_rigid_vector& arlCore::vnl_rigid_vector::operator=( const vnl_rigid_vector& rv )
{
    copy_in(rv);
    return *this;
}

arlCore::vnl_rigid_vector& arlCore::vnl_rigid_vector::operator=( const vnl_rigid_matrix& rm )
{
    copy_in(rm);
    return *this;
}

void arlCore::vnl_rigid_vector::copy_in( const vnl_rigid_vector& rv )
{
    if(this==&rv) return;
    unsigned int i;
    for( i=0 ; i<6 ; ++i )
        put(i,rv(i));
}

void arlCore::vnl_rigid_vector::copy_in( const vnl_rigid_matrix& rm )
{
    vnl_rotation3d_matrix rotationM = rm.getRotation();
    vnl_rotation3d_vector rotationV( rotationM );
    vnl_vector_fixed<double,3> translation = rm.getTranslation();
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        put(i,rotationV.get(i));
    for( i=0 ; i<3 ; ++i )
        put(i+3,translation.get(i));
}

void arlCore::vnl_rigid_vector::copy_in( const vnl_vector<double>& v )
{
    assert(v.size()==6);
    unsigned int i;
    for( i=0 ; i<6 ; ++i )
        if(i<v.size()) put(i,v.get(i));
        else put(i,0.0);
}

arlCore::vnl_rotation3d_vector arlCore::vnl_rigid_vector::getRotation3dVector( void ) const
{
    vnl_rotation3d_vector rv;
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        rv.put(i,get(i));
    return rv;
}

vnl_vector_fixed<double,3> arlCore::vnl_rigid_vector::getTranslation( void ) const
{
    vnl_vector_fixed<double,3> translation;
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        translation.put(i,get(i+3));
    return translation;
}

void arlCore::vnl_rigid_vector::addGaussianNoise( double std )
{
    unsigned int i;
    for( i=0 ; i<6 ; ++i )
        (*this)[i] += arlRandom::Random::gaussianRnd(std);
}
