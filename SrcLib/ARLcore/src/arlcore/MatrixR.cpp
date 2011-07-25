/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/MatrixR.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <utility>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_trace.h>

#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_conjugate_gradient.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#ifdef ANN
#include <ANN/ANN.h>
#endif // ANN

#include <arlcore/PointsList.h>
#include <arlcore/Misc.h>
#include <arlcore/Optimization.h>
#include <arlcore/vnl_rigid_vector.h>
#include <arlcore/vnl_rotation3d_vector.h>

arlCore::vnl_rigid_matrix::vnl_rigid_matrix( void ):
vnl_matrix_fixed<double,4,4>(),
Object(ARLCORE_CLASS_MATRIXR,"Unknown Matrix")
{
    setIdentity();
    setTime(0, 0);
    setOK(true); // FIXME
}

arlCore::vnl_rigid_matrix::vnl_rigid_matrix( const vnl_rigid_matrix& T ):
vnl_matrix_fixed<double,4,4>(),
Object(T)
{
    copy(T);
}

arlCore::vnl_rigid_matrix::vnl_rigid_matrix( const vnl_rigid_vector& T ):
vnl_matrix_fixed<double,4,4>(),
Object(ARLCORE_CLASS_MATRIXR,"Unknown Matrix")
{
    copy(T);
}

arlCore::vnl_rigid_matrix::vnl_rigid_matrix( const vnl_matrix_fixed<double,4,4>& T ):
vnl_matrix_fixed<double,4,4>(),
Object(ARLCORE_CLASS_MATRIXR,"Unknown Matrix")
{
    copy(T);
}

arlCore::vnl_rigid_matrix::vnl_rigid_matrix( const vnl_rotation3d_matrix &r, const vnl_vector_fixed<double,3> &t):
vnl_matrix_fixed<double,4,4>(),
Object(ARLCORE_CLASS_MATRIXR,"Unknown Matrix"),
m_registerRMS(-1.0),
m_registerStdDev(0.0)
{
    setTranslation( t );
    setRotation( r );
}

arlCore::vnl_rigid_matrix::vnl_rigid_matrix( const std::string &fileName ):
vnl_matrix_fixed<double,4,4>(),
Object(ARLCORE_CLASS_MATRIXR,"Unknown Matrix")
{
    setIdentity();
    setTime(0, 0);
    load(fileName);
}

arlCore::vnl_rigid_matrix::~vnl_rigid_matrix( void )
{}

bool arlCore::vnl_rigid_matrix::isValid( void ) const
{   //TODO : Complete the test
    if(get(0,3)!=0.0 || get(1,3)!=0.0 || get(2,3)!=0 || get(3,3)!=1.0) return false;
    return getRotation().is_rotation();
}

double arlCore::vnl_rigid_matrix::GaussianNoiseTranslation( double Xnoise, double Ynoise, double Znoise )
{
    if( Xnoise==0 && Ynoise==0 && Znoise==0 ) return 0.0;
    unsigned int i;
    std::vector< double > delta(3);
    delta[0] = arlRandom::Random::gaussianRnd( Xnoise );
    delta[1] = arlRandom::Random::gaussianRnd( Ynoise );
    delta[2] = arlRandom::Random::gaussianRnd( Znoise );
    for( i=0 ; i<3 ; ++i )
        (*this)[i][3] += delta[i];
    return sqrt(delta[0]*delta[0]+delta[1]*delta[1]+delta[2]*delta[2]);
}

double arlCore::vnl_rigid_matrix::GaussianNoiseRotation( double Xnoise, double Ynoise, double Znoise )
{
    if( Xnoise==0 && Ynoise==0 && Znoise==0 ) return 0.0;
    const vnl_quaternion<double> q0 = getQuaternion();
    vnl_vector_fixed<double,3> euler = q0.rotation_euler_angles();
    euler[0] += arlRandom::Random::gaussianRnd( Xnoise )*arlCore::DegToRad;
    euler[1] += arlRandom::Random::gaussianRnd( Ynoise )*arlCore::DegToRad;
    euler[2] += arlRandom::Random::gaussianRnd( Znoise )*arlCore::DegToRad;
    const vnl_quaternion<double> q1( euler[0], euler[1], euler[2] );
    setQuaternion( q1 );
    return fabs( (q1.angle()- q0.angle())*arlCore::RadToDeg );
}

double arlCore::vnl_rigid_matrix::distance( const vnl_rigid_matrix &T, ARLCORE_VRM_DISTANCE_METHOD method ) const
{
    const double Dist2 = distance2(T, method);
    if(Dist2<0.0) return -1;
    else return sqrt(Dist2);
}

double arlCore::vnl_rigid_matrix::distance2( const vnl_rigid_matrix &T, ARLCORE_VRM_DISTANCE_METHOD method ) const
{
    std::vector<double> errors2;
    return distance2(T, method, errors2);
}

double arlCore::vnl_rigid_matrix::distance2( const vnl_rigid_matrix &T, ARLCORE_VRM_DISTANCE_METHOD method, std::vector<double> &errors2 ) const
{
    errors2.clear();
    const double Unit = 1.0;
    double distance2 = 0.0;
    unsigned int i;
    arlCore::Point o1(0.0, 0.0, 0.0);
    arlCore::Point o2(0.0, 0.0, 0.0);
    arlCore::Point u1(Unit, 0.0, 0.0);
    arlCore::Point u2(Unit, 0.0, 0.0);
    arlCore::Point v1(0.0, Unit, 0.0);
    arlCore::Point v2(0.0, Unit, 0.0);
    arlCore::Point w1(0.0, 0.0, Unit);
    arlCore::Point w2(0.0, 0.0, Unit);
    T.trf(o1);T.trf(u1);T.trf(v1);T.trf(w1);
    trf(o2);trf(u2);trf(v2);trf(w2);
    switch(method)
    {
    case ARLCORE_VRM_DISTANCE_3AXIS:
        {
            errors2.push_back(u1.distance2(u2));
            errors2.push_back(v1.distance2(v2));
            errors2.push_back(w1.distance2(w2));
            break;
        }
    case ARLCORE_VRM_DISTANCE_TRANSLATION:
        {
            errors2.push_back(o1.distance2(o2));
            break;
        }
    case ARLCORE_VRM_DISTANCE_OX:
        {
            errors2.push_back(o1.distance2(o2));
            errors2.push_back(u1.distance2(u2));
            break;
        }
    case ARLCORE_VRM_DISTANCE_OY:
        {
            errors2.push_back(o1.distance2(o2));
            errors2.push_back(v1.distance2(v2));
            break;
        }
    case ARLCORE_VRM_DISTANCE_OZ:
        {
            errors2.push_back(o1.distance2(o2));
            errors2.push_back(w1.distance2(w2));
            break;
        }
    }
    for( i=0 ; i<errors2.size() ; ++i )
        distance2+=errors2[i];
    if(errors2.size()>0) return distance2/(double)errors2.size();
    else return -1;
}

void arlCore::vnl_rigid_matrix::compare( const vnl_rigid_matrix &T, double &errorTranslation, double &errorRotation ) const
{
    vnl_rigid_matrix TPrim;
    TPrim.invert(T);
    compareInverse(TPrim, errorTranslation, errorRotation);
}

void arlCore::vnl_rigid_matrix::compareInverse( const vnl_rigid_matrix &T, double &errorTranslation, double &errorRotation ) const
{
    vnl_rigid_matrix delta1, delta2;
    delta1.mult(T, *this);
    delta2.mult(*this, T);
    errorTranslation = (delta1.getTranslation().two_norm() + delta2.getTranslation().two_norm())/2.0;

    const unsigned int RotMethod = 2;
    if( RotMethod==1 )
    {   // Angle of quaternion
        vnl_quaternion<double> q( delta1.extract(3,3) );
        errorRotation = q.angle()*arlCore::RadToDeg;
    }
    if( RotMethod==2 )
    {   // arcos( (trace(A*inv(B))-1)/2 )
        const double Cosinus = ( vnl_trace( delta1.getRotation() )-1.0 )/2.0;
        if( fabs(Cosinus) <= 1.0 )
            errorRotation = acos(Cosinus)*arlCore::RadToDeg;
        else errorRotation = 0.0;
    }
}

void arlCore::vnl_rigid_matrix::uniform_random( double size )
{
    vnl_rotation3d_vector rot;
    rot.uniform_random();
    setRotation(vnl_rotation3d_matrix(rot));
    put(0,3,arlRandom::Random::uniformDoubleRnd( -size/2, size/2 ));
    put(1,3,arlRandom::Random::uniformDoubleRnd( -size/2, size/2 ));
    put(2,3,arlRandom::Random::uniformDoubleRnd( -size/2, size/2 ));
    Object::update();
}

