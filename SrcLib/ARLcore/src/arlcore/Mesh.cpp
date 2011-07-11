/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Mesh.h>

#include <fstream>
#include <math.h>

#include <vnl/vnl_math.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_triangle_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <arlcore/Misc.h>
#include <arlcore/File.h>

arlCore::Mesh::Mesh( double pointScalar ):
m_pointScalar(pointScalar)
{
    m_pointList.setDimension(3);
}

arlCore::Mesh::Mesh( const Mesh& p )
{
    copy(p);
}

arlCore::Mesh::~Mesh( void )
{}

void arlCore::Mesh::copy( const Mesh& m )
{
    m_pointList = m.m_pointList;
    m_triangles = m.m_triangles;
    m_nbReferences = m.m_nbReferences;
    // TODO Don't copy free points & triangles
    m_freePoints = m.m_freePoints;
    m_freeTriangles = m.m_freeTriangles;
}

void arlCore::Mesh::clear( void )
{
    m_pointList.clear();
    m_triangles.clear();
    m_nbReferences.clear();
    m_freePoints.clear();
    m_freeTriangles.clear();
}

arlCore::Mesh::Mesh( const std::string& fileName )
{
    load(fileName);
}

double arlCore::Mesh::getRMSDistance( void ) const
{
    double distance = 0.0;
    unsigned int i, j;
    const unsigned int Size = (unsigned int)m_triangles.size();
    for( i=0 ; i<Size ; ++i )
    {
        vnl_vector_fixed<double,3> lengths = getLengths(i);
        for( j=0 ; j<3 ; ++j )
            distance += lengths[j]*lengths[j];
    }
    return sqrt(distance/(double)(Size*3));
}

double arlCore::Mesh::computeDistance2( const arlCore::Point& p ) const
{
    unsigned int noTriangle;
    const double D = computeDistance(p, noTriangle);
    return D*D;
}

double arlCore::Mesh::computeDistance2( const arlCore::Point& p, unsigned int &noTriangle ) const
{
    const double D = computeDistance(p, noTriangle);
    return D*D;
}

double arlCore::Mesh::computeDistance( const arlCore::Point& p ) const
{
    unsigned int noTriangle;
    return computeDistance( p, noTriangle );
}

double arlCore::Mesh::computeDistance( const arlCore::Point& p, unsigned int &noTriangle ) const
{
    const vgl_point_3d<double> VglPoint(p.x(), p.y(), p.z());
    unsigned int i;
    double distance = -1.0;
    for( i=0 ; i<m_triangles.size() ; ++i )
    {
        const arlCore::Point& P0 = *m_pointList[m_triangles[i][0]];
        const vgl_point_3d<double> VglP0(P0.x(), P0.y(), P0.z());
        const arlCore::Point& P1 = *m_pointList[m_triangles[i][1]];
        const vgl_point_3d<double> VglP1(P1.x(), P1.y(), P1.z());
        const arlCore::Point& P2 = *m_pointList[m_triangles[i][2]];
        const vgl_point_3d<double> VglP2(P2.x(), P2.y(), P2.z());
        const double D = vgl_triangle_3d_distance( VglPoint, VglP0, VglP1, VglP2 );
        if(i==0) {distance = D, noTriangle = i;}
        else if(D<distance) {distance = D, noTriangle = i;}
    }
    return distance;
}

const arlCore::PointList& arlCore::Mesh::getPointList( void ) const
{
    return m_pointList;
}

const std::vector< vnl_vector_fixed<unsigned int, 3> >& arlCore::Mesh::getTriangles( void ) const
{
    return m_triangles;
}

double arlCore::Mesh::triangleSurface( unsigned int i ) const
{
    if(i>=m_triangles.size()) return -1;
    const arlCore::Point& P0 = *m_pointList[m_triangles[i][0]];
    const vgl_point_3d<double> VglP0(P0.x(), P0.y(), P0.z());
    const arlCore::Point& P1 = *m_pointList[m_triangles[i][1]];
    const vgl_point_3d<double> VglP1(P1.x(), P1.y(), P1.z());
    const arlCore::Point& P2 = *m_pointList[m_triangles[i][2]];
    const vgl_point_3d<double> VglP2(P2.x(), P2.y(), P2.z());
    return vgl_triangle_3d_area( VglP0, VglP1, VglP2 );
}

