/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Misc.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif //_WIN32

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fstream>

#include <vcl/vcl_limits.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_cross.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_powell.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>

#include <arlcore/vnl_rigid_vector.h>
#include <arlcore/Optimization.h>
#include <arlcore/PointsList.h>
#include <arlcore/MatrixR.h>
#include <arlcore/File.h>

unsigned int arlCore::fillGrid( arlCore::PointList &pl, unsigned int nbPointsX, double squareSizeX, unsigned int nbPointsY, double squareSizeY, unsigned int nbPointsZ, double squareSizeZ )
{
/*  case  72 : chess[0]= 9;chess[1]=10;chess[2]= 4;chess[3]= 4;break;
    case 132 : chess[0]=12;chess[1]=13;chess[2]=10;chess[3]=10;break;
    case 140 : chess[0]=11;chess[1]=15;chess[2]=10;chess[3]=10;break;
    case 192 : chess[0]=13;chess[1]=17;chess[2]=20;chess[3]=20;break;
    case 462 : chess[0]=23;chess[1]=22;chess[2]=10;chess[3]=10;break;
    case 418 : chess[0]=23;chess[1]=20;chess[2]=10;chess[3]=10;break;
    case 825 : chess[0]=26;chess[1]=34;chess[2]=10;chess[3]=10;break;
    // Mires robotique
    case 667 : chess[0]=30;chess[1]=24;chess[2]=5;chess[3]=5;break;
    case 154 : chess[0]=12;chess[1]=15;chess[2]=10;chess[3]=10;break;*/
    pl.clear();
    pl.setDimension(3);
    unsigned int x, y, z;
    for( z=0 ; z<nbPointsZ ; ++z )
        for( y=0 ; y<nbPointsY ; ++y )
            for( x=0 ; x<nbPointsX ; ++x )
                pl.push_back(x*squareSizeX, y*squareSizeY, z*squareSizeZ);
    return pl.size();
}

bool arlCore::plot( const std::vector< const arlCore::PointList* > &pl, const std::string &options )
{
    if(pl.size()<1) return false;
    const bool JustVisible = false;
    const bool Overwrite = true;
    const std::string Title = "PointList";
    unsigned int i;
    std::fstream file;
    file.open (TMP_FILE, std::fstream::out);
    if(!file.is_open()) return false;
    file<<"set title '"<<Title<<"'\n";
    file<<options<<"\n";
    if(pl[0]->getDimension()==3) file<<"s";
    file<<"plot";
    for( i=0 ; i<pl.size() ; ++i )
    {
        std::stringstream ss;
        ss<<"Gnuplot-"<<i<<".dat";
        if(pl[i]) pl[i]->save(ss.str(), ARLCORE_POINT_SAVE_GNUPLOT, JustVisible, Overwrite);
        if(i==0) file<<" "; else file<<",";
        file<<"\""<<ss.str()<<"\"";
    }
    file<<"\npause -1 \"Hit return to continue\"\n";
    file.close();
    system(GNUPLOT_EXE);
    //exec(GNUPLOT_EXE, TMP_FILE );
    return true;
}

unsigned int arlCore::nbPolynomialParameters( unsigned int degree, unsigned int nbEquations )
{
    return (degree+1)*(degree+2)*(degree+3)*nbEquations/6;
}

bool arlCore::computePolynomial( unsigned int degree, const vnl_vector<double> &parameters, const vnl_vector<double> &data, vnl_vector<double> &result )
{
    if(degree<1) return false;
    const unsigned int NbEquations = 3;
    const unsigned int NbParameters = nbPolynomialParameters(degree, NbEquations);
    if(NbParameters!=parameters.size()) return false;
    if(data.size()!=NbEquations) return false;
    result.set_size(NbEquations);
    result.fill(0.0);
    unsigned int i, j, k, l, m=0;
    const vnl_vector<double> &C = parameters;
    for( i=0 ; i<=degree ; ++i )
        for( j=0 ; j<=degree ; ++j )
            for( k=0 ; k<=degree ; ++k )
                if( i+j+k<=degree )
                {
                    for( l=0 ; l<NbEquations ; ++l )
                    {
                        assert(m+l<NbParameters);
                        result[l] += parameters[m+l]*pow(data[0],(int)i)*pow(data[1],(int)j)*pow(data[2],(int)k);
                    }
                    m += NbEquations;
                }
    return true;
}

double arlCore::vgl_distance_point( const vgl_line_3d_2_points<double> &line, const vgl_point_3d<double> &p0 )
{   //http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    vgl_point_3d<double> p1 = line.point1();
    vgl_point_3d<double> p2 = line.point2();
    vnl_vector_fixed<double,3> x0, x1, x2;
    x0[0]=p0.x(), x0[1]=p0.y(), x0[2]=p0.z();
    x1[0]=p1.x(), x1[1]=p1.y(), x1[2]=p1.z();
    x2[0]=p2.x(), x2[1]=p2.y(), x2[2]=p2.z();
    vnl_vector_fixed<double,3> a = vnl_cross_3d(x2-x1, x1-x0);
    vnl_vector_fixed<double,3> b = x2-x1;
    return a.two_norm()/b.two_norm();
}

vgl_line_2d<double> arlCore::vglLine2d( double slope, double rho )
{
    // FIXME Slope = slope-Pi/2
    const double a = cos(slope), b = sin(slope);
    const double x0 = a*rho, y0 = b*rho;
    vgl_point_2d<double> p1(x0 + 100.0*(-b), y0 + 100.0*(a));
    vgl_point_2d<double> p2(x0 - 100.0*(-b), y0 - 100.0*(a));
    return vgl_line_2d<double>(p1, p2);
}

