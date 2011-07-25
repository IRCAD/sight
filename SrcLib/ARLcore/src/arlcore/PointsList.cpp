/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/PointsList.h>

#include <fstream>
#include <iomanip>
#include <algorithm>

//#include <vgl/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>

#include <vnl/algo/vnl_powell.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_cross.h>

#include <arlcore/Optimization.h>
#include <arlcore/Misc.h>

arlCore::PointList::PointList( unsigned int dim, const std::string &name ):
Object(ARLCORE_CLASS_POINTLIST)
{
    m_dimension=dim;
    setName(name);
}

arlCore::PointList::PointList( const PointList& p ):
Object(ARLCORE_CLASS_POINTLIST)
{
    copy(p);
}

arlCore::PointList::PointList( const std::vector< const arlCore::Point* > &p ):
Object(ARLCORE_CLASS_POINTLIST)
{
    if(p.size()==0) m_dimension=3;
    else
    {
        m_dimension=p[0]->size();
        unsigned int i;
        for( i=0 ; i<p.size() ; ++i )
            if(p[i]!=0)
                if(p[i]->size()==m_dimension) m_pointList.push_back(new Point(*(p[i])));
    }
}

arlCore::PointList::PointList( const std::vector< const arlCore::Point* > &p, double scalarMin, double scalarMax ):
Object(ARLCORE_CLASS_POINTLIST)
{
    if(p.size()==0) m_dimension=3;
    else
    {
        m_dimension=p[0]->size();
        unsigned int i;
        for( i=0 ; i<p.size() ; ++i )
            if(p[i]!=0)
                if(p[i]->size()==m_dimension && p[i]->getScalar()>=scalarMin && p[i]->getScalar()<=scalarMax) m_pointList.push_back(new Point(*(p[i])));
    }
}

arlCore::PointList& arlCore::PointList::operator=( const PointList& p )
{
    copy(p);
    return *this;
}

void arlCore::PointList::copy( const PointList& p )
{
    if(this==&p) return;
    arlCore::Object *a=this;
    const arlCore::Object *b=&p;
    *a = *b;
    clear();
    unsigned int i;
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    m_dimension=p.m_dimension;
    for( i=0 ; i<p.size() ; ++i )
        m_pointList.push_back(new Point(*(p.m_pointList[i])));
    unlockWriteMutex();
    Object::update();
}

arlCore::PointList::~PointList( void )
{
    clear();
}

std::string arlCore::PointList::getString( void ) const
{
    std::stringstream s;
    s<<"List of "<<(unsigned int)m_pointList.size()<<" points with dimension "<<getDimension()<<"\n";
    unsigned int i;
    for( i=0 ; i<m_pointList.size() ; ++i )
        s<<m_pointList[i]->getString();
    return s.str();
}

unsigned int arlCore::PointList::getDimension( void ) const
{
    return m_dimension;
}

bool arlCore::PointList::setDimension( unsigned int dim )
{
    if(size()!=0) return false;
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    m_dimension = dim;
    unlockWriteMutex();
    Object::update();
    return true;
}

unsigned int arlCore::PointList::addGaussianNoise( double gaussianNoise )
{
    if(gaussianNoise<=0.0 || m_pointList.size()==0) return 0;
    unsigned int i;
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    for( i=0 ; i<m_pointList.size() ; ++i )
            m_pointList[i]->addGaussianNoise(gaussianNoise);
    unlockWriteMutex();
    Object::update();
    return i;
}

unsigned int arlCore::PointList::fill( const std::vector< arlCore::vnl_rigid_matrix >& list )
{
    unsigned int i;
    clear();
    Point pt(3);
    for( i=0 ; i<list.size() ; ++i )
    {
        pt.fill(0.0);
        list[i].trf( pt );
        push_back(pt);
    }
    Object::update();
    return i;
}

const std::vector< arlCore::Point::sptr >& arlCore::PointList::getList( void ) const
{
    return m_pointList;
}

template <typename T>
unsigned int internStatistic( const T &list, unsigned int dimension, arlCore::Point& pt )
{   // http://fr.wikipedia.org/wiki/Covariance
    unsigned int i, j, k;
    arlCore::Point mean(dimension);
    mean.fill(0.0);
    if(list.size()==0) return 0;
    for( i=0 ; i<list.size() ; ++i )
        for( j=0 ; j<dimension ; ++j )
            if(list[i]->size()>j)
                mean.set(j, mean[j] + list[i]->get(j));
    mean.mult(1.0/(double)list.size());
    vnl_matrix<double> &covMatrix=mean.getCovMatrix();
    covMatrix.fill(0.0);
    //TODO some explanation on the formula are mandatory
    for( i=0 ; i<list.size() ; ++i )
        for( j=0 ; j<dimension ; ++j )
            for( k=0 ; k<dimension ; ++k )
                if(list[i]->size()>j && list[i]->size()>k)
                    covMatrix[j][k] += (list[i]->get(j)-mean[j])*(list[i]->get(k)-mean[k]);
    covMatrix /= list.size();
    pt=mean;
    return (unsigned int)list.size();
}

unsigned int arlCore::statistic( const std::vector< const Point*> &list, unsigned int dimension, Point& pt )
{
    return internStatistic(list, dimension, pt);
}

unsigned int arlCore::statistic( const std::vector< Point*> &list, unsigned int dimension, Point& pt )
{
    return internStatistic(list, dimension, pt);
}

unsigned int arlCore::PointList::statistic( Point& pt ) const
{
    assert(getDimension()>0);
    return internStatistic(m_pointList, getDimension(), pt);
}
unsigned int arlCore::PointList::plane( vgl_homg_plane_3d< double >& homgPlane, double &rms, double &min, double &max, double scalar, double outliers ) const
{
    vgl_plane_3d<double> vglPlane;
    unsigned int n = plane( vglPlane, rms, min, max, scalar, outliers );
    homgPlane = vglPlane;
    return n;
}

typedef std::pair<unsigned int, double> PlaneDistance;
bool sortPoints(const PlaneDistance& left, const PlaneDistance& right)
{   // Ordre decroissant des distances
    return left.second > right.second;
}