bool arlCore::vnl_rigid_matrix::setTranslation( const vnl_vector_fixed<double,3> &v )
{
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        put(i,3,v(i) );
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::setTranslation( double x, double y, double z )
{
    put(0,3,x);
    put(1,3,y);
    put(2,3,z);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::setX( double x )
{
    put(0,3,x);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::setY( double y )
{
    put(1,3,y);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::setZ( double z )
{
    put(2,3,z);
    Object::update();
    return true;
}

double arlCore::vnl_rigid_matrix::getX( void ) const
{
    return get(0,3);
}

double arlCore::vnl_rigid_matrix::getY( void ) const
{
    return get(1,3);
}

double arlCore::vnl_rigid_matrix::getZ( void ) const
{
    return get(2,3);
}

vnl_vector_fixed<double,3> arlCore::vnl_rigid_matrix::getTranslation( void ) const
{
    vnl_vector_fixed<double,3> V;
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        V.put(i,get(i,3));
    return V;
}

vnl_vector_fixed<double,4> arlCore::vnl_rigid_matrix::getHTranslation( void ) const
{
    vnl_vector_fixed<double,4> V;
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        V.put(i,get(i,3));
    V.put(3,1.0);
    return V;
}
bool arlCore::vnl_rigid_matrix::setRotation( const vnl_rotation3d_matrix &v )
{
    vnl_matrix_fixed<double,4,4>::update(v);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::setQuaternion( const vnl_quaternion<double> &q )
{   // See http://public.kitware.com/vxl/doc/release/core/vnl/html/classvnl__quaternion.html#vnl__quaterniona4
    vnl_matrix_fixed<double,4,4>::update(q.rotation_matrix_transpose().transpose());
    Object::update();
    return true;
}

vnl_quaternion<double> arlCore::vnl_rigid_matrix::getQuaternion( void ) const
{   // See http://public.kitware.com/vxl/doc/release/core/vnl/html/classvnl__quaternion.html#vnl__quaterniona4
    return vnl_quaternion<double>(extract(3,3).transpose());
}

arlCore::vnl_rotation3d_matrix arlCore::vnl_rigid_matrix::getRotation( void ) const
{
    vnl_matrix_fixed<double,3,3>M = extract(3,3);
    return M;
}

double arlCore::vnl_rigid_matrix::getRMS( void ) const
{
    return m_registerRMS;
}

double arlCore::vnl_rigid_matrix::getStdDev( void ) const
{
    return m_registerStdDev;
}

void arlCore::vnl_rigid_matrix::setRMS( double RMS )
{
    m_registerRMS = RMS;
}

void arlCore::vnl_rigid_matrix::setStdDev( double stdDev )
{
    m_registerStdDev = stdDev;
}

std::string arlCore::vnl_rigid_matrix::getString( void ) const
{
    std::stringstream s;
    unsigned int i, j;
    if(getDate()!=0 && getTime()!=0)
        s<<"Date:"<<getDate()<<" Time:"<<getTime()<<"\n";
    s<<"RMS:"<<m_registerRMS<<" stddev:"<<m_registerStdDev<<"\n";
    for( i=0 ; i<4 ; ++i )
    {
        for( j=0 ; j<4 ; ++j )
            s<<get(i,j)<<" ";
        s<<"\n";
    }
    return s.str();
}

std::string arlCore::vnl_rigid_matrix::getText( void ) const
{
    std::stringstream s;
    unsigned int i, j;
    s<<"("<<getDate()<<"-"<<getTime()<<")RMS:"<<m_registerRMS<<"-EC:"<<m_registerStdDev<<"\\n";
    for( i=0 ; i<4 ; ++i )
    {
        for( j=0 ; j<4 ; ++j )
            s<<get(i,j)<<" ";
        s<<"\\n";
    }
    return s.str();
}

bool arlCore::vnl_rigid_matrix::save( const std::string &fileName, bool overwrite ) const
{
    if(!isOK()) return false;
    std::fstream f;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    f.open (fileName.c_str(), std::fstream::out);
    if(!f.is_open()) return false;
    f<<"FileType RigidMatrix\n";
    f<<"Version 1.0\n";
    bool b=save(f);
    f.close();
    f.flush();
    return b;
}

bool arlCore::vnl_rigid_matrix::save( std::fstream &f ) const
{
    if(!f.is_open()) return false;
    unsigned int i,j;
    f<<"Date "<<getDate()<<"\nTime "<<getTime()<<"\nRMS "<<m_registerRMS<<"\nEC "<<m_registerStdDev<<"\n";
    f<<"Matrix\n";
    for( i=0 ; i<4 ; ++i )
    {
        for( j=0 ; j<4 ; ++j )
            f<<std::setprecision(10)<<(double)get(i,j)<<"\t";
        f<<"\n";
    }
    f<<"End\n";
    f.flush();
    return true;
}

bool arlCore::vnl_rigid_matrix::load( const std::string &fileName )
{
    if(fileName=="") return false;
    std::fstream f;
    std::string token, type;
    unsigned int n=0;
    double version;
    f.open (fileName.c_str(), std::fstream::in);
    bool matrixList = false;
    if(!f.is_open()) return false;
    do
    {   // Reading header
        f>>token;
        if(token=="FileType")
        {
            f>>type;
            matrixList = (type=="MatrixList");
            if(type!="Matrix" && type!="RigidMatrix" && !matrixList) // Matrix deprecated
            {
                std::cerr<<type<<" : Filetype not expected (RigidMatrix or MatrixList)\n";
                f.close();
                return false;
            }
            n=(n|1);
        }
        if(token=="Version") {f>>version; n=(n|2);}
        if(f.eof())
        {
            std::cerr<<"Matrix Loading Header error : "<<fileName<<"\n";
            f.close();
            return false;
        }
    }while(n!=3);
    bool b = false;
    if(matrixList)
    {
        std::vector<arlCore::vnl_rigid_matrix> listMatrix;
        while(load(f)) listMatrix.push_back(*this);
        bool b = mean(listMatrix);
        setOK(b);
        return b;
    }else b=load(f);
    f.close();
    return b;
}

bool arlCore::vnl_rigid_matrix::load( std::fstream &f )
{
    unsigned int i,j;
    double val;
    bool b=false;
    std::string token;
    if(!f.is_open()) return false;
    do
    {
        f>>token;
        if(token=="Date") f>>m_date;
        if(token=="Time") f>>m_time;
        if(token=="RMS") f>>m_registerRMS;
        if(token=="EC") f>>m_registerStdDev;
        if(token=="Matrix")
        {
            for( i=0 ; i<4 ; ++i )
                for( j=0 ; j<4 ; ++j )
                {
                    f>>val;
                    put(i,j,val);
                }
            b=true;
        }
    }while(!f.eof() && token!="End");
    Object::update();
    return setOK(b);
}

std::string arlCore::vnl_rigid_matrix::serialize( void ) const
{
    std::stringstream s;
    s<<Object::serialize();
    unsigned int i,j;
    for( i=0 ; i<4 ; ++i )
        for( j=0 ; j<4 ; ++j )
            s<<get(i,j)<<" ";
    s<<m_registerRMS<<" "<<m_registerStdDev<<" ";
    return s.str();
}

bool arlCore::vnl_rigid_matrix::unserialize( std::stringstream &s )
{
    Object::unserialize(s);
    unsigned int i,j;
    double v;
    for( i=0 ; i<4 ; ++i )
        for( j=0 ; j<4 ; ++j )
        {
            s>>v;
            put(i,j,v);
        }
    s>>m_registerRMS>>m_registerStdDev;
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::copy( const vnl_rigid_matrix& T )
{
    unsigned int i, j;
    for( i=0 ; i<4 ; ++i )
        for( j=0 ; j<4 ; ++j )
            put(i,j,T(i,j));
    m_registerRMS=T.m_registerRMS;
    m_registerStdDev=T.m_registerStdDev;
    m_date=T.m_date;
    m_time=T.m_time;
    Object::update();
    //setUpdateIndex(T.getUpdateIndex());
    return setOK(T.isOK());
}

bool arlCore::vnl_rigid_matrix::copy( const vnl_rigid_vector& T )
{
    setTranslation(T.getTranslation());
    setRotation(T.getRotation3dVector());
    set_row(3,0.0);
    put(3,3,1.0);
    Object::update();
    return setOK(true);
}

bool arlCore::vnl_rigid_matrix::copy( const vnl_matrix_fixed<double,4,4>& T )
{
    unsigned int i, j;
    for( i=0 ; i<4 ; ++i )
        for( j=0 ; j<4 ; ++j )
            put(i,j,T(i,j));
/*  m_registerRMS=0.0;
    m_registerStdDev=0.0;
    m_date=0;
    m_time=0;*/
    Object::update();
    return setOK(true);
}

arlCore::vnl_rigid_matrix& arlCore::vnl_rigid_matrix::operator=( const vnl_rigid_matrix& T )
{
    if(this==&T) return *this;
    arlCore::Object *a=this;
    const arlCore::Object *b=&T;
    *a = *b;
    copy(T);
    return *this;
}

arlCore::vnl_rigid_matrix& arlCore::vnl_rigid_matrix::operator=( const vnl_rigid_vector& T )
{
    copy(T);
    return *this;
}

arlCore::vnl_rigid_matrix& arlCore::vnl_rigid_matrix::operator=( const vnl_matrix_fixed<double,4,4>& T )
{
    if(this==&T) return *this;
    copy(T);
    return *this;
}

bool arlCore::vnl_rigid_matrix::mult( const vnl_rigid_matrix& T1, const vnl_rigid_matrix& T2 )
{
    //vnl_matrix_fixed<double,4,4> M = T1 * T2;
    copy(T1 * T2);
    // timestamp with the older age
    setMinTime(T1.getDate(), T1.getTime());
    setMinTime(T2.getDate(), T2.getTime());
    return true;
}

bool arlCore::vnl_rigid_matrix::setIdentity()
{
    set_identity();
    m_registerRMS=0.0;
    m_registerStdDev=0.0;
//  m_date=0;
//  m_time=0;
    Object::update();
    return true;
}

arlCore::vnl_rigid_matrix arlCore::vnl_rigid_matrix::computeInverse( void ) const
{
    arlCore::vnl_rigid_matrix T;
    T.invert(*this);
    return T;
}

bool arlCore::vnl_rigid_matrix::invert( const vnl_rigid_matrix& T )
{   // Transpose(Rotation)*Translation
    unsigned int i,j;
    vnl_matrix_fixed<double,3,3> tmp;
    vnl_matrix_fixed<double,3,1> col, col2;
    setTime(T.m_date, T.m_time);
    m_registerRMS=T.m_registerRMS;
    m_registerStdDev=T.m_registerStdDev;
    for( i=0 ; i<3 ; ++i )
        for( j=0 ; j<3 ; ++j )
            tmp(i,j) = T(i,j);
    for( i=0 ; i<3 ; ++i )
        col(i,0) = T(i,3);
    tmp.inplace_transpose();
    col2 =  tmp.as_matrix() * col.as_matrix();
    for( i=0 ; i<3 ; ++i )
        for( j=0 ; j<3 ; ++j )
            put(i,j,tmp(i,j));
    for(i=0;i<3;i++)
    {
        put(i,3,-1* col2(i,0));
        put(3,i,0.0);
    }
    put(3,3,1.0);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::invert( void )
{   // Transpose(Rotation)*Translation
    unsigned int i, j;
    vnl_matrix_fixed<double,3,3> tmp;
    vnl_matrix_fixed<double,3,1> col, col2;
    for( i=0 ; i<3 ; i++ )
        for( j=0 ; j<3 ; j++ )
            tmp(i,j) = (*this)(i,j);
    for( i=0 ; i<3 ; i++ )
        col(i,0) = (*this)(i,3);
    tmp.inplace_transpose();
    col2 =  tmp.as_matrix() * col.as_matrix();
    for( i=0 ; i<3 ; i++ )
        for( j=0 ; j<3 ; j++ )
            put(i,j,tmp(i,j));
    for( i=0 ; i<3 ; i++ )
    {
        put(i,3,-1* col2(i,0));
        put(3,i,0.0);
    }
    put(3,3,1.0);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::mean( const std::vector<arlCore::vnl_rigid_matrix> &list )
{
    unsigned int i, n;
    std::vector<const arlCore::vnl_rigid_matrix*> listp;
    for( i=0 ; i<list.size() ; ++i )
        listp.push_back(&list[i]);
    n = filter( listp , ARLCORE_TRF_FILTER_CONSTANT, *this );
    return n>0;
/*
    * @brief The current matrix becomes the mean of listMatrix 1/N sum mat
    * since the resulting 3x3 rotation part is no more a rotation, we apply the 
    * closest_rotation function to the rotation part to ensure it is a rotation
    * TODO average with rotation vector
    * TODO average with the function in averageRotation class in Optimisation.cpp   

    if(list.size()==0) return false;
    fill(0.0);
    unsigned int i, j, k;
    double a;
    for( i=0 ; i<list.size() ; ++i )
    {
        a=0;
        for( j=0 ; j<4 ; ++j )
            for( k=0 ; k<4 ; ++k )
                put( j, k, get(j,k)+list[i].get(j,k) );
    }
    for( j=0 ; j<4 ; ++j )
        for( k=0 ; k<4 ; ++k )
            put( j, k, get(j,k)/i );
            
    vnl_rotation3d_matrix rot = this->getRotation();
    rot.closest_rotation();
    this->setRotation(rot);
    return true;*/
}

double arlCore::vnl_rigid_matrix::getAngleOXY( void )
{
    const vgl_point_3d<double> o( 0.0, 0.0, 0.0 );
    const vgl_point_3d<double> u( 1.0, 0.0, 0.0 );
    const vgl_point_3d<double> v( 0.0, 1.0, 0.0 );
    const vgl_plane_3d<double> Plane1( o, u, v );
    arlCore::Point o1( 0.0, 0.0, 0.0 );
    arlCore::Point u1( 1.0, 0.0, 0.0 );
    arlCore::Point v1( 0.0, 1.0, 0.0 );
    trf(o1);trf(u1);trf(v1);
    const vgl_point_3d<double> o2(  o1.x(), o1.y(), o1.z() );
    const vgl_point_3d<double> u2(  u1.x(), u1.y(), u1.z() );
    const vgl_point_3d<double> v2(  v1.x(), v1.y(), v1.z() );
    const vgl_plane_3d<double> Plane2( o2, u2, v2 );
    const double Angle = fmod(vgl_homg_operators_3d<double>::plane_plane_angle(Plane1, Plane2), vnl_math::pi/2.0)*arlCore::RadToDeg;
    return Angle;
}

double arlCore::vnl_rigid_matrix::getAngleOXZ( void )
{
    const vgl_point_3d<double> o( 0.0, 0.0, 0.0 );
    const vgl_point_3d<double> u( 1.0, 0.0, 0.0 );
    const vgl_point_3d<double> w( 0.0, 0.0, 1.0 );
    const vgl_plane_3d<double> Plane1( o, u, w );
    arlCore::Point o1( 0.0, 0.0, 0.0 );
    arlCore::Point u1( 1.0, 0.0, 0.0 );
    arlCore::Point w1( 0.0, 0.0, 1.0 );
    trf(o1);trf(u1);trf(w1);
    const vgl_point_3d<double> o2(  o1.x(), o1.y(), o1.z() );
    const vgl_point_3d<double> u2(  u1.x(), u1.y(), u1.z() );
    const vgl_point_3d<double> w2(  w1.x(), w1.y(), w1.z() );
    const vgl_plane_3d<double> Plane2( o2, u2, w2 );
    const double Angle = fmod(vgl_homg_operators_3d<double>::plane_plane_angle(Plane1, Plane2), vnl_math::pi/2.0)*arlCore::RadToDeg;
    return Angle;
}

double arlCore::vnl_rigid_matrix::getAngleOYZ( void )
{
    const vgl_point_3d<double> o( 0.0, 0.0, 0.0 );
    const vgl_point_3d<double> v( 0.0, 1.0, 0.0 );
    const vgl_point_3d<double> w( 0.0, 0.0, 1.0 );
    const vgl_plane_3d<double> Plane1( o, v, w );
    arlCore::Point o1( 0.0, 0.0, 0.0 );
    arlCore::Point v1( 0.0, 1.0, 0.0 );
    arlCore::Point w1( 0.0, 0.0, 1.0 );
    trf(o1);trf(v1);trf(w1);
    const vgl_point_3d<double> o2(  o1.x(), o1.y(), o1.z() );
    const vgl_point_3d<double> v2(  v1.x(), v1.y(), v1.z() );
    const vgl_point_3d<double> w2(  w1.x(), w1.y(), w1.z() );
    const vgl_plane_3d<double> Plane2( o2, v2, w2 );
    const double Angle = fmod(vgl_homg_operators_3d<double>::plane_plane_angle(Plane1, Plane2), vnl_math::pi/2.0)*arlCore::RadToDeg;
    return Angle;
}

bool arlCore::vnl_rigid_matrix::chgPlaneOXZ( const arlCore::Point &O, const arlCore::Point &X, const arlCore::Point &Z )
{
    const arlCore::Point Oo(0.0, 0.0, 0.0);
    const arlCore::Point Xo(1.0, 0.0, 0.0);
    const arlCore::Point Zo(0.0, 0.0, 1.0);
    std::vector<const arlCore::Point*> A, B;
    A.push_back(&Oo);
    A.push_back(&Xo);
    A.push_back(&Zo);
    B.push_back(&O);
    B.push_back(&X);
    B.push_back(&Z);
    return register3D3D( A, B, false);
}

bool arlCore::vnl_rigid_matrix::trf( const Point &pt1, Point &pt2 ) const
{
    assert(pt1.size()==3 && pt2.size()==3);
    vnl_vector_fixed<double,4> p1,p2;
    unsigned int i,j;
    for( i=0 ; i<3 ; ++i )
        p1.put(i,pt1[i]);
    p1.put(3,1.0);
    p2=(*this)*p1;
    for( i=0 ; i<3 ; ++i )
        pt2.set(i,p2.get(i));
    //compute uncertainty of pt2 wrt to pt1 uncertainty and this
    vnl_matrix_fixed<double,3,3> C = this->getRotation() *  pt1.getCovMatrix() * vnl_transpose( this->getRotation() );
    arlCore::vnl_covariance_matrix &cov_mat = pt2.getCovMatrix();
    for( i=0 ; i<3 ; ++i )
        for( j=0 ; j<3 ; ++j )
            cov_mat.put(i,j,C[i][j]);
    return true;    
}

bool arlCore::vnl_rigid_matrix::trf( Point &pt ) const
{
    assert(pt.size()==3);
    vnl_vector_fixed<double,4> p1,p2;
    unsigned int i,j;
    for( i=0 ; i<3 ; ++i )
        p1.put(i,pt[i]);
    p1.put(3,1.0);
    p2=(*this)*p1;
    for( i=0 ; i<3 ; ++i )
        pt.set(i,p2.get(i));
        
    //compute uncertainty of pt2 wrt to pt1 uncertainty and this
    vnl_matrix_fixed<double,3,3> res;
    res = this->getRotation() *  pt.getCovMatrix() * vnl_transpose( this->getRotation() );
    
    arlCore::vnl_covariance_matrix &cov_mat = pt.getCovMatrix();
    for(i=0;i<3;++i)
        for(j=0;j<3;++j)
            cov_mat.put(i,j,res[i][j]);
        
    return true;    
}

unsigned int arlCore::vnl_rigid_matrix::trf( const PointList &l1, PointList &l2 ) const
{
    l2.clear();
    l2.setDimension(l1.getDimension());
    Point P(l1.getDimension());
    unsigned int i;
    for( i=0 ; i<l1.size() ; ++i )
        if(trf(*l1[i],P))
            l2.push_back(P);
    return l2.size();
}

unsigned int arlCore::vnl_rigid_matrix::trf( PointList &l1 ) const
{
    unsigned int i;
    for( i=0 ; i<l1.size() ; ++i )
        if(l1[i]!=0) trf(*l1[i]);
    return l1.size();
}

unsigned int arlCore::vnl_rigid_matrix::trf( const std::vector< const Point* > &l1, std::vector< const Point* > &l2 ) const
{
    unsigned int i;
    for( i=0 ; i<l1.size() ; ++i )
    {
        Point *p=new Point(*l1[i]);
        if(trf(*l1[i],*p))
            l2.push_back(p);
        else delete p;
    }
    return (unsigned int)l2.size();
}

bool arlCore::vnl_rigid_matrix::trf( const vgl_line_3d_2_points<double> &line1, vgl_line_3d_2_points<double> &line2 ) const
{
    const vgl_point_3d<double> P1Line = line1.point1();
    const vgl_point_3d<double> P2Line = line1.point2();
    Point p1(3), p2(3);
    if(!trf(Point(P1Line.x(), P1Line.y(), P1Line.z()), p1)) return false;
    if(!trf(Point(P2Line.x(), P2Line.y(), P2Line.z()), p2)) return false;
    line2.set(vgl_point_3d<double>(p1.x(), p1.y(), p1.z()), vgl_point_3d<double>(p2.x(), p2.y(), p2.z()));
    return true;
}

bool arlCore::vnl_rigid_matrix::trf( vgl_line_3d_2_points<double> &line ) const
{
    const vgl_point_3d<double> P1Line = line.point1();
    const vgl_point_3d<double> P2Line = line.point2();
    Point p1(3), p2(3);
    if(!trf(Point(P1Line.x(), P1Line.y(), P1Line.z()), p1)) return false;
    if(!trf(Point(P2Line.x(), P2Line.y(), P2Line.z()), p2)) return false;
    line.set(vgl_point_3d<double>(p1.x(), p1.y(), p1.z()), vgl_point_3d<double>(p2.x(), p2.y(), p2.z()));
    return true;
}

bool arlCore::vnl_rigid_matrix::trf( const vgl_line_segment_3d<double> &line1, vgl_line_segment_3d<double> &line2 ) const
{
    const vgl_point_3d<double> P1Line = line1.point1();
    const vgl_point_3d<double> P2Line = line1.point2();
    Point p1(3), p2(3);
    if(!trf(Point(P1Line.x(), P1Line.y(), P1Line.z()), p1)) return false;
    if(!trf(Point(P2Line.x(), P2Line.y(), P2Line.z()), p2)) return false;
    line2.set(vgl_point_3d<double>(p1.x(), p1.y(), p1.z()), vgl_point_3d<double>(p2.x(), p2.y(), p2.z()));
    return true;
}

bool arlCore::vnl_rigid_matrix::trf( vgl_line_segment_3d<double> &line ) const
{
    const vgl_point_3d<double> P1Line = line.point1();
    const vgl_point_3d<double> P2Line = line.point2();
    Point p1(3), p2(3);
    if(!trf(Point(P1Line.x(), P1Line.y(), P1Line.z()), p1)) return false;
    if(!trf(Point(P2Line.x(), P2Line.y(), P2Line.z()), p2)) return false;
    line.set(vgl_point_3d<double>(p1.x(), p1.y(), p1.z()), vgl_point_3d<double>(p2.x(), p2.y(), p2.z()));
    return true;
}

bool arlCore::vnl_rigid_matrix::registerICP( const PointList &pointsListA, const PointList &pointsListB, double &firstRMS, double &lastRMS, unsigned int &iterations, bool justVisible, double RMSMax, unsigned int iterationsMax )
{   //Model (scanner) = pointsListA ; Points cloud (Acquisition) = pointsListB
    const bool Verbose = false;
    firstRMS = -1.0;
    lastRMS = -1.0;
    m_registerRMS = -1.0;
    m_registerStdDev = 0.0;
    iterations = 0;
#ifdef ANN
//  http://csdl2.computer.org/persagen/DLAbsToc.jsp?resourcePath=/dl/trans/tp/&toc=comp/trans/tp/1992/02/i2toc.xml&DOI=10.1109/34.121791
//  http://ieeexplore.ieee.org/iel1/34/3469/00121791.pdf?tp=&arnumber=121791&isnumber=3469
    //  To be continued : Automatic initialization or using the current matrix for initialization
    const unsigned int Dimension = pointsListA.getDimension();
    assert(Dimension==pointsListB.getDimension());
    const int    bs = 1; // Bucket size
    const double tau = 10e-8;
    ANNpointArray modelPoints, dataPoints;
    ANNpointArray Pk,Yk,Pi;
    ANNidxArray     nn_idx; // near neighbor indices
    ANNdistArray    squaredDists; // near neighbor distances
    double eps = 0.0;// Error bound
    unsigned int i,j,k,n;
    unsigned int modelSize;
    if(justVisible) modelSize = pointsListA.visibleSize();
    else modelSize = pointsListA.size();
    if(modelSize<1) return false;
    modelPoints = annAllocPts( modelSize, Dimension );
    for( i=0, n=0 ; i<modelSize ; ++i )
    {
        if(!justVisible || (justVisible && pointsListA[i]->isVisible()))
        {
            for( j=0 ; j<Dimension ; ++j )
                modelPoints[n][j]=pointsListA[i]->get(j);
            ++n;
        }
    }
    unsigned int acquisitionSize;
    if(justVisible) acquisitionSize = pointsListB.visibleSize();
    else acquisitionSize = pointsListB.size();
    if(acquisitionSize<1)
    {
        annDeallocPts( modelPoints );
        return false;
    }
    dataPoints = annAllocPts( acquisitionSize, Dimension );
    for( i=0, n=0 ; i<acquisitionSize ; ++i )
    {
        if(!justVisible || (justVisible && pointsListB[i]->isVisible()))
        {
            for( j=0 ; j<Dimension ; ++j )
                dataPoints[n][j]=pointsListB[i]->get(j);
            ++n;
        }
    }
    const int NbNN = 1;          // number of nearest neighbors
    nn_idx = new ANNidx[NbNN];   // allocate near neigh indices
    squaredDists = new ANNdist[NbNN];    // allocate near neighbor squaredDists
   // ************* build the kdtree ************************
   ANNkd_tree *the_tree = new ANNkd_tree( modelPoints,// data points
                                          modelSize,// number of points
                                          Dimension, // points dimension
                                          bs, // Bucket size
                                          ANN_KD_SL_MIDPT );
    // ************* vectors allocation ************************
    Pk = annAllocPts( acquisitionSize, Dimension );
    Pi = annAllocPts( acquisitionSize, Dimension );
    Yk = annAllocPts( acquisitionSize, Dimension );
    // **************************************************************
    // first transformation =>P0, icp release the matching between
    // P0 and model point
    // **************************************************************
    vnl_vector<double> gravityP(Dimension,0.0);
    vnl_matrix<double> Rot(3,3);
    vnl_vector<double> Tra(3,0.0), TraTemp(3);
    // Initialization
    invert();
    Tra = getTranslation();
    Rot = getRotation();
    vnl_quaternion<double> qr(0,0,0,1);
    //Rot = vnl_transpose(qr.rotation_matrix_transpose()).asMatrix();
    for( i=0 ; i<acquisitionSize ; ++i )
        for( j=0 ; j<3; ++j )
        {
            Pi[i][j] = Pk[i][j] = Rot[j][0]*dataPoints[i][0]+Rot[j][1]*dataPoints[i][1]+Rot[j][2]*dataPoints[i][2]+Tra[j];
            gravityP[j]+= Pi[i][j];
        }
    gravityP/= (double)acquisitionSize;
    // ********************** ICP Initialisation ************************
    // ** P={pi} points de données (Np) et X={xi} points de modèle (Nx) *
    vnl_vector<double> gravityY(Dimension);
    int index = 0;  
    double oldRMS = FLT_MAX;
    firstRMS = -1;
    vnl_matrix<double> Id3(3,3);    Id3.set_identity();
    vnl_matrix<double> Rend(3,3);   Rend.set_identity();
    vnl_matrix<double> Spy(3,3);
    vnl_vector<double> Delta (3);
    vnl_matrix<double> QSpy(4,4);   
    vnl_matrix<double> Ricp (3,3);
    vnl_symmetric_eigensystem<double> eigQSpy(QSpy);
    lastRMS = FLT_MAX/2; // /2;
    // ****************************** ICP algorithm ***************************
    while( fabs(lastRMS-oldRMS)>tau && iterations<iterationsMax ) //&& RMS>RMSMax )
    //while( iterations<iterationsMax ) //&& RMS>RMSMax )
    {
        ++iterations;
        oldRMS = lastRMS;
        lastRMS = 0.0;
        Spy.set_identity();     
        gravityY.fill(0);
        // step 1 :     
        // search the matching point for every data shape point
        for( i=0 ; i<acquisitionSize ; ++i )
          {
            // ****************************************************
            // step 1 : 
            // search the matching point for every data shape point
            // ****************************************************
             the_tree->annkSearch( Pk[i],  // query point
                                   NbNN,      // number of near neighbors
                                   nn_idx, // nearest neighbors (returned)
                                   squaredDists,  // distance (returned)
                                   eps );  // error bound
            // ***************************************************
            // calculate the center of mass for Y and query_point
            // ***************************************************
            for( j = 0; j < 3; j++ )
              gravityY[j]+= Yk[i][j] = modelPoints[nn_idx[0]][j];
            // Update the value of error
            lastRMS += squaredDists[0];
          } //end search matching for       
        // Calculate the new mean square point matching error
        // LG lastRMS/=(double)AcquisitionSize;
        lastRMS = sqrt(lastRMS/(double)acquisitionSize);
        if(firstRMS<0) firstRMS = lastRMS;
        // calculate the center of mass for Y and query_point
        // by divise the sum by the number of point
        gravityY/= (double)acquisitionSize;
        // step 2
        // Calculate the cross-variance matrix between Pk and Yo
        // Cov( P0, Yk ) = 1/n*somme(P0*Yk') - ComP0*ComYk'
        for( i=0 ; i<acquisitionSize ; ++i )
          for( j=0 ; j<3 ; ++j )
            for( k=0 ; k<3 ; ++k )
            Spy[j][k]+= Pi[i][j]*Yk[i][k];
        for( i=0 ; i<3 ; ++i )
          for( j=0 ; j<3 ; ++j )
            Spy[i][j] = Spy[i][j]/(double)acquisitionSize - gravityP[i]*gravityY[j];
        // Delta = [A23 A31 A12] with Amn = Spy[m][n]-Spy[n][m]
        Delta[0] = Spy[1][2]-Spy[2][1];
        Delta[1] = Spy[2][0]-Spy[0][2];
        Delta[2] = Spy[0][1]-Spy[1][0];
        // calculate the symmetric 4x4 matrix needed to determine
        // the max eigenvalue
        QSpy[0][0] = vnl_trace( Spy ); // calculate the matrix trace
        for( i=1 ; i<4 ; ++i )
        {
            QSpy[i][0] = Delta[i-1];
            for( j = 1; j < 4; ++j )
            {
                QSpy[0][j] = Delta[j-1];
                QSpy[i][j] = Spy[i-1][j-1]+Spy[j-1][i-1] - QSpy[0][0]*Id3[i-1][j-1];
            }
        }
        // calculate the eigen value of QSpy
        eigQSpy = vnl_symmetric_eigensystem<double>( QSpy );
        // optimal Rotation matrix calculate from the quaternion
        // vector qr=[q0 q1 q2 q3] obtained by obtained the max eigen value
        // http://paine.wiau.man.ac.uk/pub/doc_vxl/core/vnl/html/vnl__symmetric__eigensystem_8h.html
        qr.update(eigQSpy.get_eigenvector(3));
        qr = vnl_quaternion<double>(qr[1], qr[2], qr[3], qr[0]);
        Ricp = vnl_transpose(qr.rotation_matrix_transpose()).asMatrix();
        // optimal translation vector Ticp T = ComY - Ricp.ComP
        TraTemp = gravityY - Ricp*gravityP;
        // step 3 : Application of the transformation
        for( i=0 ; i<acquisitionSize ; ++i )
          for( j=0 ; j<3 ; ++j )
            Pk[i][j] = Ricp[j][0]*Pi[i][0] + Ricp[j][1]*Pi[i][1] + Ricp[j][2]*Pi[i][2] + TraTemp[j];
    }// while
    m_registerRMS = lastRMS;
    // give the transformation from model data ==> acquisition data
    // first we add the first transformation to Rend
    // ******************************************************
    // ***************** Changement *************************
    // ******************************************************
    Rend = Ricp.transpose();
    vnl_vector<double> Tend(3);
    for( i=0 ; i<3 ; ++i )
        Tend[i] = gravityP[i]-(Rend[i][0]*gravityY[0]+Rend[i][1]*gravityY[1]+Rend[i][2]*gravityY[2]);
    // matrice initiale [Rot Tra;0 0 0 1]
    // matrice trouvee [Rend Tend;0 0 0 1]

    /*this->vnl_matrix_fixed<double,4,4>::update(Rend*Rot);
    this->set_column(3,Rend*Tra + Tend); //,0);
    (*this)[3][3] = 1.0;*/
    this->vnl_matrix_fixed<double,4,4>::update(vnl_inverse(Rot)*Rend);
    this->set_column(3,vnl_inverse(Rot)*(Tend-Tra));
    (*this)[3][3] = 1.0;
    // ******************* free memory ******************
    delete[] nn_idx;
    delete[] squaredDists;
    delete the_tree;
    annDeallocPts( modelPoints );
    annDeallocPts( dataPoints );
    annDeallocPts( Pk );
    annDeallocPts( Yk );
    annDeallocPts( Pi );
    annClose(); 
    if(Verbose)
    {
        std::cout<<"ICP registration\n";
        std::cout<<"firstRMS="<<firstRMS<<" lastRMS="<<lastRMS<<" Iterations="<<iterations<<" PtsModel="<<modelSize<<" PtsAcquisition="<<acquisitionSize<<"\n";
        std::cout<<"Matrix ="<<*this<<"\n";
    }   
    Object::update();
    std::cout<<"Tau="<<fabs(lastRMS-oldRMS)*1e8<<"\n";
    return true;
#else // ANN
    return false;
#endif // ANN
}

/****************************************************************
*
*  Routine Name: rotate_dim4
*  Purpose: Fonction utilisée uniquement par ma fonction spec

*  Remarque : IL FAUT SUPPRIMER CETTE FONCTION ET LA REMPLACER PAR UNE FONCTION 
*  DE LA LIBRAIRIE VNL
*****************************************************************/
void rotate_dim4( double a[][4] , double tau , double s , int i , int j ,int k , int l )
{
   double g = a[i][j]; 
   double h = a[k][l];
   a[i][j] = g-s*(h+g*tau);
   a[k][l] = h + s*(g-h*tau);
   return;
}

/************************************************************************************
*
*  Routine Name: spec
*  Purpose: This function returns the vectors and eigenvalues of a matrix 
*  Input: m            - matrix (double)
*  Output: d,v         - eigenvalues and vector
*  
*  Remarque : IL FAUT SUPPRIMER CETTE FONCTION ET LA REMPLACER PAR UNE FONCTION 
*  DE LA LIBRAIRIE VNL
**************************************************************************************/
void spec( const double m[4][4], double *d, double (*v)[4] )
{
    const unsigned int IT_MAX = 50;
    register int i , j , ip , iq ; 
    double tresh,theta,tau,t,sm,s,h,g,c;
    double a[4][4], b[4], z[4];
    int p = 4;
    for(i=0;i<4; i++)
        for(j=0;j<4; j++)
            if(i==j) v[i][j]=1.0; else v[i][j]=0.0;
   for( ip=0 ; ip<p ; ip++ )
      for( iq = 0 ; iq < p ; iq++ )  
         a[ip][iq] = m[ip][iq];
   for( ip = 0 ; ip < p ; ip++ )
   {
      b[ip] = a[ip][ip];
      d[ip] = a[ip][ip];
      z[ip] = 0.0;
   }
   for( i = 0 ; i < IT_MAX/2 ; i++ )
   {
      sm = 0.0;
      for( ip = 0 ; ip < p-1 ; ip++ )
         for( iq = ip+1 ; iq < p ; iq++ )
            sm += fabs(a[ip][iq]);
      if( sm == 0.0 ) return;
      if( i < 3 )  tresh = 0.2 * sm / (p*p); 
      else  tresh = 0.0;
      for( ip = 0 ; ip < p-1 ; ip++ )
      {
         for( iq = ip+1 ; iq < p ; iq++ )
         {
            g = 100.0 * fabs(a[ip][iq]);
            if(( i > 4 ) && ( fabs(d[ip]) + g == fabs(d[ip]) )
                && ( fabs(d[iq]) + g == fabs(d[iq]) ))
               a[ip][iq] = 0.0;
            else if( fabs(a[ip][iq]) > tresh )
            {
               h = d[iq] - d[ip];
               if( fabs(h) + g == fabs(h) )  t = a[ip][iq]/h;
               else
               {
                  theta = 0.5 * h / a[ip][iq];
                  t     = 1.0 / ( fabs(theta) + sqrt(1.0+theta*theta) );
                  if( theta < 0 )  
                     t = -t;
               }
               c      = 1.0 / sqrt(1.0+t*t);
               s      = t * c;
               tau    = s / (1.0+c);
               h      = t * a[ip][iq];
               z[ip] += - h;   
               z[iq] +=   h;
               d[ip] += - h;
               d[iq] +=   h;
               a[ip][iq] = 0.0;
               for( j = 0 ; j <= ip-1 ; j++ )
                  rotate_dim4( a , tau , s , j , ip , j , iq );
               for( j = ip+1 ; j <= iq-1 ; j++ )
                  rotate_dim4( a , tau , s , ip , j , j , iq );
               for( j = iq+1; j < p ; j++ )
                  rotate_dim4( a , tau , s , ip , j , iq , j );
               for( j = 0 ; j < p ; j++ )
                  rotate_dim4( v , tau , s , j , ip , j , iq );
            }
         }
      }
      for( ip = 0 ; ip < p ; ip++ )
      {
         b[ip] += z[ip];
         d[ip]  = b[ip];
         z[ip]  = 0.0;
      }
   }
   return;
}

bool arlCore::vnl_rigid_matrix::oldRegisterICP( const PointList &pointsListA, const PointList &pointsListB, double &RMS, unsigned int &iterations, double RMSMax, unsigned int iterationsMax )
{
    return false;
#ifdef ANN
//  To be continued : Automatic initialization or using the current matrix for initialization
    vgl_plane_3d< double >planeA, planeB;
    double rms, min, max;
    pointsListA.plane(planeA, rms, min, max);
    pointsListB.plane(planeB, rms, min, max);
    // Recaler planeA vers planeB et s'en servir comme initialisation
/*  unsigned int ii;
    vgl_point_3d< double > pt[3];
    OptimisePlane SLCalibration;
    for( ii=0 ; ii<pointsListB.size() ; ++ii )
    {
        vgl_point_3d< double > vglPt(pointsListA[ii]->x(),pointsListA[ii]->y(),pointsListA[ii]->z());
        SLCalibration.addData( vglPt);
    }
    if(SLCalibration.size()<3) return false;
    SLCalibration.getPoint(0,pt[0]);
    SLCalibration.getPoint(unsigned int(SLCalibration.size()/2),pt[1]);
    SLCalibration.getPoint(SLCalibration.size()-1,pt[2]);
    vnl_vector<double> planSolution(3);
    vgl_plane_3d< double >plane(pt[0],pt[1],pt[2]);
    if(plane.d()==0) // Problem, could we use the four parameters ?
        return false;
    planSolution(0)=plane.a()/plane.d();
    planSolution(1)=plane.b()/plane.d();
    planSolution(2)=plane.c()/plane.d();
    if(SLCalibration.size()>3)
    {
        vnl_powell computeSLCalibration(&SLCalibration);
        computeSLCalibration.minimize(planSolution);
    }
    double a=planSolution(0);
    double b=planSolution(1);
    double c=planSolution(2);
    double d=1.0;//planSolution(3);

    return false;*/
#define  SQR( x ) ((x)*(x))
    const unsigned int dim = 3;
    const double tau = 0.0;
    //Model (scanner) = pointsListA ; Points cloud (Acquisition) = pointsListB
    const PointList& plA = pointsListA;
    const PointList& plB = pointsListB;
/*  // Test with a simple translation
    std::vector< const Point* > plB;
    setIdentity();
    (*this)[0][3]=0.5;
    trf(plA,plB);
*/
    ANNpointArray modelPoints, dataPoints;
    ANNpointArray Pk,Yk,Pi;
    double eps = 0.0;// error bound
    unsigned int i,j,k;
    unsigned int modelSize, acquisitionSize;
    ANNpoint gravityA = annAllocPt( dim, 0.0 );
    modelSize=plA.size();
    modelPoints = annAllocPts( modelSize, dim );
    for( i=0 ; i<modelSize ; ++i )
        //if(plA[i]->size()==dim)
            for( j=0 ; j<dim ; ++j )
                modelPoints[i][j]=plA[i]->get(j);
    acquisitionSize=plB.size();
    dataPoints = annAllocPts( acquisitionSize, dim );
    for( i=0 ; i<acquisitionSize ; ++i )
    {
        if(plB[i]->size()==dim)
            for( j=0 ; j<dim ; ++j )
            {
                dataPoints[i][j]=plB[i]->get(j);
                gravityA[j]+= dataPoints[i][j];
            }
    }
    for( i=0 ; i<dim ; ++i )
        gravityA[i] /= (double)acquisitionSize;
//  // ******************* Time chrono ******************
//  // Calcul du temps d'éxécution du recalage en millisecondes
//  DWORD dwStart = GetTickCount();
    // ************* vectors allocation ************************
    Pk = annAllocPts( acquisitionSize, dim );
    Pi = annAllocPts( acquisitionSize, dim );
    Yk = annAllocPts( acquisitionSize, dim );
    // **************************************************************
    // first transformation =>P0, icp release the matching between
    // P0 and model point
    // **************************************************************
    ANNpoint gravityP = annAllocPt(dim, 0.0);
    // FIXME : Voir probl�me d'initialisation !!!
    vnl_matrix<double> Rot(3,3);
    vnl_vector<double> Tra(3);
    Tra.fill (0.0);
    vnl_vector<double> qr(7);
    qr[0] = 1.0;
    for( i=1 ; i<4 ; ++i )
        qr[i] = 0.0;
    Rot[0][0] = pow(qr[0],2)+pow(qr[1],2)-pow(qr[2],2)-pow(qr[3],2);
    Rot[0][1] = 2*(qr[1]*qr[2]-qr[0]*qr[3]);
    Rot[0][2] = 2*(qr[1]*qr[3]+qr[0]*qr[2]);
    Rot[1][0] = 2*(qr[1]*qr[2]+qr[0]*qr[3]);
    Rot[1][1] = pow(qr[0],2)+pow(qr[2],2)-pow(qr[1],2)-pow(qr[3],2);
    Rot[1][2] = 2*(qr[2]*qr[3]-qr[0]*qr[1]);
    Rot[2][0] = 2*(qr[1]*qr[3]-qr[0]*qr[2]);
    Rot[2][1] = 2*(qr[2]*qr[3]+qr[0]*qr[1]);
    Rot[2][2] = pow(qr[0],2)+pow(qr[3],2)-pow(qr[1],2)-pow(qr[2],2);
    for( i=0 ; i<acquisitionSize ; ++i )
    {  //julie : on applique l'identit� � l'ensemble des points de l'acquisition
        for( j=0 ; j<3; ++j )
        {
            Pi[i][j] = Pk[i][j] = Rot[j][0]*dataPoints[i][0]+Rot[j][1]*dataPoints[i][1]+Rot[j][2]*dataPoints[i][2]+Tra[j];
            gravityP[j]+= Pi[i][j];
        }
    }
    for( i=0 ; i<3; ++i )
        gravityP[i]/= (double)acquisitionSize;
    // ********************** ICP Initialisation ************************
    // ** P={pi} points de donnees (Np) et X={xi} points de modele (Nx) *
    ANNpoint gravityY = annAllocPt(dim, .0);
    int index = 0;
    RMS = FLT_MAX/2; //FLT_MAX defini dans Visual
    double oldRMS = FLT_MAX, traSpy;
    vnl_matrix<double> Id3(3,3);
    Id3.set_identity();
    vnl_matrix<double> Rend(3,3);
    Rend.set_identity();
    vnl_matrix<double> Spy(3,3);
    vnl_vector<double> Delta (3);
    double QSpy[4][4];
    vnl_matrix<double> Ricp (3,3);
    vnl_matrix<double> buffer_mul (3,3);
    iterations = 0;
    // ****************************** ICP algorithm ***************************
    //while( fabs(RMS-oldRMS)>tau && iterations<iterationsMax && RMS>RMSMax )
    while( iterations<iterationsMax && RMS>RMSMax )
    {
        ++iterations;
        double testnorm2, norm2;
        testnorm2 = FLT_MAX;
        oldRMS = RMS;
        RMS = 0.0;
        max = -FLT_MAX;
        Spy.set_identity();
        for( i=0 ; i<3 ; ++i )
            gravityY[i] = 0.0;
        // step 1 :
        // search the matching point for every data shape point
        for( i=0; i<acquisitionSize; ++i )
        {
            for( j=0 ; j<modelSize ; ++j )
            {
                norm2=SQR(Pk[i][0]-modelPoints[j][0])+SQR(Pk[i][1]-modelPoints[j][1])+SQR(Pk[i][2]-modelPoints[j][2]);
                if( norm2 < testnorm2 )
                {
                    testnorm2=norm2;
                    for( k=0 ; k<3 ; ++k )
                        Yk[i][k] = modelPoints[j][k];
/*                  Yk[i][0] = modelPoints[j][0];
                    Yk[i][1] = modelPoints[j][1];
                    Yk[i][2] = modelPoints[j][2];*/
                }
            }
            testnorm2=FLT_MAX;
            for( j=0 ; j<3 ; ++j )
                gravityY[j]+= Yk[i][j];
            RMS+=sqrt(SQR(Pk[i][0]-Yk[i][0])+SQR(Pk[i][1]-Yk[i][1])+SQR(Pk[i][2]-Yk[i][2]));
        } //end search matching for
        // calculate the new mean square point matching error
        RMS/=(double)acquisitionSize;
        // calculate the center of mass for Y and query_point
        // by divise the sum by the number of point
        for( i=0 ; i<3 ; ++i )
            gravityY[i] /= (double)acquisitionSize;
        // step 2
        // Calculate the cross-variance matrix between Pk and Yo
        // Cov( P0, Yk ) = 1/n*somme(P0*Yk') - ComP0*ComYk'
        for( i=0 ; i<acquisitionSize ; ++i )
            for( j=0 ; j<3 ; ++j )
                for( k=0 ; k<3 ; ++k )
                    Spy[j][k]+= Pi[i][j]*Yk[i][k];
/*          Spy[0][0]+= Pi[i][0]*Yk[i][0];
            Spy[0][1]+= Pi[i][0]*Yk[i][1];
            Spy[0][2]+= Pi[i][0]*Yk[i][2];
            Spy[1][0]+= Pi[i][1]*Yk[i][0];
            Spy[1][1]+= Pi[i][1]*Yk[i][1];
            Spy[1][2]+= Pi[i][1]*Yk[i][2];
            Spy[2][0]+= Pi[i][2]*Yk[i][0];
            Spy[2][1]+= Pi[i][2]*Yk[i][1];
            Spy[2][2]+= Pi[i][2]*Yk[i][2];*/
        for( i=0 ; i<3 ; ++i )
            for( j=0 ; j<3 ; ++j )
                Spy[i][j] = Spy[i][j]/(double)acquisitionSize - gravityP[i]*gravityY[j];
        // Delta = [A23 A31 A12] with Amn = Spy[m][n]-Spy[n][m]
        Delta[0] = Spy[1][2]-Spy[2][1];
        Delta[1] = Spy[2][0]-Spy[0][2];
        Delta[2] = Spy[0][1]-Spy[1][0];
        // calculate the matrix trace
        traSpy = Spy[0][0]+Spy[1][1]+Spy[2][2];
        // calculate the symmetric 4x4 matrix needed to determine
        // the max eigenvalue
        QSpy[0][0]=traSpy;
        for( i=1 ; i<4 ; ++i )
        {
            QSpy[i][0] = Delta[i-1];
            for( j=1 ; j<4; ++j )
            {
                QSpy[0][j] = Delta[j-1];
                QSpy[i][j] = Spy[i-1][j-1]+Spy[j-1][i-1]-traSpy*Id3[i-1][j-1];
            }
        }
        // calculate the eigen value of QSpy
        double valeur_propre [4];
        double vecteur_propre [4][4];
        spec( QSpy, valeur_propre, vecteur_propre);
        vnl_vector<double> Valp(4);
        vnl_matrix<double> Vectp(4,4);
        for(i=0;i<4; i++)
            for(j=0;j<4; j++)
            {
                Valp[i]=valeur_propre[i];
                Vectp[i][j]=vecteur_propre[i][j];
            }
        // optimal Rotation matrix calculate from the quaternion
        // vector qr=[q0 q1 q2 q3] obtained by obtained the
        // max eigen value
        for( i = 0; i < 4; i++ )
            if( Valp[i] > max )
            {
                max   = Valp[i];
                index = i;
            }
        for( i=0 ; i<4 ; i++ )
            qr[i] = Vectp[i][index];
        // matrix determinate from the quaternion vector
        Ricp[0][0] = pow(qr[0],2)+pow(qr[1],2)-pow(qr[2],2)-pow(qr[3],2);
        Ricp[0][1] = 2*(qr[1]*qr[2]-qr[0]*qr[3]);
        Ricp[0][2] = 2*(qr[1]*qr[3]+qr[0]*qr[2]);
        Ricp[1][0] = 2*(qr[1]*qr[2]+qr[0]*qr[3]);
        Ricp[1][1] = pow(qr[0],2)+pow(qr[2],2)-pow(qr[1],2)-pow(qr[3],2);
        Ricp[1][2] = 2*(qr[2]*qr[3]-qr[0]*qr[1]);
        Ricp[2][0] = 2*(qr[1]*qr[3]-qr[0]*qr[2]);
        Ricp[2][1] = 2*(qr[2]*qr[3]+qr[0]*qr[1]);
        Ricp[2][2] = pow(qr[0],2)+pow(qr[3],2)-pow(qr[1],2)-pow(qr[2],2);
        // optimal translation vector Ticp T = ComY - Ricp.ComP
        for( i=0 ; i<3 ; ++i )
            qr[4+i] = gravityY[i]-(Ricp[i][0]*gravityP[0]+Ricp[i][1]*gravityP[1]+Ricp[i][2]*gravityP[2]);
        // step 3 : Application the transformation
        for( i=0 ; i<acquisitionSize ; ++i )
            for( j=0 ; j<3 ; ++j )
                Pk[i][j] = Ricp[j][0]*Pi[i][0]+Ricp[j][1]*Pi[i][1]+Ricp[j][2]*Pi[i][2]+qr[j+4];
/*          Pk[i][0] = Ricp[0][0]*Pi[i][0]+Ricp[0][1]*Pi[i][1]+Ricp[0][2]*Pi[i][2]+qr[4];
            Pk[i][1] = Ricp[1][0]*Pi[i][0]+Ricp[1][1]*Pi[i][1]+Ricp[1][2]*Pi[i][2]+qr[5];
            Pk[i][2] = Ricp[2][0]*Pi[i][0]+Ricp[2][1]*Pi[i][1]+Ricp[2][2]*Pi[i][2]+qr[6];*/
    } //end while
    // give the transformation from model data ==> acquisition data
    // first we add the first transformation to Rend
    Rend = Ricp*Rot; // FIXME Voir Origine de Rot !!!
    buffer_mul = Rend.transpose(); // ATTENTION : transpose au lieu de inverse!!!!!!!!!
    for( i=0 ; i<3 ; ++i )
    {
        (*this)[3][i] = 0.0;
        for( j=0 ; j<3 ; ++j )
        {
            (*this)[i][j] = buffer_mul[i][j];
            Rend[i][j] = buffer_mul[i][j];
        }
    }
    // translation vector = gravityA-Rend*gravityY
    for( i=0 ; i<3 ; ++i )
        (*this)[0][3] = gravityA[i]-(Rend[i][0]*gravityY[0]+Rend[i][1]*gravityY[1]+Rend[i][2]*gravityY[2]);
//  (*this)[0][3] = gravityA[0]-(Rend[0][0]*gravityY[0]+Rend[0][1]*gravityY[1]+Rend[0][2]*gravityY[2]);
//  (*this)[1][3] = gravityA[1]-(Rend[1][0]*gravityY[0]+Rend[1][1]*gravityY[1]+Rend[1][2]*gravityY[2]);
//  (*this)[2][3] = gravityA[2]-(Rend[2][0]*gravityY[0]+Rend[2][1]*gravityY[1]+Rend[2][2]*gravityY[2]);
    (*this)[3][3] = 1.0;
    // ******************* Time chrono ******************
//  DWORD dwEnd = GetTickCount();
//  long dif = dwEnd-dwStart;
    // destroy() ne marche pas sur toutes les matrices vnl_matrix
    annDeallocPts( modelPoints );
    annDeallocPts( dataPoints );
    annDeallocPts( Pk );
    annDeallocPts( Yk );
    annDeallocPts( Pi );
    //annDeallocPts( resu );
    annDeallocPt( gravityA );
    //annDeallocPt( gravityP );
    //annDeallocPt( gravityY );
    annClose(); 
    m_registerRMS = RMS;
    std::cout<<"ICP registration\n";
    std::cout<<"RMS="<<RMS<<" Iterations="<<iterations<<" PtsModel="<<modelSize<<" PtsAcquisition="<<acquisitionSize<<"\n";
    std::cout<<"Matrix ="<<*this<<"\n";
    Object::update();
    return true;
#else // ANN
    return false;
#endif // ANN
}

void tensorProduct( const arlCore::Point& a, const arlCore::Point& b, vnl_matrix_fixed<double,3,3> &mat )
{
    unsigned int i, j;
    for( i=0 ; i<3 ; ++i )
        for( j=0 ; j<3 ; ++j )
            mat.put(i,j, a[i]*b[j] );
}

template <typename T>
bool internRegister3D3D( const T &src, const T &dst,  arlCore::vnl_rigid_matrix &mat, bool computeRMS )
{   // Registration only with the visible points ** NEW METHOD **
    //assert(src.size()==dst.size() && src.size()>=3);
    if( src.size()!=dst.size() || src.size()<3 ) return false;
    unsigned int i, j;
    // Arun et al. direct solution technique
    // Compute matrice H and mean vector
    arlCore::Point meanA(0.0, 0.0, 0.0), meanB(0.0, 0.0, 0.0);
    vnl_matrix_fixed<double,3,3> HMat, tempMat;
    HMat.fill(0.0); tempMat.fill(0.0);
    double n=0.0;
    for( i=0 ; i<src.size() ; ++i )
        if(dst[i]!=0 && src[i]!=0)
            if(dst[i]->isVisible() && src[i]->isVisible())
            {
                meanA.add(*dst[i]);
                meanB.add(*src[i]);
                tensorProduct(*dst[i], *src[i], tempMat);
                HMat+=tempMat;
                ++n;
            }
    if(n<3) return false;
    meanA.mult(1/n);
    meanB.mult(1/n);
    // now compute K = sum(Tensors products) / N - tensorProduct( meanA, meanB )
    // Divide H / N
    HMat *= 1.0 / n;
    tensorProduct( meanA, meanB, tempMat );
    // Compute K = sum(Tensors products) / N - tensorProduct( meanA, meanB )
    HMat -= tempMat;
    arlCore::vnl_rotation3d_matrix RMat(HMat);
    // calcul de la rotation la plus proche au sens de la norme de frobenius
    RMat.closest_rotation();
    // Compute translation = meanA - R * meanB
    vnl_vector_fixed<double,3> vector_meanA, vector_meanB, translation;
    for( i=0 ; i<3 ; ++i)
    {
        vector_meanA.put(i,meanA[i]);
        vector_meanB.put(i,meanB[i]);
    }
    translation = vector_meanA - RMat*vector_meanB;
    for( i=0 ; i<3 ; ++i)
    {
        mat[i][3] = translation[i];
        mat[3][i] = 0.0f;
        for( j=0 ; j<3 ; ++j)
            mat.put( i,j, RMat(i,j) );
    }
    mat[3][3] = 1.0f;
    if(computeRMS)
    {
        std::vector<double> errors;
        mat.RMS3D3D( src, dst, errors );
    }
    return true;
}

bool arlCore::vnl_rigid_matrix::register3D3D( const PointList& a, const PointList& b, bool computeRMS )
{
    m_registerRMS = -1;
    m_registerStdDev = -1;
    Object::update();
    return internRegister3D3D(a, b, *this, computeRMS);
}

bool arlCore::vnl_rigid_matrix::register3D3D( const std::vector< const Point* >& a, const std::vector< const Point* >& b, bool computeRMS )
{
    m_registerRMS = -1;
    m_registerStdDev = -1;
    Object::update();
    return internRegister3D3D(a, b, *this, computeRMS);
}

bool arlCore::vnl_rigid_matrix::register3D3DUncertainty( const PointList & a, const PointList & b, arlCore::ARLCORE_REGISTER3D3D methode, std::vector<double> optimiserParameters, std::vector<double> &log/*, unsigned int numberOfPoints*/ )
{
    bool computeRMS = false;
//  Object::update();
//  return internRegister3D3D(a, b, numberOfPoints,*this);
    vnl_vector<double> init(6);
    arlCore::vnl_rigid_matrix T;
//  for(unsigned int i = 0;i< b.size();i++)
//  {
//      const arlCore::vnl_covariance_matrix &cov_mat = b[i]->getCovMatrix();
//      std::cerr<<"mat cov = " << cov_mat <<std::endl;
//  }

    //initialization computation
    T.register3D3D(a,b,computeRMS/*,numberOfPoints*/);
    arlCore::vnl_rigid_vector vec(T);
    init = vec;
    
    if(methode == arlCore::ARLCORE_REGISTER3D3D_LM)
    {
        arlCore::register3D3DUncertainty_LS_cost_function reg3D3D_LM(a, b, 6, /*numberOfPoints*/0, vnl_least_squares_function::use_gradient);
        vnl_levenberg_marquardt computeReg3D3D_LM(reg3D3D_LM);
        computeReg3D3D_LM.minimize_using_gradient(init);
        log.push_back(computeReg3D3D_LM.get_end_error());
        log.push_back(computeReg3D3D_LM.get_start_error());
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        (*this) = T;
        return true;
    }
    if(methode == arlCore::ARLCORE_REGISTER3D3D_POW)
    {
        arlCore::register3D3DUncertainty_cost_function reg3D3D(a, b);
        vnl_powell computeReg3D3D(&reg3D3D);
        computeReg3D3D.minimize(init);
        log.push_back(computeReg3D3D.get_end_error());
        log.push_back(computeReg3D3D.get_start_error());
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        (*this) = T;
        return true;
    }
    if(methode == arlCore::ARLCORE_REGISTER3D3D_GC)
    {
        arlCore::register3D3DUncertainty_cost_function reg3D3D(a, b);
        vnl_conjugate_gradient computeReg3D3D(reg3D3D);
        computeReg3D3D.minimize(init);
        log.push_back(computeReg3D3D.get_end_error());
        log.push_back(computeReg3D3D.get_start_error());
        vec = init;
        T = arlCore::vnl_rigid_matrix(vec);
        (*this) = T;
        return true;
    }
    return false;
}

bool arlCore::vnl_rigid_matrix::RMS3D3D( const PointList& src, const PointList& dst, std::vector<double>& errors )
{
    m_registerRMS = 0.0, m_registerStdDev = 0.0;
    assert(src.size()==dst.size());
    if(src.size()!=dst.size()) return false;
    errors.clear();
    double err;
    unsigned int i;
    arlCore::Point A, B;
    double distance_average = 0;
    for( i=0 ; i<src.size() ; ++i )
    {
        if(src[i]->isVisible() && dst[i]->isVisible())
        {
            trf(*(src[i]),B);
            err = B.distance(*(dst[i]));
            distance_average += err;
            errors.push_back(err);
            m_registerRMS += err*err;
        }
    }
    const double n = (double)errors.size();
    distance_average /= n;
    m_registerRMS = sqrt(m_registerRMS/n);
    for( i=0 ; i<n ; ++i )
    {
        err = (errors[i] - distance_average);
        m_registerStdDev += err*err;
    }
    m_registerStdDev = sqrt(m_registerStdDev/n);
    Object::update();
    return true;
}

bool arlCore::vnl_rigid_matrix::register3D3DwithoutMatching( const PointList& a, const PointList& b, bool computeRMS, double gaussianError, double decimage )
{   // b = T*a ; a=echantillon mesure ; b=modele reel
    const bool Verbose = false;
    if(a.size()<3 || b.size()<3) return false;
    std::vector< const Point* > Va, Vb;
    const unsigned int NbMatching=arlCore::matching3D3D( a, b, gaussianError, decimage, Va, Vb );
    if(NbMatching>2)
        return register3D3D( Va, Vb, computeRMS );
    else
        if(Verbose) std::cout<<"3D/3D Matching Error\n";
    return false;
}

// ****************** ROTATIONS ******************
// TODO Integrate in vnl_rigid_matrix et vnl_rotation3d_matrix
vnl_matrix_fixed<double,4,4> arlCore::rotX(double thetaX)
{
    const double Angle = thetaX * arlCore::DegToRad;
    vnl_matrix_fixed<double,4,4> Rx;
    Rx.set_identity();
    //Rx = [1      0       0 0
    //      0 cos(a) -sin(a) 0
    //      0 sin(a)  cos(a) 0
    //      0      0       0 1]
    Rx[1][1] = Rx[2][2] = cos( Angle );
    Rx[1][2] = -sin( Angle );
    Rx[2][1] = -Rx[1][2];
    return Rx;
}

vnl_matrix_fixed<double,4,4> arlCore::rotY(double thetaY)
{
    const double Angle = thetaY * arlCore::DegToRad;
    vnl_matrix_fixed<double,4,4> Ry;
    Ry.set_identity();
    // Ry = [ cos(a) 0 sin(a) 0
    //             0 1 0      0
    //       -sin(a) 0 cos(a) 0
    //             0 0 0      1]
    Ry[0][0] = Ry[2][2]  = cos( Angle );
    Ry[0][2] = sin( Angle );
    Ry[2][0] = -Ry[0][2];
    return Ry;
}

vnl_matrix_fixed<double,4,4> arlCore::rotZ(double thetaZ)
{
    const double Angle = thetaZ * arlCore::DegToRad;
    vnl_matrix_fixed<double,4,4> Rz;
    Rz.set_identity();
    // Rz = [cos(a) -sin(a) 0 0
    //       sin(a)  cos(a) 0 0
    //            0       0 1 0
    //            0       0 0 1]
    Rz[0][0] = Rz[1][1] = cos( Angle );
    Rz[0][1] = -sin( Angle );
    Rz[1][0] = -Rz[0][1];
    return Rz;
}

vnl_matrix_fixed<double,4,4> arlCore::rotXYZ(double thetaX, double thetaY, double thetaZ)
{
    const double AngleX = thetaX * arlCore::DegToRad;
    const double AngleY = thetaY * arlCore::DegToRad;
    const double AngleZ = thetaZ * arlCore::DegToRad;
    vnl_matrix_fixed<double,4,4> R;
    R.set_identity();
    //R = [cos(z)cos(y)     sin(z)cos(x)-cos(z)sin(y)sin(x)     sin(z)sin(x)+cos(z)sin(y)cos(x) 0
    //     -sin(z)cos(y)    cos(z)cos(x)+sin(z)sin(y)sin(x)     cos(z)sin(x)-sin(z)sin(y)cos(x) 0
    //     -sin(y)          -cos(y)sin(x)                       cos(y)cos(x)                    0
    //      0               0                                   0                               1]
    R[0][0] = cos(AngleZ)*cos(AngleY);
    R[0][1] = sin(AngleZ)*cos(AngleX)-cos(AngleZ)*sin(AngleY)*sin(AngleX);
    R[0][2] = sin(AngleZ)*sin(AngleX)+cos(AngleZ)*sin(AngleY)*cos(AngleX);
    R[1][0] = -sin(AngleZ)*cos(AngleY);
    R[1][1] = cos(AngleZ)*cos(AngleX)+sin(AngleZ)*sin(AngleY)*sin(AngleX);
    R[1][2] = cos(AngleZ)*sin(AngleX)-sin(AngleZ)*sin(AngleY)*cos(AngleX);
    R[2][0] = -sin(AngleY);
    R[2][1] = -cos(AngleY)*sin(AngleX);
    R[2][2] = cos(AngleY)*cos(AngleX);
    return R;
}

vnl_matrix_fixed<double,4,4> arlCore::rotVecFromOrigin(double Ux, double Uy, double Uz, double a)
{
    const double Alpha = a * arlCore::DegToRad;
    const double Norm = sqrt(Ux*Ux+Uy*Uy+Uz*Uz);
    vnl_matrix_fixed<double,4,4> R;
    R.set_identity();
    if(Norm==0.0) return R;
    if(Norm!=1.0) Ux /= Norm, Uy /= Norm, Uz /= Norm;
    const double Ux2 = Ux*Ux, Uy2 = Uy*Uy, Uz2 = Uz*Uz;
    //R = [Ux²+cos(a)*(1-Ux²)           Ux*Uy*(1-cos(a))-Uz*sin(a)  Ux*Uz*(1-cos(a))+Uy*sin(a)  0
    //     Ux*Uy*(1-cos(a))+Uz*sin(a)   Uy²+cos(a)*(1-Uy²)          Uy*Uz*(1-cos(a))-Ux*sin(a)  0
    //     Ux*Uz*(1-cos(a))-Uy*sin(a)   Uy*Uz*(1-cos(a))+Ux*sin(a)  Uz²+cos(a)*(1-Uz²)          0
    //      0                           0                           0                           1]
    R[0][0] = Ux2+cos(Alpha)*(1-Ux2);
    R[0][1] = Ux*Uy*(1-cos(Alpha))-Uz*sin(Alpha);
    R[0][2] = Ux*Uz*(1-cos(Alpha))+Uy*sin(Alpha);
    R[1][0] = Ux*Uy*(1-cos(Alpha))+Uz*sin(Alpha);
    R[1][1] = Uy2+cos(Alpha)*(1-Uy2);
    R[1][2] = Uy*Uz*(1-cos(Alpha))-Ux*sin(Alpha);
    R[2][0] = Ux*Uz*(1-cos(Alpha))-Uy*sin(Alpha);
    R[2][1] = Uy*Uz*(1-cos(Alpha))+Ux*sin(Alpha);
    R[2][2] = Uz2+cos(Alpha)*(1-Uz2);
    return R;
}

vnl_matrix_fixed<double,4,4> arlCore::rotVecFromPoint(double a, double b, double c, double u, double v, double w, double angle)
{   // http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/ArbitraryAxisRotation.html
    const double Alpha = angle * arlCore::DegToRad;
    const double L2 = u*u+v*v+w*w;
    const double L = sqrt(L2);
    unsigned int i, j;
    vnl_matrix_fixed<double,4,4> R;
    R.set_identity();
    if(L==0.0) return R;
    if(L!=1.0) u /= L, v /= L, w /= L;
    const double u2 = u*u, v2 = v*v, w2 = w*w;
    R[0][0] = u2+(v2+w2)*cos(Alpha);
    R[0][1] = u*v*(1-cos(Alpha))-w*L*sin(Alpha);
    R[0][2] = u*w*(1-cos(Alpha))+v*L*sin(Alpha);
    R[0][3] = a*(v2+w2)-u*(b*v+c*w)+(u*(b*v+c*w)-a*(v2+w2))*cos(Alpha)+(b*w-c*v)*L*sin(Alpha);
    R[1][0] = u*v*(1-cos(Alpha))+w*L*sin(Alpha);
    R[1][1] = v2+(u2+w2)*cos(Alpha);
    R[1][2] = v*w*(1-cos(Alpha))-u*L*sin(Alpha);
    R[1][3] = b*(u2+w2)-v*(a*u+c*w)+(v*(a*u+c*w)-b*(u2+w2))*cos(Alpha)+(c*u-a*w)*L*sin(Alpha);
    R[2][0] = u*w*(1-cos(Alpha))-v*L*sin(Alpha);;
    R[2][1] = v*w*(1-cos(Alpha))+u*L*sin(Alpha);
    R[2][2] = w2+(u2+v2)*cos(Alpha);;
    R[2][3] = c*(u2+v2)-w*(a*u+b*v)+(w*(a*u+b*v)-c*(u2+v2))*cos(Alpha)+(a*v-b*u)*L*sin(Alpha);
    for( i=0 ; i<3 ; ++i )
        for( j=0 ; j<4 ; ++j )
            R[i][j] /= L2;
    return R;
}

// ****************** TRANSFORMATION FILTER ******************
// time=[0,1]
double getCoeff( double time, arlCore::ARLCORE_TRF_FILTER_TYPE filterType )
{   // Default : ARLCORE_TRF_FILTER_CONSTANT
    assert( time>=0.0 && time<=1.0 );
    const double LogRange = 2.71828182845904523536 - 1.0;
    switch(filterType)
    {
    case arlCore::ARLCORE_TRF_FILTER_LINEAR: return time;
    case arlCore::ARLCORE_TRF_FILTER_LOG: return log(1.0+time*LogRange);
    case arlCore::ARLCORE_TRF_FILTER_SQUARE: return time*time;
    case arlCore::ARLCORE_TRF_FILTER_CUBIC: return time*time*time;
    }
    return 1.0;
}

unsigned int arlCore::filter( const std::vector<const arlCore::vnl_rigid_matrix*> &list , ARLCORE_TRF_FILTER_TYPE type, vnl_rigid_matrix &average )
{
    const unsigned int Size = (unsigned int)list.size();
    if(Size==0) return 0;
    assert(list.back());
    average.setTime(list.back()->getDate(), list.back()->getTime());
    const double MatrixTime = (double)average.getTime();
    const double CurrentTime = (double)average.getTime(); // FIXME Planesystem time
    assert(list[0]);
    const double LastTime = (double)list[0]->getTime(); // FIXME Planesystem time - duration
    const double RangeTime = CurrentTime - LastTime;
    assert( RangeTime>=0.0 );
    if(Size==1)
    {
        average.copy(*list[0]);
        return Size;
    }
    vnl_quaternion<double>   Q;
    vnl_vector_fixed<double,3>  T,euler, eulermean(0,0,0),Tmean(0,0,0);
    vnl_rotation3d_matrix rot,rotinit;
    double coeffTotal = 0.0, coeff;
    unsigned int i;
    for( i=0 ; i<Size ; ++i )
    {
        if(type==ARLCORE_TRF_FILTER_CONSTANT || RangeTime==0.0 ) coeff = 1.0;
        else
        {
            const double time = list[i]->getTime();
            assert( time>=LastTime && time<=CurrentTime );
            coeff = getCoeff( (time-LastTime)/RangeTime, type);
        }
        assert(list[i]);
        Q = list[i]->getQuaternion();
        T = list[i]->getTranslation();
        euler = Q.rotation_euler_angles();
        eulermean+=euler*coeff; 
        Tmean+=T*coeff;
        coeffTotal += coeff;
    }
    eulermean/=coeffTotal;
    Tmean/=coeffTotal;          
    vnl_quaternion<double> q2(eulermean[0],eulermean[1],eulermean[2]);  
    rot = q2.rotation_matrix_transpose();
    rot = rot.transpose();
    average.setTranslation(Tmean);
    average.setRotation(rot);
    return Size;
}
