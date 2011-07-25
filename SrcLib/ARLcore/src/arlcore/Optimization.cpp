/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Optimization.h>

#include <time.h>

#include <vnl/vnl_cross.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_plane_3d.h>

#include <arlcore/vnl_rotation3d_vector.h>
#include <arlcore/vnl_rigid_vector.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Camera.h>
#include <arlcore/Misc.h>

using namespace std;

double arlCore::CostFunction::m_errorMax = 9e99;

arlCore::CostFunction::CostFunction():
m_observer(false),
m_iterations(0),
m_errorMin(m_errorMax),
m_firstIteration(true)
{}

double arlCore::CostFunction::getCriterion( const vnl_vector<double> &x )
{
    m_error = m_errorMax;
    return m_error;
}

void arlCore::CostFunction::setObserver( bool b )
{
    m_observer = b;
    m_errorList.clear();
    m_iterations = 0;
}

unsigned int arlCore::CostFunction::getNbIterations() const
{
    return m_iterations;
}

double arlCore::CostFunction::getError() const
{
    return m_error;
}

const arlCore::PointList& arlCore::CostFunction::getObservation() const
{
    return m_errorList;
}

double arlCore::CostFunction::error( const vnl_vector<double> &x )
{
    m_error = m_errorMax;
    return observe(x);
}

double arlCore::CostFunction::observe( const vnl_vector<double> &x )
{
    if(m_firstIteration)
    {
        m_solution = x;
        m_firstIteration = false;
    }
    if(m_error<m_errorMin)
    {
        m_errorMin = m_error;
        m_solution = x;
    }
    if(m_observer)
    {
        long int now = clock ();// / (double)CLK_TCK * 10);
        if(m_iterations==0)
        {
            m_time = now;
            m_errorList.push_back(0.0, m_error);
        }else m_errorList.push_back((double)(now-m_time), m_error);
    }
    ++m_iterations;
    return m_error;
}

bool arlCore::CostFunction::verify( const vnl_vector<double> &x, double endError )
{
    bool b1 = (endError==getCriterion(x));
    if(!b1) std::cout<<"Distorsion error/solution : "<<endError<<"*"<<getCriterion(x)<<"\n";
    bool b2 = (endError==m_errorMin);
    if(!b2) std::cout<<"Distorsion error min : "<<endError<<"*"<<m_errorMin<<"\n";
    bool b3 = (x==m_solution);
    if(!b3) std::cout<<"Distorsion solution : "<<x<<"*"<<m_solution<<"\n";
    return b1 && b2 && b3;
}

void arlCore::CostFunction::plot() const
{
    std::string options = "set logscale y\n\
        set title 'Optimization observer'\n\
        set xlabel 'time (ms)'\n\
        set ylabel 'error'\n";
    m_errorList.plot( options );
}

// OptimiseICP
arlCore::OptimiseICP::OptimiseICP( arlCore::ICP& icp):
vnl_cost_function(6),
m_ICP(icp)
{}

arlCore::OptimiseICP::~OptimiseICP(){}

double arlCore::OptimiseICP::f(vnl_vector< double > const &x)
{
    assert(x.size()==6);
    m_error = m_ICP.computeCriterion(vnl_rigid_matrix(vnl_rigid_vector(x)));
//  std::cout<<m_error<<" ";
    return observe(x);
}

// OptimiseICP_LS
arlCore::OptimiseICP_LS::OptimiseICP_LS( arlCore::ICP& icp, UseGradient g ):
vnl_least_squares_function(6, icp.getCloudSize(), g),
m_ICP(icp)
{}

arlCore::OptimiseICP_LS::~OptimiseICP_LS(){}

void arlCore::OptimiseICP_LS::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==6);
    assert(fx.size()==m_ICP.getCloudSize());
    m_error = m_ICP.computeCriterion(vnl_rigid_matrix(vnl_rigid_vector(x)), fx);
    //std::cout<<m_error<<" ";
    observe(x);
}

void arlCore::OptimiseICP_LS::gradf(vnl_vector< double > const &x, vnl_matrix<double>& g)
{
    vnl_least_squares_function::fdgradf(x, g, 1e-8);
}

// OptimisePlane
arlCore::OptimisePlane::OptimisePlane():
vnl_cost_function(4)
{}

arlCore::OptimisePlane::~OptimisePlane()
{
    unsigned int i;
    for( i=0 ; i<m_points.size() ; ++i )
        if(m_points[i]!=0) delete m_points[i];
}

void arlCore::OptimisePlane::addPoint( const vgl_point_3d< double >&point )
{
    m_points.push_back(new vgl_point_3d< double >(point));
}

void arlCore::OptimisePlane::delPoint( unsigned int index )
{
    if(m_points.size()<=index) return;
    if(m_points[index]!=0) delete m_points[index];
    m_points[index]=0;
}

unsigned int arlCore::OptimisePlane::size( void )
{
    return (unsigned int)m_points.size();
}

const std::vector< vgl_point_3d< double >* > &arlCore::OptimisePlane::getPoints( void )
{
    return m_points;
}

double arlCore::OptimisePlane::f(vnl_vector< double > const &x)
{
    assert(x.size()==4);
    m_error=0.0;
    vgl_plane_3d< double >p(x[0],x[1],x[2],x[3]);
    unsigned int i;
    for( i=0 ; i<m_points.size() ; i++ )
        if(m_points[i]!=0)
        {
            const double Distance = vgl_distance(p, *(m_points[i]));
            m_error += Distance*Distance;
        }
    return observe(x);
}

// Optimise3DLine
arlCore::Optimise3DLine::Optimise3DLine(const std::vector< vgl_point_3d<double> >& pointList):
vnl_cost_function(6),
m_points(pointList)
{}

arlCore::Optimise3DLine::~Optimise3DLine()
{}

double arlCore::Optimise3DLine::f(vnl_vector< double > const &x)
{
    m_error=0.0;
    vgl_line_3d_2_points<double> line;
    vgl_point_3d<double>point1(x[0],x[1],x[2]);
    vgl_point_3d<double>point2(x[3],x[4],x[5]);
    line.set(point1, point2);
    unsigned int i;
    for( i=0 ; i<m_points.size() ; i++ )
    {
        double a = arlCore::vgl_distance_point( line, m_points[i] );
        m_error += a*a;
    }
    return observe(x);
}

unsigned int arlCore::Optimise3DLine::size( void )
{
    return (unsigned int)m_points.size();
}

// OptimisePivot
arlCore::OptimisePivot::OptimisePivot( const arlCore::PointList &points ):
vnl_cost_function(points.getDimension()),
m_points(points),
m_radius(0.0)
{}

arlCore::OptimisePivot::~OptimisePivot()
{}

double arlCore::OptimisePivot::getRadius( void )
{
    return m_radius;
}

double arlCore::OptimisePivot::f(vnl_vector< double > const &x)
{
    m_error=0.0;
    unsigned int i;
    vnl_vector<double> distances(m_points.size());
    arlCore::Point center(m_points.getDimension());
    for( i=0 ; i<center.size() ; ++i )
        center.set(i, x[i]);
    for( i=0 ; i<m_points.size() ; i++ )
        distances[i] = m_points[i]->distance(center);
    m_radius = distances.mean();
    double stdDev = 0;
    for( i=0 ; i<distances.size() ; ++i )
        m_error += (distances[i]-m_radius)*(distances[i]-m_radius);
    return observe(x);
}

// OptimiseVideoRobot1
arlCore::OptimiseVideoRobot1::OptimiseVideoRobot1( const std::vector< arlCore::vnl_rigid_matrix > &M06, const arlCore::PointList &Tip ):
vnl_cost_function(6),
m_tip(Tip),
m_M06(M06),
m_distance(0.0),
m_stdDev(0.0)
{
    assert(M06.size()==m_tip.size());
    assert(m_tip.getDimension()==3);
}

arlCore::OptimiseVideoRobot1::~OptimiseVideoRobot1()
{}

double arlCore::OptimiseVideoRobot1::getDistance( void )
{
    return m_distance;
}

double arlCore::OptimiseVideoRobot1::getStdDev( void )
{
    return m_stdDev;
}

double arlCore::OptimiseVideoRobot1::f(vnl_vector< double > const &x)
{
    const double ReconsErrorMax = -1;
/*  static vnl_vector< double >xx;
    static bool bb=false;
    if(!bb)
    {
        bb=true;
        xx=x;
    }*/
    m_error=0.0;
    m_stdDev=0.0;
    m_distance = 0.0;
    unsigned int i;
    double n = 0.0;
    const Point EndEffectorOrg(0.0, 0.0, 0.0);
    Point TipEndEffectorPlane(3);
    const vnl_rigid_vector V( x );
    vnl_rigid_matrix T( V );
    vnl_rigid_matrix video2EndEffector;
    vnl_vector<double> distances((unsigned int)m_M06.size());
    std::vector< vnl_vector<double> > coordinates(m_M06.size());
    vnl_vector<double> meanCoordinates(3/*, 0.0*/);
    meanCoordinates.fill(0.0);
    for( i=0 ; i<m_M06.size() ; ++i )
        if(ReconsErrorMax<0 || m_tip[i]->getError()<ReconsErrorMax)
        {
            video2EndEffector.mult( m_M06[i], T );
            video2EndEffector.trf( *(m_tip[i]), TipEndEffectorPlane );
            coordinates[i] = TipEndEffectorPlane.getCoordinates();
            meanCoordinates += coordinates[i];
            distances[i]=EndEffectorOrg.distance(TipEndEffectorPlane);
            m_distance += distances[i];
            ++n;
        }
    if(n==0.0) return 0.0;
    m_distance /= n;
    for( i=0 ; i<distances.size() ; ++i )
        if(ReconsErrorMax<0 || m_tip[i]->getError()<ReconsErrorMax)
        {
            //const double Error = fabs(distances[i]-m_distance);
            const double Error = (meanCoordinates-coordinates[i]).two_norm();
            //std::cout<<"* "<<meanCoordinates<<" "<<coordinates[i]<<" "<<(meanCoordinates-coordinates[i]).two_norm()<<"\n";
            //const double Error = (meanCoordinates-coordinates[i]).two_norm() + fabs(distances[i]-m_distance);
            //std::cout<<"* "<<Error<<"\n";
            m_error += Error*Error;
        }
    m_stdDev = sqrt(m_error/n);
    return observe(x);
}