unsigned int arlCore::PointList::plane( vgl_plane_3d< double >& plane, double &rms, double &min, double &max, double scalar, double outliers ) const
{
    if(outliers>1.0) outliers = 1.0;
    // TODO Use #include <vgl_fit_plane_3d.h>
    rms=-1, min=-1, max=-1;
    if(m_dimension!=3) return 0;
    vgl_point_3d< double > pt[3];
    unsigned int i, j;
//  for( i=0 ; i<3 ; ++i )
//      pt[i].fill(0.0);
    pt[0].set(0.0,0.0,0.0);
    pt[1].set(0.0,0.0,0.0);
    pt[2].set(0.0,0.0,0.0);
    OptimisePlane meanPlane;
    double n[3]={0.0,0.0,0.0};
    for( i=0 ; i<size() ; ++i )
    {
        if(m_pointList[i]!=0)
            if(scalar==0 || scalar==m_pointList[i]->getScalar())
            {
                vgl_point_3d< double > vglPt(m_pointList[i]->x(),m_pointList[i]->y(),m_pointList[i]->z());
                meanPlane.addPoint( vglPt);
                if(i<size()/3)
                {
                    n[0]++;
                    pt[0].set(pt[0].x()+m_pointList[i]->x(), pt[0].y()+m_pointList[i]->y() , pt[0].z()+m_pointList[i]->z() );
                }
                if(i>=size()/3 && i<size()/3*2)
                {
                    n[1]++;
                    pt[1].set(pt[1].x()+m_pointList[i]->x(), pt[1].y()+m_pointList[i]->y() , pt[1].z()+m_pointList[i]->z() );
                }
                if(i>=size()/3*2)
                {
                    n[2]++;
                    pt[2].set(pt[2].x()+m_pointList[i]->x(), pt[2].y()+m_pointList[i]->y() , pt[2].z()+m_pointList[i]->z() );
                }
            }
    }
    if(meanPlane.size()<3) return 0;
    for( i=0 ; i<3 ; ++i )
        if(n[i]!=0) pt[i].set(pt[i].x()/n[i] , pt[i].y()/n[i] , pt[i].z()/n[i] );
//  meanPlane.getPoint(0,pt[0]);
//  meanPlane.getPoint(unsigned int(meanPlane.size()/2),pt[1]);
//  meanPlane.getPoint(meanPlane.size()-1,pt[2]);
    plane = vgl_plane_3d< double >(pt[0],pt[1],pt[2]);
    if(meanPlane.size()==3) rms=0.0, min=0.0, max=0.0;
    else
    {
        std::vector<PlaneDistance> distances;
        vnl_vector<double> planSolution(4);
        planSolution(0)=plane.a();
        planSolution(1)=plane.b();
        planSolution(2)=plane.c();
        planSolution(3)=plane.d();
        for( i=0 ; i<2 ; ++i )
        {   //Erase outliers in the second iteration
            if(i==1)
            {
                unsigned int n = distances.size()*outliers;
                if(distances.size()-n < 3) n = distances.size()-3;
                for( j=0 ; j<n ; ++j )
                    meanPlane.delPoint(distances[j].first);
            }
            vnl_powell computeMeanPlane(&meanPlane);
            computeMeanPlane.minimize(planSolution);
            plane.set(planSolution(0),planSolution(1),planSolution(2),planSolution(3));
            rms=sqrt(computeMeanPlane.get_end_error()/meanPlane.size());
            const std::vector< vgl_point_3d< double >* > &points = meanPlane.getPoints();
            distances.resize(points.size());
            for( j=0 ; j<points.size() ; j++ )
                if(points[j]!=0)
                {
                    distances[j].first = j;
                    distances[j].second = vgl_distance(plane, *(points[j]));
                    if(min<0 || distances[j].second<min) min=distances[j].second;
                    if(max<0 || distances[j].second>max) max=distances[j].second;
                }
            std::sort(distances.begin(), distances.end(), sortPoints);
        }
    }
    return meanPlane.size();
}

/*double arlCore::PointList::pivot( arlCore::Point &center, double &error ) const
{
    unsigned int i;
    const unsigned int Dim = getDimension();
    center.init(Dim);
    OptimisePivot pivot(*this);
    vnl_vector<double> solution(Dim);
    solution.fill(0.0);
    vnl_powell computePivot(&pivot);
    computePivot.minimize(solution);
    for( i=0 ; i<Dim ; ++i )
        center.set(i, solution[i]);
    error = sqrt(computePivot.get_end_error()/size());
    return pivot.getScalar();
}*/