bool arlCore::Mesh::generateX( double ySize, double zSize, double yStep, double zStep, double degree, double gaussianNoise, arlCore::PointList& summits )
{
    return generate( 0, ySize, zSize, yStep, zStep, degree, gaussianNoise, summits );
}

bool arlCore::Mesh::generateY( double xSize, double zSize, double xStep, double zStep, double degree, double gaussianNoise, arlCore::PointList& summits )
{
    return generate( 1, xSize, zSize, xStep, zStep, degree, gaussianNoise, summits );
}

bool arlCore::Mesh::generateZ( double xSize, double ySize, double xStep, double yStep, double degree, double gaussianNoise, arlCore::PointList& summits )
{
    return generate( 2, xSize, ySize, xStep, yStep, degree, gaussianNoise, summits );
}

bool arlCore::Mesh::generate( unsigned int axis, double width, double length, double stepW, double stepL, double degree, double gaussianNoise, arlCore::PointList& summits )
{
    clear();
    const unsigned int Width = (unsigned int)(width/stepW);
    const unsigned int Length = (unsigned int)(length/stepL);
    if(Width<1 || Length<1) return false;
    std::vector< std::vector<double> > grid(Width);
    unsigned int i, j, k;
    for( i=0 ; i<Width ; ++i )
    {
        const double Wi = (double)i*stepW;
        grid[i].resize(Length);
        for( j=0 ; j<Length ; ++j )
        {
            const double Li = (double)j*stepL;
            bool end = false;
            double altitude = 0.0;
            vnl_vector<double> invDists(summits.size());
            for( k=0 ; k<summits.size() && !end ; ++k )
            {
                double distance2;
                switch(axis)
                {
                case 0:
                    distance2 = (Wi-summits[k]->y())*(Wi-summits[k]->y())+(Li-summits[k]->z())*(Li-summits[k]->z());
                    break;
                case 1:
                    distance2 = (Wi-summits[k]->x())*(Wi-summits[k]->x())+(Li-summits[k]->z())*(Li-summits[k]->z());
                    break;
                case 2:
                    distance2 = (Wi-summits[k]->x())*(Wi-summits[k]->x())+(Li-summits[k]->y())*(Li-summits[k]->y());
                    break;
                default: return false;
                }
                const double Distance = sqrt(distance2);
                const double DistancePow = pow(Distance, degree);
                if(Distance<1e-8)
                {
                    altitude = summits[k]->z();
                    end = true;
                }else invDists[k] = 1.0/DistancePow;
            }
            if(!end)
            {
                const double Sum = invDists.sum();
                for( k=0 ; k<summits.size() ; ++k )
                {
                    const double Coeff = invDists[k]/Sum;
                    altitude += summits[k]->z()*Coeff;
                }
            }
            grid[i][j]=altitude;
        }
    }
    for( i=0 ; i<Width-1 ; ++i )
    {
        const double Width0 = (double)i*stepW;
        const double Width1 = (double)(i+1)*stepW;
        for( j=0 ; j<Length-1 ; ++j )
        {
            
            const double Length0 = (double)j*stepL;
            const double Length1 = (double)(j+1)*stepL;
            const double Alt0 = grid[i+0][j+0];
            const double Alt1 = grid[i+1][j+0];
            const double Alt2 = grid[i+1][j+1];
            const double Alt3 = grid[i+0][j+1];
            std::vector<unsigned int> index(4);
            switch(axis)
            {
            case 0:
                index[0] = addPoint(arlCore::Point(Alt0, Width0, Length0), gaussianNoise);
                index[1] = addPoint(arlCore::Point(Alt1, Width1, Length0), gaussianNoise);
                index[2] = addPoint(arlCore::Point(Alt2, Width1, Length1), gaussianNoise);
                index[3] = addPoint(arlCore::Point(Alt3, Width0, Length1), gaussianNoise);
                break;
            case 1:
                index[0] = addPoint(arlCore::Point(Width0, Alt0, Length0), gaussianNoise);
                index[1] = addPoint(arlCore::Point(Width1, Alt1, Length0), gaussianNoise);
                index[2] = addPoint(arlCore::Point(Width1, Alt2, Length1), gaussianNoise);
                index[3] = addPoint(arlCore::Point(Width0, Alt3, Length1), gaussianNoise);
                break;
            case 2:
                index[0] = addPoint(arlCore::Point(Width0, Length0, Alt0), gaussianNoise);
                index[1] = addPoint(arlCore::Point(Width1, Length0, Alt1), gaussianNoise);
                index[2] = addPoint(arlCore::Point(Width1, Length1, Alt2), gaussianNoise);
                index[3] = addPoint(arlCore::Point(Width0, Length1, Alt3), gaussianNoise);
                break;
            default: return false;
            }
            addTriangle(index[0], index[1], index[2]);
            addTriangle(index[0], index[2], index[3]);
        }
    }
    return true;
}