// OptimiseVideoRobot
arlCore::OptimiseVideoRobot::OptimiseVideoRobot(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList ):
m_M06(M06),
m_chessboards(model3D),
m_cameras(cams),
m_points2DList(points2DList)
{}

arlCore::OptimiseVideoRobot::~OptimiseVideoRobot()
{}

double arlCore::OptimiseVideoRobot::criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z )
{
    vnl_vector<double> fx;
    arlCore::vnl_rigid_matrix invZ = Z.computeInverse();
    if(criterion(X,invZ,fx)) return m_error;
    else return -1;
}

bool arlCore::OptimiseVideoRobot::criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &invZ, vnl_vector<double> &fx )
{
    fx.fill(0.0);
    unsigned int i, j, k, index=0;
    const unsigned int NbCams = (unsigned int)m_cameras.size();
    assert(m_chessboards.size()==NbCams);
    assert(m_points2DList.size()==NbCams);
    const unsigned int NbPoses = (unsigned int)m_M06.size();
    arlCore::Point pt2D(2), pt3D(3);
    double n = 0.0;
    m_error = 0.0;
    for( j=0 ; j<NbPoses ; ++j )
    {   // T = inv( X . M06i . inv(Z) )
        arlCore::vnl_rigid_matrix T = X * m_M06[j] * invZ;
        T.invert();
        for( i=0 ; i<NbCams ; ++i )
        {
            assert(m_chessboards[i].size()==NbPoses);
            const unsigned int NbPoints = m_chessboards[i][j].size();
            assert(m_points2DList[i][j].size()==NbPoints);
            for( k=0 ; k<NbPoints ; ++k, ++index )
            {
                T.trf(*(m_chessboards[i][j][k]), pt3D);
                if(m_cameras[i]->project3DPoint(pt3D, pt2D))
                {
                    const double Error2 = pt2D.distance2(*m_points2DList[i][j][k]);
                    m_error += Error2;
                    if(index<fx.size()) fx[index] = Error2;
                    ++n;
                }
            }
        }
    }
    if(n>0) m_error = sqrt(m_error/n); else return false;
    return true;
}

// OptimiseVideoRobotX
arlCore::OptimiseVideoRobotX::OptimiseVideoRobotX(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &Z,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList ):
vnl_cost_function(6),
OptimiseVideoRobot(M06, model3D, cams, points2DList),
m_Z(Z)
{}

arlCore::OptimiseVideoRobotX::~OptimiseVideoRobotX()
{}

double arlCore::OptimiseVideoRobotX::f(vnl_vector< double > const &x)
{
    assert(x.size()==6);
    const arlCore::vnl_rigid_vector V(x);
    arlCore::vnl_rigid_matrix invZ(m_Z), X(V);
    invZ.invert();
    vnl_vector<double> fx;
    if(!criterion(X, invZ, fx)) return error(x);
    return observe(x);
}

// OptimiseVideoRobotX LS
arlCore::OptimiseVideoRobotX_LS::OptimiseVideoRobotX_LS(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &Z,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        unsigned int number_of_residuals, UseGradient g ):
vnl_least_squares_function(6, number_of_residuals, g),
OptimiseVideoRobot(M06, model3D, cams, points2DList),
m_Z(Z)
{}

arlCore::OptimiseVideoRobotX_LS::~OptimiseVideoRobotX_LS()
{}

void arlCore::OptimiseVideoRobotX_LS::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==6);
    const arlCore::vnl_rigid_vector V(x);
    arlCore::vnl_rigid_matrix invZ(m_Z), X(V);
    invZ.invert();
    if(!criterion(X, invZ, fx)) error(x);
    observe(x);
}

void arlCore::OptimiseVideoRobotX_LS::gradf(vnl_vector< double > const &x, vnl_matrix<double>& g)
{
    vnl_least_squares_function::fdgradf(x, g, 1e-8);
}

// OptimiseVideoRobotZ
arlCore::OptimiseVideoRobotZ::OptimiseVideoRobotZ(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &X,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList ):
vnl_cost_function(6),
OptimiseVideoRobot(M06, model3D, cams, points2DList),
m_X(X)
{}

arlCore::OptimiseVideoRobotZ::~OptimiseVideoRobotZ()
{}

double arlCore::OptimiseVideoRobotZ::f(vnl_vector< double > const &x)
{
    assert(x.size()==6);
    const arlCore::vnl_rigid_vector V(x);
    arlCore::vnl_rigid_matrix invZ(V), X(m_X);
    invZ.invert();
    vnl_vector<double> fx;
    if(!criterion(X, invZ, fx)) return error(x);
    return observe(x);
}

// OptimiseVideoRobotZ LS
arlCore::OptimiseVideoRobotZ_LS::OptimiseVideoRobotZ_LS(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &X,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        unsigned int number_of_residuals, UseGradient g ):
vnl_least_squares_function(6, number_of_residuals, g),
OptimiseVideoRobot(M06, model3D, cams, points2DList),
m_X(X)
{}

arlCore::OptimiseVideoRobotZ_LS::~OptimiseVideoRobotZ_LS()
{}

void arlCore::OptimiseVideoRobotZ_LS::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==6);
    const arlCore::vnl_rigid_vector V(x);
    arlCore::vnl_rigid_matrix invZ(V), X(m_X);
    invZ.invert();
    if(!criterion(X, invZ, fx)) error(x);
    observe(x);
}

void arlCore::OptimiseVideoRobotZ_LS::gradf(vnl_vector< double > const &x, vnl_matrix<double>& g)
{
    vnl_least_squares_function::fdgradf(x, g, 1e-8);
}

// OptimiseVideoRobotXZ
arlCore::OptimiseVideoRobotXZ::OptimiseVideoRobotXZ(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList ):
vnl_cost_function(12),
OptimiseVideoRobot(M06, model3D, cams, points2DList)
{}

arlCore::OptimiseVideoRobotXZ::~OptimiseVideoRobotXZ()
{}

double arlCore::OptimiseVideoRobotXZ::f(vnl_vector< double > const &x)
{
    assert(x.size()==12);
    unsigned int i;
    vnl_vector< double > x0(6), z0(6);
    for( i=0 ; i<6 ; ++i )
    {
        x0[i] = x[i];
        z0[i] = x[i+6];
    }
    const arlCore::vnl_rigid_vector VX(x0), VZ(z0);
    arlCore::vnl_rigid_matrix X(VX), invZ(VZ);
    invZ.invert();
    vnl_vector<double> fx;
    if(!criterion(X, invZ, fx)) return error(x);
    return observe(x);
}

// OptimiseVideoRobotXZ LS
arlCore::OptimiseVideoRobotXZ_LS::OptimiseVideoRobotXZ_LS(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        unsigned int number_of_residuals, UseGradient g ):
vnl_least_squares_function(12, number_of_residuals, g),
OptimiseVideoRobot(M06, model3D, cams, points2DList)
{}

arlCore::OptimiseVideoRobotXZ_LS::~OptimiseVideoRobotXZ_LS()
{}

void arlCore::OptimiseVideoRobotXZ_LS::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==12);
    unsigned int i;
    vnl_vector< double > x0(6), z0(6);
    for( i=0 ; i<6 ; ++i )
    {
        x0[i] = x[i];
        z0[i] = x[i+6];
    }
    const arlCore::vnl_rigid_vector VX(x0), VZ(z0);
    arlCore::vnl_rigid_matrix X(VX), invZ(VZ);
    invZ.invert();
    if(!criterion(X, invZ, fx)) error(x);
    else observe(x);
}

void arlCore::OptimiseVideoRobotXZ_LS::gradf(vnl_vector< double > const &x, vnl_matrix<double>& g)
{
    vnl_least_squares_function::fdgradf(x, g, 1e-8);
}

// OptimiseReprojection
arlCore::OptimiseReprojection::OptimiseReprojection(const vector<const arlCore::Camera*> &cameras, const vector<arlCore::Point::csptr> &points2D):
vnl_cost_function(3),
m_points2D(points2D),
m_cameras(cameras){}

arlCore::OptimiseReprojection::~OptimiseReprojection(){}

double arlCore::OptimiseReprojection::f(vnl_vector< double > const &x)
{
    assert(x.size()==3);
    vnl_vector_fixed<double,4> point3DH(x[0], x[1], x[2], 1.0);
    vnl_vector_fixed<double,2> reprojPoint;
    m_error = 0.0;
    double errorX, errorY;
    unsigned int i;
    for( i=0 ; i<m_cameras.size() ; i++ )
    {
        assert(m_points2D[i]->size()==2);
        m_cameras[i]->project3DPoint(point3DH, reprojPoint);
        errorX = (reprojPoint(0)-m_points2D[i]->x());
        errorY = (reprojPoint(1)-m_points2D[i]->y());
        m_error += errorX*errorX + errorY*errorY;
    }
    return observe(x);
}

// AxxBReproj_cost_function
/*arlCore::AxxBReproj_cost_function::AxxBReproj_cost_function(
    const std::vector< arlCore::vnl_rigid_matrix > &A,
    const std::vector< arlCore::vnl_rigid_matrix > &B,
    const std::vector<const arlCore::Camera*> &videoCams,
    const std::vector< arlCore::PointList > &models3DList,
    const std::vector< std::vector<arlCore::PointList> > &points2DList):
m_A_matrix(A),
m_B_matrix(B),
m_videoCams(videoCams),
m_models3DList(models3DList), //[pose]
m_points2DList(points2DList) //[cam][pose]
{
    assert(m_A_matrix.size()==m_B_matrix.size());
    if(m_A_matrix.size()!=m_B_matrix.size()) std::cerr<<"AxxBReproj_cost_function : assert(m_A_matrix.size()==m_B_matrix.size());\n";
    assert(points2DList.size()==m_videoCams.size());
    if(points2DList.size()!=m_videoCams.size()) std::cerr<<"AxxBReproj_cost_function : assert(points2DList.size()==m_videoCams.size());\n";
//  for(int i=0 ; i<m_videoCams.size() ; ++i )
//      m_videoCams[i]->print();
}

arlCore::AxxBReproj_cost_function::~AxxBReproj_cost_function(){}

double arlCore::AxxBReproj_cost_function::f(vnl_vector< double > const &v)
{
    const unsigned int NbPoses = 1;//m_A_matrix.size();
    const unsigned int NbCams = m_videoCams.size(); 
    m_error = 0.0;
    const vnl_rigid_matrix X = arlCore::vnl_rigid_vector(v);
    const vnl_rigid_matrix Z = arlCore::vnl_rigid_vector(v);
    unsigned int i, j, k;
    double n=0;
    for( i=0 ; i<NbCams ; ++i )
    {
        assert(m_points2DList[i].size()==m_models3DList.size());
        if(m_points2DList[i].size()!=m_models3DList.size()) std::cerr<<"AxxBReproj_cost_function : assert(points2DList[i].size()==m_models3DList.size());\n";
        if(m_videoCams[i]!=0)
            for( j=0 ; j<NbPoses ; ++j )
            {
                assert(m_points2DList[i][j].size()==m_models3DList[j].size());
                if(m_points2DList[i][j].size()!=m_models3DList[j].size()) std::cerr<<"AxxBReproj_cost_function : assert(m_points2DList[i][j].size()==m_models3DList[j].size());\n";
                vnl_rigid_matrix T = X;
                arlCore::Point pt2D(2), pt3D(3);
                for( k=0 ; k<m_models3DList[j].size() ; ++k )
                {
                    T.trf(*(m_models3DList[j][k]), pt3D);
                    if(!m_videoCams[i]->project3DPoint( pt3D, pt2D )) std::cerr<<"AxxBReproj_cost_function : Reprojection Error\n";
                    else
                    {
                        m_error += m_points2DList[i][j][k]->distance(pt2D);
                        ++n;
                    }
                }
            }
    }
    if(n!=0.0) m_error /= n;
    std::cout<<m_error<<"\n";
    return observe(x);
}*/