///////////////////////////////////////////////////////////////////////////////////////
//
// We suppose that the points in the PointList are on a  sphere surface
// This function provide an estimation of the sphere center and its radius from
// the PointList. Three methods are now available:
// ARLCORE_SCE_ANALYTIC, ARLCORE_SCE_GEOMETRIC, ARLCORE_SCE_OPTIMIZATION
// How does ARLCORE_SCE_ANALYTIC work ?
// the equation of a sphere is : (x-a)^2 + (y-b)^2 + ((z-c)^2 = R^2 where (a,b,c)
// is the center of the sphere and R its radius.
// By developing the parenthesis we can rewrite it:
// x^2 +y^2 +z^2 + dx + ey + fz +g = R^2
// where d = -2a  e = -2b  f = -2c  and a^2 + b^2 + c^2 -R^2 = g
// Here we want to minimize : sum_i || x_i^2 +y_i^2 +z_i^2 + dx_i + ey_i + fz_i +g_i - R^2 ||^2
// A direct derivation wrt the vector [d e f g]^T gives the following result:
//
// [d e f g] = -sum( (x_i^2 + y_i^2 + z_i^2)[x_i y_i z_i 1]) * inverse(sum(transpose([x_i y_i z_i 1]) . [x_i y_i z_i 1]) )
//
// ARLCORE_SCE_GEOMETRIC: theoretically, the sphere center should lie on each
// plane P_i that is orthogonal to each point pair and intersecting their gravity
// center. This method finds the point C that minimize the squared distance
// between each mediator plane P_i and C.
// argmin sum_i dist(C, P_i) = argmin sum_i ||ai*xc + bi*yc + ci*zc + d_i ||^2/ (a_i^2 + b_i^2 +c_i^2)
//
// ARLCORE_SCE_OPTIMIZATION :
// here we use an iterative optimization to find C. The criterion that is optimized is:
// sum_i dist(C, M_i)^2 where M_i are the points that belong to the sphere
//
// log[0] = sqrt( mean  (sum_i || estimated_radius_pt_i - estimated_radius ||^2) )
// It corresponds to the average of the distance of the point cloud to the estimated sphere
// log[1] = sqrt( mean  (sum_i || distance_to_sphere_pt_i - average_distance_to_sphere ||^2) )
// It corresponds to the standard deviation of the distance to the sphere
// log[2] = maximum distance of the point cloud to the estimated sphere
////////////////////////////////////////////////////////////////////////////////////////
bool arlCore::PointList::sphereCenterEstimation( arlCore::Point & centerEstimation, double &radius, arlCore::ARLCORE_SCE methode, std::vector<double> &optimiser_parameter, std::vector<double> &log) const
{
    const bool Verbose = false;
    const unsigned int Size = size();
    if (size() < 4)
    {
        std::cerr<<"Number of point is < 5 in arlCore::PointList::sphereCenterEstimation"<<std::endl;
        assert(size() < 4);
    }
    if(methode==arlCore::ARLCORE_SCE_ANALYTIC)
    {
        vnl_matrix_fixed <double,4,4> matrix_sum(0.0);
        vnl_matrix_fixed <double,1,4> xyz1(0.0), vector_sum(0.0), defg(0.0);
        vnl_vector<double> radius_error( Size ), std_distance( Size );
        unsigned int i;
        for( i=0 ; i<Size ; ++i )
        {
            xyz1.put( 0, 0, get(i)->x() );
            xyz1.put( 0, 1, (*this)[i]->y() );
            xyz1.put( 0, 2, (*this)[i]->z() );
            xyz1.put( 0, 3, 1.0 );
            matrix_sum = matrix_sum.as_matrix() + xyz1.as_matrix().transpose() * xyz1;
            vector_sum = vector_sum.as_matrix() + (xyz1(0,0)*xyz1(0,0) + xyz1(0,1)*xyz1(0,1) + xyz1(0,2)*xyz1(0,2))*xyz1.as_matrix();
        }
        defg = - vector_sum.as_matrix() * vnl_matrix_inverse<double> (matrix_sum.as_matrix());
        double a = -1*defg(0,0)/2;
        double b = -1*defg(0,1)/2;
        double c = -1*defg(0,2)/2;
        radius = sqrt( a*a + b*b + c*c - defg(0,3) );
        centerEstimation.set( 0, a );
        centerEstimation.set( 1, b );
        centerEstimation.set( 2, c );
        for( i=0 ; i<Size ; ++i )
        {
            radius_error[i] = fabs( get(i)->distance(centerEstimation) - radius );
            if(Verbose) std::cerr<<"radius_error[i] = " <<radius_error[i]<<std::endl;
        }
        const double Mean = radius_error.mean();
        for( i=0 ; i<Size ; ++i )
            std_distance.put(i, radius_error[i] -  Mean );
        log.clear();
        log.push_back( radius_error.rms() );
        centerEstimation.setError(log.back());
        log.push_back( std_distance.rms() );
        log.push_back( radius_error.max_value() );
        return true;
    }
    if(methode==arlCore::ARLCORE_SCE_GEOMETRIC)
    {
        vnl_matrix_fixed <double,3,3> matrix_sum(0.0);
        std::vector< vnl_matrix_fixed<double,1,3> > normal_vec;
        vnl_vector<double>radius_error( Size ), std_distance( Size );
        std::vector<double> dk;
        vnl_matrix_fixed<double,1,3> vector_sum(0.0), center, tmp;
        unsigned int i, j, k=0;
        for( i=0 ; i<Size ; ++i )
            for( j=0 ; j<Size ; ++j )
                    if(j!=i)
                    {
                        tmp.put(0, 0, get(i)->x() - get(j)->x());
                        tmp.put(0, 1, get(i)->y() - get(j)->y());
                        tmp.put(0, 2, get(i)->z() - get(j)->z());
                        normal_vec.push_back(tmp);
                        dk.push_back( -1*(get(i)->x()*get(i)->x() + get(i)->y()*get(i)->y() + get(i)->z()*get(i)->z()
                        - get(j)->x()*get(j)->x() - get(j)->y()*get(j)->y() - get(j)->z()*get(j)->z() ) /2 );
                    }
        if(Verbose) std::cerr<< "normal_vec size = " << (unsigned int)normal_vec.size()<<std::endl;
        for( i=0 ; i<normal_vec.size() ; ++i )
        {
            matrix_sum =  matrix_sum + (normal_vec[i].as_matrix().transpose() * normal_vec[i])/( normal_vec[i](0,0)* normal_vec[i](0,0)
            + normal_vec[i](0,1)* normal_vec[i](0,1) + normal_vec[i](0,2)* normal_vec[i](0,2) );

            vector_sum = vector_sum + dk[i]/( normal_vec[i](0,0)* normal_vec[i](0,0) + normal_vec[i](0,1)* normal_vec[i](0,1)
            + normal_vec[i](0,2)* normal_vec[i](0,2) ) * normal_vec[i];
        }
        center =  vector_sum.as_matrix() * vnl_matrix_inverse<double> (matrix_sum.as_matrix()) *(-1);
        radius = 0;
        centerEstimation.set(0, center(0,0) );
        centerEstimation.set(1, center(0,1) );
        centerEstimation.set(2, center(0,2) );
        for( i = 0; i<Size; ++i)
            radius += centerEstimation.distance(*(get(i)));
        radius /= (double)Size;
        for( i=0 ; i<Size ; ++i )
        {
            radius_error[i] = fabs(centerEstimation.distance( *(get(i)) ) - radius);
            //std::cerr<< "radius_error[i] = " <<radius_error[i]<<std::endl;
        }
        for( i=0 ; i<Size ; ++i )
        {
            std_distance[i] = radius_error.get(i)-radius_error.mean();
        }
        log.clear();
        log.push_back( radius_error.rms() );
        centerEstimation.setError(log.back());
        log.push_back( std_distance.rms() );
        log.push_back( radius_error.max_value() );
        return true;
    }
    if(methode==arlCore::ARLCORE_SCE_OPTIMIZATION)
    {
        unsigned int i;
        const unsigned int Dim = getDimension();
        centerEstimation.init(Dim);
        OptimisePivot pivot(*this);
        vnl_vector<double> solution(Dim);
        solution.fill(0.0); // Initialisation
        std::vector<double> tmpLog;
        sphereCenterEstimation( centerEstimation, radius, ARLCORE_SCE_ANALYTIC, optimiser_parameter, tmpLog );
        for( i=0 ; i<Dim ; ++i )
            solution[i] = centerEstimation[i];
        vnl_powell computePivot(&pivot);
        computePivot.minimize(solution);
        for( i=0 ; i<Dim ; ++i )
            centerEstimation.set(i, solution[i]);
        double error = sqrt(computePivot.get_end_error()/Size);
        radius = pivot.getRadius();
        log.clear();
        log.push_back( error );
        centerEstimation.setError(log.back());
        return true;
    }
    if(methode==arlCore::ARLCORE_SCE_4POINTS)
    {
        //computation of two lines which intersection in an estimation of
        //the sphere center
        vnl_vector<double>radius_error( Size ), std_distance( Size );
        arlCore::PointList trian1, trian2, fourPoints;
        arlCore::Point line_center1(3), line_center2(3), line1_another_point, line2_another_point;
        vgl_plane_3d< double > plane1, plane2;
        for(int i=0; i<3;++i)
        {
            trian1.push_back( *(this->get(i)) );
            trian2.push_back( *(this->get(i+1)) );
        }
        trian1.triangleCircumcircleOrthoLine(line_center1, plane1);
        trian2.triangleCircumcircleOrthoLine(line_center2, plane2);
        line1_another_point.set(0, line_center1[0] + plane1.a());
        line1_another_point.set(1, line_center1[1] + plane1.b());
        line1_another_point.set(2, line_center1[2] + plane1.c());
        line2_another_point.set(0, line_center2[0] + plane2.a());
        line2_another_point.set(1, line_center2[1] + plane2.b());
        line2_another_point.set(2, line_center2[2] + plane2.c());
        fourPoints.push_back(line_center1);
        fourPoints.push_back(line1_another_point);
        fourPoints.push_back(line_center2);
        fourPoints.push_back(line2_another_point);
        double line_error=0;
        fourPoints.twoLinesClosestPoint( centerEstimation, line_error);
//      std::cerr<< "line_error = "<< line_error<<std::endl;
        radius = 0;
        for( int i = 0; i<Size; ++i)
            radius += centerEstimation.distance(*(get(i)));
        radius /= (double)Size;
        for( int i=0 ; i<Size ; ++i )
        {
            radius_error[i] = fabs(centerEstimation.distance( *(get(i)) ) - radius);
            //std::cerr<< "radius_error[i] = " <<radius_error[i]<<std::endl;
        }
        for( int i=0 ; i<Size ; ++i )
        {
            std_distance[i] = radius_error.get(i)-radius_error.mean();
        }
        log.clear();
        log.push_back( radius_error.rms() );
        centerEstimation.setError(log.back());
        log.push_back( std_distance.rms() );
        log.push_back( radius_error.max_value() );
        return true;
    }
    return false;
}