unsigned int arlCore::computeNeedleROI( const Point &Tip, const Point &Top, unsigned int width, std::vector<arlCore::Point> &corners, unsigned int shape )
{   // shape 0=Rectangular shape 1=Triangular
    Point corner;
    corners.clear();
    const arlCore::Point Center((Tip.x()+Top.x())/2.0, (Tip.y()+Top.y())/2.0);
    vgl_vector_2d<double> tip2top(Top.x()-Tip.x(), Top.y()-Tip.y());
    vgl_vector_2d<double> rot = rotated(tip2top, vnl_math::pi/2.0);
    const vgl_vector_2d<double> Tip2corner = width * normalize(rot);
    // First corner, orthogonal with the axis of the needle (Tip Right)
    corner.x(Tip.x() + Tip2corner.x_);
    corner.y(Tip.y() + Tip2corner.y_);
    corners.push_back(corner);
    // Second corner, central symmetry of first corner with Tip (Tip Left)
    corner.x(2.0*Tip.x() - corners[0].x());
    corner.y(2.0*Tip.y() - corners[0].y());
    corners.push_back(corner);
    switch(shape)
    {
        case 0: // Rectangular shape
            // Third corner, central symmetry of first corner with Center of line Tip-Top (Top Left)
            corner.x(2.0*Center.x() - corners[0].x());
            corner.y(2.0*Center.y() - corners[0].y());
            corners.push_back(corner);
            // Fourth corner, central symmetry of 2st corner with Center of line Tip-Top (Top Right)
            corner.x(2.0*Center.x() - corners[1].x());
            corner.y(2.0*Center.y() - corners[1].y());
            corners.push_back(corner);
            break;
        case 1: // Triangular shape
            // Third corner, Top (Top Center)
            corner.x(Top.x());
            corner.y(Top.y());
            corners.push_back(corner);
            break;
    }
    return (unsigned int)corners.size();
}
/*
double arlCore::computeVideo1Robot( const std::vector< arlCore::vnl_rigid_matrix > &M06, const PointList &tip, vnl_rigid_matrix &T, double &distance, double &stdDev )
{
    OptimiseVideoRobot1 OVR( M06, tip );
    vnl_powell computeT( &OVR );
    vnl_rigid_vector V( T );
    vnl_vector<double> solution(6);
    unsigned int i;
    for( i=0 ; i<6 ; ++i )
        solution[i] = V[i];
    computeT.minimize( solution );
    distance = OVR.getDistance();
    stdDev = OVR.getStdDev();
    for( i=0 ; i<6 ; ++i )
        V[i] = solution[i];
    T = V;
    return distance;
}*/

double arlCore::computeVideo2RobotX(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &Z,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        arlCore::vnl_rigid_matrix &X )
{
    const bool Verbose = false;
    unsigned int i, j;
    // POWELL VERSION
//  arlCore::OptimiseVideoRobotX OVR( M06, Z, model3D, cams, points2DList );
//  vnl_powell Optimizer( &OVR );
    // LEVENBERG MARQUARDT VERSION
    unsigned int number_of_residuals = 0;
    for( i=0 ; i<model3D.size() ; ++i )
        for( j=0 ; j<model3D[i].size() ; ++j )
            number_of_residuals += model3D[i][j].size();
    arlCore::OptimiseVideoRobotX_LS OVR( M06, Z, model3D, cams, points2DList, number_of_residuals, vnl_least_squares_function::use_gradient );
    vnl_levenberg_marquardt Optimizer( OVR );
    Optimizer.set_trace(true);
//  Optimizer.set_check_derivatives(1);
    Optimizer.set_f_tolerance(1e-6);
    Optimizer.set_x_tolerance(1e-8);
    Optimizer.set_g_tolerance(1e-6);
    if(Verbose)
    {
        std::cerr<<"f tolerance ="<<Optimizer.get_f_tolerance()<<std::endl;
        std::cerr<<"x tolerance ="<<Optimizer.get_x_tolerance()<<std::endl;
        std::cerr<<"g tolerance ="<<Optimizer.get_g_tolerance()<<std::endl;
        std::cerr<<"check derivative="<<Optimizer.get_check_derivatives()<<std::endl;
    }
    //********
    vnl_rigid_vector V( X );
    vnl_vector<double> solution(6);
    for( i=0 ; i<6 ; ++i )
        solution[i] = V[i];
    //Optimizer.minimize( solution ); // POWELL VERSION
    Optimizer.minimize_using_gradient( solution ); // LM VERSION
    const double StartError = Optimizer.get_start_error();
    const double EndError = Optimizer.get_end_error();
    const double NbEvaluations = Optimizer.get_num_evaluations();
    const double GetError = OVR.getError();
    OVR.criterion(X,Z);
    const double Criterion = OVR.getError();
    if(Verbose)
    {
        std::cout<<"Start error = "<<StartError<<"\n";
        std::cout<<"End error = "<<EndError<<"\n";
        std::cout<<"Get error = "<<GetError<<"\n";
        std::cout<<"Criterion = "<<Criterion<<"\n";
        std::cout<<"Nb evaluations = "<<NbEvaluations<<"\n";
    }
    for( i=0 ; i<6 ; ++i )
        V[i] = solution[i];
    X = V;
    return Criterion;
}

double arlCore::computeVideo2RobotZ(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &X,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        arlCore::vnl_rigid_matrix &Z )
{
    const bool Verbose = false;
    unsigned int i, j;
    // POWELL VERSION
//  arlCore::OptimiseVideoRobotZ OVR( M06, X, model3D, cams, points2DList );
//  vnl_powell Optimizer( &OVR );
    // LEVENBERG MARQUARDT VERSION
    unsigned int number_of_residuals = 0;
    for( i=0 ; i<model3D.size() ; ++i )
        for( j=0 ; j<model3D[i].size() ; ++j )
            number_of_residuals += model3D[i][j].size();
    arlCore::OptimiseVideoRobotZ_LS OVR( M06, X, model3D, cams, points2DList, number_of_residuals, vnl_least_squares_function::use_gradient );
    vnl_levenberg_marquardt Optimizer( OVR );
    Optimizer.set_trace(true);
//  Optimizer.set_check_derivatives(1);
    Optimizer.set_f_tolerance(1e-6);
    Optimizer.set_x_tolerance(1e-8);
    Optimizer.set_g_tolerance(1e-6);
    if(Verbose)
    {
        std::cerr<<"f tolerance ="<<Optimizer.get_f_tolerance()<<std::endl;
        std::cerr<<"x tolerance ="<<Optimizer.get_x_tolerance()<<std::endl;
        std::cerr<<"g tolerance ="<<Optimizer.get_g_tolerance()<<std::endl;
        std::cerr<<"check derivative="<<Optimizer.get_check_derivatives()<<std::endl;
    }
    //********
    vnl_rigid_vector V( Z );
    vnl_vector<double> solution(6);
    for( i=0 ; i<6 ; ++i )
        solution[i] = V[i];
    //Optimizer.minimize( solution ); // POWELL VERSION
    Optimizer.minimize_using_gradient( solution ); // LM VERSION
    const double StartError = Optimizer.get_start_error();
    const double EndError = Optimizer.get_end_error();
    const double NbEvaluations = Optimizer.get_num_evaluations();
    const double GetError = OVR.getError();
    OVR.criterion(X,Z);
    const double Criterion = OVR.getError();
    if(Verbose)
    {
        std::cout<<"Start error = "<<StartError<<"\n";
        std::cout<<"End error = "<<EndError<<"\n";
        std::cout<<"Get error = "<<GetError<<"\n";
        std::cout<<"Criterion = "<<Criterion<<"\n";
        std::cout<<"Nb evaluations = "<<NbEvaluations<<"\n";
    }
    for( i=0 ; i<6 ; ++i )
        V[i] = solution[i];
    Z = V;
    return Criterion;
}