// Dornaika_cost_function
arlCore::Dornaika_cost_function::Dornaika_cost_function(const std::vector< vnl_rigid_matrix >&A, const std::vector< vnl_rigid_matrix >&B):
vnl_cost_function(7),
m_A_matrix(A),
m_B_matrix(B){}

arlCore::Dornaika_cost_function::~Dornaika_cost_function(){}
  
double arlCore::Dornaika_cost_function::f(vnl_vector< double > const &x)
{
    assert(x.size()==7);
    const double Lambda1=1; // 1000
    const double Lambda2=1; // 10
    const unsigned int Lambda = 2000000;
    unsigned int i, j;
    m_error=0.0;
    vnl_quaternion<double> q(x[0],x[1],x[2],x[3]);
    vnl_vector_fixed<double,3> translation(x[4],x[5],x[6]);
    vnl_rotation3d_matrix  rotA, rotB, rotX;
    vnl_vector_fixed<double,3>  pi, piprime, sum2;
    vnl_matrix_fixed<double,3,3> identite, sum1;
    identite.set_identity();
    sum1.fill(0.0);    
    sum2.fill(0.0);    
    vnl_rigid_matrix X;
    double norme=0.0, a=0.0, frob;
    for( i=0 ; i<m_A_matrix.size() ; ++i )
    {
        //extraction des vecteurs rotation et translation
        X.setQuaternion(q);
        rotX = X.extract(3,3);
        rotB = m_B_matrix[i].getRotation();
        pi = m_B_matrix[i].getTranslation();
        rotA = m_A_matrix[i].getRotation();
        piprime = m_A_matrix[i].getTranslation();
        //mystere : si on normalise les vecteurs translation pi et piprime la rotation est juste
        //calcul du premier terme
        sum1 = rotX*rotB - rotA*rotX;
        sum1 = sum1*sum1 ;
        //calcul du  ( K - Id )
        rotA = rotA - identite;
        //rotation du vecteur translation de X
        translation = rotA * translation;
        //calcul du 2ieme terme
        sum2 = rotX*pi - piprime - translation;
        for( j=0 ; j<sum2.size() ; ++j )             
            sum2[j] = sum2[j]*sum2[j] ;
        X.fill(0.0);
        X.setRotation(sum1);
        for( j=0 ; j<sum2.size() ; ++j )    
            m_error+=sum2[j]*Lambda2;
        frob = X.frobenius_norm();    
        frob=frob*frob;
        m_error+=frob*Lambda1;
        translation[0] = x[4];
        translation[1] = x[5];
        translation[2] = x[6];
    }
    //calcul du terme qui assure que q est unitaire
    for(i=0;i<q.size();i++)
        norme+=(q[i]*q[i]);
    norme =sqrt(norme);
    m_error += (Lambda*(1- norme))*(Lambda*(1- norme));
    return observe(x);
}

// Dornaika_LS_cost_function
arlCore::Dornaika_LS_cost_function::Dornaika_LS_cost_function(const std::vector< vnl_rigid_matrix >&A, const std::vector< vnl_rigid_matrix >&B, unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g):
vnl_least_squares_function(number_of_unknowns, number_of_residuals, g),
m_A_matrix(A),
m_B_matrix(B){}

arlCore::Dornaika_LS_cost_function::~Dornaika_LS_cost_function(){}

void arlCore::Dornaika_LS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
//  assert(x.size()==number_of_unknowns);
    const double Lambda1=1;
    const double Lambda2=1;
    const unsigned int Lambda = 2000000; // 80000
    unsigned int i, j;
    m_error=0.0;
    vnl_quaternion<double> q(x[0],x[1],x[2],x[3]);
    vnl_vector_fixed<double,3> translation(x[4],x[5],x[6]);
    vnl_rotation3d_matrix  rotA, rotB, rotX;
    vnl_vector_fixed<double,3>  pi, piprime, sum2;
    vnl_matrix_fixed<double,3,3> identite, sum1;
    identite.set_identity();
    sum1.fill(0.0);    
    sum2.fill(0.0);    
    vnl_rigid_matrix X;
    double norme=0.0, a=0.0, frob, temp;
    for( i=0 ; i<m_A_matrix.size()  ; ++i )
    {
        temp=0;
        //extraction des vecteurs rotation et translation
        X.setQuaternion(q);
        rotX = X.extract(3,3);
        rotB = m_B_matrix[i].getRotation();
        pi = m_B_matrix[i].getTranslation();
        rotA = m_A_matrix[i].getRotation();
        piprime = m_A_matrix[i].getTranslation();
        //calcul du premier terme
        sum1 = rotX*rotB - rotA*rotX;
        sum1 = sum1*sum1 ;
        //calcul du  ( K - Id )
        rotA = rotA - identite;
        //rotation du vecteur translation de X
        translation = rotA * translation;
        //calcul du 2ieme terme
        sum2 = rotX*pi - piprime - translation;
        for( j=0 ; j<sum2.size() ; ++j )                        
        {
            sum2[j] = sum2[j]*sum2[j] ;
            temp+=sum2[j]*Lambda2;
        }
        X.fill(0.0);
        X.setRotation(sum1);
        frob = X.frobenius_norm();    
        norme=0.0;
        for(j=0;j<q.size();j++)
            norme += (q[j]*q[j]);
        norme = sqrt(norme);
        fx[i] = temp+frob*frob*Lambda1+Lambda*(1- norme)*(Lambda*(1- norme));    
        m_error += fx[i];
        translation[0] = x[4];
        translation[1] = x[5];
        translation[2] = x[6];
    }
    observe(x);
}
            
void arlCore::Dornaika_LS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& g)
{
    vnl_least_squares_function::fdgradf(x, g, 1e-8);
}

// Polynomial_cost_function
arlCore::Polynomial_cost_function::Polynomial_cost_function( const arlCore::PointList &real, const arlCore::PointList &distorded, unsigned int degree ) :
m_nbEquations(3),
vnl_cost_function(nbPolynomialParameters(degree, m_nbEquations)),
m_real(real),
m_distorded(distorded),
m_degree(degree){}  

arlCore::Polynomial_cost_function::~Polynomial_cost_function(){}

double arlCore::Polynomial_cost_function::f( vnl_vector<double > const &x )
{
    assert(m_distorded.size()==m_real.size());
    assert(nbPolynomialParameters(m_degree, m_nbEquations)==x.size());
    m_error=0;
    unsigned int i, j;
    for( i=0 ; i<m_distorded.size() ; ++i )
    {
        vnl_vector<double> distorded(m_nbEquations), distorsion;
        for( j=0 ; j<m_nbEquations ; ++j )
            distorded[j] = m_distorded[i]->get(j);
        if(!computePolynomial( m_degree, x, distorded, distorsion ))
            m_error += 10000;
        else
            for( j=0 ; j<m_nbEquations ; ++j )
                m_error += (distorded[j]-distorsion[j]-m_real[i]->get(j))*(distorded[j]-distorsion[j]-m_real[i]->get(j));
    }
    return observe(x);  
}

unsigned int arlCore::Polynomial_cost_function::getNbParameters()
{
    return nbPolynomialParameters(m_degree, m_nbEquations);
}

// OptimiseReprojectionUncertainty
arlCore::OptimiseReprojectionUncertainty::OptimiseReprojectionUncertainty(const vector<const arlCore::Camera*> &cameras, const vector<arlCore::Point::csptr> &points2D):
vnl_cost_function(3),
m_points2D(points2D),
m_cameras(cameras)
{
    unsigned int i;
    //m_inv_cov_matrix.resize(m_cameras.size());
    for( i=0 ; i<m_cameras.size() ; i++ )
    {
        vnl_matrix<double> tmp(2,2);
        tmp = vnl_matrix_inverse<double>(points2D[i]->getCovMatrix());
        m_inv_cov_matrix.push_back(tmp);
    }
}

arlCore::OptimiseReprojectionUncertainty::~OptimiseReprojectionUncertainty(){}

double arlCore::OptimiseReprojectionUncertainty::f(vnl_vector< double > const &x)
{
    assert(x.size()==3);
    vnl_vector_fixed<double,4> point3DH(x[0], x[1], x[2], 1.0);
    vnl_vector_fixed<double,2> reprojPoint2D;
    vnl_matrix_fixed<double,2,1> pointSubtraction;
    vnl_matrix_fixed<double,1,1> error(0.0);
    unsigned int i;
    for( i=0 ; i<m_cameras.size() ; i++ )
    {
        assert(m_points2D[i]->size()==2);
        m_cameras[i]->project3DPoint(point3DH, reprojPoint2D);
        pointSubtraction[0][0] = reprojPoint2D[0]-m_points2D[i]->x();
        pointSubtraction[1][0] = reprojPoint2D[1]-m_points2D[i]->y();
        if(m_points2D[i]->getCovMatrix().is_zero())
            error += pointSubtraction.transpose() * pointSubtraction;
        else error += pointSubtraction.transpose().as_matrix() * ((m_inv_cov_matrix[i])) * pointSubtraction;
        //std::cerr<<"mat cov  ="<<(m_inv_cov_matrix[i])<<std::endl;
    }
    m_error = error[0][0];
    return observe(x);
}