bool arlCore::Mesh::load( const std::string& fileName, double gaussianNoise )
{
    const bool Verbose = true;
    clear();
    if(fileName=="") return false;
    std::string extractFolder, extractFile, ext;
    arlString::splitFilename( fileName, extractFolder, extractFile, ext );
    std::string filetype = File::getFiletype( fileName );
    if(filetype=="Planes")
    {
        arlCore::File config(fileName, "Planes");
        const double Version = config.readHeader();
        if(Version>0.0)
        {
            std::vector< vgl_homg_plane_3d<double> > planes;
            std::vector< double > planeRMS, planeMin, planeMax;
            std::vector< unsigned int > planeN;
            std::vector< vgl_homg_point_3d<double> > points;
            unsigned int i, index;
            std::string token, name;
            while(config.getToken(token))
            {
                unsigned int p1, p2, p3;
                double x, y, z, outliers=0.0, scalar=0.0;
                if(token=="Planes")
                {
                    config.get(name);
                    config.get(outliers);
                    std::vector< vnl_vector<double> > vnlPoints;
                    std::vector< std::string > names;
                    std::stringstream ss;
                    ss<<extractFolder<<"/"<<name;
                    if(arlCore::load( vnlPoints, names, ss.str() ))
                    {
                        std::vector< arlCore::PointList > PL;
                        for( i=0 ; i<vnlPoints.size() ; ++i )
                        {
                            if(arlString::valueOf(names[i], index))
                            {
                                if(PL.size()<=index) PL.resize(index+1);
                                arlCore::Point pt(vnlPoints[i][0], vnlPoints[i][1], vnlPoints[i][2]);
                                PL[index].push_back(pt);
                            }
                        }
                        const unsigned int Size = (unsigned int)PL.size();
                        planes.resize(Size);
                        planeRMS.resize(Size);
                        planeMin.resize(Size);
                        planeMax.resize(Size);
                        planeN.resize(Size);
                        for( i=0 ; i<PL.size() ; ++i )
                            planeN[i] = PL[i].plane( planes[i], planeRMS[i], planeMin[i], planeMax[i], scalar, outliers );
                    }
                }
                if(token=="Point")
                {
                    config.get(index);
                    if(points.size()<=index) points.resize(index+1);
                    config.getToken(token);
                    if(token=="Intersection")
                    {
                        config.get(p1);
                        config.get(p2);
                        config.get(p3);
                        if(planes.size()>p1 && planes.size()>p2 && planes.size()>p3)
                            points[index]=vgl_homg_point_3d<double>(planes[p1], planes[p2], planes[p3]);
                    }
                    if(token=="Coordinates")
                    {
                        config.get(x);
                        config.get(y);
                        config.get(z);
                        points[index]=vgl_homg_point_3d<double>(x, y, z);
                    }
                }
                if(token=="Triangle")
                {
                    config.get(p1);
                    config.get(p2);
                    config.get(p3);
                    if(points.size()>p1 && points.size()>p2 && points.size()>p3)
                    {
                        points[p1].get_nonhomogeneous( x, y, z );
                        const arlCore::Point Pt1( x, y, z );
                        points[p2].get_nonhomogeneous( x, y, z );
                        const arlCore::Point Pt2( x, y, z );
                        points[p3].get_nonhomogeneous( x, y, z );
                        const arlCore::Point Pt3( x, y, z );
                        addTriangle( Pt1, Pt2, Pt3 );
                    }
                }
                double compareValue = 0.0;
                const bool Compare = (token=="Compare");
                if(Compare)
                    config.get(compareValue);
                if(token=="Display" || Compare)
                {   // FIXME Find another way to display this informations
                    config.getToken(token);
                    if(token=="Point")
                    {
                        config.get(p1);
                        if(points.size()>p1)
                        {
                            points[p1].get_nonhomogeneous( x, y, z );
                            std::cout<<"Point "<<p1<<" = "<<x<<" "<<y<<" "<<z<<"\n";
                        }
                    }
                    if(token=="Plane")
                    {
                        config.get(p1);
                        if(planes.size()>p1)
                        {
                            std::cout<<"Plane "<<p1<<" RMS="<<planeRMS[p1]<<" Min="<<planeMin[p1];
                            std::cout<<" Max="<<planeMax[p1]<<" n="<<planeN[p1];
                            //std::cout<<" : "<<planes[p1];
                            std::cout<<"\n";
                        }
                    }
                    if(token=="PlaneAngle")
                    {
                        config.get(p1);
                        config.get(p2);
                        if(planes.size()>p1 && planes.size()>p2)
                        {
                            double angle = fmod(vgl_homg_operators_3d<double>::plane_plane_angle(planes[p1], planes[p2]), vnl_math::pi/2.0);
                            if(angle>=vnl_math::pi/4.0) angle = vnl_math::pi/2.0 - angle;
                            std::cout<<"Plane Angle "<<p1<<"-"<<p2<<" = "<<angle*arlCore::RadToDeg<<" deg";
                            if(Compare) std::cout<<" (Delta="<<angle*arlCore::RadToDeg-compareValue<<")";
                            std::cout<<"\n";
                        }
                    }
                    if(token=="PointDistance")
                    {
                        config.get(p1);
                        config.get(p2);
                        if(points.size()>p1 && points.size()>p2)
                        {
                            const double Distance = vgl_distance(points[p1], points[p2]);
                            std::cout<<"Point Distance "<<p1<<"-"<<p2<<" = "<<Distance<<" mm";
                            if(Compare) std::cout<<" (Delta="<<Distance-compareValue<<")";
                            std::cout<<"\n";
                        }
                    }
                    if(token=="PointAngle")
                    {
                        config.get(p1);
                        config.get(p2);
                        config.get(p3);
                        if(points.size()>p1 && points.size()>p2 && points.size()>p3)
                        {
                            double x1, x2, x3, y1, y2, y3, z1, z2, z3;
                            points[p1].get_nonhomogeneous( x1, y1, z1 );
                            points[p2].get_nonhomogeneous( x2, y2, z2 );
                            points[p3].get_nonhomogeneous( x3, y3, z3 );
                            vnl_vector_fixed<double, 3> dir1( x1-x2, y1-y2, z1-z2 );
                            vnl_vector_fixed<double, 3> dir2( x3-x2, y3-y2, z3-z2 );
                            dir1.normalize();
                            dir2.normalize();
                            double angle = vcl_acos( dot_product( dir1, dir2 ) );
                            std::cout<<"Point Angle "<<p1<<"-"<<p2<<"-"<<p3<<" = "<<angle*arlCore::RadToDeg<<" deg";
                            if(Compare) std::cout<<" (Delta="<<angle*arlCore::RadToDeg-compareValue<<")";
                            std::cout<<"\n";                        }
                    }
                }
            }
        }
        config.close();
        return m_triangles.size()>0;
    }
    if(filetype=="Surface")
    {
        arlCore::File config(fileName, "Surface");
        const double Version = config.readHeader();
        if(Version>0.0)
        {
            arlCore::PointList summits(3);
            unsigned int xSize=0, ySize=0;
            double xStep = 1.0, yStep = 1.0, degree = 2.0, noise = 0.0;
            vnl_vector<double> point(3);
            std::string token, name;
            while(config.getToken(token))
            {
                if(token=="Name") config.get(name);
                // TODO Generate altitude on X or Y-axis
                if(token=="XSize" || token=="Width") config.get(xSize);
                if(token=="YSize" || token=="Length") config.get(ySize);
                if(token=="XStep") config.get(xStep);
                if(token=="YStep") config.get(yStep);
                if(token=="Step")
                {
                    config.get(xStep);
                    yStep = xStep;
                }
                if(token=="Degree") config.get(degree);
                if(token=="GaussianNoise") config.get(noise);
                if(token=="Summit")
                    if(config.get(point, 3))
                        summits.push_back(arlCore::Point(point[0],point[1],point[2]));
            }
            if(gaussianNoise>0.0) noise = gaussianNoise;
            generateZ( xSize, ySize, xStep, yStep, degree, noise, summits );
        }
        config.close();
        return m_triangles.size()>0;
    }
    std::ifstream file;
    file.open (fileName.c_str(), std::fstream::in);
    if(!file.is_open())
    {
        std::cerr<<"Mesh Loading error : "<<fileName<<"\n";
        return false;
    }
    const bool Trian = (ext=="trian");
    const bool Raw = (ext=="raw");
    unsigned int i=0, a, b, c;
    double x, y, z;
    Point p(3);
    if(Trian)
    {
        unsigned int nbPts, nbTrian;
        file>>nbPts;
        for( ; i<nbPts ; ++i )
        {
            file>>x>>y>>z;
            p.x(x);
            p.y(y);
            p.z(z);
            addPoint(p, gaussianNoise);
        }
        file>>nbTrian;
        for( i=0 ; i<nbTrian ; ++i )
        {
            file>>a>>b>>c>>x>>y>>z;
            addTriangle(a, b, c);
        }
        file.close();
        return m_triangles.size()>0;
    }
    if(Raw)
    {
        while(!file.eof())
        {
            file>>x>>y>>z;
            p.x(x);
            p.y(y);
            p.z(z);
            unsigned int index = addPoint(p, gaussianNoise);
            if((i+1)%3==0) addTriangle(index-2, index-1, index);
            ++i;
        }
        file.close();
        return m_triangles.size()>0;
    }
    return false;
}