bool arlCore::PointList::triangleCircumcircleOrthoLine( arlCore::Point & centerEstimation, vgl_plane_3d< double > &trian_plane) const
{
    if (this->size() < 3)
    {
        std::cerr<<"in PointList::triangleCircumcircle Point list size in below 3"<<std::endl;
        assert(this->size() < 3);
    }
    else
    {
        vgl_vector_3d< double > AB(this->get(0)->x() - this->get(1)->x(), this->get(0)->y() - this->get(1)->y(), this->get(0)->z() - this->get(1)->z());
        vgl_vector_3d< double > AC(this->get(0)->x() - this->get(2)->x(), this->get(0)->y() - this->get(2)->y(), this->get(0)->z() - this->get(2)->z());

        vgl_point_3d< double > A(this->get(0)->x(), this->get(0)->y(), this->get(0)->z());
        vgl_point_3d< double > B(this->get(1)->x(), this->get(1)->y(), this->get(1)->z());
        vgl_point_3d< double > C(this->get(2)->x(), this->get(2)->y(), this->get(2)->z());
        vgl_point_3d< double > middle_AB( (A.x() + B.x())/2, (A.y() + B.y())/2, (A.z() + B.z())/2);
        vgl_point_3d< double > middle_AC( (C.x() + A.x())/2, (C.y() + A.y())/2, (C.z() + A.z())/2);
        vgl_plane_3d< double > first_plane(AB, middle_AB);
        vgl_plane_3d< double > second_plane(AC, middle_AC);
        vgl_plane_3d< double > triangle_plane(A, B, C);
        vgl_point_3d< double > circumcircle_center(first_plane, second_plane, triangle_plane);

        trian_plane = triangle_plane;
        arlCore::Point var(circumcircle_center.x(), circumcircle_center.y(), circumcircle_center.z());
        centerEstimation=var;
    }
    return true;
}

bool arlCore::PointList::twoLinesClosestPoint( arlCore::Point & closestPoint, double &log) const
{
    //p1 = pointlist[0]
    //p2 = pointlist[1]
    //p3 = pointlist[2]
    //p4 = pointlist[3]
    arlCore::Point pa, pb;
    arlCore::Point p1(*(this->get(0))), p2((*this->get(1))), p3((*this->get(2))), p4((*this->get(3)));
    arlCore::Point p13,p43,p21;
    double d1343,d4321,d1321,d4343,d2121;
    double numer,denom, eps=1e-12;
    p13.set(0, p1.x() - p3.x() );    p13.set(1, p1.y() - p3.y() );    p13.set(2, p1.z() - p3.z() );
    p43.set(0, p4.x() - p3.x() );    p43.set(1, p4.y() - p3.y() );    p43.set(2, p4.z() - p3.z() );
    if (fabs(p43.x())  < eps && fabs(p43.y())  < eps && fabs(p43.z())  < eps)
        return(false);

    p21.set(0, p2.x() - p1.x() );    p21.set(1, p2.y() - p1.y() );    p21.set(2, p2.z() - p1.z() );
    if (fabs(p21.x())  < eps && fabs(p21.y())  < eps && fabs(p21.z())  < eps)
       return(false);

    d1343 = p13.x() * p43.x() + p13.y() * p43.y() + p13.z() * p43.z();
    d4321 = p43.x() * p21.x() + p43.y() * p21.y() + p43.z() * p21.z();
    d1321 = p13.x() * p21.x() + p13.y() * p21.y() + p13.z() * p21.z();
    d4343 = p43.x() * p43.x() + p43.y() * p43.y() + p43.z() * p43.z();
    d2121 = p21.x() * p21.x() + p21.y() * p21.y() + p21.z() * p21.z();

    denom = d2121 * d4343 - d4321 * d4321;
    if (fabs(denom) < eps)
        return(false);
    numer = d1343 * d4321 - d1321 * d4343;

    double mua = numer / denom;
    double mub = (d1343 + d4321 * (mua)) / d4343;
    pa.set(0, p1.x() + mua * p21.x() );
    pa.set(1, p1.y() + mua * p21.y() );
    pa.set(2, p1.z() + mua * p21.z() );
    pb.set(0, p3.x() + mub * p43.x() );
    pb.set(1, p3.y() + mub * p43.y() );
    pb.set(2, p3.z() + mub * p43.z() );
//  std::cerr<<"I = " << pa.getCoordinates()<<std::endl;
//  std::cerr<<"J = " << pb.getCoordinates()<<std::endl;
    closestPoint.x(0.5*pa.x() + 0.5*pb.x());
    closestPoint.y(0.5*pa.y() + 0.5*pb.y());
    closestPoint.z(0.5*pa.z() + 0.5*pb.z());
    log = pa.distance(pb);
    return true;
}

unsigned int arlCore::PointList::shapeRandom( unsigned int nbPoints, ARLCORE_SHAPE type, const arlCore::Point &centre, const double size, const double angle)
{
    m_pointList.clear();
    if(type<=ARLCORE_SHAPE_UNKNOWN ||type>=ARLCORE_SHAPE_NBSHAPES) return 0;
    unsigned int i, j;
    if(type==ARLCORE_SHAPE_CHESSBOARD)
    {   // nbPoints = Number of points by side of the square
        if(centre.size()<2) return 0; // Point dimension at least = 2
        const double Step = size/(double)(nbPoints-1);
        arlCore::Point currentPoint(centre);
        // firstPoint is the Upper left coin
        const double Xorg = centre.x()-size/2.0;
        const double Yorg = centre.y()-size/2.0;
        for( i=0 ; i<nbPoints ; ++i )
            for( j=0 ; j<nbPoints ; ++j )
            {
                currentPoint.x(Xorg + (double)i*Step);
                currentPoint.y(Yorg + (double)j*Step);
                push_back(currentPoint);
            }
        return nbPoints*nbPoints;
    }
    switch(type)
    {
    case ARLCORE_SHAPE_CUBE: setName("Random points in a cube");break;
    case ARLCORE_SHAPE_SPHERE: setName("Random points in a sphere");break;
    case ARLCORE_SHAPE_SPHERE_CAP: setName("Random points on a sphere cap");break;
    case ARLCORE_SHAPE_DISC: setName("Random points on a disc");break;
    case ARLCORE_SHAPE_PLAINSQUARE: setName("Random points on a plain square");break;
    case ARLCORE_SHAPE_CIRCLE: setName("Random points on a circle");break;
    case ARLCORE_SHAPE_EDGESQUARE: setName("Random points on a edge square");break;
    case ARLCORE_SHAPE_SOLIDANGLE: setName("Random points in a solid angle");break;
    case ARLCORE_SHAPE_SOLIDANGLE_SURFACE: setName("Random points on a solid angle surface");break;
    }
    m_dimension=3;
    arlCore::Point tmp(centre);
    for( i=0 ; i<nbPoints ; ++i )
    {
        tmp.shapeRandom(type, size, angle);
        push_back(tmp);
        tmp = centre;
    }
    Object::update();
    return nbPoints;
}

unsigned int arlCore::PointList::randomList( const arlCore::PointList &A, unsigned int n )
{
    unsigned int i;
    std::vector<unsigned int> v;
    clear();
    arlRandom::randomIndex( A.size(), v, n );
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    for( i=0 ; i<n ; ++i )
        m_pointList.push_back(new Point(*(A.m_pointList[i])));
    unlockWriteMutex();
    Object::update();
    return n;
}

bool arlCore::PointList::plot( const std::string &options ) const
{
    const bool JustVisible = false;
    const bool Overwrite = true;
    save("000000.dat", ARLCORE_POINT_SAVE_GNUPLOT, JustVisible, Overwrite);
    std::fstream file;
    file.open (TMP_FILE, std::fstream::out);
    if(!file.is_open()) return false;
    file<<"set title '"<<getName()<<"'\n";
    file<<options<<"\n";
    if(getDimension()==3) file<<"s";
    file<<"plot ";
    file<<"\"000000.dat\"\n";
    file<<"pause -1 \"Hit return to continue\"\n";
    file.close();
    system(GNUPLOT_EXE);
    //exec(GNUPLOT_EXE, TMP_FILE );
    return true;
}