// OptimiseLineIntersection
arlCore::OptimiseLineIntersection::OptimiseLineIntersection(const std::vector< vgl_line_3d_2_points <double> > &lines):
vnl_cost_function(3),
m_lineList(lines){}

arlCore::OptimiseLineIntersection::~OptimiseLineIntersection(){}

double arlCore::OptimiseLineIntersection::f(vnl_vector< double > const &x)
{
    assert(x.size()==3);
    vgl_point_3d< double > vglPoint3D(x[0],x[1],x[2]);
    m_error=0.0;
    unsigned int i;
    for( i=0 ; i<m_lineList.size() ; i++ )
        m_error += vgl_distance(m_lineList[i], vglPoint3D);
    return observe(x);
}

// EPPC
arlCore::EPPC_cost_function::EPPC_cost_function(
    const std::vector < const arlCore::Camera *> &c,
    const std::vector<arlCore::Point*> &model3D,
    const std::vector< std::vector<arlCore::Point::csptr> >&points2DList,
    vnl_matrix_fixed<double,4,4> &trsf):
vnl_cost_function(3*(unsigned int)model3D.size()),
m_cameras(c),
m_model3D(model3D),
m_points2DList(points2DList),
m_trsf(trsf)
{
    unsigned int i,j;
    m_invCovMat3D.resize(model3D.size());
    for( i=0 ; i<model3D.size() ; ++i )
    {
        if( model3D[i]->getCovMatrix().is_zero())
            m_invCovMat3D[i].set_identity();
        else
            m_invCovMat3D[i] = vnl_matrix_inverse<double>(model3D[i]->getCovMatrix());
    }
    m_invCovMat2D.resize(points2DList.size());
    for( i=0 ; i<points2DList.size() ; ++i )
    {
        m_invCovMat2D[i].resize(points2DList[i].size());
        for( j=0 ; j<points2DList[i].size() ; ++j )
        {
            if( points2DList[i][j]->getCovMatrix().is_zero())
                m_invCovMat2D[i][j].set_identity();
            else
                m_invCovMat2D[i][j] = vnl_matrix_inverse<double>(points2DList[i][j]->getCovMatrix());
        }
    }
}

double arlCore::EPPC_cost_function::f(vnl_vector< double > const &x)
{
    const unsigned int ModelSize = (unsigned int)m_model3D.size();
    assert(x.size()==ModelSize*3);
    std::vector< vnl_vector_fixed<double,4> > points3DRepereCam(ModelSize);
    vnl_vector_fixed<double,4> points3DEstimes(1.0);
    vnl_matrix<double> vector3Dsub(3,1);
    vnl_matrix<double> vector2Dsub(2,1);
    unsigned int i,j;
    m_2Derror(0,0)=0.0;
    m_3Derror(0,0)=0.0;
    for( i=0 ; i<ModelSize ; ++i )
    {
        assert(m_model3D[i]->size()==3);
        points3DEstimes.put(0, x[3*i]);
        points3DEstimes.put(1, x[3*i+1]);
        points3DEstimes.put(2, x[3*i+2]);
        points3DRepereCam[i] = m_trsf * points3DEstimes;
        for( j=0 ; j<3 ; ++j )
            vector3Dsub.put(j, 0,  points3DEstimes(j) - m_model3D[i]->get(j) );
        m_3Derror += vector3Dsub.transpose() * m_invCovMat3D[i] * (vector3Dsub);
    }
    for( i=0 ; i<m_cameras.size() ; ++i )
    {
        assert(m_points2DList[i].size()==ModelSize);
        for(j=0 ; j<m_points2DList[i].size() ; ++j)
            if(m_points2DList[i][j]!=0)
            {
                assert(m_points2DList[i][j]->size()==2);
                if(m_points2DList[i][j]->isVisible())
                {
                    vnl_vector_fixed<double,2> projPoint2D, point2D(m_points2DList[i][j]->x(),m_points2DList[i][j]->y());
                    m_cameras[i]->project3DPoint(points3DRepereCam[j], projPoint2D);
                    vector2Dsub.set_column(0, projPoint2D - point2D);
                    m_2Derror += vector2Dsub.transpose() * m_invCovMat2D[i][j] * vector2Dsub;
                }
            }
    }   
    m_error = m_2Derror(0,0) + m_3Derror(0,0);
    //std::cerr<<"erreur EPPC 2D :"<< m_2Derror(0,0)<<"    3D :"<< m_3Derror(0,0)<<std::endl;
    return observe(x);
}

void arlCore::EPPC_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// EPPC LS
arlCore::EPPC_LS_cost_function::EPPC_LS_cost_function(const std::vector < const Camera *> &c,
                                                    const std::vector<Point*> &model3D, const std::vector< std::vector<Point::csptr> >&points2DList,
                                                    vnl_matrix_fixed<double,4,4> &trsf, unsigned int number_of_unknowns,
                                                    unsigned int number_of_residuals, UseGradient g):
vnl_least_squares_function(3*model3D.size(), number_of_residuals, g),
m_cameras(c),
m_model3D(model3D),
m_points2DList(points2DList),
m_trsf(trsf)
{
    unsigned int i,j;
    m_invCovMat3D.resize(model3D.size());
    for( i=0 ; i<model3D.size() ; ++i )
    {
        if( model3D[i]->getCovMatrix().is_zero())
            m_invCovMat3D[i].set_identity();
        else
            m_invCovMat3D[i] = vnl_matrix_inverse<double>(model3D[i]->getCovMatrix());
    }
    m_invCovMat2D.resize(points2DList.size());
    for( i=0 ; i<points2DList.size() ; ++i )
    {
        m_invCovMat2D[i].resize(points2DList[i].size());
        for( j=0 ; j<points2DList[i].size() ; ++j )
        {
            if( points2DList[i][j]->getCovMatrix().is_zero())
                m_invCovMat2D[i][j].set_identity();
            else
                m_invCovMat2D[i][j] = vnl_matrix_inverse<double>(points2DList[i][j]->getCovMatrix());
        }
    }
}

void arlCore::EPPC_LS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    const unsigned int ModelSize = (unsigned int)m_model3D.size();
    assert(x.size()==ModelSize*3);
    std::vector< vnl_vector_fixed<double,4> > points3DRepereCam(ModelSize);
    vnl_vector_fixed<double,4> points3DEstimes(1.0);
    vnl_matrix<double> vector3Dsub(3,1);
    vnl_matrix<double> vector2Dsub(2,1);
    unsigned int i,j, index=0;
    m_2Derror(0,0)=0.0;
    m_3Derror(0,0)=0.0;
    for( i=0 ; i<ModelSize ; ++i )
    {
        assert(m_model3D[i]->size()==3);
        points3DEstimes.put(0, x[3*i]);
        points3DEstimes.put(1, x[3*i+1]);
        points3DEstimes.put(2, x[3*i+2]);
        points3DRepereCam[i] = m_trsf * points3DEstimes;
        for( j=0 ; j<3 ; ++j )
            vector3Dsub.put(j, 0,  points3DEstimes(j) - m_model3D[i]->get(j) );
        m_3Derror += vector3Dsub.transpose() * m_invCovMat3D[i] * (vector3Dsub);
        fx[index] = sqrt( (vector3Dsub.transpose() * m_invCovMat3D[i] * (vector3Dsub))(0,0) );
        ++index;
    }
    for( i=0 ; i<m_cameras.size() ; ++i )
    {
        assert(m_points2DList[i].size()==ModelSize);
        for(j=0 ; j<m_points2DList[i].size() ; ++j)
        {
            if(m_points2DList[i][j]!=0)
            {
                assert(m_points2DList[i][j]->size()==2);
                if(m_points2DList[i][j]->isVisible())
                {
                    vnl_vector_fixed<double,2> point2D(m_points2DList[i][j]->x(),m_points2DList[i][j]->y());
                    vnl_vector_fixed<double,2> projPoint2D;
                    m_cameras[i]->project3DPoint(points3DRepereCam[j], projPoint2D);
                    vector2Dsub.set_column(0, projPoint2D - point2D);
                    m_2Derror += vector2Dsub.transpose() * m_invCovMat2D[i][j] * vector2Dsub;
                    fx[index] = sqrt((vector2Dsub.transpose() * m_invCovMat2D[i][j] * vector2Dsub)(0,0));
                }
            }
            ++index;
        }
    }
    //std::cerr<<"erreur EPPC 2D :"<< m_2Derror(0,0)<<"    3D :"<< m_3Derror(0,0)<<std::endl;
    m_error = m_2Derror(0,0) + m_3Derror(0,0);
    observe(x);
}

void arlCore::EPPC_LS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// ISPPC
arlCore::ISPPC_cost_function::ISPPC_cost_function(const std::vector<const Camera *>&a,
            const std::vector< vnl_vector_fixed<double,4> > &points3D,
            const std::vector< std::vector<Point::csptr> > &points2D, bool noise_feature):
vnl_cost_function(6),
m_cameras(a),
m_points3D(points3D),
m_points2D(points2D),
m_noise_feature(noise_feature) 
{
    if(m_noise_feature == true)
    {
        unsigned int i,j;
        m_invCovMat2D.resize(points2D.size());
        for( i=0 ; i<points2D.size() ; ++i )
        {
            assert(m_points3D.size()==m_points2D[i].size());
            m_invCovMat2D[i].resize(points2D[i].size());
            for( j=0 ; j<points2D[i].size() ; ++j )
            {
                if( points2D[i][j]->getCovMatrix().is_zero())
                    m_invCovMat2D[i][j].set_identity();
                else
                    m_invCovMat2D[i][j] = vnl_matrix_inverse<double>(points2D[i][j]->getCovMatrix());
            }
        }
    }
}