bool arlCore::Mesh::save( const std::string& fileName, bool overwrite ) const
{
    std::fstream file;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    file.open(fileName.c_str(), std::fstream::out);
    if(!file.is_open()) return false;
    std::string extractFolder, extractFile, ext;
    arlString::splitFilename( fileName, extractFolder, extractFile, ext );
    const bool Trian = (ext=="trian");
    const bool Raw = (ext=="raw");
    const bool Vtk = (ext=="vtk");
    unsigned int i, j, k;
    if(Trian)
    {
        file<<(unsigned int)m_pointList.size()<<"\n";
        for( i=0 ; i<(unsigned int)m_pointList.size() ; ++i )
        {
            for( j=0 ; j<3 ; ++j )
                file<<m_pointList[i]->get(j)<<" ";
            file<<"\n";
        }
        file<<(unsigned int)m_triangles.size()<<"\n";
        for( i=0 ; i<(unsigned int)m_triangles.size() ; ++i )
        {
            for( j=0 ; j<3 ; ++j )
                file<<m_triangles[i][j]<<" ";
            file<<"-1 -1 -1\n";
        }
        file.close();
        return true;
    }
    if(Raw)
    {
        for( i=0 ; i<(unsigned int)m_triangles.size() ; ++i )
        {
            for( j=0 ; j<3 ; ++j )
                for( k=0 ; k<3 ; ++k )
                    file<<m_pointList[m_triangles[i][j]]->get(k)<<" ";
            file<<"\n";
        }
        file.close();
        return true;
    }
    if(Vtk)
    {
        file<<"# vtk DataFile Version 1.0\nFileName\nASCII\n\n";
        //file<<DATASET UNSTRUCTURED_GRID";
        file<<"DATASET POLYDATA";
        file<<"\nPOINTS "<<(unsigned int)m_pointList.size()<<" double\n";
        for( i=0 ; i<(unsigned int)m_pointList.size() ; ++i )
        {
            for( j=0 ; j<3 ; ++j )
                file<<m_pointList[i]->get(j)<<" ";
            file<<"\n";
        }

        //file<<"\nCELLS ";
        file<<"\nPOLYGONS ";
        file<<(unsigned int)m_triangles.size()<<" "<<(unsigned int)m_triangles.size()*4<<"\n";
        for( i=0 ; i<(unsigned int)m_triangles.size() ; ++i )
        {
            file<<"3 ";
            for( j=0 ; j<3 ; ++j )
                file<<m_triangles[i][j]<<" ";
            file<<"\n";
        }
    /*  file<<"\nCELLS_TYPES "<<(unsigned int)m_triangles.size()<<"\n";
        for( i=0 ; i<(unsigned int)m_triangles.size() ; ++i )
            file<<"5\n";*/
        file.close();
        return true;
    return true;
    }
    return false;
}

