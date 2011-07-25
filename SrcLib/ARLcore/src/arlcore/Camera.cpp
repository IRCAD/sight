/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Camera.h>

#include <sstream>
#include <fstream>
#include <vector>
#include <time.h>

#include <vxl/core/vxl_version.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
//#ifdef __MACOSX__ // because vxl 1.11.0
#if ( VXL_VERSION_MAJOR >= 1 ) && ( VXL_VERSION_MINOR >= 11 )
#include <vgl/vgl_intersection.h>
#endif
#include <vgl/algo/vgl_homg_operators_2d.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_real_npolynomial.h>
#include <vnl/algo/vnl_rnpoly_solve.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <arlcore/MatrixR.h>
#include <arlcore/PointsList.h>
#include <arlcore/PlaneSystem.h>
#include <arlcore/Reconst3D.h>
#include <arlcore/Optimization.h>
#include <arlcore/Misc.h>
#include <arlcore/File.h>

arlCore::Camera::Camera( PlaneSystem &universe ):
Object(ARLCORE_CLASS_CAMERA),
Particle( universe )
{
    m_cameraGUID.Low=0;
    m_cameraGUID.High=0;
    init();
    universe.setPlaneName(getPlane(), Object::getName());
}

arlCore::Camera::Camera( PlaneSystem &universe, const uint32HL &GUID ):
Object(ARLCORE_CLASS_CAMERA),
Particle( universe )
{
    m_cameraGUID.Low = GUID.Low;
    m_cameraGUID.High = GUID.High;
    init();
    universe.setPlaneName(getPlane(), Object::getName());
}

arlCore::Camera::Camera( const arlCore::Camera& c ):
Object( ARLCORE_CLASS_CAMERA, c.Object::getName() ),
Particle( c.getPlaneSystem(), c.Particle::getName() )
{
    copy(c);
}

arlCore::Camera& arlCore::Camera::operator=(const arlCore::Camera& c)
{
    copy(c);
    return *this;
}

void arlCore::Camera::copy(const Camera& c)
{
    if(this==&c) return;
    arlCore::Object *a=this;
    const arlCore::Object *b=&c;
    *a = *b;
    m_cameraGUID = c.m_cameraGUID;
    m_noCalibration = c.m_noCalibration;
    m_extrinsic = c.m_extrinsic;
    m_intrinsicMatrix = c.m_intrinsicMatrix;
    m_distortionCoeffs = c.m_distortionCoeffs;
    m_alphaC = c.m_alphaC;
    m_Qint = c.m_Qint;
    m_Qext = c.m_Qext;
    m_Cint = c.m_Cint;
    m_Cext = c.m_Cext;
    m_Bint = c.m_Bint;
    m_Bext = c.m_Bext;
    m_invExtrinsic = c.m_invExtrinsic;
    m_invIntrinsicMatrix = c.m_invIntrinsicMatrix;
    m_opticalCenterInExtrinsicFrame = c.m_opticalCenterInExtrinsicFrame;
    getPlaneSystem().setPlaneName(getPlane(), Object::getName());
    Object::update();
    return;
}

void arlCore::Camera::init( void )
{
    m_extrinsic.setIdentity();
    m_invExtrinsic.setIdentity();
    m_noCalibration=0;
    m_Bint.fill(0.0); // FIXME Never changed, Never used
    m_Bext.fill(0.0);
    m_Cint.fill(0.0); // FIXME Never changed, Never used
    m_Cint.put(2,1.0);
    m_Cext.fill(0.0);
    m_Qint.fill(0.0);
    m_Qext.fill(0.0);
    m_intrinsicMatrix.set_identity();
    m_invIntrinsicMatrix.set_identity();
    m_opticalCenterInExtrinsicFrame.fill(0.0);
    m_alphaC=0;
    m_distortionCoeffs.fill(0.0);
    m_log << "CamRef " << m_cameraGUID.High << "-" << m_cameraGUID.Low;
    setName(m_log.str());
    m_log<<" created ("<<getNo()<<")";
    log(ARLCORE_LOG_INFO_LEVEL2);
}

arlCore::Camera::~Camera( )
{}

std::string arlCore::Camera::getString( void ) const
{
    std::stringstream s;
    s<<this->Object::getString();
    s<<this->Particle::getString();
    s<<"GUID : "<<m_cameraGUID.High<<"-"<<m_cameraGUID.Low;
    s<<"\nCalibration #"<<m_noCalibration;
    s<<"\nExtrinsic matrix :\n"<<m_extrinsic;
    s<<"\nIntrinsic matrix :\n"<<m_intrinsicMatrix;
    s<<"\nk1 : "<<m_distortionCoeffs[0];
    s<<"\tk2 : "<<m_distortionCoeffs[1];
    s<<"\tk3 : "<<m_distortionCoeffs[4];
    s<<"\np1 : "<<m_distortionCoeffs[2];
    s<<"\tp2 : "<<m_distortionCoeffs[3];
    s<<"\tAlphaC : "<<m_alphaC<<"\n";
    return s.str();
}

long int arlCore::Camera::getCalibrationID( void ) const
{
    return m_noCalibration;
}