double arlCore::ISPPC_cost_function::f(vnl_vector< double > const &x)
{
    assert(x.size()==6);
    vnl_vector_fixed<double,2> projPoint2D;
    unsigned int i,j;
    m_error=0.0;
    double errorX, errorY;
    arlCore::vnl_rigid_vector vec(x);
    arlCore::vnl_rigid_matrix trsf(vec);
    if(!m_noise_feature)
    {
        for(i=0 ; i<m_cameras.size() ; ++i)
        {
            assert(m_points3D.size()==m_points2D[i].size());
            for(j=0 ; j<m_points2D[i].size() ; ++j)
                if(m_points2D[i][j]!=0)
                    if(m_points2D[i][j]->isVisible())
                    {
                        vnl_vector_fixed<double,4> points3DRepereCam = trsf * m_points3D[j];
                        if(m_cameras[i]->project3DPoint(points3DRepereCam, projPoint2D))
                        {
                            errorX = projPoint2D(0) - m_points2D[i][j]->x();
                            errorY = projPoint2D(1) - m_points2D[i][j]->y();
                            //std::cerr<< errorX << "  "<<errorY <<std::endl;
                            m_error += errorX*errorX + errorY*errorY;
                        }
                    }
        }
    }
    else
    {
        vnl_matrix<double> vector2Dsub(2,1);
        vnl_matrix_fixed<double,1,1> matrix_2Derror;
        matrix_2Derror.fill(0.0);
        for(i=0 ; i<m_cameras.size() ; ++i)
        {
            assert(m_points3D.size()==m_points2D[i].size());
            for(j=0 ; j<m_points2D[i].size() ; ++j)
                if(m_points2D[i][j]!=0)
                    if(m_points2D[i][j]->isVisible())
                    {
                        vnl_vector_fixed<double,4> points3DRepereCam = trsf * m_points3D[j];
                        if(m_cameras[i]->project3DPoint(points3DRepereCam, projPoint2D))
                        {   
                            vnl_vector_fixed<double,2> point2D(m_points2D[i][j]->x(),m_points2D[i][j]->y());
                            vector2Dsub.set_column(0, projPoint2D - point2D);
                            matrix_2Derror += vector2Dsub.transpose() * m_invCovMat2D[i][j] * vector2Dsub;
                        }
                    }
        }
        m_error = matrix_2Derror.get(0,0);
    }
    //std::cerr<<"error ISPPC POW= "<< m_error<<std::endl;
    return observe(x);
}
void arlCore::ISPPC_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// ISPPC LS
arlCore::ISPPC_LS_cost_function::ISPPC_LS_cost_function(const std::vector<const Camera *>&a,
            const std::vector< vnl_vector_fixed<double,4> > &points3D,
            const std::vector< std::vector<Point::csptr> > &points2D,
            unsigned int number_of_unknowns,
            unsigned int number_of_residuals, UseGradient g, bool noise_feature):
vnl_least_squares_function(number_of_unknowns, number_of_residuals, g),
m_cameras(a),
m_points3D(points3D),
m_points2D(points2D),
m_noise_feature(noise_feature) 
{
    if(m_noise_feature == true)
    {
        unsigned int i,j;
        m_invCovMat2D.resize(points2D.size());
        for( i=0 ; i<points2D.size() ; ++i )
        {
            m_invCovMat2D[i].resize(points2D[i].size());
            for( j=0 ; j<points2D[i].size() ; ++j )
            {
                if( points2D[i][j]->getCovMatrix().is_zero())
                    m_invCovMat2D[i][j].set_identity();
                else
                    m_invCovMat2D[i][j] = vnl_matrix_inverse<double>(points2D[i][j]->getCovMatrix());
            }
        }
    }
}

// Il faut connaitre  l'avance le nombre de points visibles (comptabilis par index).
// Ce nombre de points visibles doit tre donn en paramtre lors de la construction
// de l'objet arlCore::ISPPC_LS_cost_function
void arlCore::ISPPC_LS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==6);
    vnl_vector_fixed<double,2> projPoint2D;
    unsigned int i,j, index=0;
    double errorX, errorY, toto=0;
    arlCore::vnl_rigid_vector vec(x);
    arlCore::vnl_rigid_matrix trsf(vec);
    m_error = 0.0;
    if(m_noise_feature == false)
    {
        for(i=0 ; i<m_cameras.size() ; ++i)
        {
            assert(m_points3D.size()==m_points2D[i].size());
            for(j=0 ; j<m_points2D[i].size() ; ++j)
                if(m_points2D[i][j]!=0)
                    if(m_points2D[i][j]->isVisible())
                    {
                        vnl_vector_fixed<double,4> points3DRepereCam = trsf * m_points3D[j];
                        if(m_cameras[i]->project3DPoint(points3DRepereCam, projPoint2D))
                        {
                            errorX = projPoint2D(0)-m_points2D[i][j]->x();
                            errorY = projPoint2D(1)-m_points2D[i][j]->y();
                            fx[index] = sqrt(errorX *errorX + errorY*errorY);
                            m_error += fx[index];
                        }
                        index++;
                    }
        }
    }
    else
    {
        vnl_matrix<double> vector2Dsub(2,1);
        vnl_matrix_fixed<double,1,1> matrix_2Derror;
        for(i=0 ; i<m_cameras.size() ; ++i)
        {
            assert(m_points3D.size()==m_points2D[i].size());
            for(j=0 ; j<m_points2D[i].size() ; ++j)
                if(m_points2D[i][j]!=0)
                    if(m_points2D[i][j]->isVisible())
                    {
                        vnl_vector_fixed<double,4> points3DRepereCam = trsf * m_points3D[j];
                        if(m_cameras[i]->project3DPoint(points3DRepereCam, projPoint2D))
                        {
                            vnl_vector_fixed<double,2> point2D(m_points2D[i][j]->x(),m_points2D[i][j]->y());
                            vector2Dsub.set_column(0, projPoint2D - point2D);
                            matrix_2Derror = vector2Dsub.transpose() * m_invCovMat2D[i][j] * vector2Dsub;
                            //matrix_2Derror = vector2Dsub.transpose() * vector2Dsub;
                            //toto +=  matrix_2Derror(0,0);
                            fx[index] = sqrt(matrix_2Derror(0,0));
                            m_error += fx[index];
                        }
                        index++;
                    }
        }
    }
    observe(x);
}

void arlCore::ISPPC_LS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// OSPPC
arlCore::OSPPC_cost_function::OSPPC_cost_function(const std::vector<const arlCore::Camera *>&a,
    const std::vector< arlCore::Point* > &points3D,
    const std::vector< std::vector<arlCore::Point::csptr> > &points2D):
vnl_cost_function(6),
m_cameras(a),
m_points3D(points3D),
m_points2D(points2D)
{
    unsigned int i,j;
    m_extPoints2D.resize(m_cameras.size());
    for( i=0 ; i<m_cameras.size() ; ++i )
    {
        m_extPoints2D[i].resize(m_points2D[i].size());
        for( j=0 ; j<m_points2D[i].size() ; ++j )
            if(m_points2D[i][j]!=0)
                if(m_points2D[i][j]->isVisible())
                    m_cameras[i]->pixelPlaneToUnitFocalPlane( *(m_points2D[i][j]), m_extPoints2D[i][j], false );
    }
}

double arlCore::OSPPC_cost_function::f(vnl_vector< double > const &x)
{
    assert(x.size()==6);
    m_error = 0.0;
    unsigned int i,j;
    vnl_vector_fixed<double,3> crossProduct;
    vnl_vector_fixed<double,4> extPoints3D;
    arlCore::vnl_rigid_vector vec(x);
    arlCore::vnl_rigid_matrix trsf(vec);
    for(i=0 ; i<m_cameras.size() ; ++i)
    {
        assert(m_points3D.size()==m_points2D[i].size());
        for(j=0 ; j<m_points3D.size() ; ++j)
            if(m_points2D[i][j]!=0)
                if(m_points2D[i][j]->isVisible())
                {   //Ce critere calcule la distance entre les demi-droites [Cimij) et les points T*Mj
                    //On exprime d'abord le point mij dans le repere optique de la camera i
                    //Puis on calcule les coordonnes de T*Mj dans le repere optique de la camera i
                    //Puis on calcule cette distance en faisant le produit vectoriel [Cimij)xCiMj / norm([Cimij))= ||CiMj||sin(theta)
                    extPoints3D = m_cameras[i]->getExtrinsic()* trsf * m_points3D[j]->getHCoordinates();
                    crossProduct = vnl_cross_3d( m_extPoints2D[i][j], extPoints3D.extract(3) )/ m_extPoints2D[i][j].two_norm();
                    m_error += crossProduct.squared_magnitude();
                }
    }
    return observe(x);
}

void arlCore::OSPPC_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// OSPPC LS
arlCore::OSPPC_LS_cost_function::OSPPC_LS_cost_function(const std::vector<const Camera *>&a,
                                                        const std::vector< Point* > &points3D, const std::vector< std::vector<Point::csptr> > &points2D,
    unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g):
vnl_least_squares_function(number_of_unknowns, number_of_residuals, g),
m_cameras(a),
m_points3D(points3D),
m_points2D(points2D)
{
    unsigned int i,j;
    m_extPoints2D.resize(m_cameras.size());
    for( i=0 ; i<m_cameras.size() ; ++i )
    {
        m_extPoints2D[i].resize(m_points2D[i].size());
        for( j=0 ; j<m_points2D[i].size() ; ++j )
            if(m_points2D[i][j]!=0)
                if(m_points2D[i][j]->isVisible())
                    m_cameras[i]->pixelPlaneToUnitFocalPlane( *(m_points2D[i][j]), m_extPoints2D[i][j], false );
    }
}

void arlCore::OSPPC_LS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==6);
    unsigned int i,j, index=0;
    vnl_vector_fixed<double,3> crossProduct;
    vnl_vector_fixed<double,4> extPoints3D;
    arlCore::vnl_rigid_vector vec(x);
    arlCore::vnl_rigid_matrix trsf(vec);
    m_error = 0.0;
    for(i=0 ; i<m_cameras.size() ; ++i)
    {
        assert(m_points3D.size()==m_points2D[i].size());
        for(j=0 ; j<m_points3D.size() ; ++j)
        {
            if(m_points2D[i][j]!=0)
                if(m_points2D[i][j]->isVisible())
                {   //Ce critere calcule la distance entre les demi-droites [Cimij) et les points T*Mj
                    //On exprime d'abord le point mij dans le repere optique de la camera i
                    //Puis on calcule les coordonnes de T*Mj dans le repere optique de la camera i
                    //Puis on calcule cette distance en faisant le produit vectoriel [Cimij)xCiMj / norm([Cimij))= ||CiMj||sin(theta)
                    extPoints3D = m_cameras[i]->getExtrinsic()* trsf * m_points3D[j]->getHCoordinates();
                    crossProduct = vnl_cross_3d( m_extPoints2D[i][j], extPoints3D.extract(3) )/ m_extPoints2D[i][j].two_norm();
                    fx[index]=sqrt(crossProduct.squared_magnitude());
                    m_error += fx[index];
                }
            ++index;
        }
    }
    observe(x);
}