bool arlCore::Mesh::resampling( double step )
{
    if(step<=0) return false;
    unsigned int i, j;
    const double Tol = 2.0*step;
    std::vector<arlCore::Point> points(3);
    for( i=0 ; i<(unsigned int)m_triangles.size() ; )
    {
        vnl_vector_fixed<double,3> lengths = getLengths(i);
        if((lengths[0]+lengths[1])>=Tol)
        {
            std::vector<unsigned int> index(6);
            for( j=0 ; j<3 ; ++j )
                index[j] = m_triangles[i][j];
            const arlCore::Point &A = *m_pointList[index[0]];
            const arlCore::Point &B = *m_pointList[index[1]];
            const arlCore::Point &C = *m_pointList[index[2]];
            m_freeTriangles.push_back(i);
            --m_nbReferences[m_triangles[i][0]];
            --m_nbReferences[m_triangles[i][1]];
            --m_nbReferences[m_triangles[i][2]];
            for( j=0 ; j<3 ; ++j )
            {
                points[0].set(j, (A[j]+B[j])/2.0);
                points[1].set(j, (B[j]+C[j])/2.0);
                points[2].set(j, (C[j]+A[j])/2.0);
            }
            for( j=0 ; j<3 ; ++j )
                index[3+j] = addPoint(points[j]);
            addTriangle( index[0], index[3], index[5] );
            addTriangle( index[3], index[1], index[4] );
            addTriangle( index[4], index[2], index[5] );
            addTriangle( index[5], index[3], index[4] );
        }else ++i;
    }
    assert(m_freePoints.size()==0);
    assert(m_freeTriangles.size()==0);
    return true;
}