bool arlCore::Camera::loadParameters( const std::string &fileName, long int date, unsigned int width, unsigned int height )
{
    std::string folder, file, ext;
    arlString::splitFilename( fileName, folder, file, ext );
    arlCore::File config(fileName, "Cameras");
    const double Version = config.readHeader();
    if(Version>0.0)
    {
        std::string token, filename, lastName;
        unsigned int numero=0, xSize, ySize, year, month, day, hour, minute, second;
        uint32HL GUID;
        double rms;
        bool sharedPlane = false;
        time_t lastTime = 0;
        while(config.getToken(token))
        {
            if(token=="GUID")
            {
                config.get(GUID.High);
                config.get(GUID.Low);
            }
            if(token=="Resolution")
            {
                config.get(xSize);
                config.get(ySize);
            }
            if(token=="Date")
            {
                config.get(year);
                config.get(month);
                config.get(day);
            }
            if(token=="SharedPlaneWithPreviousCamera")
                sharedPlane = true;
            if(token=="Time")
            {
                config.get(hour);
                config.get(minute);
                config.get(second);
            }
            if(token=="RMS") config.get(rms);
            if(token=="Filename") config.getString(filename);
            if(token=="End")
            {
                if(!sharedPlane) ++numero;
                long int tps;
                struct tm t;
                t.tm_year = year - 1900; // years since 1900
                t.tm_mon  = month - 1; // months since January - [0,11]
                t.tm_mday = day; // day of the month - [1,31]
                t.tm_hour = hour; // hours since midnight - [0,23]
                t.tm_min  = minute; // minutes after the hour - [0,59]
                t.tm_sec  = second; // seconds after the minute - [0,59]
                // FIXME Arbitrairement mis a zero pour que le resultat soit repetable
                // Est-ce pour autant correct ?
                t.tm_wday = 0; // days since Sunday - [0,6]
                t.tm_yday = 0; // days since January 1 - [0,365]
                //The Daylight Saving Time flag (tm_isdst) is greater than zero if Daylight Saving Time is in effect,
                //zero if Daylight Saving Time is not in effect, and less than zero if the information is not available
                t.tm_isdst= 0; // daylight savings time flag
                tps = (long int)mktime(&t);
                //std::cout<<No<<":"<<tps<<" "<<Y<<" "<<M<<" "<<D<<" "<<H<<" "<<Mn<<" "<<S<<"\n";
                // difftime(date2, date1) => If date1 avant date2 return +sec else -sec
                if(m_cameraGUID.Low==GUID.Low && m_cameraGUID.High==GUID.High
                    && tps<date && tps>=lastTime && xSize==width && ySize==height)
                {
                    lastTime = tps;
                    if(folder=="") lastName = filename;
                    else lastName = folder+"/"+filename;
                    m_noCalibration = numero;
                }
                sharedPlane = false;
            }
        }
        config.close();
        if(lastTime!=0) return load(lastName);
        std::cerr<<"Camera parameters of "<<m_cameraGUID.High<<"-"<<m_cameraGUID.Low<<" ("<<width<<","<<height<<") not found\n";
        return false;
    }else
    {
        config.close();
        return loadOldParameters(fileName, date, width, height);
    }
}

bool arlCore::Camera::loadOldParameters( const std::string &fileName, long int date, unsigned int width, unsigned int height )
{
    const unsigned int NBPARAMS = 13;
    if(strcmp(fileName.c_str(),"")==0)
    {
        log(ARLCORE_LOG_ERROR,"loadParameters : Empty filename");
        return false;
    }
    FILE* file = fopen( fileName.c_str(), "r" );
    if( !file )
    {
        m_log<<"loadParameters:File \""<<fileName<<"\" doesn't exist";
        log(ARLCORE_LOG_ERROR);
        return false;
    }
    std::string path;
    unsigned int pos = (unsigned int)fileName.find_last_of ("/");
    if(pos!=std::string::npos) path = fileName.substr (0,pos+1);
    else path="";
    int n=NBPARAMS, Y, M, D, H, S, Mn, WH, HT;
    float RMS;
    long unsigned int No;
    uint32HL GUID;
    char name[500];
    std::string lastName;
    long int tps, lastTime=0;
    struct tm t;
    //Header : Numero is the same if 2 calibrations is calculated in the same 3D plane
    fscanf(file, " Numero | High ID Low   |  Dim  |    Acquisition    |   RMS  |      Filename\n");
    while(!feof(file) &&  n==NBPARAMS)
    {
        n=fscanf(file, "%ld %ld %ld %d %d %d %d %d %d %d %d %f %s\n", &No, &GUID.High, &GUID.Low, &WH, &HT, &Y, &M, &D, &H, &Mn, &S, &RMS, name );
        if(n==NBPARAMS)
        {
            t.tm_year = Y - 1900; // years since 1900
            t.tm_mon  = M - 1; // months since January - [0,11]
            t.tm_mday = D; // day of the month - [1,31]
            t.tm_hour = H; // hours since midnight - [0,23]
            t.tm_min  = Mn; // minutes after the hour - [0,59]
            t.tm_sec  = S; // seconds after the minute - [0,59]
            // FIXME Arbitrairement mis a zero pour que le resultat soit repetable
            // Est-ce pour autant correct ?
            t.tm_wday = 0; // days since Sunday - [0,6]
            t.tm_yday = 0; // days since January 1 - [0,365]
            t.tm_isdst= 0; // daylight savings time flag
            tps = (long int)mktime(&t);
            //std::cout<<No<<":"<<tps<<" "<<Y<<" "<<M<<" "<<D<<" "<<H<<" "<<Mn<<" "<<S<<"\n";
            if(m_cameraGUID.Low==GUID.Low && m_cameraGUID.High==GUID.High && tps<date && tps>=lastTime && WH==width && HT==height)
            {
                lastTime = tps;
                lastName = path+name;
                m_noCalibration = No;
            }
        }
    }
    fclose(file);
    // TODO Si fichier non trouve, chercher le suivant ?
    if(lastTime!=0) return load(lastName);
    std::cerr<<"Camera parameters of "<<m_cameraGUID.Low<<"-"<<m_cameraGUID.High<<" ("<<width<<","<<height<<") not found\n";
    return false;
}

bool arlCore::Camera::load( const std::string &fileName )
{
    std::ifstream f;
    f.open (fileName.c_str(), std::fstream::in);
    if(!f.is_open()) return false;
    double M[5];
    unsigned int i, j;
    setOK(false);
    for( i=0 ; i<4 ; ++i )
        for( j=0; j<4; ++j )
        {
            f>>M[j];
            m_extrinsic[i][j]=M[j];
        }
    // alpha values : Focale en X ; Focale en Y
    for( i=0; i<5; ++i )
        f>>M[i];
    setfx(M[0]); // AlphaU
    setfy(M[1]); // AlphaV
    // Principal point : Position du centre optique u0 ; v0
    setcx(M[2]); // u0
    setcy(M[3]); // v0
    setAlphaC(M[4]); // skew (Orthogonalité de la plaque CCD)
    // Radial 1er ordre ; 2ème ordre ; Tangentiel 1er ordre ; 2ème ordre ; Radial 3ème ordre
    for( i=0 ; i<5 ; ++i )
    {
        f>>M[i];
        setkc(i,M[i]);
    }
    extCompute();
    setOK(true);
    m_log<<fileName<<" loaded";
    log(ARLCORE_LOG_INFO_LEVEL2);

    f.close();
    return true;
}