double arlCore::computeVideo2RobotXZ(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        arlCore::vnl_rigid_matrix &X,
        arlCore::vnl_rigid_matrix &Z )
{
    const bool Verbose = false;
    unsigned int i, j;
    // POWELL VERSION
//  arlCore::OptimiseVideoRobotXZ OVR( M06, model3D, cams, points2DList );
//  vnl_powell Optimizer( &OVR );

    // LEVENBERG MARQUARDT VERSION
    unsigned int number_of_residuals = 0;
    for( i=0 ; i<model3D.size() ; ++i )
        for( j=0 ; j<model3D[i].size() ; ++j )
            number_of_residuals += model3D[i][j].size();
    arlCore::OptimiseVideoRobotXZ_LS OVR( M06, model3D, cams, points2DList, number_of_residuals, vnl_least_squares_function::use_gradient );
    vnl_levenberg_marquardt Optimizer( OVR );
    Optimizer.set_trace(true);
//  Optimizer.set_check_derivatives(1);
    Optimizer.set_f_tolerance(1e-6);
    Optimizer.set_x_tolerance(1e-8);
    Optimizer.set_g_tolerance(1e-6);
    if(Verbose)
    {
        std::cerr<<"f tolerance ="<<Optimizer.get_f_tolerance()<<std::endl;
        std::cerr<<"x tolerance ="<<Optimizer.get_x_tolerance()<<std::endl;
        std::cerr<<"g tolerance ="<<Optimizer.get_g_tolerance()<<std::endl;
        std::cerr<<"check derivative="<<Optimizer.get_check_derivatives()<<std::endl;
    }
    //********
    vnl_rigid_vector VX( X );
    vnl_rigid_vector VZ( Z );
    vnl_vector<double> solution(12);
    for( i=0 ; i<6 ; ++i )
    {
        solution[i] = VX[i];
        solution[i+6] = VZ[i];
    }
    //Optimizer.minimize( solution ); // POWELL VERSION
    Optimizer.minimize_using_gradient( solution ); // LM VERSION
    for( i=0 ; i<6 ; ++i )
    {
        VX[i] = solution[i];
        VZ[i] = solution[i+6];
    }
    X = VX;
    Z = VZ;
    const double StartError = Optimizer.get_start_error();
    const double EndError = Optimizer.get_end_error();
    const double NbEvaluations = Optimizer.get_num_evaluations();
    const double GetError = OVR.getError();
    OVR.criterion(X,Z);
    const double Criterion = OVR.getError();
    if(Verbose)
    {
        std::cout<<"Start error = "<<StartError<<"\n";
        std::cout<<"End error = "<<EndError<<"\n";
        std::cout<<"Get error = "<<GetError<<"\n";
        std::cout<<"Criterion = "<<Criterion<<"\n";
        std::cout<<"Nb evaluations = "<<NbEvaluations<<"\n";
    }
    return Criterion;
}

// arlRandom
std::vector< double> arlRandom::Random::m_randomTable;
long int arlRandom::Random::m_index = 0;

long int arlRandom::Random::initRandom( const std::string &filename, long int max )
{
    m_randomTable.clear();
    m_index = 0;
    std::fstream file;
    file.open( filename.c_str(), std::fstream::in);
    if(file.is_open())
    {
        double value = 0;
        while(!file.eof() && (max<=0 || m_randomTable.size()<max))
        {
            file>>value;
            m_randomTable.push_back(value);
//          std::cout<<m_randomTable.size()<<" "<<value<<"\n";
        }
        file.close();
    }
    if(m_randomTable.size()>0) return 0;
    else return privateRndInit();
}

long int arlRandom::Random::initRandom( long int seed )
{
    m_randomTable.clear();
    return privateRndInit( seed );
}

double arlRandom::Random::uniformDoubleRnd( double min, double max )
{
    return privateUniformDoubleRnd( min, max );
}

int arlRandom::Random::uniformIntRnd( int min, int max )
{
    return privateUniformIntRnd( min, max );
}

double arlRandom::Random::gaussianRnd( double std )
{
    return privateGaussianRnd( std );
}

unsigned int arlRandom::Random::generate( long int seed, const std::string &filename, long int n )
{
    if(n<1) return 0;
    std::ofstream file;
    file.open(filename.c_str(), std::fstream::out);
    if(!file.is_open()) return 0;
    srand(seed);
    long int i;
    for( i=0 ; i<n-1 ; ++i )
        file<<rand()<<"\n";
    file<<rand();
    file.close();
    return n;
}

double arlRandom::Random::privateRand( void )
{
    if(m_randomTable.size()>0)
    {
        const double Value = m_randomTable[m_index];
        ++m_index;
        if(m_index>=(long int)m_randomTable.size()) m_index = 0;
        return Value;
    }else return rand();
}

long int arlRandom::Random::privateRndInit(long int seed)
{   // TODO : Remplacer par vnl_random
    long int f;
    if(seed==0) f=(long int)time(NULL);
    else f=seed;
    srand(f);
    return f;
}

double arlRandom::Random::privateUniformDoubleRnd( double min, double max )
{
    assert(min<=max);
    const double r = privateRand();
    const double d = RAND_MAX ;//+ 1.0;
    const double rnd = min+(max-min)*r/d;
    assert(rnd>=min && rnd<=max);
    return rnd;
}