// Private methods

unsigned int arlCore::Mesh::addPoint( const arlCore::Point& p, double gaussianNoise )
{
    unsigned int index;
    if(m_freePoints.size()>0)
    {
        index = m_freePoints.back();
        m_freePoints.pop_back();
        m_pointList[index]->copy(p);
    }else
    {
        index = m_pointList.size();
        m_pointList.push_back(p);
        m_pointList.back()->setScalar(m_pointScalar);
        m_nbReferences.push_back(0);
    }
    if(gaussianNoise>0.0) m_pointList[index]->addGaussianNoise(gaussianNoise);
    return index;
}

unsigned int arlCore::Mesh::releasePoint( unsigned int noPoint )
{
    if(m_nbReferences[noPoint]<1) return 0;
    --m_nbReferences[noPoint];
    if(m_nbReferences[noPoint]<1)
    {
        m_freePoints.push_back(noPoint);
        m_pointList[noPoint]->setVisible(false);
    }
    return m_nbReferences[noPoint];
}

bool arlCore::Mesh::addTriangle( const arlCore::Point &P1, const arlCore::Point &P2, const arlCore::Point &P3 )
{
    std::vector<unsigned int> index;
    index.push_back(addPoint(P1));
    index.push_back(addPoint(P2));
    index.push_back(addPoint(P3));
    return addTriangle(index[0], index[1], index[2]);
}