void arlCore::OSPPC_LS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// INTRINSIC
arlCore::Intrinsic_cost_function::Intrinsic_cost_function(unsigned int nbParameters):
    m_nbParameters(nbParameters)//nb de parametres que l'on souhaite optimiser
{
    if(nbParameters==0 || nbParameters>8) m_nbParameters=8;
    else m_nbParameters = nbParameters;
    assert(m_nbParameters>=4);
    m_camera = new Camera(m_universe);
    m_verbose = false; 
    m_available_reprojection_error = false;
}

arlCore::Intrinsic_cost_function::~Intrinsic_cost_function()
{
    unsigned int i;
    for( i=0 ; i<m_2DpatternsList.size() ; ++i )
        if(m_2DpatternsList[i]!=0) delete m_2DpatternsList[i];
    for( i=0 ; i<m_3DpatternsList.size() ; ++i )
        if(m_3DpatternsList[i]!=0) delete m_3DpatternsList[i];
    if(m_camera!=0) delete m_camera;
}

unsigned int arlCore::Intrinsic_cost_function::getNbParameters( void ) const
{
    return m_nbParameters;
}

bool arlCore::Intrinsic_cost_function::save( const std::string &fileName ) const
{
    if(m_camera==0) return false;
    return m_camera->save(fileName);
}

unsigned int arlCore::Intrinsic_cost_function::addPattern( const std::vector<arlCore::Point*>& points2D, const std::vector<arlCore::Point*>& model3D )
{// Call addPattern for each rigid pattern and each pose
    assert(points2D.size()==model3D.size());
    unsigned int i, size = (unsigned int)m_2DpatternsList.size();
    PointList *patternsList2D = new PointList(2);
    PointList *patternsList3D = new PointList(3);
    for( i=0 ; i<points2D.size() ; ++i )
        if(patternsList2D->push_back(*(points2D[i])))
            if(!patternsList3D->push_back(*(model3D[i])))
                patternsList2D->pop_back();
    m_2DpatternsList.push_back(patternsList2D);
    m_3DpatternsList.push_back(patternsList3D);
    assert(m_2DpatternsList[size]->size()==m_3DpatternsList[size]->size());
    assert(m_2DpatternsList.size()==m_3DpatternsList.size());
    set_number_of_unknowns(m_nbParameters + 6 * m_2DpatternsList.size());
    return(m_2DpatternsList[size]->size());
}

void arlCore::Intrinsic_cost_function::setVerbose(bool verbose)
{m_verbose = verbose;}

std::vector<double> arlCore::Intrinsic_cost_function::getReprojectionError( vnl_vector< double > const &x)
{
    m_available_reprojection_error = true;
    f(x);
    return m_point_reprojection_error;
}

double arlCore::Intrinsic_cost_function::f(vnl_vector< double > const &x)
{
    m_error = 0.0;
    double errorX, errorY;
    unsigned int i,j;
    long int nbPoints = 0;
    Point point2D(2);
    m_camera->setIntrinsic(x.extract(m_nbParameters));
    for( i=0 ; i<m_2DpatternsList.size() ; ++i )
    {
        arlCore::vnl_rigid_vector vec(x.extract(6,m_nbParameters+6*i));
        arlCore::vnl_rigid_matrix Ti(vec);
        m_camera->setExtrinsic(Ti);
        for( j=0 ; j<m_2DpatternsList[i]->size() ; ++j,++nbPoints )
            if(m_camera->project3DPoint(*(m_3DpatternsList[i]->get(j)), point2D))
            {
                errorX = point2D.x() - m_2DpatternsList[i]->get(j)->x();
                errorY = point2D.y() - m_2DpatternsList[i]->get(j)->y();
                m_error += errorX*errorX + errorY*errorY;
                if(m_available_reprojection_error)
                {
                    m_point_reprojection_error.push_back(sqrt(errorX *errorX + errorY*errorY));
                }
            }
            else // Focal plane
                return error(x);
    }
    m_error /= nbPoints;
    if(m_verbose){std::cerr<<"reprojection error= "<<sqrt(m_error)<<std::endl;}
    return observe(x);
}

void arlCore::Intrinsic_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& g)
{
    fdgradf(x, g);
}

// INTRINSIC LS
arlCore::IntrinsicLS_cost_function::IntrinsicLS_cost_function(unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g):
    vnl_least_squares_function(number_of_unknowns, number_of_residuals, g)
    //nbParameters = nb de parametre que l'on souhaite optimiser
{
    m_nbParameters = 8;
//  assert(nbParameters<=10);//verifie la coherence du nombre de parametre estime
//  if(nbParameters==0 || nbParameters>8) m_nbParameters=8;
//  else m_nbParameters = nbParameters;
//  assert(m_nbParameters>=4);
    m_verbose = false;
    m_camera = new Camera(m_universe);
}

arlCore::IntrinsicLS_cost_function::~IntrinsicLS_cost_function()
{
    unsigned int i;
    for( i=0 ; i<m_2DpatternsList.size() ; ++i )
        if(m_2DpatternsList[i]!=0) delete m_2DpatternsList[i];
    for( i=0 ; i<m_3DpatternsList.size() ; ++i )
        if(m_3DpatternsList[i]!=0) delete m_3DpatternsList[i];
    if(m_camera!=0) delete m_camera;
}

unsigned int arlCore::IntrinsicLS_cost_function::getNbParameters( void ) const
{
    return m_nbParameters;
}

bool arlCore::IntrinsicLS_cost_function::save( const std::string &fileName ) const
{
    if(m_camera==0) return false;
    return m_camera->save(fileName);
}

unsigned int arlCore::IntrinsicLS_cost_function::addPattern( const std::vector<arlCore::Point*>& points2D, const std::vector<arlCore::Point*>& model3D )
{// faire addPattern pour chaque pattern rigide et chaque pose
    assert(points2D.size()==model3D.size());
    unsigned int i, size = (unsigned int)m_2DpatternsList.size();
    PointList *patternsList2D = new PointList(2);
    PointList *patternsList3D = new PointList(3);
    for( i=0 ; i<points2D.size() ; ++i )
        if(patternsList2D->push_back(*(points2D[i])))
            if(!patternsList3D->push_back(*(model3D[i])))
                patternsList2D->pop_back();
    m_2DpatternsList.push_back(patternsList2D);
    m_3DpatternsList.push_back(patternsList3D);
    assert(m_2DpatternsList[size]->size()==m_3DpatternsList[size]->size());
    assert(m_2DpatternsList.size()==m_3DpatternsList.size());
    //set_number_of_unknowns(m_nbParameters + 6 * m_2DpatternsList.size());
    return(m_2DpatternsList[size]->size());
}

void arlCore::IntrinsicLS_cost_function::getReprojectionError( vnl_vector< double > const &x, vnl_vector< double > &fx )
{
    f(x,fx);
}

void arlCore::IntrinsicLS_cost_function::setVerbose(bool verbose)
{m_verbose = verbose;}

void arlCore::IntrinsicLS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    m_error = 0.0;
    unsigned int i,j;
    long int index = 0, nbPoints = 0;
    Point point2D(2);
    m_camera->setIntrinsic(x.extract(m_nbParameters));
    for( i=0 ; i<m_2DpatternsList.size() ; ++i )
    {
        arlCore::vnl_rigid_vector vec(x.extract(6,m_nbParameters+6*i));
        arlCore::vnl_rigid_matrix Ti(vec);
        m_camera->setExtrinsic(Ti);
        for( j=0 ; j<m_2DpatternsList[i]->size() ; ++j,++index, ++nbPoints )
            if(m_camera->project3DPoint(*(m_3DpatternsList[i]->get(j)), point2D))
            {
                //fx[index + j] = point2D.distance(*(m_2DpatternsList[i]->get(j)));
                fx[index] = point2D.distance(*(m_2DpatternsList[i]->get(j)));
                m_error += fx[index]*fx[index];
            }else
            {   //! @todo Fixer le problme des singularits
                //fx[index + j]=10000;
                fx[index]=10000;
                m_error += 10000;
                std::cout<<"PlanFOCAL\n";
            }
        //index += m_2DpatternsList[i]->size();
    }
    m_error /= nbPoints;
    observe(x);
}

void arlCore::IntrinsicLS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& j)
{
    const bool Verbose = false;
    fdgradf(x, j, 1e-8);
    if(Verbose)
    {
        std::cerr<<"calcul du gradient"<<std::endl;
        std::cerr<<"reprojection error= "<<sqrt(m_error)<<std::endl;
    }
}

// EXTRINSIC
arlCore::Extrinsic_cost_function::Extrinsic_cost_function(const std::vector<const arlCore::Camera *>&a, unsigned int nou):
vnl_cost_function(nou),
m_cameras(a)
{
    m_verbose = false; 
    m_available_reprojection_error = false;
}


arlCore::Extrinsic_cost_function::~Extrinsic_cost_function(){}

unsigned int arlCore::Extrinsic_cost_function::addPattern(const std::vector<std::vector<arlCore::Point*> >& points2D, const std::vector<arlCore::Point*>& model3D)
{// faire addPattern pour chaque pattern rigide et chaque pose
    //assert(points2D.size()==model3D.size());
    unsigned int i, j, size = (unsigned int)m_3DpatternsList.size();
    
    std::vector<std::vector<arlCore::Point*> > patternsList2D;
    std::vector< arlCore::Point* > patternsList3D(model3D.size());
    for( i=0 ; i<model3D.size() ; ++i )
    {
        patternsList3D[i] = model3D[i];
//          if(patternsList2D->push_back(*(points2D[i])))
//              if(!patternsList3D->push_back(*(model3D[i])))
//                  patternsList2D->pop_back();
    }
    for( j=0 ; j<m_cameras.size() ; ++j )
    {
        std::vector<arlCore::Point*> tmp(points2D[j].size());
        for( i=0 ; i<model3D.size() ; ++i )
            tmp[i]=points2D[j][i];
        patternsList2D.push_back(tmp);              
    }           
    m_2DpatternsList.push_back(patternsList2D);
    m_3DpatternsList.push_back(patternsList3D);
    //assert(m_2DpatternsList[size]->size()==m_3DpatternsList[size]->size());
    //assert(m_2DpatternsList.size()==m_3DpatternsList.size());
    //set_number_of_unknowns(m_nbParameters + 6 * m_2DpatternsList.size());
    return(0);
}