bool arlCore::PointList::SLplot( void ) const
{
    return false;
    double rms, min, max;
    int i;
    vgl_plane_3d< double > p;
    unsigned int n;
    const bool JustVisible = false;
    const bool Overwrite = true;
    save("c:/000000.dat", ARLCORE_POINT_SAVE_GNUPLOT, JustVisible, Overwrite);
    for( i=-30 ; i<30 ; ++i)
    {
        if(i!=0)
        {
            n=this->plane( p, rms, min, max, i);
            if(n>3)
            {
                std::fstream file;
                file.open ("c:/000000.dem", std::fstream::out);
                file<<"set title 'Nuage de points & plans mdians\n";
                file<<"splot ";
                if(!file.is_open()) return false;
                file<<"("<<p.a()<<"*x+("<<p.b()<<")*y+("<<p.d()<<"))/(-("<<p.c()<<")),";
                file<<"\"c:/000000.dat\"\n";
                file<<"pause -1 \"Hit return to continue\"\n";
                file.close();
                file.flush();
                system ("d:/gnuplot/bin/wgnuplot c:/000000.dem" );
            }
        }
    }
    return true;
}

template <typename T>
bool internSave( const T &pl, const std::string &fileName, const std::string &name, arlCore::ARLCORE_POINT_SAVE_TYPE type,  bool justVisible, bool overwrite )
{
    if(pl.size()==0) return false;
    const unsigned int Dim = pl[0]->size();
    unsigned int i, n;
#ifdef _DEBUG
    // List shoud contains points with the same dimension
    for( i=0 ; i<pl.size() ; ++i )
    {
        if(pl[i]!=0)
            if(pl[i]->isOK())
            {
                assert(pl[i]->size()==Dim);
            }
    }
#endif // _DEBUG
    std::fstream file;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    file.open(fileName.c_str(), std::fstream::out);
    if(!file.is_open()) return false;
    if(type==arlCore::ARLCORE_POINT_SAVE_FULL || type==arlCore::ARLCORE_POINT_SAVE_LIGHT)
    {
        file<<"FileType PointList\n";
        file<<"Version 1.0\n";
        file<<"Dimension "<<Dim<<"\n";
    }
    if(type==arlCore::ARLCORE_POINT_SAVE_GNUPLOT)
        file<<"#ARL points list for GNUPLOT\n";
    if(type==arlCore::ARLCORE_POINT_SAVE_VTK)
    {
        file<<"# vtk DataFile Version 3.1\n";
        file<<name<<"\n";
        file<<"ASCII\n";
        file<<"DATASET UNSTRUCTURED_GRID\n\n";
        file<<"POINTS "<<(unsigned int)pl.size()<<" double\n";
    }
    for( i=0, n=0 ; i<pl.size() ; ++i )
        if(pl[i]!=0)
            if(pl[i]->isOK())
                if(!justVisible || pl[i]->isVisible())
                {
                    pl[i]->save(file, n, type);
                    ++n;
                }
    if(type==arlCore::ARLCORE_POINT_SAVE_VTK)
    {
        file<<"\nCELLS 1 "<<(unsigned int)pl.size()+1<<"\n";
        file<<(unsigned int)pl.size()<<"\t";
        for( i=0 ; i<pl.size() ; ++i )
            file<<i<<" ";
        file<<"\n\nCELL_TYPES 1\n2\n";
    }
    file.close();
    return true;
}

bool arlCore::PointList::save( const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type,  bool justVisible, bool overwrite ) const
{
    bool b = internSave( m_pointList, fileName, getName(), type, justVisible, overwrite );
    if( b && type==arlCore::ARLCORE_POINT_SAVE_VTK )
    {
        unsigned int i;
        std::fstream file;
        file.open(fileName.c_str(), std::fstream::out | std::fstream::app);
        if(!file.is_open()) return b;
        file<<"\nPOINT_DATA "<<size();
        file<<"\n\nSCALARS error double 1\nLOOKUP_TABLE default\n";
        for( i=0 ; i<size() ; ++i )
            if(m_pointList[i]!=0)
                if(m_pointList[i]->isOK())
                    file<<m_pointList[i]->getError()<<" ";
        file<<"\nSCALARS scalar double 1\nLOOKUP_TABLE default\n";
        for( i=0 ; i<size() ; ++i )
            if(m_pointList[i]!=0)
                if(m_pointList[i]->isOK())
                    file<<m_pointList[i]->getScalar()<<" ";
        file<<"\n\nSCALARS visible char 1\nLOOKUP_TABLE default\n";
        for( i=0 ; i<size() ; ++i )
            if(m_pointList[i]!=0)
                if(m_pointList[i]->isOK())
                    if(m_pointList[i]->isVisible())
                        file<<"1 ";
                    else file<<"0 ";
        if(getDimension()==3)
        {
            file<<"\n\nTENSORS covariance double\n";
            for( i=0 ; i<size() ; ++i )
                if(m_pointList[i]!=0)
                    if(m_pointList[i]->isOK())
                        file<<m_pointList[i]->getCovMatrix()<<"\n";
        }
        file.close();
    }
    return b;
}
unsigned int addPoints( arlCore::PointList &ptsList, double step )
{
    return 0;
    // Reechantillonne le triangle form� par les 3 derniers points
    unsigned int n=0;
    const unsigned int Size = ptsList.size();
    if(Size<3) return n;
    const arlCore::Point &A = *ptsList[Size-1];
    const arlCore::Point &B = *ptsList[Size-2];
    const arlCore::Point &C = *ptsList[Size-3];
    std::cout<<A.distance(B)<<"-";
    std::cout<<C.distance(B)<<"-";
    std::cout<<A.distance(C)<<"\n";
    return n;
}

bool arlCore::PointList::load( const std::string &fileName, double step )
{
    if(fileName=="") return false;
    std::ifstream file;
    file.open (fileName.c_str(), std::fstream::in);
    if(!file.is_open())
    {
        std::cerr<<"PointList Loading error : "<<fileName<<"\n";
        return false;
    }
    std::string extractFolder, extractFile, ext;
    arlString::splitFilename( fileName, extractFolder, extractFile, ext );
    const bool Trian = (ext=="trian");
    const bool Raw = (ext=="raw");
    unsigned int i=0, n=0;
    int no;
    std::string token,text;
    double version;
    Object::update(); // TODO Optimize its position
    m_pointList.clear();
    Point p;
    if(Trian)
    {
        m_dimension = 3;
        unsigned int nbPts;
        double x,y,z;
        file>>nbPts;
        while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
        for( ; i<nbPts ; ++i )
        {
            file>>x>>y>>z;
            p.x(x);
            p.y(y);
            p.z(z);
            m_pointList.push_back(new Point(p));
            if(step>0)
                if((i+1)%3==0) addPoints(*this, step);
        }
        unlockWriteMutex();
        file.close();
        return true;
    }
    if(Raw)
    {
        m_dimension = 3;
        double x,y,z;
        while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
        while(!file.eof())
        {
            file>>x>>y>>z;
            p.x(x);
            p.y(y);
            p.z(z);
            m_pointList.push_back(new Point(p));
            if(step>0)
                if((i+1)%3==0) addPoints(*this, step);
            ++i;
        }
        unlockWriteMutex();
        file.close();
        return true;
    }
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    do
    {   // Reading header
        file>>token;
        if(token=="FileType")
        {
            file>>text;
            if(text!="PointList" && text!="PointsList")
            {   // FIXME PointsList deprecated : to delete
                unlockWriteMutex();
                return false;
            }
            n=(n|1);
        }
        if(token=="Version") {file>>version; n=(n|2);}
        if(token=="Dimension") {file>>m_dimension; n=(n|4);}
        if(file.eof())
        {
            std::cerr<<"PointList Loading Header error : "<<fileName<<"\n";
            unlockWriteMutex();
            return false;
        }
    } while(n!=7);
    unlockWriteMutex();
    bool b;
    do
    {
        b=p.load(file,no,m_dimension);
        if(b)
        {
            if(no<0) push_back(p);
            else
            {
                while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
                assert(no>=0);
                for( i=size() ; i<=(unsigned int)no ; ++i )
                    m_pointList.push_back(0);
                if(m_pointList[no]==0)
                    m_pointList[no]=new Point(p);
                else m_pointList[no]->pond(p);
                unlockWriteMutex();
            }
        }
    }while(b);
    file.close();
    return true;
}