int arlRandom::Random::privateUniformIntRnd( int min, int max )
{
    assert(min<=max);
    const double r = uniformDoubleRnd( (double)min, (double)max );
    int rnd;
    if(r<0) rnd = (int)ceil(r-0.5);
    else rnd = (int)floor(r+0.5);
    assert(rnd>=min && rnd<=max);
    return rnd;
}

double arlRandom::Random::privateGaussianRnd( double std )
{
// cde essaye avec la librairie libpdf
// vnl_vector<double> noise_sample( 1 );
// pdf1d_gaussian noise_model(mean, variance);
// pdf1d_gaussian_sampler noise_generator;
// noise_generator.set_model(noise_model);
// noise_generator.get_samples(noise_sample);
// return noise_sample(0);
    return(std*sqrt(-2.0*log(uniformDoubleRnd(1e-15,1.0)))*cos(2*vnl_math::pi*uniformDoubleRnd(1e-15,1.0)));
}

unsigned int arlRandom::randomIndex( unsigned int n, std::vector<unsigned int> &v, unsigned int m )
{
    unsigned int i, tmp, r;
    if(m<n && m>0) r=m+1;   else r=n;
    const unsigned int Max = n-1;
    v.clear();
    for( i=0 ; i<n ; ++i )
        v.push_back(i);
    for( i=0 ; i<r-1 ; ++i )
    {
        unsigned int index = arlRandom::Random::uniformIntRnd( i, Max );
        if(index>i)
        {
            tmp = v[i];
            v[i] = v[index];
            v[index] = tmp;
        }
    }
    return n;
}

// arlTime
long int arlTime::getNowDate( void )
{   // TODO : use pt::now();
    return (long int)time(NULL);
}

long int arlTime::getNowTime( void )
{   // TODO : use pt::now();
    return (long int)((double)clock() / (double)CLK_TCK * 10000);
}

bool arlTime::sleep( double time )
{
#ifdef _WIN32
    Sleep(time);
#else
    sleep(time);
#endif
    return true;
}

// arlString
unsigned int arlString::replace( std::string &s, char org, char dst )
{
    unsigned int n=0;
    if(org==dst) return n;
    size_t found;
    std::stringstream orgS, dstS;
    orgS<<org;
    dstS<<dst;
    found=s.find_last_of(orgS.str());
    while(found!=std::string::npos)
    {
        s.replace(found,1,dstS.str());
        found=s.find_last_of(orgS.str());
    }
    return n;
}

bool arlString::splitFilename( const std::string &path, std::string &folder, std::string &fullFileName, std::string &ext )
{
    size_t found;
    found=path.find_last_of("/\\");
    if(found==std::string::npos)
    {
        folder = "";
        fullFileName = path;
    }
    else
    {
        folder = path.substr(0,found);
        fullFileName = path.substr(found+1);
    }
    found=fullFileName.find_last_of(".");
    if(found==std::string::npos) ext = "";
    else ext = fullFileName.substr(found+1);
    return true;
}

bool arlString::extractExt( const std::string &path, std::string &ext )
{
    size_t found;
    found=path.find_last_of(".");
    if(found==std::string::npos) ext = "";
    else ext = path.substr(found+1);
    return true;
}

void arlString::getFullDateString( long int date, std::string &s, char separator )
{
    char name[19+1];
    time_t tt = (time_t)date;
    struct tm *t = localtime(&tt);
    // TODO Use setfill ('0')<<setw(2) with std::stringstream
    sprintf(name,"%04d%c%02d%c%02d%c%02d%c%02d%c%02d", t->tm_year+1900, separator, t->tm_mon+1, separator, t->tm_mday, separator, t->tm_hour, separator, t->tm_min, separator, t->tm_sec);
    s = name;
}

void arlString::getDateString( long int date, std::string &s, char separator )
{
    char name[10+1];
    time_t tt = (time_t)date;
    struct tm *t = localtime(&tt);
    // TODO Use setfill ('0')<<setw(2) with std::stringstream
    sprintf(name,"%04d%c%02d%c%02d", t->tm_year+1900, separator, t->tm_mon+1, separator, t->tm_mday);
    s = name;
}

void arlString::getTimeString( long int date, std::string &s, char separator )
{
    char name[8+1];
    time_t tt = (time_t)date;
    struct tm *t = localtime(&tt);
    // TODO Use setfill ('0')<<setw(2) with std::stringstream
    sprintf(name,"%02d%c%02d%c%02d", t->tm_hour, separator, t->tm_min, separator, t->tm_sec);
    s = name;
}

template<typename T>
bool convert(const std::string &s, T &obj)
{
    std::istringstream is(s);
    return is >> obj;
}

bool arlString::valueOf(const std::string &s, unsigned int &obj)
{
    return convert(s,obj);
}

bool arlString::valueOf(const std::string &s, int &obj)
{
    return convert(s,obj);
}

bool arlString::valueOf(const std::string &s, double &obj)
{
    return convert(s,obj);
}

unsigned int arlString::extractDoubles( const std::string &s, std::vector<double> &v, char separator )
{
    v.clear();
    if(s=="") return 0;
    unsigned int pos = (unsigned int)s.find_last_of(separator);
    if( pos==std::string::npos ) pos=0; else pos++;
    if( pos>0 ) extractDoubles(s.substr(0,pos-1),v,separator);
    // TODO Use convert
    double d;
    if( pos<s.length() && convert(s.substr(pos),d) ) v.push_back(d);
    //if(pos<s.length()) v.push_back(atof(s.substr(pos).c_str()));
    return (unsigned int)v.size();
}

unsigned int arlString::extractInts( const std::string &s, std::vector<int> &v, char separator )
{
    v.clear();
    std::vector<double> vd;
    unsigned int i, n=extractDoubles(s,vd,separator);
    for( i=0 ; i<n ; i++ )
        v.push_back((int)vd[i]);
    return n;
}

unsigned int arlString::extractUInts( const std::string &s, std::vector<unsigned int> &v, char separator )
{
    v.clear();
    std::vector<double> vd;
    unsigned int i, n=extractDoubles(s,vd,separator);
    for( i=0 ; i<n ; i++ )
        v.push_back((unsigned int)vd[i]);
    return n;
}