void arlCore::Extrinsic_cost_function::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

std::vector<double> arlCore::Extrinsic_cost_function::getReprojectionError(vnl_vector< double > const &x)
{
    m_available_reprojection_error = true;
    f(x);
    return m_point_reprojection_error;
}

double arlCore::Extrinsic_cost_function::f(vnl_vector< double > const &x)
{
    const unsigned int NbCameras = (unsigned int)m_cameras.size();
    const unsigned int NbPoses = (unsigned int)m_3DpatternsList.size();
    std::vector <arlCore::vnl_rigid_matrix> listTransfo(NbPoses + NbCameras -1);         
    vnl_vector_fixed<double,4> points3DRepereCam;
    vnl_vector_fixed<double,2> projPoint2D;
    unsigned int i,j,k,index=0;
    m_error = 0.0;
    double errorX, errorY;
    // - il y a NbPoses Tei ie transformation entre les differentes poses de la mire
    // et la camera 1
    // - plus NbCameras-1 Tsj entre la camera 1 et les autres cameras
    // TOTAL NbPoses + NbCameras -1 transformations a estimer
    // par convention les NbPoses sont en premier dans le vnl_vector< double > x
    for( i=0 ; i<NbPoses+NbCameras-1 ; ++i )
    {
        arlCore::vnl_rigid_vector rigid_vect;
        for( j=0 ; j<6; ++j )
            rigid_vect.put(j, x[6*i+j]);
        listTransfo[i]=arlCore::vnl_rigid_matrix(rigid_vect);
    } 
    //Calcul des coordonnes de points3d aprs application de la transfo recherche
    for( i=0 ; i<NbPoses; ++i )
        for( j=0 ; j<NbCameras; ++j )
            for( k=0 ; k<m_3DpatternsList[i].size() ; ++k )
            {
                if(j==0) points3DRepereCam = listTransfo[i] * m_3DpatternsList[i][k]->getHCoordinates();
                else points3DRepereCam = listTransfo[NbPoses-1+j] * listTransfo[i] * m_3DpatternsList[i][k]->getHCoordinates();
                if(m_2DpatternsList[i][j][k]!=0)
                {
                    m_cameras[j]->project3DPoint(points3DRepereCam, projPoint2D);
                    errorX = projPoint2D(0) - m_2DpatternsList[i][j][k]->x();
                    errorY = projPoint2D(1) - m_2DpatternsList[i][j][k]->y();
                    m_error += errorX *errorX + errorY*errorY;
                    if(m_available_reprojection_error)
                    {
                        m_point_reprojection_error.push_back(sqrt(errorX *errorX + errorY*errorY));
                        //std::cerr<<"hopla "<<errorX *errorX + errorY*errorY<<" "<<m_point_reprojection_error.size()<<"\n";
                    }
                    ++index;
                }
            }
    m_error /= index;
    if(m_verbose){std::cerr<<"reprojection error= "<<sqrt(m_error)<<std::endl;}
    return observe(x);
}

//void arlCore::Extrinsic_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& g)
//{
//  fdgradf(x, g);  
//}

// EXTRINSIC LS
arlCore::ExtrinsicLS_cost_function::ExtrinsicLS_cost_function(const std::vector<const arlCore::Camera *>&a,unsigned int number_of_unknowns, unsigned int number_of_residuals, UseGradient g):
    vnl_least_squares_function(number_of_unknowns, number_of_residuals, g),
    m_cameras(a)
{
    m_verbose = false; 
}

arlCore::ExtrinsicLS_cost_function::~ExtrinsicLS_cost_function(){}

void arlCore::ExtrinsicLS_cost_function::setVerbose(bool verbose)
{m_verbose = verbose;}

unsigned int arlCore::ExtrinsicLS_cost_function::addPattern( const std::vector<std::vector<arlCore::Point*> >& points2D, const std::vector<arlCore::Point*>& model3D )
{// faire addPattern pour chaque pattern rigide et chaque pose
    //assert(points2D.size()==model3D.size());
    unsigned int i, j, size = (unsigned int)m_3DpatternsList.size();
    
    std::vector<std::vector<arlCore::Point*> > patternsList2D;
    std::vector< arlCore::Point* > patternsList3D(model3D.size());
    for( i=0 ; i<model3D.size() ; ++i )
    {
        patternsList3D[i] = model3D[i];
//          if(patternsList2D->push_back(*(points2D[i])))
//              if(!patternsList3D->push_back(*(model3D[i])))
//                  patternsList2D->pop_back();
    }
    for( j=0 ; j<m_cameras.size() ; ++j )
    {
        std::vector<arlCore::Point*> tmp(points2D[j].size());
        for( i=0 ; i<model3D.size() ; ++i )
            tmp[i]=points2D[j][i];
        patternsList2D.push_back(tmp);              
    }           
    m_2DpatternsList.push_back(patternsList2D);
    m_3DpatternsList.push_back(patternsList3D);
    //assert(m_2DpatternsList[size]->size()==m_3DpatternsList[size]->size());
    //assert(m_2DpatternsList.size()==m_3DpatternsList.size());
    //set_number_of_unknowns(m_nbParameters + 6 * m_2DpatternsList.size());
    return 0;
}

void arlCore::ExtrinsicLS_cost_function::getReprojectionError( vnl_vector< double > const &x, vnl_vector< double > &fx )
{
    f(x,fx);
}

void arlCore::ExtrinsicLS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    long int index=0;// tmp_index=0;
    //index increments for each 2D point visible in the video image.
    //tmp_index is used in a loop and contain the number of visible 2D points for a video image
    const unsigned int NbCameras = (unsigned int)m_cameras.size();
    const unsigned int NbPoses = (unsigned int)m_3DpatternsList.size();
    std::vector <arlCore::vnl_rigid_matrix> listTransfo(NbPoses+NbCameras-1);         
    vnl_vector_fixed<double,4> points3DRepereCam;
    vnl_vector_fixed<double,2> projPoint2D;
    unsigned int i,j,k;
    m_error = 0.0;
    double errorX, errorY;
    // - il y a NbPoses Tei ie transformation entre les differentes poses de la mire
    // et la camera 1
    // - plus NbCameras-1 Tsj entre la camera 1 et les autres cameras
    // TOTAL NbPoses + NbCameras -1 transformations a estimer
    // par convention les NbPoses sont en premier dans le vnl_vector< double > x
    for( i=0 ; i<NbPoses+NbCameras-1 ; ++i)
    {
        arlCore::vnl_rigid_vector rigid_vect;
        for(j=0 ; j<6; ++j)
            rigid_vect.put(j, x[6*i+j]);
        listTransfo[i]=arlCore::vnl_rigid_matrix(rigid_vect);
    } 
    for( i=0 ; i<NbPoses; ++i )
        for( j=0 ; j<NbCameras; ++j )
//      {
            for( k=0 ; k<m_3DpatternsList[i].size() ; ++k )
            {
                if(j==0) points3DRepereCam = listTransfo[i] * m_3DpatternsList[i][k]->getHCoordinates();
                else points3DRepereCam = listTransfo[NbPoses-1+j] * listTransfo[i] * m_3DpatternsList[i][k]->getHCoordinates();
                if(m_2DpatternsList[i][j][k]!=0)
                {
                    m_cameras[j]->project3DPoint(points3DRepereCam, projPoint2D);//[i][j][k]);
                    errorX  = projPoint2D(0) - m_2DpatternsList[i][j][k]->x();
                    errorY  = projPoint2D(1) - m_2DpatternsList[i][j][k]->y();
                    fx[index] = sqrt(errorX *errorX + errorY*errorY);
                    m_error += fx[index]*fx[index];
                    ++index;
//                  ++tmp_index;
                }
            }
//          index += tmp_index;
//          tmp_index = 0;
//      }
            
    m_error /= index;
    //std::cerr<<"erreur reprojection = "<<sqrt(m_error/index) <<"  " <<index<<std::endl;
    observe(x);
}

void arlCore::ExtrinsicLS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& j)
{
    fdgradf(x, j, 1e-8);
    if(m_verbose)
    {
        std::cerr<<"calcul du gradient"<<std::endl;
        std::cerr<<"reprojection error= "<<sqrt(m_error)<<std::endl;
    }   
}

// AX=XB
double arlCore::AX_XB_cost_function::f(vnl_vector< double > const &x)
{
    const bool Verbose = false;
    assert(x.size()==6); //6
    assert(m_A_matrix.size()==m_B_matrix.size());
    arlCore::vnl_rigid_vector vX(x.extract(6));
    arlCore::vnl_rigid_matrix mX(vX);
    m_error = criterion( mX );
    if(Verbose) cerr<<"error AX XB = "<<m_error/m_A_matrix.size()<<endl;
    return observe(x);
}

double arlCore::AX_XB_cost_function::criterion( const arlCore::vnl_rigid_matrix &X )
{
    unsigned int i;
    // 1=Norme de frobenius de AX-XB
    // 2=Klaus Strobl & Gerd Hirzinger
    const unsigned int Method = 2;
    double error = 0.0, tErr, rErr;
    for( i=0 ; i<m_A_matrix.size() ; ++i )
    {
        const vnl_rigid_matrix AX = m_A_matrix[i] * X;
        const vnl_rigid_matrix XB = X * m_B_matrix[i];
        AX.compare( XB, tErr, rErr );
        const double TErr2 = tErr*tErr, RErr2 = rErr*rErr;
        if(Method==1) error += (AX-XB).frobenius_norm();
        if(Method==2) error += RErr2 + m_alpha*TErr2;
    }
    return error;
}

double arlCore::AX_XB_cost_function::getAlpha( const arlCore::vnl_rigid_matrix &X )
{
    const bool Verbose = false;
    unsigned int i;
    double tErr, rErr;
    std::vector< double > translationErrors, rotationErrors;
    for( i=0 ; i<m_A_matrix.size() ; ++i )
    {
        const vnl_rigid_matrix AX = m_A_matrix[i] * X;
        const vnl_rigid_matrix XB = X * m_B_matrix[i];
        AX.compare( XB, tErr, rErr );
        translationErrors.push_back( tErr );
        rotationErrors.push_back( rErr );
    }
    double min, max, rms, mean, stddev;
    arlCore::statistic( translationErrors, min, max, rms, mean, stddev, false );
    const double SigmaT = stddev;
    arlCore::statistic( rotationErrors, min, max, rms, mean, stddev, false );
    const double SigmaR = stddev;
    if(SigmaR<1e-14) return 1.0;
    const double Ratio = (SigmaT/SigmaR)*(SigmaT/SigmaR);
    if(Verbose) std::cout<<"Ratio="<<Ratio<<" Sigma T"<<SigmaT<<" R"<<SigmaR<<"\n";
    if(Ratio>1e-5) return 1.0/Ratio; else return 1.0;
}