arlCore::Point* arlCore::PointList::operator[]( unsigned int i )
{   // FIXME Problem de validite de l'objet retourne
    Object::update();
    return get(i);
}

const arlCore::Point* arlCore::PointList::operator[]( unsigned int i ) const
{   // FIXME Problem de validite de l'objet retourne
    return get(i);
}

arlCore::Point* arlCore::PointList::get( unsigned int i )
{   // FIXME Problem de validite de l'objet retourne
    if(i<0 || i>=size()) return 0;
    else return m_pointList[i];
}

const arlCore::Point* arlCore::PointList::get( unsigned int i ) const
{
    if(i<0 || i>=size()) return 0;
    const arlCore::Point* P = m_pointList[i];
    return P;
}

arlCore::Point* arlCore::PointList::back( void )
{   // FIXME Problem de validite de l'objet retourne
    if(size()<1) return 0;
    return get(size()-1);
}

const arlCore::Point* arlCore::PointList::back( void ) const
{   // FIXME Problem de validite de l'objet retourne
    if(size()<1) return 0;
    else return get(size()-1);
}

unsigned int arlCore::PointList::push_back( const PointList &p )
{
    unsigned int i,n=0;
    for( i=0 ; i<p.size() ; ++i )
        if(push_back(*(p[i]))) ++n;
    Object::update();
    return n;
}

bool arlCore::PointList::push_back( const Point &p )
{
    if(m_dimension!=p.size())
        if(size()!=0) return false;
    setDimension(p.size());
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    m_pointList.push_back(new Point(p));
    unlockWriteMutex();
    Object::update();
    return true;
}

bool arlCore::PointList::push_back( double x, double y )
{
    if(m_dimension!=2)
        if(size()!=0) return false;
    setDimension(2);
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    m_pointList.push_back(new Point(x,y));
    unlockWriteMutex();
    Object::update();
    return true;
}

bool arlCore::PointList::push_back( double x, double y, double z )
{
    if(m_dimension!=3)
        if(size()!=0) return false;
    setDimension(3);
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    m_pointList.push_back(new Point(x,y,z));
    unlockWriteMutex();
    Object::update();
    return true;
}

void arlCore::PointList::pop_back( void )
{
    if(m_pointList.size()<1) return;
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    if(m_pointList.back()!=0) delete m_pointList.back();
    m_pointList.pop_back();
    unlockWriteMutex();
    Object::update();
}

void arlCore::PointList::clear( void )
{
    unsigned int i;
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    for( i=0 ; i<m_pointList.size() ; ++i )
    {
        assert(m_pointList[i]!=0);
        delete m_pointList[i];
    }
    m_pointList.clear();
    unlockWriteMutex();
    Object::update();
}

unsigned int arlCore::PointList::size( void ) const
{
    return (unsigned int)m_pointList.size();
}

unsigned int arlCore::PointList::visibleSize( void ) const
{
    unsigned int i, n=0;
    for( i=0 ; i<m_pointList.size() ; ++i )
        if(m_pointList[i])
            if(m_pointList[i]->isVisible()) ++n;
    return n;
}

bool arlCore::PointList::properties( arlCore::Point& gravity, arlCore::Point& boundingBox1, arlCore::Point& boundingBox2,
                        double &minScalar, double &maxScalar, double &std ) const

{
    if(size()<1) return false;
    unsigned int i, j;
    gravity.set(*(m_pointList[0]));
    boundingBox1.init( getDimension() );
    boundingBox2.init( getDimension() );
    boundingBox1.set(*(m_pointList[0]));
    boundingBox2.set(*(m_pointList[0]));
    minScalar=m_pointList[0]->getScalar();
    maxScalar=m_pointList[0]->getScalar();
    for( i=1 ; i<size() ; ++i )
    {
        if(m_pointList[0]->getScalar()>maxScalar) maxScalar=m_pointList[0]->getScalar();
        if(m_pointList[0]->getScalar()<minScalar) minScalar=m_pointList[0]->getScalar();
        gravity.pond(*(m_pointList[i]));
        for( j=0 ; j<m_dimension ; ++j )
        {
            if(m_pointList[i]->get(j)<boundingBox1[j]) boundingBox1.set(j,m_pointList[i]->get(j));
            if(m_pointList[i]->get(j)>boundingBox2[j]) boundingBox2.set(j,m_pointList[i]->get(j));
        }
    }
    vnl_vector<double> sub(size());
    for( i=0 ; i<size() ; ++i )
        sub(i) = m_pointList[i]->distance(gravity);
    std = sub.rms();
    return true;
}

#ifdef ANN
unsigned int arlCore::PointList::generateANN( ANNpointArray& annArray ) const
{
    unsigned int i,j,n=0;
    annDeallocPts( annArray );
    annArray=annAllocPts(size(), getDimension());
    for( i=0 ; i<size() ; ++i )
        if(m_pointList[i]!=0)
        {
            ++n;
            for( j=0 ; j<getDimension() ; ++j )
                annArray[i][j]=m_pointList[i]->get(j);
        }
    return n;
}
#endif // ANN

template <typename T>
unsigned int internFindNearPoint( const T &from, const arlCore::Point &pt, std::vector< unsigned int > &pos, double &distMin, double scalar)
{
    pos.clear();
    if(from.size()==0) return 0;
    unsigned int i=0;
    distMin = arlCore::distance(*(from[i]),pt);
    if(distMin<=scalar) pos.push_back(i);
    for( i=1 ; i<from.size() ; ++i )
    {
        double d = arlCore::distance(*(from[i]),pt);
        if(d<=scalar) pos.push_back(i);
        if(d<distMin) distMin=d;
    }
    return (unsigned int)pos.size();
}

unsigned int arlCore::findNearPoint( const std::vector<arlCore::Point::csptr> &from, const Point &pt, std::vector< unsigned int > &pos, double &distMin, double scalar)
{
    return internFindNearPoint(from, pt, pos, distMin, scalar);
}
/*
unsigned int arlCore::findNearPoint( const std::vector<arlCore::Point*> &from, const Point &pt, std::vector< unsigned int > &pos, double &distMin, double scalar)
{
    return internFindNearPoint(from, pt, pos, distMin, scalar);
}*/

unsigned int arlCore::PointList::findNearPoint(const Point &pt, std::vector< unsigned int > &pos, double &distMin, double scalar) const
{
    return internFindNearPoint(m_pointList, pt, pos, distMin, scalar);
}

bool arlCore::PointList::collapse( double g )
{
    Point gravity, boundingBox1, boundingBox2;
    double minScalar, maxScalar, std;
    unsigned int i,j;
    properties( gravity, boundingBox1, boundingBox2, minScalar, maxScalar, std );
    vnl_vector<double> vector(gravity.size());
    while(!lockWriteMutex()) std::cerr<<"PointList write locked\n";
    for( i=0 ; i<size() ; ++i )
    {
        vector = gravity.getCoordinates()-m_pointList[i]->getCoordinates();
        vector.normalize();
        for( j=0 ; j<m_pointList[i]->size() ; ++j )
            m_pointList[i]->set(j,g*vector[j]+m_pointList[i]->get(j));

    }
    unlockWriteMutex();
    Object::update();
    return true;
}