unsigned int arlString::extractBools( const std::string &s, std::vector<bool> &v, char separator )
{
    v.clear();
    std::vector<double> vd;
    unsigned int i, n=extractDoubles(s,vd,separator);
    for( i=0 ; i<n ; i++ )
        v.push_back(vd[i]>0);
    return n;
}

bool arlString::extractIP( const std::string &s, unsigned int &IP1, unsigned int &IP2, unsigned int &IP3, unsigned int &IP4 )
{
    std::vector<unsigned int> v;
    unsigned int n=extractUInts(s,v,'.');
    if(n!=4) return false;
    IP1 = v[0];
    IP2 = v[1];
    IP3 = v[2];
    IP4 = v[3];
    return true;
}

bool arlString::extractIP( const std::string &s, std::string &IP, unsigned int &port )
{   // "192.168.3.69/8089"
    if(s=="") return false;
    unsigned int IP1, IP2, IP3, IP4, p=port, pos = (unsigned int)s.find_last_of('/');
    if( pos==std::string::npos ) IP = s;
    else
    {
        IP = s.substr(0,pos);
        if(valueOf(s.substr(pos+1), p)) port=p;
    }
    return(extractIP(IP, IP1, IP2, IP3, IP4));
}

// arlFile
bool arlFile::fileExist( const std::string &s )
{
    std::ifstream file;
    file.open(s.c_str(), std::ifstream::in);
    bool b=file.is_open();
    file.close();
    return b;
}

bool arlFile::mkdir( const std::string &s )
{
    std::string dir = s;
    std::stringstream ss;
    //FIXME for LINUX version
    arlString::replace( dir, '/', '\\' );
    ss<<"mkdir "<<dir;
    system(ss.str().c_str());
    return true;
}

bool arlFile::fileCopy( const std::string &OriginFilename, const std::string &DestinationFilename )
{
    std::string f1 = OriginFilename, f2 = DestinationFilename;
    std::stringstream ss;
    //FIXME for LINUX version
    arlString::replace( f1, '/', '\\' );
    arlString::replace( f2, '/', '\\' );
    ss<<"copy "<<f1<<" "<<f2<<" /Y >system.txt";
    system(ss.str().c_str());
    return true;
/*
    std::ifstream inFile;
    inFile.open(OriginFilename.c_str(), std::ifstream::in);
    if(!inFile.is_open()) return false;
    std::ofstream outFile;
    outFile.open(DestinationFilename.c_str(), std::ifstream::out);
    if(!outFile.is_open())
    {
        inFile.close();
        return false;
    }
    inFile.seekg(0, std::ifstream::end);
    const long BufferSize = inFile.tellg();
    inFile.seekg(0);
    char* buffer = new char[BufferSize];
    inFile.read(buffer, BufferSize);
    outFile.write(buffer, BufferSize);
    outFile.close();
    inFile.close();
    delete[] buffer;
    return true;*/
}

bool arlFile::convertTrianToVtk( const std::string &trianFilename, const std::string &vtkFilename )
{
    unsigned int i, nbPts, nbCells;
    int a, b, c, d, e, f;
    double x,y,z;
    std::fstream fin, fout;
    fin.open(trianFilename.c_str(), std::fstream::in);
    if(!fin.is_open())
    {
        std::cerr<<"Trian file Loading error : "<<trianFilename<<"\n";
        return false;
    }
    fout.open(vtkFilename.c_str(), std::fstream::out);
    if(!fout.is_open())
    {
        std::cerr<<"VTK file Loading error : "<<vtkFilename<<"\n";
        return false;
    }
    fin>>nbPts;
    fout<<"# vtk DataFile Version 1.0\nFileName\nASCII\n\n";
    //fout<<DATASET UNSTRUCTURED_GRID";
    fout<<"DATASET POLYDATA";
    fout<<"\nPOINTS "<<nbPts<<" double\n";
    for( i=0 ; i<nbPts ; ++i )
    {
        fin>>x>>y>>z;
        fout<<x<<" "<<y<<" "<<z<<"\n";
    }
    fin>>nbCells;
    //fout<<"\nCELLS ";
    fout<<"\nPOLYGONS ";
    fout<<nbCells<<" "<<nbCells*4<<"\n";
    for( i=0 ; i<nbCells ; ++i )
    {
        fin>>a>>b>>c>>d>>e>>f;
        fout<<"3 "<<a<<" "<<b<<" "<<c<<"\n";
    }
/*  fout<<"\nCELLS_TYPES "<<nbCells<<"\n";
    for( i=0 ; i<nbCells ; ++i )
        fout<<"5\n";*/
    return true;
}

bool arlSound::play( double frequency, double time )
{
#ifdef _WIN32
    Beep( frequency, time );
#else //_WIN32
    std::cout<<"\a";
#endif //_WIN32
    return true;
}

unsigned int arlCore::save( const std::vector<double> &doubles, const std::string &fileName, bool overwrite )
{
    if(doubles.size()<1) return 0;
    unsigned int i=0;
    std::fstream f;
    if(arlFile::fileExist(fileName) && !overwrite) return 0;
    f.open (fileName.c_str(), std::fstream::out);
    if(f.is_open())
        for( ; i<doubles.size() ; ++i )
            f<<doubles[i]<<"\n";
    f.close();
    f.flush();
    return i;
}

unsigned int arlCore::save( const std::vector< vnl_vector<double> > &q, vnl_vector<double> ratio, const std::vector< std::string > &names, const std::string &fileName, bool overwrite )
{
    if(q.size()<1) return 0;
    unsigned int i=0;
    std::fstream f;
    if(arlFile::fileExist(fileName) && !overwrite) return 0;
    f.open (fileName.c_str(), std::fstream::out);
    if(f.is_open())
    {
        f<<"FileType Vectors\nVersion 1.0\n\n";
        f<<"Ratio "<<ratio.size()<<"\n"<<ratio<<"\n";
        for( ; i<q.size() ; ++i )
        {
            if(names.size()>i) f<<"\nName "<<names[i]<<"\n";
            f<<"Vector "<<q[i].size()<<"\n"<<q[i]<<"\n";
        }
    }
    f.close();
    f.flush();
    return i;
}