bool arlCore::Camera::save( const std::string &fileName, bool overwrite ) const
{
    if(strcmp(fileName.c_str(),"")==0) return false;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    FILE *file = fopen( fileName.c_str(), "w" );
    unsigned int i, j;
    for( i=0 ; i<4 ; ++i )
    {
        for( j=0; j<4; ++j )
            fprintf(file, " %lf", m_extrinsic[i][j]);
        fprintf(file, "\n");
    }
    // alpha values : Focale en X ; Focale en Y
    fprintf( file, "%lf %lf\n", getfx(), getfy() );
    // Principal point : Position du centre optique u0 ; v0
    fprintf( file, "%lf %lf\n", getcx(), getcy() );
    // alphaC=skew (Orthogonalité de la plaque CCD)
    fprintf( file, "%lf\n",getAlphaC() );
    // Radial 1er ordre ; 2ème ordre ; Tangentiel 1er ordre ; 2ème ordre ; Radial 3ème ordre
    for( i=0 ; i<5 ; ++i )
        fprintf( file, "%lf ", m_distortionCoeffs[i] );
    fprintf( file, "0.0\n" );
    double a,b,c,d,rms;

    fclose( file );
    return true;
}

bool arlCore::Camera::isIntrinsicCalibrated( void ) const
{
    return !getIntrinsicMatrix().is_identity();
}

const vnl_matrix_fixed<double,3,3>& arlCore::Camera::getIntrinsicMatrix( void ) const
{
    return m_intrinsicMatrix;
}

const vnl_matrix_fixed<double,3,3>& arlCore::Camera::getInvIntrinsicMatrix( void ) const
{
    return m_invIntrinsicMatrix;
}

const vnl_vector<double> arlCore::Camera::getIntrinsicVector( void ) const
{
    vnl_vector<double> intrinsic(10);
    intrinsic[0] = getfx();
    intrinsic[1] = getfy();
    intrinsic[2] = getcx();
    intrinsic[3] = getcy();
    intrinsic[4] = getkc(0);
    intrinsic[5] = getkc(1);
    intrinsic[6] = getkc(2);
    intrinsic[7] = getkc(3);
    intrinsic[8] = getkc(4);
    intrinsic[9] = getAlphaC();
    return intrinsic;
}

unsigned int arlCore::Camera::setIntrinsic( const vnl_vector<double> &p )
{
    unsigned int size = p.size();
    if(size>0) setfx(p[0]);
    if(size>1) setfy(p(1));
    if(size>2) setcx(p(2));
    if(size>3) setcy(p(3));
    if(size>4) setkc(0,p(4));
    if(size>5) setkc(1,p(5));
    if(size>6) setkc(2,p(6));
    if(size>7) setkc(3,p(7));
    if(size>8) setkc(4,p(8));
    if(size>9) setAlphaC(p(9));
    if(size>9) return 10;
    else return size;
}

const vnl_vector_fixed<double,5>& arlCore::Camera::getDistortion ( void )const
{
    return m_distortionCoeffs;
}

const arlCore::vnl_rigid_matrix& arlCore::Camera::getExtrinsic (void ) const
{
    return m_extrinsic;
}

const arlCore::vnl_rigid_matrix& arlCore::Camera::getInvExtrinsic (void ) const
{
    return m_invExtrinsic;
}

double arlCore::Camera::getcx() const
{
    return m_intrinsicMatrix(0,2);
}

void arlCore::Camera::setcx( double a)
{
    m_intrinsicMatrix(0,2)=a;
    m_Qint.put(0,2,getcx());
    m_invIntrinsicMatrix = vnl_matrix_inverse<double>(m_intrinsicMatrix.as_matrix());
}

double arlCore::Camera::getcy() const
{
    return m_intrinsicMatrix(1,2);
}

void arlCore::Camera::setcy( double a)
{
    m_intrinsicMatrix(1,2)=a;
    m_Qint.put(1,2,getcy());
    m_invIntrinsicMatrix = vnl_matrix_inverse<double>(m_intrinsicMatrix.as_matrix());
}

double arlCore::Camera::getfx() const
{
    return m_intrinsicMatrix(0,0);
}

void arlCore::Camera::setfx( double a)
{
    m_intrinsicMatrix(0,0)=a;
    m_Qint.put(0,0,getfx());
    m_invIntrinsicMatrix = vnl_matrix_inverse<double>(m_intrinsicMatrix.as_matrix());
}

double arlCore::Camera::getfy() const
{
    return m_intrinsicMatrix(1,1);
}

void arlCore::Camera::setfy( double a)
{
    m_intrinsicMatrix(1,1)=a;
    m_Qint.put(1,1,getfy());
    m_invIntrinsicMatrix = vnl_matrix_inverse<double>(m_intrinsicMatrix.as_matrix());
}

double arlCore::Camera::getAlphaC() const
{
    return m_alphaC;
}

void arlCore::Camera::setAlphaC( double a)
{
    m_alphaC=a;
}

double arlCore::Camera::getkc(unsigned int i) const
{
    if(i<5)
        return m_distortionCoeffs.get(i);
    else return 0.0;
}

bool arlCore::Camera::setkc(unsigned int i, double a)
{
    if(i>=5) return false;
    m_distortionCoeffs.put(i,a);
    return true;
}

void arlCore::Camera::extCompute(void)
{
    unsigned int i,j;
    m_invExtrinsic.invert(getExtrinsic());
    vnl_vector_fixed< double, 3 >opticalCenter(0.0,0.0,0.0);
    focalFrameToExtrinsicFrame(opticalCenter,m_opticalCenterInExtrinsicFrame);

    vnl_matrix_fixed<double,3,4> T;
    T.fill(0.0);
    T.put(0,0,getcx());
    T.put(1,1,getcy());
    T.put(0,2,getfx());
    T.put(1,2,getfy());
    T.put(2,2,1.0);
//  for( i=0 ; i<2 ; ++i )  // FIXME INUTILE
//      T.put(0,3,0.0);     // FIXME INUTILE
    vnl_matrix_fixed<double,3,4> P = T * getExtrinsic();

    m_Bext.put(0,P.get(0,3));
    m_Bext.put(1,P.get(1,3));

    m_Cext.put(0,P.get(2,0));
    m_Cext.put(1,P.get(2,1));
    m_Cext.put(2,P.get(2,2));

    for( i=0 ; i<2 ; ++i ) // FIXME See definition : Qext<2,3>
        for( j=0 ; j<2 ; ++j )
            m_Qext.put(i,j,P.get(i,j));
}

const vnl_matrix_fixed<double,2,3>& arlCore::Camera::getQint( void ) const
{
    return m_Qint;
}