unsigned int arlCore::PointList::tagCalibration( const std::vector<arlCore::PointList> &tagList, double delta, unsigned int nbMaxIterations, double &xLength, double &yLength )
{   // TODO Code cleaning & code optimization
    xLength = 0.0, yLength = 0.0;
    assert(tagList.size()>0);
    if(tagList.size()<1) return 0;
    *this = tagList[0];
    if(tagList.size()==1) return 1;
    unsigned int i, j, nbIterations=0;
    double criterion=0;
    arlCore::vnl_rigid_matrix T;
    arlCore::PointList modelRegistration;
    std::vector< arlCore::PointList > regPL;
    vnl_vector<double> mean1, mean2;
    mean1.set_size(this->size());
    mean2.set_size(tagList.size());
    mean2.fill(0.0);
    mean1.fill(0.0);
    do{
        ++nbIterations;
        criterion=0;
        for( i=0 ; i<tagList.size() ; ++i )
        {
            modelRegistration.clear();
            T.register3D3D(tagList[i],*this, false);
            T.trf(tagList[i], modelRegistration);
            for( j=0 ; j<modelRegistration.size() ; ++j )
                mean1[j] = modelRegistration[j]->distance(arlCore::Point(get(j)->x(),get(j)->y(),get(j)->z()));
            const double M = mean1.mean();
            criterion += abs((M - mean2[i]));
            mean2[i] = M;
            regPL.push_back(modelRegistration);
        }
        this->clear();
        for( i=0 ; i<regPL[0].size() ; ++i )
        {
            arlCore::PointList temp;
            arlCore::Point moyenne;
            for( j=0 ; j<regPL.size() ; ++j )
                temp.push_back(arlCore::Point(regPL[j][i]->x(),regPL[j][i]->y(),regPL[j][i]->z()));
            temp.statistic(moyenne);
            this->push_back(moyenne);
        }
    }while(criterion>delta && nbIterations<nbMaxIterations);
    if(this->size()>2)
    {   // Frame choice
        const unsigned int OrgPoint=3, XaxisPoint=2, YaxisPoint=0; // TODO Choisir d'autres reperes
        arlCore::PointList modelAxis, unitAxis;
        modelAxis.push_back(get(OrgPoint)->x(),get(OrgPoint)->y(),get(OrgPoint)->z());
        modelAxis.push_back(get(XaxisPoint)->x(),get(XaxisPoint)->y(),get(XaxisPoint)->z());
        xLength = get(OrgPoint)->distance(*get(XaxisPoint));
        modelAxis.push_back(get(YaxisPoint)->x(),get(YaxisPoint)->y(),get(YaxisPoint)->z());
        yLength = get(OrgPoint)->distance(*get(YaxisPoint));
        vnl_vector_fixed<double,3> pX, pY;
        pX[0] = get(XaxisPoint)->x() - get(OrgPoint)->x();
        pX[1] = get(XaxisPoint)->y() - get(OrgPoint)->y();
        pX[2] = get(XaxisPoint)->z() - get(OrgPoint)->z();
        pY[0] = get(YaxisPoint)->x() - get(OrgPoint)->x();
        pY[1] = get(YaxisPoint)->y() - get(OrgPoint)->y();
        pY[2] = get(YaxisPoint)->z() - get(OrgPoint)->z();
        const double CosAlpha = dot_product(pX,pY)/(yLength*yLength);
        const double SinAlpha = sqrt(1-(CosAlpha*CosAlpha));
        unitAxis.push_back(arlCore::Point(0.0, 0.0, 0.0));
        unitAxis.push_back(arlCore::Point(xLength, 0.0, 0.0));
        unitAxis.push_back(arlCore::Point(yLength*CosAlpha, yLength*SinAlpha, 0.0));
        T.register3D3D(modelAxis, unitAxis, true);
        T.trf(*this);
    }
    return nbIterations;
}

double smallerRatio( double gaussianError, const vnl_vector<double> &V )
{   // Elements de V croissant. Ratio = Grand/Petit
    assert(V.size()>1);
    double ratio, min=DBL_MAX;
    unsigned int i;//,j;
    for( i=0 ; i<V.size()-1 ; ++i )
    {
        ratio = (V[i+1]-gaussianError)/(V[i]+gaussianError);
        if(ratio<min) min=ratio;
    }
/*  {
        std::cout<<V<<" ";
        for( j=i+1 ; j<V.size() ; ++j )
        {
            std::cout<<V[j]/V[i]<<" ";
            if(V[j]/V[i]<min) min=(V[j]-gaussianError)/(V[i]+gaussianError);
        }
        std::cout<<"\n";
    }*/
    return min;
}

bool shapesProperties( double gaussianError, const std::vector<vnl_vector<double> > &shapes, vnl_vector<double> &ratiosMin, vnl_vector<double> &perimeters )
{
    const bool Verbose = false;
    if(shapes.size()<1) return false;
    unsigned int i;
//  std::vector< vnl_vector<unsigned int> >index;
    if(Verbose) std::cerr<<"Nb de "<<shapes[0].size()<<"-formes : "<<(unsigned int)shapes.size()<<"\n";
    ratiosMin.set_size((unsigned int)shapes.size());
    perimeters.set_size((unsigned int)shapes.size());
    for( i=0 ; i<(unsigned int)shapes.size() ; ++i )
    {
        ratiosMin[i] = smallerRatio(gaussianError, shapes[i]);
        perimeters[i] = shapes[i].sum();
//      std::cout<<"Shape "<<i<<" : Perimeter="<<perimeters[i]<<" - ratioMin="<<ratiosMin[i]<<"\n";
    }
    if(Verbose) std::cerr<<"Smaller Ratio : Min="<<ratiosMin.min_value()<<" Max="<<ratiosMin.max_value()<<" Mean="<<ratiosMin.mean()<<"\n";
    if(Verbose) std::cerr<<"Perimeters : Min="<<perimeters.min_value()<<" Max="<<perimeters.max_value()<<" Mean="<<perimeters.mean()<<"\n";
    return true;
}

double shapesMatching( double gaussianError, const std::vector<vnl_vector<double> >& V1, const std::vector<vnl_vector<double> >& V2, std::vector<std::pair<double, vnl_vector_fixed<unsigned int,2> > > &matching)
{
    const double DiscrSommetTol = 1.0;
    if(V1.size()<=0 || V2.size()<=0) return 0.0;
    unsigned int ShapeDim = V2[0].size();
    vnl_vector<double> ratiosMin;
    vnl_vector<double> perimeters;
    shapesProperties(gaussianError,V2,ratiosMin,perimeters);
    // Seuil mini du coefficient de discrimination des sommets
    // Les 2 cts les plus similaires sont au moins x fois plus grand l'un que l'autre
    const double DiscrSommetMin = ratiosMin.mean();
//  if(DiscrSommetMin<DiscrSommetTol) DiscrSommetMin=DiscrSommetTol;
    const unsigned int size = V1[0].size();
    unsigned int i,j;
    double realMax=-1;
    for( i=0 ; i<V1.size() ; ++i )
        for( j=0 ; j<V2.size() ; ++j )
        {
            assert(V1[i].size()==size && V2[j].size()==size);
            //if(smallerRatio(V1[i])>=DiscrSommetMin && smallerRatio(V2[j])>=DiscrSommetMin)
            if(ratiosMin[j]>=DiscrSommetMin)
            {
                vnl_vector_fixed<unsigned int,2> index(i,j);
                double similarite = (V1[i]-V2[j]).rms();
            //  if(similarite<3) std::cout<<similarite<<"\n";
                if(similarite>realMax) realMax=similarite;
                if(similarite<=gaussianError*2*ShapeDim) matching.push_back(std::make_pair(similarite,index));
            }
        }
//  std::cout<<"Shape matching Real Max : "<<realMax<<" shapematching size="<<matching.size()<<"\n";
    return realMax;
}