unsigned int arlCore::save( const std::vector< vnl_vector<double> > &q, const std::string &fileName, bool overwrite )
{
    if(q.size()<1) return 0;
    unsigned int i=0;
    std::fstream f;
    if(arlFile::fileExist(fileName) && !overwrite) return 0;
    f.open (fileName.c_str(), std::fstream::out);
    if(f.is_open())
    {
        f<<"FileType Vectors\nVersion 1.0\n\n";
        for( ; i<q.size() ; ++i )
            f<<"Vector "<<q[i].size()<<"\n"<<q[i]<<"\n";
    }
    f.close();
    f.flush();
    return i;
}

unsigned int arlCore::load( std::vector< vnl_vector<double> >&v, std::vector<std::string>&names, const std::string &filename )
{
    if(filename=="") return 0;
    unsigned int i;
    arlCore::File config(filename, "Vectors");
    std::string name="";
    vnl_vector<double> q, ratio, offset;
    v.clear();
    const double Version = config.readHeader();
    if(Version>0.0)
    {
        std::string token;
        while(config.getToken(token))
        {
            if(token=="Name") config.get(name);
            if(token=="Offset")
                config.get(offset);
            if(token=="Ratio")
                if(!config.get(ratio))
                {
                    ratio.set_size(1);
                    ratio.fill(1.0);
                }
            if(token=="Vector")
                if(config.get(q))
                {
                    for( i=0 ; i<offset.size() && i<q.size() ; ++i )
                        q[i] += offset[i];
                    if(ratio.size()==1)
                        q *= ratio[0];
                    if(ratio.size()>1)
                        for( i=0 ; i<ratio.size() && i<q.size() ; ++i )
                            q[i] *= ratio[i];
                    v.push_back(q);
                    names.push_back(name);
                    name="";
                }
        }
    }
    config.close();
    return (unsigned int)v.size();
}

unsigned int arlCore::load( std::vector< vnl_vector<double> >&v, const std::string &filename )
{
    std::vector<std::string> names;
    return arlCore::load(v, names, filename);
}

unsigned int arlCore::load( std::vector< std::string >&v, const std::string &filename )
{
    if(filename=="") return 0;
    std::string path, folder, file, ext;
    arlString::splitFilename( filename, folder, file, ext );
    arlCore::File config(filename, "Filenames");
    const double Version = config.readHeader();
    std::string name, string;
    v.clear();
    if(Version>0.0)
    {
        std::string token;
        while(config.getToken(token))
        {
            if(token=="Name") config.get(name);
            if(token=="Filename")
                if(config.get(string))
                {
                    std::stringstream ss;
                    if(folder!="") ss<<folder<<"/"<<string;
                    else ss<<string;
                    v.push_back(ss.str());
                }
        }
    }
    config.close();
    return (unsigned int)v.size();
}

bool arlCore::save( const vnl_matrix<double>& matrix, const std::string &fileName, bool overwrite )
{
    if(matrix.size()<1) return false;
    unsigned int i, j;
    std::fstream f;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    f.open (fileName.c_str(), std::fstream::out);
    if(f.is_open())
    {
        f<<"FileType Matrix\nVersion 1.0\n\n";
        f<<"Matrix "<<matrix.rows()<<" "<<matrix.cols()<<"\n";
        for( i=0 ; i<matrix.rows() ; ++i )
        {
            for( j=0 ; j<matrix.cols() ; ++j )
                f<<matrix.get(i, j)<<" ";
            f<<"\n";
        }
    }
    f<<"End\n";
    f.close();
    f.flush();
    return true;
}

bool arlCore::load( vnl_matrix<double>& matrix, const std::string &fileName )
{
    if(fileName=="") return false;
    unsigned int i, j;
    arlCore::File config(fileName, "Matrix");
    const double Version = config.readHeader();
    bool b = false;
    if(Version>0.0)
    {
        std::string token;
        while(config.getToken(token))
        {
            if(token=="Matrix")
            {
                unsigned int rows, cols;
                config.get(rows);
                config.get(cols);
                if(rows!=matrix.rows() || cols!=matrix.columns())
                    return false;
                for( i=0 ; i<rows ; ++i )
                    for( j=0 ; j<cols ; ++j )
                    {
                        double value;
                        if(!config.get(value)) return false;
                        matrix.put(i, j, value);
                    }
                b = true;
            }
        }
    }
    config.close();
    return b;
}

double arlCore::evaluationAXXB( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z,
            const std::vector< vnl_rigid_matrix > &M06,
            const std::vector< const Camera* > &videoCams,
            const std::vector< std::vector<arlCore::PointList> > &models3DList,
            const std::vector< std::vector<arlCore::PointList> > &points2DList,
            std::vector< double > &errors)
{
//  for(int i=0 ; i<videoCams.size() ; ++i )
//      videoCams[i]->print();
    errors.clear();
    const unsigned int NbPoses = (unsigned int)M06.size();
    const unsigned int NbCams = (unsigned int)videoCams.size();
    unsigned int i, j, k;
    double n=0.0, RMS=0.0;
    assert(models3DList.size()==NbCams);
    const arlCore::vnl_rigid_matrix InvZ = Z.computeInverse();
    arlCore::Point pt2D(2), pt3D(3);
    for( i=0 ; i<NbPoses ; ++i )
    {   // Compute T = inv( X . M06i . inv(Z) )
        arlCore::vnl_rigid_matrix T = X * M06[i] * InvZ;
        T.invert();
        for( j=0 ; j<NbCams ; ++j )
            if(videoCams[j]!=0)
            {
                if(i==0)
                {
                    assert(points2DList[j].size()==models3DList[j].size());
                    if(points2DList[j].size()!=models3DList[j].size()) std::cerr<<"AxxBReproj_cost_function : assert(points2DList[j].size()==models3DList[j].size());\n";
                }
                assert(points2DList[j][i].size()==models3DList[j][i].size());
                if(points2DList[j][i].size()!=models3DList[j][i].size()) std::cerr<<"AxxBReproj_cost_function : assert(points2DList[j][i].size()==models3DList[j][i].size());\n";
                for( k=0 ; k<models3DList[j][i].size() ; ++k )
                {
                    T.trf(*(models3DList[j][i][k]), pt3D);
                    if(!videoCams[j]->project3DPoint( pt3D, pt2D )) std::cerr<<"AxxBReproj_cost_function : Reprojection Error\n";
                    else
                    {
                        const double Dist2 = points2DList[j][i][k]->distance2(pt2D);
                        errors.push_back(sqrt(Dist2));
                        RMS += Dist2;
                        ++n;
                    }
                }
            }
    }
    if(n>0.0) RMS = sqrt(RMS/n);
    return RMS;
}