const vnl_matrix_fixed<double,2,3>& arlCore::Camera::getQext( void ) const
{
    return m_Qext;
}

const vnl_vector_fixed<double,3>& arlCore::Camera::getCint( void ) const
{
    return m_Cint;
}

const vnl_vector_fixed<double,3>& arlCore::Camera::getCext( void ) const
{
    return m_Cext;
}

const vnl_vector_fixed<double,2>& arlCore::Camera::getBint( void ) const
{
    return m_Bint;
}

const vnl_vector_fixed<double,2>& arlCore::Camera::getBext( void ) const
{
    return m_Bext;
}

bool arlCore::Camera::setExtrinsic( const vnl_rigid_matrix& T )
{
    m_extrinsic = T;
    extCompute();
    return true;
}

///////////////////////////FONCTIONS PROJECTIONS//////////////////////////
unsigned int arlCore::Camera::project3DPoint(const PointList& list3D, PointList& list2D, bool focalPlane) const
{
    list2D.clear();
    list2D.setDimension(2);
    Point point2D(2);
    unsigned int i, n=0;
    for( i=0 ; i<list3D.size() ; ++i )
        if(project3DPoint(*(list3D[i]),point2D,focalPlane))
        {
            list2D.push_back(point2D);
            ++n;
        }
        else
        {
            //std::cout<<"ERROR dans project3DPoint(const PointList& list3D, PointList& list2D, bool focalPlane)"<<std::endl;
            //std::cerr<<"project3DPoint ERROR on "<<i<<"th point\n";
        }
    return n;
}

bool arlCore::Camera::project3DPoint(const Point& pt3D, Point& pt2D, bool focalPlane) const
{
    assert(pt3D.size()==3 && pt2D.size()==2);
    if(pt3D.size()<3 || pt2D.size()<2) return false;
    unsigned int i;
    vnl_vector_fixed<double,4> point3D;
    for( i=0 ; i<3 ; ++i )
        point3D(i)=pt3D[i];
    point3D(3)=1.0;
    vnl_vector_fixed<double,2> point2D;
    if(project3DPoint (point3D, point2D, focalPlane))
    {
        for( i=0 ; i<2 ; ++i )
            pt2D.set(i,point2D(i));
        return true;
    }else return false;
}

// Projection d'un point 3D sur le plan image (focalPlane=true) ou le plan pixel (focalPlane=false)
// en fonction de la matrice de projection et des parametres de distortion de la camera
// Les notations sont celles de la matlab camera calibration toolbox
bool arlCore::Camera::project3DPoint(const vnl_vector_fixed<double,4> &point3D, vnl_vector_fixed<double,2> &pt2D, bool focalPlane) const
{
    double xn_x, xn_y, xd_x, xd_y, r2, r4, r6;
    double radial_disto, tang_disto_x, tang_disto_y;
    if(!focalPlane)
    {
        assert(point3D(3)==1.0);
        // on applique la matrice extrinseque au point3D pour le placer dans le
        // repere de la camera : Matrix4x4 * HVec4
        vnl_vector_fixed<double,4> point3DRepCam = getExtrinsic() * point3D;
        // on calcule les coordonnees normalisées de la projection de point3DRepCam
        // dans le plan focal image de la caméra
        if(point3DRepCam.get(2)==0)
        {
            //std::cout<<"ERROR dans project3DPoint(const vnl_vector_fixed<double,4> &point3D, vnl_vector_fixed<double,2> &pt2D, bool focalPlane)"<<std::endl;
            return false;
        }
        assert(point3DRepCam.get(2)!=0); // Le point est dans le plan parallèle à la focale passant par le centre optique
        xn_x = point3DRepCam.get(0)/point3DRepCam.get(2);
        xn_y = point3DRepCam.get(1)/point3DRepCam.get(2);
    }else
    {
        assert(point3D(2)==1.0); // Le point n'est pas dans le plan de la focale
        xn_x = point3D(0);
        xn_y = point3D(1);
    }
    // on calcule en avance r² = x²+y²
    r2 = xn_x * xn_x + xn_y * xn_y;
    r4 = r2*r2;
    r6 = r4*r2;
    // radial_disto = 1 + kc[0]* r2 + kc[1]* r4 + kc[4]* r6;
    radial_disto = 1 + getkc(0)* r2 + getkc(1)* r4 + getkc(4)* r6;
    tang_disto_x = 2 * getkc(2) * xn_x * xn_y + getkc(3) * (r2 + 2 * xn_x * xn_x);
    tang_disto_y = getkc(2) * (r2 + 2 * xn_y * xn_y) + 2 * getkc(3) * xn_x * xn_y;
    // en fait [ xd_x ] = (1+kc(1)*r² + kc(2)*r²*r² + kc(5)*r²*r²*r²)* [ xn_x ] + [ tang_disto_x ]
    //         [ xd_y ]   (                                         )  [ xn_y ]   [ tang_disto_y ]
    xd_x = radial_disto * xn_x + tang_disto_x;
    xd_y = radial_disto * xn_y + tang_disto_y;
    // application de la matrice intrinsèque
    pt2D.put(0,getfx() * (xd_x + getAlphaC() * xd_y) + getcx());
    pt2D.put(1,getfy() * xd_y + getcy());
    return true;
}