unsigned int generateTrianglesList( /*unsigned int shapeSize, */double decimage, const arlCore::PointList &points, std::vector<vnl_vector<double> > &shapes,  std::vector< vnl_vector<unsigned int> > &index )
{
    const bool Verbose = false;
    const unsigned int ShapeSize = 3;
    unsigned int step = (unsigned int)(1.0/decimage);
    if(step<1) step=1;
    const bool ImpossiblePath = true;
    unsigned int i,j,k/*,l*/;
    vnl_vector<unsigned int> ix(ShapeSize);
    vnl_vector<double> sh(ShapeSize);
//  std::vector<std::pair<double,unsigned int> > V0(ShapeSize);
    for( i=0 ; i<points.size() ; i+=step )
        for( j=i+1 ; j<points.size() ; j+=step )
            for( k=j+1 ; k<points.size() ; k+=step )
            {   // Ordonnancement des distances
                const double d0 = arlCore::distance(*(points[i]),*(points[j]));
                const double d1 = arlCore::distance(*(points[j]),*(points[k]));
                const double d2 = arlCore::distance(*(points[k]),*(points[i]));
/*              V0[0]=std::make_pair(d0,i);
                V0[1]=std::make_pair(d1,j);
                V0[2]=std::make_pair(d2,k);
                std::sort( V0.begin(), V0.end() );
                for( l=0 ; l<ShapeSize ; ++l )
                {
                    sh[l]=V0[l].first;
                    ix(l)=V0[l].second;
                }*/
                if( d0<=d1 && d1<=d2 ) {sh[0]=d0,sh[1]=d1,sh[2]=d2,ix[0]=i,ix[1]=j,ix[2]=k;}
                else if( d0<=d2 && d2<=d1 ) {sh[0]=d0,sh[1]=d2,sh[2]=d1,ix[0]=j,ix[1]=i,ix[2]=k;}
                    else if( d1<=d0 && d0<=d2 ) {sh[0]=d1,sh[1]=d0,sh[2]=d2,ix[0]=k,ix[1]=j,ix[2]=i;}
                        else if( d1<=d2 && d2<=d0 ) {sh[0]=d1,sh[1]=d2,sh[2]=d0,ix[0]=j,ix[1]=k,ix[2]=i;}
                            else if( d2<=d0 && d0<=d1 ) {sh[0]=d2,sh[1]=d0,sh[2]=d1,ix[0]=k,ix[1]=i,ix[2]=j;}
                                else if( d2<=d1 && d1<=d0 ) {sh[0]=d2,sh[1]=d1,sh[2]=d0,ix[0]=i,ix[1]=k,ix[2]=j;}
                                    else assert(ImpossiblePath);
                shapes.push_back(sh);
                index.push_back(ix);
            }
    if(Verbose)
    {   // Affichage de la liste de points, de segments et d'index
        for( i=0 ; i<points.size() ; ++i )
            std::cout<<i<<" : "<<points[i]->getString()<<"\n";
        for( i=0 ; i<shapes.size() ; ++i )
            std::cout<<i<<" : "<<shapes[i]<<" Index : "<<index[i]<<"\n";
    }
    return (unsigned int)shapes.size();
}

unsigned int arlCore::matching3D3D( const arlCore::PointList &a, const arlCore::PointList &b, double gaussianError, double decimage,
                          std::vector< const arlCore::Point* > &Va, std::vector< const arlCore::Point* > &Vb )
{   // b = T*a ; A=echantillon mesure ; B=modele reel
    const bool Verbose = false;
    const unsigned int ShapeDim = 3; // Triangle = 3 cotes
    const double ProbaMin=10e-1;//10e-6;
    const double NumericNoise=1e-13;
    const double SimilariteMax=gaussianError*ShapeDim+NumericNoise; // mm

    Va.clear(); Vb.clear();
    assert(a.getDimension()==b.getDimension());
    assert(a.size()>2 && b.size()>2);
    unsigned int i,j;
    std::vector<vnl_vector<double> > V1, V2;
    std::vector< vnl_vector<unsigned int> >index1, index2;
    generateTrianglesList( /*ShapeDim, */decimage, a, V1, index1 );
    generateTrianglesList( /*ShapeDim, */decimage, b, V2, index2 );

    std::vector<std::pair<double, vnl_vector_fixed<unsigned int,2> > > matching;
    double simMax = shapesMatching( gaussianError, V1, V2, matching);
    if(simMax<0) return 0;
    // Tableau des probabilits d'appariement
    vnl_matrix<double> matchingProb(a.size(),b.size());
    matchingProb.fill(1.0);
    double prob;
    for( i=0 ; i<matching.size() ; ++i )
        if(matching[i].first<SimilariteMax)
        {
            if(simMax==0) prob = 1.0; else prob = matching[i].first/simMax;
            const unsigned int shape1Index = matching[i].second(0);
            const unsigned int shape2Index = matching[i].second(1);
            for( j=0 ; j<ShapeDim ; ++j )
                matchingProb(index1[shape1Index](j),index2[shape2Index](j)) *= prob;
        }
    if(Verbose) std::cout<<matchingProb<<"\n";
    for( i=0 ; i<a.size() ; ++i )
    {
        unsigned int indexMinR=0, indexMinC;
        double pMin = 1.0;
        vnl_vector<double> row=matchingProb.get_row(i);
        for( j=0 ; j<row.size() ; ++j )
            if(row(j)<pMin){pMin=row(j);indexMinR=j;}
        if(pMin<ProbaMin)
        {
            vnl_vector<double> column=matchingProb.get_column(indexMinR);
            pMin=1.0;indexMinC=0;
            for( j=0 ; j<column.size() ; ++j )
                if(column(j)<pMin){pMin=column(j);indexMinC=j;}
            if(indexMinC==i) // Est-ce bijectif ?
            {
                Va.push_back(a[i]);
                Vb.push_back(b[indexMinR]);
            }
        }
    }
    assert(Va.size()==Vb.size());
    if(Verbose) std::cout<<"Matching 3D3D Nb matching : "<<(unsigned int)Va.size()<<"\n";
    return (unsigned int)Va.size();
}

bool arlCore::distance( const arlCore::PointList &p1, const arlCore::PointList &p2, std::vector<double> &distances )
{
    if(p1.getDimension()!=p2.getDimension()) return false;
    unsigned int i, size=p1.size();
    if(p2.size()<size) size=p2.size();
    for( i=0 ; i<size ; ++i )
        if(p1[i]!=0 && p2[i]!=0)
            distances.push_back(p1[i]->distance(*p2[i]));
        else distances.push_back(-1);
    return true;
}

bool arlCore::save( const  std::vector< Point* >&pl, const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type, bool justVisible, bool overwrite )
{
    return internSave( pl, fileName, std::string(""), type, justVisible, overwrite );
}

bool arlCore::save( const  std::vector< const Point* >&pl, const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type, bool justVisible, bool overwrite )
{
    return internSave( pl, fileName, std::string(""), type, justVisible, overwrite );
}