unsigned int arlCore::statistic( const std::vector<double> &v, double &min, double &max, double &rms, double &mean, double &stddev, bool square )
{
    unsigned int i;
    rms=0.0, mean=0.0, stddev=0.0;
    const unsigned int Size = (unsigned int)v.size();
    if(Size<1) return Size;
    const double N = (double)Size;
    double value, value2;
    for( i=0 ; i<Size ; ++i )
    {
        if(square) value2 = v[i], value = sqrt(v[i]);
        else value = v[i], value2 = v[i]*v[i];
        if(i==0) min = value, max = value;
        else
        {
            if(value<min) min=value;
            if(value>max) max=value;
        }
        mean += value;
        rms += value2;
    }
    rms = sqrt(rms/N);
    mean /= N;
    for( i=0 ; i<Size ; ++i )
        if(square) stddev += (sqrt(v[i])-mean)*(sqrt(v[i])-mean);
        else stddev += (v[i]-mean)*(v[i]-mean);
    stddev = sqrt(stddev/N);
    return Size;
}

double arlCore::addValue( vnl_vector_fixed<double,5>& stat, double error, bool square )
{   // stat = { n ; min ; max ; sum(error) ; sum(error²) }
    double err=error, err2=error;
    if(square) err=sqrt(error);
    else err2 = error*error;
    stat[0]++; // n
    if(stat[0]==1)
    {
        stat[1]=err;
        stat[2]=err;
    }else
    {
        if(err<stat[1]) stat[1]=err;
        if(err>stat[2]) stat[2]=err;
    }
    stat[3] += err;
    stat[4] += err2;
    return error;
}

unsigned int arlCore::computeStat( const vnl_vector_fixed<double,5>& stat, double &min, double &max, double &mean, double &stdDev, double &RMS )
{   // stat = { n ; min ; max ; sum(error) ; sum(error²) }
    const double N = stat[0];
    min = stat[1];
    max = stat[2];
    if(N>0)
    {
        mean = stat[3]/N;
        stdDev = sqrt(stat[4]/N-stat[3]*stat[3]/(N*N));
        RMS = sqrt(stat[4]/N);
    }
    else
    {
        mean = 0.0;
        stdDev = 0.0;
        RMS = 0.0;
    }
    return (unsigned int)N;
}

bool arlCore::save( const std::vector< arlCore::vnl_rigid_matrix > &v, const std::string &fileName, bool overwrite )
{
    std::fstream file;
    unsigned int i;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    file.open(fileName.c_str(), std::fstream::out);
    if(!file.is_open()) return false;
    file<<"FileType MatrixList\n";
    file<<"Version 1.0\n";
    for( i=0 ; i<v.size() ; ++i )
        v[i].save(file);
    file.close();
    return true;
}

bool arlCore::load( std::vector< arlCore::vnl_rigid_matrix > &v, const std::string &fileName, bool invert )
{
    if(fileName=="") return false;
    std::fstream file;
    file.open(fileName.c_str(), std::fstream::in);
    if(!file.is_open())
    {
        std::cerr<<"MatrixList Loading error : "<<fileName<<"\n";
        return false;
    }
    unsigned int n=0;
    std::string token,text;
    double version;
    do
    {   // Reading header
        file>>token;
        if(token=="FileType")
        {
            file>>text;
            if(text!="MatrixList") return false;
            n=(n|1);
        }
        if(token=="Version") {file>>version; n=(n|2);}
        if(file.eof())
        {
            std::cerr<<"MatrixList Loading Header error : "<<fileName<<"\n";
            return false;
        }
    } while(n!=3);
    v.clear();
    vnl_rigid_matrix m;
    bool b;
    do
    {
        b=m.load(file);
        if(b)
        {
            if(invert) m.invert();
            v.push_back(m);
        }
    }while(b);
    file.close();
    return true;
}