bool arlCore::Camera::undistort2DPoint(const Point& p2D, Point& p2D_undistorted) const
{
    const double pixSquareMax = 1e-50;
    const unsigned int IterationsMax = 50;
    if(!p2D.isVisible() || p2D.size()<2) return false;
    const double tol=1e-12;
    double pinit[3]={p2D.x(),p2D.y(),1.0};
    vnl_vector<double> points2DH(3,3,pinit),points2DH_undistorted(3);
    unsigned int i, nbIterations=0;
    double xn_x, xn_y, xd_x, xd_y;
    double radial_disto, tang_disto_x, tang_disto_y;
    double r2, r4, r6, kc[5];
    for( i=0 ; i<5 ; ++i )
        kc[i] = getkc(i);
    vnl_vector_fixed<double,3> result = getInvIntrinsicMatrix() * points2DH;
    xn_x = result(0);
    xn_y = result(1);
    // prise en compte de la distorsion de manière non linéaire et non précise
    // on calcule le point non-distordu en approximant la distorsion
    if(true)
    {   // on calcule en avance r² = x²+y²
        double tmp_x = xn_x, tmp_y = xn_y;
        r2 = xn_x * xn_x + xn_y * xn_y;
        r4 = r2*r2;
        r6 = r4*r2;
        xd_x = xn_x;
        xd_y = xn_y;
        i=0;
        do
        {
            ++nbIterations;
            tmp_x = xn_x;
            tmp_y = xn_y;
            // calcul de la distortion à appliquer
            radial_disto = 1 + kc[0]* r2 + kc[1]* r4 + kc[4]* r6;
            tang_disto_x = 2 * kc[2] * xn_x * xn_y + kc[3] * (r2 + 2 * xn_x * xn_x);
            tang_disto_y = kc[2] * (r2 + 2 * xn_y * xn_y) + 2 * kc[3] * xn_x * xn_y;
            // en fait [ xd_x ] = (1+kc(1)*r² + kc(2)*r²*r² + kc(5)*r²*r²*r²)* [ xn_x ] + [ tang_disto_x ]
            //         [ xd_y ]   (                                         )  [ xn_y ]   [ tang_disto_y ]
            xn_x = (xd_x - tang_disto_x)/radial_disto;
            xn_y = (xd_y - tang_disto_y)/radial_disto;
            r2 = xn_x * xn_x + xn_y * xn_y;
            r4 = r2*r2;
            r6 = r4*r2;
        }while( (tmp_x-xn_x)*(tmp_x-xn_x)*getfx()*getfx() + (tmp_y-xn_y)*(tmp_y-xn_y)*getfy()*getfy() > pixSquareMax && nbIterations<IterationsMax );
        points2DH_undistorted.put(0, xn_x);
        points2DH_undistorted.put(1, xn_y);
        points2DH_undistorted.put(2, 1.0);
        p2D_undistorted.set(0, (getIntrinsicMatrix() * points2DH_undistorted)(0));
        p2D_undistorted.set(1, (getIntrinsicMatrix() * points2DH_undistorted)(1));
        //cerr << "sans correction parfaite : " << result[i] << endl;
    }
    return true;
}

bool arlCore::Camera::projectiveLine( const vnl_vector_fixed<double,3> &focalPt2D, vgl_line_3d_2_points <double> &line ) const
{
    unsigned int i;
    vnl_vector_fixed<double,4> m, m_cam, c_cam, C(0.0, 0.0, 0.0, 1.0);
    for( i=0 ; i<3 ; ++i )
        m[i] = focalPt2D[i];
    m[3] = 1.0;
    m_cam = getInvExtrinsic() * m;
    c_cam = getInvExtrinsic() * C;
    vgl_point_3d <double> m_cam_vgl(m_cam[0],m_cam[1],m_cam[2]);
    vgl_point_3d <double> c_cam_vgl(c_cam[0],c_cam[1],c_cam[2]);
    if(m_cam_vgl==c_cam_vgl) return false;
    vgl_line_3d_2_points <double> line1(m_cam_vgl, c_cam_vgl);
    line = line1;
    return true;
}

bool arlCore::Camera::unitFocalPlaneToPixelPlane( const vgl_point_2d<double>&p1, vgl_point_2d<double>& p2 ) const
{
    double xn_x, xn_y, xd_x, xd_y, r2, r4, r6;
    double radial_disto, tang_disto_x, tang_disto_y;
    xn_x = p1.x();
    xn_y = p1.y();
    // on calcule en avance r² = x²+y²
    r2 = xn_x*xn_x + xn_y*xn_y;
    r4 = r2*r2;
    r6 = r4*r2;
    radial_disto = 1 + getkc(0) * r2 + getkc(1) * r4 + getkc(4) * r6;
    tang_disto_x = 2 * getkc(2) * xn_x * xn_y + getkc(3) * (r2 + 2 * xn_x * xn_x);
    tang_disto_y = 2 * getkc(3) * xn_x * xn_y + getkc(2) * (r2 + 2 * xn_y * xn_y);
    // en fait [ xd_x ] = (1+kc(1)*r² + kc(2)*r²*r² + kc(5)*r²*r²*r²)* [ xn_x ] + [ tang_disto_x ]
    //         [ xd_y ]   (                                         )  [ xn_y ]   [ tang_disto_y ]
    xd_x = radial_disto * xn_x + tang_disto_x;
    xd_y = radial_disto * xn_y + tang_disto_y;
    // application de la matrice intrinsèque
    p2.x()=getfx() * (xd_x + getAlphaC() * xd_y) + getcx();
    p2.y()=getfy() * xd_y + getcy();
    return true;
}