void arlCore::AX_XB_cost_function::setAlpha( double alpha )
{
    m_alpha = alpha;
}

void arlCore::AX_XB_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// AXB_Z_cost_function
double arlCore::AXB_Z_cost_function::criterion( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z )
{
    unsigned int i;
    double error1 = 0.0, error2 = 0.0, error3 = 0.0, error4 = 0.0;
    const arlCore::vnl_rigid_matrix invZ = Z.computeInverse();
    assert( m_A_matrix.size()==m_B_matrix.size() );
    for( i=0 ; i<m_A_matrix.size() ; ++i )
    {
        const arlCore::vnl_rigid_matrix AXB = m_A_matrix[i] * X * m_B_matrix[i];
        // 1st Method : Optimization of A_iXB_i - Z
        const vnl_matrix_fixed<double,4,4> Sum = AXB - Z.as_matrix() ;
        error1 += Sum.frobenius_norm();

        // 2nd Method : Optimization of sum_i
        const arlCore::vnl_rigid_vector V( AXB * invZ );
        error2 += V.as_vector().two_norm();

        // 3rd Method : Optimization of distance2 between AXB & Z       
        error3 += AXB.distance2( Z, arlCore::ARLCORE_VRM_DISTANCE_3AXIS );

        // 4th Method Klaus H. Strobl & Gerd Hirzinger
        // Proceedings of the 2006 IEEE/RSJ International Conference on intelligent Robots and Systems
        // October 9 -15, 2006, Beijing, China
        double tErr, rErr;
        AXB.compare( Z, tErr, rErr );
        const double TErr2 = tErr*tErr, RErr2 = rErr*rErr;
        error4 += RErr2 + m_alpha*TErr2;
    }
    return error4;
}

double arlCore::AXB_Z_cost_function::getAlpha( const arlCore::vnl_rigid_matrix &X, const arlCore::vnl_rigid_matrix &Z )
{
    unsigned int i;
    double tErr, rErr;
    std::vector< double > translationErrors, rotationErrors;
    for( i=0 ; i<m_A_matrix.size() ; ++i )
    {
        const vnl_rigid_matrix BXA = m_B_matrix[i] * X * m_A_matrix[i];
        BXA.compare( Z, tErr, rErr );
        translationErrors.push_back( tErr );
        rotationErrors.push_back( rErr );
    }
    double min, max, rms, mean, stddev;
    arlCore::statistic( translationErrors, min, max, rms, mean, stddev, false );
    const double SigmaT = stddev;
    arlCore::statistic( rotationErrors, min, max, rms, mean, stddev, false );
    const double SigmaR = stddev;
    if(SigmaR<1e-14) return 1.0;
    const double Ratio = (SigmaT/SigmaR)*(SigmaT/SigmaR);
    std::cout<<"Ratio="<<Ratio<<" Sigma T"<<SigmaT<<" R"<<SigmaR<<"\n";
    if(Ratio>1e-5) return 1.0/Ratio; else return 1.0;

}

double arlCore::AXB_Z_cost_function::f(vnl_vector< double > const &x)
{
    assert(x.size()==12);
    arlCore::vnl_rigid_vector vX(x.extract(6));
    arlCore::vnl_rigid_vector vZ(x.extract(6,6));
    arlCore::vnl_rigid_matrix X(vX);
    arlCore::vnl_rigid_matrix Z(vZ);
    m_error = criterion( X, Z );
    //cerr<<"AXBZ = "<<m_error / (double)m_A_matrix.size()<<endl;
    return observe(x);
}

void arlCore::AXB_Z_cost_function::setAlpha( double alpha )
{
    m_alpha = alpha;
}

void arlCore::AXB_Z_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    fdgradf(x, j, 1e-8);
}

// register3D3DUncertainty_cost_function
arlCore::register3D3DUncertainty_cost_function::register3D3DUncertainty_cost_function(const arlCore::PointList &points3D_source,
            const arlCore::PointList &points3D_cible):
vnl_cost_function(6),
m_points3D_source(points3D_source),
m_points3D_cible(points3D_cible)
{
    assert(m_points3D_source.size()==m_points3D_cible.size());
    unsigned int i;
    //m_inv_cov_matrix.resize(m_cameras.size());
    for( i=0 ; i<m_points3D_source.size() ; i++ )
    {
        vnl_matrix<double> tmp(3,3);
        tmp = vnl_matrix_inverse<double>(points3D_cible[i]->getCovMatrix());
        m_inv_cov_matrix.push_back(tmp);
    }
}

double arlCore::register3D3DUncertainty_cost_function::f(vnl_vector< double > const &x)
{
    assert(x.size()==6);
    unsigned int i;
    vnl_matrix_fixed<double,3,1> pointSubtraction;
    vnl_matrix_fixed<double,1,1> error(0.0);
    arlCore::vnl_rigid_vector vec(x);
    arlCore::vnl_rigid_matrix trsf(vec);
    m_error=0.0;    
    for(i=0 ; i<m_points3D_source.size() ; ++i)
    {
        arlCore::Point points3D_recale;
        trsf.trf( *(m_points3D_source[i]), points3D_recale);
        pointSubtraction[0][0] = points3D_recale[0]-(*m_points3D_cible[i])[0];
        pointSubtraction[1][0] = points3D_recale[1]-(*m_points3D_cible[i])[1];
        pointSubtraction[2][0] = points3D_recale[2]-(*m_points3D_cible[i])[2];
        {
            if(m_points3D_cible[i]->getCovMatrix().is_zero())
            {
                error = pointSubtraction.transpose() * pointSubtraction;
                m_error += error(0,0);
                //std::cerr<<"covariance nulle"<<std::endl;
            }
            else 
            {
                error = pointSubtraction.transpose().as_matrix() * ((m_inv_cov_matrix[i])) * pointSubtraction;
                //error = pointSubtraction.transpose().as_matrix() * pointSubtraction;
                m_error += error(0,0);
            }
            //std::cerr<<"mat cov  ="<<(m_inv_cov_matrix[i])<<std::endl;
        }
    }
    return observe(x);
}

// register3D3DUncertainty_cost_function
void arlCore::register3D3DUncertainty_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    //cerr<<"Calcul de gradf register3D3DUncertainty_LS_cost_function------------------"<<endl;
    fdgradf(x, j, 1e-8);
}


arlCore::register3D3DUncertainty_LS_cost_function::register3D3DUncertainty_LS_cost_function(const arlCore::PointList &points3D_source,
            const arlCore::PointList &points3D_cible,
            unsigned int number_of_unknowns,
            unsigned int number_of_residuals, UseGradient g):
vnl_least_squares_function(number_of_unknowns, number_of_residuals, g),
m_points3D_source(points3D_source),
m_points3D_cible(points3D_cible)
{
    assert(m_points3D_source.size()==m_points3D_cible.size());
    unsigned int i;
    //m_inv_cov_matrix.resize(m_cameras.size());
    for( i=0 ; i<m_points3D_source.size() ; i++ )
    {
        vnl_matrix<double> tmp(3,3);
        tmp = vnl_matrix_inverse<double>(points3D_cible[i]->getCovMatrix());
        m_inv_cov_matrix.push_back(tmp);
    }
}
void arlCore::register3D3DUncertainty_LS_cost_function::f(vnl_vector< double > const &x, vnl_vector< double > &fx)
{
    assert(x.size()==6);
    unsigned int i;
    vnl_matrix_fixed<double,3,1> pointSubtraction;
    vnl_matrix_fixed<double,1,1> error(0.0);
    arlCore::vnl_rigid_vector vec(x);
    arlCore::vnl_rigid_matrix trsf(vec);
    
    for(i=0 ; i<m_points3D_source.size() ; ++i)
    {
        arlCore::Point points3D_recale;
        trsf.trf( *(m_points3D_source[i]), points3D_recale);
        pointSubtraction[0][0] = points3D_recale[0]-(*m_points3D_cible[i])[0];
        pointSubtraction[1][0] = points3D_recale[1]-(*m_points3D_cible[i])[1];
        pointSubtraction[2][0] = points3D_recale[2]-(*m_points3D_cible[i])[2];
        {
            if(m_points3D_cible[i]->getCovMatrix().is_zero())
            {
                error = pointSubtraction.transpose() * pointSubtraction;
                fx[i] = sqrt( error(0,0) );
                //std::cerr<<"covariance nulle"<<std::endl;
            }
            else 
            {
                error = pointSubtraction.transpose().as_matrix() * ((m_inv_cov_matrix[i])) * pointSubtraction;
                //error = pointSubtraction.transpose().as_matrix() * pointSubtraction;
                fx[i] = sqrt( error(0,0) );
            }
            //std::cerr<<"mat cov  ="<<(m_inv_cov_matrix[i])<<std::endl;
        }
    }
    observe(x);
}
void arlCore::register3D3DUncertainty_LS_cost_function::gradf(vnl_vector< double > const &x, vnl_matrix<double>& j)
{
    //cerr<<"Calcul de gradf register3D3DUncertainty_LS_cost_function------------------"<<endl;
    fdgradf(x, j, 1e-8);
}

// averageRotation_cost_function
double arlCore::averageRotation_cost_function::f (vnl_vector< double > const &x)
{
//   vnl_vector_fixed<double,3> V_mean;
    arlCore::vnl_rotation3d_vector tmp(x[0],x[1],x[2] );
    arlCore::vnl_rotation3d_matrix M_mean(tmp);
    unsigned int i;
    m_error=0;
    for(i=0;i<m_rotation_list.size();i++)
        m_error+= M_mean.sq_rieman_dist(*(m_rotation_list[i]));
    return observe(x);
}

void arlCore::averageRotation_cost_function::gradf(vnl_vector< double > const &x, vnl_vector<double>& j)
{
    //cerr<<"Calcul de gradf averageRotation------------------"<<endl;
    fdgradf(x, j, 1e-8);
}