bool arlCore::needleCalibration( const std::vector< arlCore::vnl_rigid_matrix >& tip, arlCore::vnl_rigid_matrix &calibration, const std::vector< arlCore::vnl_rigid_matrix > &orientation, double &RMS )
{
    // NEEDLE TIP CALIBRATION
    // TODO Choose method
    //ARLCORE_SCE_GEOMETRIC ARLCORE_SCE_ANALYTIC ARLCORE_SCE_OPTIMIZATION
    const arlCore::ARLCORE_SCE methode = arlCore::ARLCORE_SCE_GEOMETRIC;
    const bool Verbose = false;
    RMS = -1;
    unsigned int i;
    Point center(calibration.getX(), calibration.getY(), calibration.getZ());
    double radius,a,b,c;
    if(tip.size()>3)
    {
        PointList pl;
        pl.fill(tip);
        std::vector<double> optimiser_parameter, log;
        if(!pl.sphereCenterEstimation(center, radius, methode, optimiser_parameter, log)) return false;
        if(log.size()>0) RMS = log[0];
        std::cout<<"Log sphereCenterEstimation : ";
        for(i=0 ; i<log.size() ; ++i)
            std::cout<<log[i]<<" ";
        std::cout<<"\n";
        if(Verbose) std::cout<<"Radius = "<<radius<<" Error = "<<RMS<<" center = "<<center.x()<<" "<<center.y()<<" "<<center.z()<<"\n";
    }
    // NEEDLE ORIENTATION CALIBRATION
    double x = center.x(), y = center.y(), z = center.z();
    radius = sqrt(x*x + y*y + z*z);
    if(radius==0) return false;
    Point ptA1(0,0,0), ptA2(1,0,0), ptA3(0,0,1);
    Point ptB1(x,y,z), ptB2(x-1,y-1,0), ptB3(x,y,z);
    // Equation du plan perpendiculaire � l'axe z de l'outil
    // ax + by + cz + d = 0
    if(orientation.size()<2)
        a=x, b=y, c=z;
    else
    {
        std::vector< vgl_point_3d<double> > points;
        for( i=0 ; i<orientation.size() ; ++i )
        {
            vnl_rigid_matrix M;
            Point p0(x,y,z);
            M.invert(orientation[i]);
            M.trf(p0);
            points.push_back(vgl_point_3d<double>(p0.x(),p0.y(),p0.z()));
        }
        double lineRMS;
        vgl_line_3d_2_points<double> line;
        if(vgl_fit_line_3d( points, line, lineRMS ))
        {
            if(Verbose) std::cout<<"RMS line fit = "<<lineRMS<<"\n";
            vgl_point_3d<double> vglp0=line.point1();
            vgl_point_3d<double> vglp1=line.point2();
            Point p0(vglp0.x(),vglp0.y(),vglp0.z());
            Point p1(vglp1.x(),vglp1.y(),vglp1.z());
            orientation[0].trf(p0);
            orientation[0].trf(p1);
            p0.mult(-1);
            p1.add(p0);
            p1.normalize();
            a=p1.x();
            b=p1.y();
            c=p1.z();
            ptB3.add(p1);
        }else
        {
            lineRMS = 0.0;
            vnl_rigid_matrix M;
            Point p0(x,y,z), p1(x,y,z);
            M.invert(orientation[1]);
            M.trf(p1);
            orientation[0].trf(p1);
            p0.mult(-1);
            p1.add(p0);
            p1.normalize();
            a=p1.x();
            b=p1.y();
            c=p1.z();
            ptB3.add(p1);
        }
        std::cout<<"Line RMS : "<<lineRMS<<"\n";
    }
    // d pour le plan passant par l'origine de l'outil (x,y,z)
    double d = -(a*x + b*y + c*z);
    if(orientation.size()<2)
    {
        ptB3.normalize();
        ptB3.mult(radius-1.0);
    }
    // FIXME cas o� c==0
    ptB2.z((-a*ptB2.x()-b*ptB2.y()-d)/c);
    ptA2.x(ptB2.distance(ptB1));

    std::vector< const Point* > A, B;
//  vgl_point_3d<double> p0(points[0].x(), points[0].y(), points[0].z());
//  vgl_point_3d<double> p0(points[1].x(), points[1].y(), points[1].z());
//  std::cout<<ptA1.print()<<" "<<ptA2.print()<<" "<<ptA3.print();
//  std::cout<<ptB1.print()<<" "<<ptB2.print()<<" "<<ptB3.print();

    A.push_back(&ptA1); B.push_back(&ptB1);
    A.push_back(&ptA2); B.push_back(&ptB2);
    A.push_back(&ptA3); B.push_back(&ptB3);
/*  if(!calibration.register3D3D( A, B, true )) return false;
    if(Verbose)
    {
        std::vector<double> errors;
        calibration.RMS3D3D( A, B, errors );
    }
    calibration.setRMS(RMS);
    return true;*/
    return calibration.register3D3D( A, B, true );
}

bool arlCore::vgl_fit_line_3d( const std::vector< vgl_point_3d<double> > &points, vgl_line_3d_2_points<double> &line, double &RMS )
{
    RMS = -1;
    if(points.size()<2) return false;
    if(points.size()==2)
    {
        line.set( points[0], points[1] );
        RMS = 0.0;
        return true;
    }
    vnl_vector<double> solution(6);
    solution(0)=points[0].x();
    solution(1)=points[0].y();
    solution(2)=points[0].z();
    solution(3)=points[1].x();
    solution(4)=points[1].y();
    solution(5)=points[1].z();
    Optimise3DLine meanLine(points);
    vnl_powell computeMeanLine(&meanLine);
    computeMeanLine.minimize(solution);
    const vgl_point_3d<double> Point1(solution[0], solution[1], solution[2]);
    const vgl_point_3d<double> Point2(solution[3], solution[4], solution[5]);
    line.set(Point1, Point2);
    RMS = sqrt(computeMeanLine.get_end_error()/meanLine.size());
    return true;
}

vgl_vector_3d<double> arlCore::orthogonal_vectors( const vgl_vector_3d<double> &vec, double s)
{
    //enforce parameter range
    if(s<0.0) s=0.0;
    if(s>1.0) s = 1.0;
    //double tol = vgl_tolerance<double>::position;
    double tol = vcl_numeric_limits<double>::epsilon();
    double nx = vec.x_, ny = vec.y_, nz =  vec.z_;
    double mnz = vcl_fabs(nz);
    double two_pi = 2*vnl_math::pi;
    double co = vcl_cos(two_pi*s), si = vcl_sin(two_pi*s);
    //General case
    if(mnz>tol)
    {
        double co = vcl_cos(two_pi*s), si = vcl_sin(two_pi*s);
        double rx = nx/nz, ry = ny/nz;
        double q = co*rx +si*ry;
        double a = 1.0/vcl_sqrt(1+q*q);
        double vx = a*co, vy = a*si;
        double vz = -(rx*vx + ry*vy);
        return vgl_vector_3d<double>(vx, vy, vz);
    }
    //Degenerate case, nz ~ 0
    double r = nx/ny;
    double a = 1/vcl_sqrt(1+co*co*r*r);
    double vx = a*co, vz = a*si;
    double vy = -a*co*r;
    return vgl_vector_3d<double>(vx, vy, vz);
}

bool arlCore::intersection( const vnl_vector_fixed<double, 6> &bound1, const vnl_vector_fixed<double, 6> &bound2 )
{
    vgl_box_3d< double > box1, box2, intersection;
    box1.set_min_x(bound1[0]);
    box1.set_max_x(bound1[1]);
    box1.set_min_y(bound1[2]);
    box1.set_max_y(bound1[3]);
    box1.set_min_z(bound1[4]);
    box1.set_max_z(bound1[5]);

    box2.set_min_x(bound2[0]);
    box2.set_max_x(bound2[1]);
    box2.set_min_y(bound2[2]);
    box2.set_max_y(bound2[3]);
    box2.set_min_z(bound2[4]);
    box2.set_max_z(bound2[5]);

    intersection = vgl_intersection ( box1, box2 );
    return !intersection.is_empty();
}