bool arlCore::Camera::pixelPlaneToUnitFocalPlane( const arlCore::Point& p2D, arlCore::Point& p3D, bool perfectDisto ) const
{
    assert(p2D.size()==2);
    assert(p3D.size()==3);
    const double pixSquareMax = 1e-50;
    const unsigned int IterationsMax = 50;
    if(!p2D.isVisible() || p2D.size()<2) return false;
    const double tol=1e-12;
    p3D.fill(0.0);
    if(p3D.size()>2) p3D.set(2, 1.0);
    double pinit[3]={p2D.x(),p2D.y(),1.0};
    vnl_vector<double> points2DH(3,3,pinit);
    unsigned int i;
    vnl_matrix_fixed<double,3,3> Intrinseq, var;
    double xn_x, xn_y, xd_x, xd_y;
    double radial_disto, tang_disto_x, tang_disto_y;
    double r2, r4, r6, kc[5];
    // DECLARATION DES VARIABLES NECESSAIRES A LA RESOLUTION POLYNOMIALE
    vnl_real_npolynomial P1, P2;
    vnl_vector< double > coef1(10), coef2(10);
    vnl_matrix< unsigned int > power1(10,2), power2(10,2);
    vcl_vector< vnl_vector< double > * > roots;
    vcl_vector<vnl_real_npolynomial*> PolyToSolve;
    for( i=0 ; i<5 ; ++i )
        kc[i] = getkc(i);
    vnl_vector_fixed<double,3> result = getInvIntrinsicMatrix() * points2DH;
    xn_x = result(0);
    xn_y = result(1);
    // prise en compte de la distorsion de manière non linéaire et non précise
    // on calcule le point non-distordu en approximant la distorsion
    if(!perfectDisto)
    {   // on calcule en avance r² = x²+y²
        double tmp_x = xn_x, tmp_y = xn_y;
        r2 = xn_x * xn_x + xn_y * xn_y;
        r4 = r2*r2;
        r6 = r4*r2;
        xd_x = xn_x;
        xd_y = xn_y;
        i=0;
        do
        {
            i++;
            tmp_x = xn_x;
            tmp_y = xn_y;
            // calcul de la distortion à appliquer
            radial_disto = 1 + kc[0]* r2 + kc[1]* r4 + kc[4]* r6;
            tang_disto_x = 2 * kc[2] * xn_x * xn_y + kc[3] * (r2 + 2 * xn_x * xn_x);
            tang_disto_y = kc[2] * (r2 + 2 * xn_y * xn_y) + 2 * kc[3] * xn_x * xn_y;
            // en fait [ xd_x ] = (1+kc(1)*r² + kc(2)*r²*r² + kc(5)*r²*r²*r²)* [ xn_x ] + [ tang_disto_x ]
            //         [ xd_y ]   (                                         )  [ xn_y ]   [ tang_disto_y ]
            xn_x = (xd_x - tang_disto_x)/radial_disto;
            xn_y = (xd_y - tang_disto_y)/radial_disto;
            r2 = xn_x * xn_x + xn_y * xn_y;
            r4 = r2*r2;
            r6 = r4*r2;
        }while( (tmp_x-xn_x)*(tmp_x-xn_x)*getfx()*getfx() + (tmp_y-xn_y)*(tmp_y-xn_y)*getfy()*getfy() > pixSquareMax && i<IterationsMax );
        //cerr<< "NOMBRE D'ITERATIONS : " << i << endl;
        p3D.set(0, xn_x);
        p3D.set(1, xn_y);
        //cerr << "sans correction parfaite : " << result[i] << endl;
    }
    // recherche des valeurs exactes non distordues avec une résolution polynomiale
    // xd = x + k[0]. x^3+ k[0].y^2.x +k[1]x^5 +2k[1]x^3y^2 +k[1]y^4.x +2.k[2]x.y +3k[3]x^2 +k[3]y^2
    // yd = y + k[0]. y^3+ k[0].x^2.y +k[1]y^5 +2k[1]y^3x^2 +k[1]x^4.y +2.k[3]x.y +3k[2]y^2 +k[2]x^2
    if(perfectDisto)
    {
        coef1.put(0 , 1.0);
        coef1.put(1 , kc[0]);
        coef1.put(2 , kc[0]);
        coef1.put(3 , kc[1]);
        coef1.put(4 , 2*kc[1]);
        coef1.put(5 , kc[1]);
        coef1.put(6 , 2*kc[2]);
        coef1.put(7 , 3*kc[3]);
        coef1.put(8 , kc[3]);
        coef1.put(9 , -1.0 * xn_x);
        coef2.put(0 , 1.0);
        coef2.put(1 , kc[0]);
        coef2.put(2 , kc[0]);
        coef2.put(3 , kc[1]);
        coef2.put(4 , 2*kc[1]);
        coef2.put(5 , kc[1]);
        coef2.put(6 , 2*kc[3]);
        coef2.put(7 , 3*kc[2]);
        coef2.put(8 , kc[2]);
        coef2.put(9 , -1.0 * xn_y);
        power1.put(0,0,1);power1.put(0,1,0);
        power1.put(1,0,3);power1.put(1,1,0);
        power1.put(2,0,1);power1.put(2,1,2);
        power1.put(3,0,5);power1.put(3,1,0);
        power1.put(4,0,3);power1.put(4,1,2);
        power1.put(5,0,1);power1.put(5,1,4);
        power1.put(6,0,1);power1.put(6,1,1);
        power1.put(7,0,2);power1.put(7,1,0);
        power1.put(8,0,0);power1.put(8,1,2);
        power1.put(9,0,0);power1.put(9,1,0);
        for( i=0 ; i<10 ; ++i )
        {
            power2.put(i, 0, power1(i,1));
            power2.put(i, 1, power1(i,0));
        }
        P1.set(coef1,power1);
        P2.set(coef2,power2);
        PolyToSolve.push_back(&P1);
        PolyToSolve.push_back(&P2);
        vnl_rnpoly_solve Solve( PolyToSolve );
        roots = Solve.realroots(tol);
        if(roots.size()>0)
        {   // Pas de racines : FIXME
            p3D.set(0, roots[0]->get(0));
            p3D.set(1, roots[0]->get(1));
        }else std::cerr<<"Camera::pixelPlaneToUnitFocalPlane : No root\n";
        PolyToSolve.clear();
        roots.clear();
    }
    return true;
}

bool arlCore::Camera::pixelPlaneToUnitFocalPlane( const arlCore::Point& p, vnl_vector_fixed<double,3>& r, bool perfectDisto ) const
{
    unsigned int i;
    Point p2(3);
    bool b=pixelPlaneToUnitFocalPlane(p, p2, perfectDisto);
    for( i=0 ; i<3 ; ++i )
        r.put(i, p2[i]);
    return b;
}

bool arlCore::Camera::pixelPlaneToUnitFocalPlane( const arlCore::Point& p, vgl_point_3d<double>& r, bool perfectDisto ) const
{
    Point p2(3);
    bool b=pixelPlaneToUnitFocalPlane(p, p2, perfectDisto);
    r.set(p2.x(), p2.y(), p2.z());
    return b;
}

bool arlCore::Camera::pixelPlaneToUnitFocalPlane( const arlCore::Point& p, vgl_point_2d<double>& r, bool perfectDisto ) const
{
    Point p2(3);
    bool b=pixelPlaneToUnitFocalPlane(p, p2, perfectDisto);
    r.set(p2.x(), p2.y());
    return b;
}

bool arlCore::Camera::pixelPlaneToUnitFocalPlane( const vgl_point_2d<double>& p, vgl_point_3d<double>& r, bool perfectDisto ) const
{
    return pixelPlaneToUnitFocalPlane(Point(p.x(), p.y()), r, perfectDisto);
}

bool arlCore::Camera::focalFrameToExtrinsicFrame( const vnl_vector_fixed< double, 3 >& uff, vnl_vector_fixed< double, 3 >& ef ) const
{
    vnl_vector<double> point3DRepCam(4), point3D_H(4);
    point3D_H.put(0,uff(0));
    point3D_H.put(1,uff(1));
    point3D_H.put(2,uff(2));
    point3D_H.put(3,1.0);
    point3DRepCam = getInvExtrinsic() * point3D_H;
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        ef.put(i, point3DRepCam(i));
    return true;
}