bool arlCore::Mesh::addTriangle( unsigned int a, unsigned int b, unsigned int c )
{
    if(m_freeTriangles.size()==0) m_triangles.push_back(vnl_vector_fixed<unsigned int, 3>(a,b,c));
    else
    {
        unsigned int index = m_freeTriangles.back();
        m_freeTriangles.pop_back();
        m_triangles[index] = vnl_vector_fixed<unsigned int, 3>(a,b,c);
    }
    ++m_nbReferences[a];
    ++m_nbReferences[b];
    ++m_nbReferences[c];
    return true;
}

bool arlCore::Mesh::releaseTriangle( unsigned int noTriangle )
{
    unsigned int i;
    for( i=0 ; i<3 ; ++i )
        releasePoint(m_triangles[noTriangle][i]);
    m_freeTriangles.push_back(noTriangle);
    return true;
}

vnl_vector_fixed<double,3> arlCore::Mesh::getLengths( unsigned int noTriangle ) const
{
    vnl_vector_fixed<double,3> lengths;
    assert(noTriangle<m_triangles.size());
    const arlCore::Point &A = *m_pointList[m_triangles[noTriangle][0]];
    const arlCore::Point &B = *m_pointList[m_triangles[noTriangle][1]];
    const arlCore::Point &C = *m_pointList[m_triangles[noTriangle][2]];
    lengths[0] = A.distance(B);
    lengths[1] = B.distance(C);
    lengths[2] = C.distance(A);
    return lengths;
}

bool arlCore::Mesh::simplify( void )
{
#ifndef ANN
    return false;
#else // ANN
    unsigned int i, j;
    const unsigned int Size = m_pointList.visibleSize();
    const unsigned int Dimension = m_pointList.getDimension();
    ANNpointArray ANNPoints = annAllocPts( Size, Dimension );
    for( i=0 ; i<m_pointList.size() ; ++i )
        for( j=0 ; j<Dimension ; ++j )
            ANNPoints[i][j]=m_pointList[i]->get(j);
    const int BucketSize = 1;
    ANNkd_tree* ANNtree = new ANNkd_tree( ANNPoints, Size, Dimension, BucketSize, ANN_KD_SL_MIDPT );
    const double Epsilon = 1e-8;// Error bound
    const double SquaredEpsilon = Epsilon*Epsilon;
    ANNpoint ANNPt = annAllocPt(Dimension); // Query point
    const unsigned int NbNeighbors = 20;
    ANNidxArray Nn_idx = new ANNidx[NbNeighbors]; // near neighbor indices
    ANNdistArray SquaredDists = new ANNdist[NbNeighbors]; // near neighbor distances
    for( i=0 ; i<m_pointList.size() ; ++i )
        if(m_pointList[i]->isVisible())
        {
            std::vector<unsigned int> oldIndex;
            for( j=0 ; j<Dimension; ++j )
                ANNPt[j] = m_pointList[i]->get(j);
            ANNtree->annkSearch( ANNPt, NbNeighbors, Nn_idx, SquaredDists, Epsilon );
            // Cherche points les plus proches
            for( j=0 ; j<NbNeighbors ; ++j )
                if(SquaredDists[j]<=SquaredEpsilon)
                {
                     releasePoint(Nn_idx[j]);
                     oldIndex.push_back(Nn_idx[j]);
                }
            replacePointIndex(oldIndex, i);
        }
    delete ANNtree;
    annDeallocPt( ANNPt );
    annDeallocPts( ANNPoints );
    delete[] Nn_idx;
    delete[] SquaredDists;
    annClose(); 
    return true;
#endif // ANN
}

unsigned int arlCore::Mesh::replacePointIndex( std::vector<unsigned int> oldIndex, unsigned int newIndex )
{   // Replace in m_triangles values of oldIndex by newIndex
    unsigned int i, j, k, n=0;
    const unsigned int OldIndexSize = (unsigned int)oldIndex.size();
    for( i=0 ; i<m_triangles.size() ; ++i )
        for( j=0 ; j<OldIndexSize ; ++j )
            for( k=0 ; k<3 ; ++k )
                if(m_triangles[i][k]==oldIndex[j])
                    m_triangles[i][k] = newIndex;
    return n;
}