bool arlCore::Camera::focalFrameToExtrinsicFrame( const vgl_point_3d< double >& uff, vgl_point_3d< double >& ef ) const
{
    vnl_vector<double> point3DRepCam(4), point3D_H(4);
    point3D_H.put(0,uff.x());
    point3D_H.put(1,uff.y());
    point3D_H.put(2,uff.z());
    point3D_H.put(3,1.0);
    point3DRepCam = getInvExtrinsic() * point3D_H;
    ef.set(point3DRepCam(0), point3DRepCam(1), point3DRepCam(2));
    return true;
}





bool arlCore::Camera::focalToPixelLine( double a, double b, double c, arlCore::Point &pt1, arlCore::Point &pt2 ) const
{
    // FIXME : Cas gal actuel : Droite relativement horizontale : Intersection avec les bords gauche et droit de la ROI
    // Traiter les autres cas : Intersection avec les bords haut et bas, pour les 2 points
    // Soient G, bord gauche, D, droit, H Haut, B, Bas
    // 7 cas : G-D G-H G-B H-B B-H H-D B-D, cas déjà traité : G-D
    // Traiter d'urgence H-B B-H

    // Si l'equation de la droite dans le plan focal est ax+by+c=0
    // x = (u -u0)/alpha_u    et  y = (v -v0)/alpha_v
    // calcul de l'ordonnée pour u=pt1.x
    pt1.y( getfy()/b *(-1*a*pt1.x()/getfx() + a*getcx()/getfx() +b*getcy()/getfy() - c) );
    // calcul de l'ordonnée pour u=pt2.x
    pt2.y( getfy()/b *(-1*a*pt2.x()/getfx() + a*getcx()/getfx() +b*getcy()/getfy() - c) );
    return true;
}

bool arlCore::Camera::focalToPixelLine( double a, double b, double c, const arlCore::Point &pixOrg, const arlCore::Point &pixDst, PointList &pl, unsigned int nbSegments ) const
{
    const bool PerfectDisto = true;
    unsigned int i, n=nbSegments;
    pl.clear();
    vgl_point_2d<double> focOrg, focDst, pixCur;
    if(!pixelPlaneToUnitFocalPlane( pixOrg, focOrg, PerfectDisto )) return false;
    if(!pixelPlaneToUnitFocalPlane( pixDst, focDst, PerfectDisto )) return false;
    vnl_vector_fixed<double,2> AB( focDst.x()-focOrg.x(), focDst.y()-focOrg.y() );
    if(n<1)
    {
        const double DeltaX = fabs(pixDst.x()-pixOrg.x());
        const double DeltaY = fabs(pixDst.y()-pixOrg.y());
        if(DeltaX>DeltaY) n = (unsigned int)DeltaX;
        else n = (unsigned int)DeltaY;
        if( n<1 ) return false;
    }
    const double Step = ((double)AB.two_norm()) / (double)n;
    double next = Step;
    const vnl_vector_fixed<double,2> N = AB.normalize();
    pl.push_back(pixOrg);
    for( i=0 ; i<n-1 ; ++i, next+=Step )
    {
        vgl_point_2d<double> focCur( focOrg.x()+next*N[0], focOrg.y()+next*N[1] );
        if(!unitFocalPlaneToPixelPlane( focCur, pixCur )) return false;
        pl.push_back( Point(pixCur.x(), pixCur.y()) );
    }
    pl.push_back(pixDst);
    return true;
}

bool arlCore::Camera::pixelToFocalLine( const vgl_line_2d<double> &line2D, vgl_line_3d_2_points<double> &line3D ) const
{
    const bool PerfectDisto = true;
    const vgl_vector_2d<double> Dir = line2D.direction();
    const vgl_homg_line_2d<double> HLine(line2D);
    const vgl_homg_point_2d<double> Center( getcx(), getcy() );
    const vgl_homg_point_2d<double> Central2DHPoint = vgl_homg_operators_2d<double>::closest_point( HLine, Center );
    const vgl_point_2d<double> Central2DPoint( Central2DHPoint );
    vgl_point_3d<double> central3DPoint;
    if(!pixelPlaneToUnitFocalPlane( Central2DPoint, central3DPoint, PerfectDisto )) return false;
    assert(central3DPoint.z()==1.0);
    vgl_point_3d<double> second3DPoint(central3DPoint);
    second3DPoint.set(second3DPoint.x()+Dir.x(), second3DPoint.y()+Dir.y(), 1.0);
    line3D = vgl_line_3d_2_points<double>( central3DPoint, second3DPoint );
    return false;
}

bool arlCore::getEpipolar(const arlCore::Point &p, const vnl_matrix_fixed<double,4,4> &PM, double &a, double &b, double &c)
{
    if(p.size()<2) return false;
    vnl_vector_fixed<double,3> v(p.x(),p.y(),1.0);
    return getEpipolar( v, PM, a, b, c );
}

bool arlCore::getEpipolar(const vnl_vector_fixed<double,3> &point, const vnl_matrix_fixed<double,4,4> &PM, double &a, double &b, double &c)
{   // PM=cams[1]->getExtrinsic()).as_matrix()*vnl_matrix_inverse<double>(cams[0]->getExtrinsic());
    // Calcul la droite épipolaire dans la camera 1 du point de la caméra 0
    // cf livre horaud p192 - La droite se situe dans le plan focal (x,y,1.0)
    const double x = point(0);
    const double y = point(1);
    const double a1 = (PM(1,3)*PM(2,0) - PM(2,3)*PM(1,0));
    const double a2 = (PM(1,3)*PM(2,1) - PM(2,3)*PM(1,1));
    const double a3 = (PM(1,3)*PM(2,2) - PM(2,3)*PM(1,2));
    a = a1*x + a2*y + a3;
    const double b1 = (PM(2,3)*PM(0,0) - PM(0,3)*PM(2,0));
    const double b2 = (PM(2,3)*PM(0,1) - PM(0,3)*PM(2,1));
    const double b3 = (PM(2,3)*PM(0,2) - PM(0,3)*PM(2,2));
    b = b1*x + b2*y + b3;
    const double c1 = (PM(0,3)*PM(1,0) - PM(1,3)*PM(0,0));
    const double c2 = (PM(0,3)*PM(1,1) - PM(1,3)*PM(0,1));
    const double c3 = (PM(0,3)*PM(1,2) - PM(1,3)*PM(0,2));
    c = c1*x + c2*y + c3;
    return true;
}

bool arlCore::getEpipolar(const arlCore::Point &p, const vnl_matrix_fixed<double,4,4> &PM, vgl_line_2d< double > &d)
{
    if(p.size()<2) return false;
    vnl_vector_fixed<double,3> v(p.x(),p.y(),1.0);
    return getEpipolar( v, PM, d );
}

bool arlCore::getEpipolar(const vnl_vector_fixed<double,3> &point, const vnl_matrix_fixed<double,4,4> &PM, vgl_line_2d< double > &d)
{
    double a, b, c;
    arlCore::getEpipolar(point, PM, a, b, c);
    vgl_point_2d< double > vglPts2D[2];
    if( b<-0.5 || b>0.5 )
    {
        vglPts2D[0].set(-100, (a*100.0-c)/b);
        vglPts2D[1].set(+100, (-a*100-c)/b);
    }
    else
    {
        vglPts2D[0].set((b*100-c)/a, -100);
        vglPts2D[1].set((-b*100-c)/a, +100);
    }
    vgl_line_2d< double > d1(vglPts2D[0],vglPts2D[1]);
    d=d1;
    return true;
}

// Methode : le vecteur [viewPoint;center] définit l'axe Z du repere lie au centre optique
// de la camera. Pour choisir les axes X et Y (y en a pas de bon par defaut), il faut choisir
// un vecteur dans le plan normal a [viewPoint;center] passant par viewPoint
// l'equation de ce plan normal est ax+by+cz+d = 0
// ou a = coord selon x du vecteur [viewPoint;center]
// et b = coord selon y du vecteur [viewPoint;center]
// et c = coord selon z du vecteur [viewPoint;center]
// et d = -( a*viewPoint(x) + b*viewPoint(y) + c*viewPoint(z) )
//
// Ensuite on choisit arbitrairement un vecteur dans ce plan pour definir l'axe X
// du repere centre optique : pour cela on choisit x=1 (si a!=0), y=1 (si b!=0)
// z= (-d - ax - by)/c . Ca nous fournit un vecteur qu'on normalise.
// On a donc definit un repere lie au centre optique. Reste a calculer la transformation
// rigide entre ce repere et le repere monde. Pour cela, on effectue un recalage 3D3D
// entre deux reperes. Simpliste !
bool arlCore::Camera::syntheticCamera( const arlCore::Point& center, const arlCore::Point& viewPoint, const std::vector< double > & intrinsicParams)
{
    setOK(false);
    assert(intrinsicParams.size()==10);
    arlCore::Point XaxisPoint(3), ZaxisPoint(3);
    arlCore::Point O_monde(0,0,0), Ox_monde(1,0,0), Oz_monde(0,0,1);
    std::vector< const arlCore::Point* > points_source, points_cible;
    vnl_vector_fixed<double,3> vecteur_normal, vecteur_Ox_camera;
    double d=0, test_egalite;
    unsigned int i;
    test_egalite = (center.x()-viewPoint.x())*(center.x()-viewPoint.x())
                 + (center.y()-viewPoint.y())*(center.y()-viewPoint.y())
                 + (center.z()-viewPoint.z())*(center.z()-viewPoint.z());
    if( test_egalite < 1e-4 ) //TODO si c des mm alors on devrait mettre 1 mm
        return false;
    // a<=>vecteur_normal[0], b<=>vecteur_normal[1] et c<=>vecteur_normal[2]
    for( i=0 ; i<3 ; ++i )
    {
        vecteur_normal.put(i,center[i] - viewPoint[i]);
        d += -vecteur_normal[i]*viewPoint[i];
    }
    //std::cerr << "vecteur_normal = " <<  vecteur_normal << std::endl;
    //std::cerr << "d = " <<  d << std::endl;
    XaxisPoint.fill(1.0);
    if( vecteur_normal[0] != 0 )
        XaxisPoint.set(0, (-d-vecteur_normal[1]-vecteur_normal[2])/ vecteur_normal[0]);
    else // Le vecteur normal est parallele au plan yOz du repere monde
        if( vecteur_normal[1] != 0 )
            XaxisPoint.set(1,(-d-vecteur_normal[2])/vecteur_normal[1]);
        else // Le vecteur normal est parallele a l'axe Oz du repere monde
            XaxisPoint.set(2,-d/vecteur_normal[2]);
    // Creation du vecteur Ox dans repere optique
    for( i=0 ; i<3 ; ++i )
        vecteur_Ox_camera.put(i, XaxisPoint[i]-viewPoint[i]);
    vecteur_Ox_camera.normalize();
    vecteur_normal.normalize();
    //std::cerr << "vecteur_Ox_camera normalise = " <<  vecteur_Ox_camera << std::endl;
    for( i=0 ; i<3 ; ++i )
    {
        XaxisPoint.set(i, viewPoint[i]+vecteur_Ox_camera[i]);
        // Calcul du point sur l'axe z du repere optique eloigne de 1 unite du centre optique
        ZaxisPoint.set(i, viewPoint[i]+vecteur_normal[i]);
    }
    //std::cerr << "XaxisPoint = " <<  XaxisPoint.getString() << std::endl;
    //std::cerr << "vecteur_normal normalise = " <<  vecteur_normal << std::endl;
    //std::cerr << "ZaxisPoint = " <<  ZaxisPoint.getString() << std::endl;
    // On a maintenant 3 points du repere optique (center, XaxisPoint, ZaxisPoint)
    // a mettre en correspondance avec 3 points du repere monde
    // Le recalage de ces 3 points nous fournit la transformation extrinseque recherchee
    points_source.push_back(&O_monde);
    points_source.push_back(&Ox_monde);
    points_source.push_back(&Oz_monde);
    points_cible.push_back(&viewPoint);
    points_cible.push_back(&XaxisPoint);
    points_cible.push_back(&ZaxisPoint);
    vnl_rigid_matrix extrinsic_trsf;
    extrinsic_trsf.register3D3D( points_cible, points_source, false );
    //initialisation des parametres de la camera
    setExtrinsic(extrinsic_trsf);
    setfx(intrinsicParams[0]);
    setfy(intrinsicParams[1]);
    setcx(intrinsicParams[2]);
    setcy(intrinsicParams[3]);
    for( i=0 ; i<5 ; ++i )
        setkc(i,intrinsicParams[4+i]);
    setAlphaC(intrinsicParams[9]);
    return setOK(true);
}
