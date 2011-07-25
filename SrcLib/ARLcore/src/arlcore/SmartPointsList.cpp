/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/SmartPointsList.h>

#include <fstream>
#include <iomanip>
#include <algorithm>

#include <arlcore/PointsList.h>
#include <arlcore/Misc.h>

arlCore::SmartPointList::SmartPointList( void )
{   // First is always an empty list
    m_listOfLists.resize(1);
}

arlCore::SmartPointList::SmartPointList(const SmartPointList& p)
{
        copy(p);
}

arlCore::SmartPointList& arlCore::SmartPointList::operator=(const SmartPointList& p)
{
    copy(p);
    return *this;
}

void arlCore::SmartPointList::copy(const SmartPointList& p)
{
    if(this==&p) return;
//  arlCore::Object *a=this;
//  const arlCore::Object *b=&p;
//  *a = *b;
    clear();
    // TODO : Fill this with p
//      std::vector< std::vector< Point::csptr > > m_listOfLists;
//      std::map< double, std::vector< Point::csptr > > m_mapOfLists;
//      std::vector< std::vector< Point::csptr > > m_listsByCam
}

arlCore::SmartPointList::~SmartPointList( void )
{
    clear();
}

bool arlCore::SmartPointList::integrity( void ) const
{
    bool b=true;
    b &= (m_listOfLists.size()==(m_mapOfLists.size()+1));
    if(m_mapOfLists.size()>0) b &= (m_listsByCam.size()>0);
    return b;
}

std::string arlCore::SmartPointList::getString( void ) const
{
    std::stringstream s;
    unsigned int i,j;
    for( i=0 ; i<m_listsByCam.size() ; ++i )
    {
        s<<"* Cam "<<i<<"\n";
        for( j=0 ; j<m_listsByCam[i].size() ; ++j )
        {
            s<<m_listsByCam[i][j]->getString();
            unsigned int h=(unsigned int)m_listsByCam[i].size();
        }
    }
    s<<"\n";
    return s.str();
}

bool arlCore::SmartPointList::save( const std::string &fileName, bool overwrite ) const
{
    std::fstream file;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    file.open(fileName.c_str(), std::fstream::out);
    if(!file.is_open()) return false;
    file<<"FileType SmartPointList\n";
    file<<"Version 1.0\n";
    unsigned int cam, i, n=0;
    void* tag;
    int fringe;
    std::map<double, unsigned int, mapOrder>::const_iterator it;
    it = m_mapOfLists.begin();
    while(it!=m_mapOfLists.end())
    {
        double key = (*it).first;
        unhash(key, cam, tag, fringe);
        unsigned int index = (*it).second;
        for( i=0 ; i<m_listOfLists[index].size() ; ++i )
            if(m_listOfLists[index][i]!=0)
                if(m_listOfLists[index][i]->isOK())
                    m_listOfLists[index][i]->save( file, cam, tag, fringe );
        ++it, ++n;
    }
    assert(m_listOfLists.size()==n+1);
    file.close();
    return true;
}
bool arlCore::SmartPointList::load( const std::string &fileName )
{
    if(fileName=="") return false;
    std::ifstream file;
    file.open (fileName.c_str(), std::fstream::in);
    if(!file.is_open())
    {
        std::cerr<<"SmartPointList Loading error : "<<fileName<<"\n";
        return false;
    }
    unsigned int n=0;
    std::string token,text;
    double version;
    clear();
//  Object::update(); // TODO Optimize its position
    Point p;
    do
    {   // Reading header
        file>>token;
        if(token=="FileType")
        {
            file>>text;
            if(text!="SmartPointList") return false;
            n=(n|1);
        }
        if(token=="Version") {file>>version; n=(n|2);}
        if(file.eof())
        {
            std::cerr<<"SmartPointList Loading Header error : "<<fileName<<"\n";
            return false;
        }
    } while(n!=3);
    bool b;
    do
    {
        unsigned int cam;
        void* tag;
        int fringe;
        b=p.load(file, cam, tag, fringe);
        if(b) push_back(p, cam, tag, fringe);
    }while(b);
    file.close();
    return true;
}
unsigned int arlCore::SmartPointList::findNearPoint(double x, double y, unsigned int cam, std::vector< Point::csptr > &found, double &distance, double radius) const
{
    found.clear();
    if(m_listsByCam.size()<cam) return 0;
    Point pt(x,y);
    unsigned int i, n;
    std::vector<unsigned int> pos;
    n=arlCore::findNearPoint(m_listsByCam[cam-1], pt, pos, distance, radius);
    for( i=0 ; i<pos.size() ; ++i )
        found.push_back((Point*)m_listsByCam[cam-1][pos[i]]);
    return (unsigned int)found.size();
}
/*
const arlCore::Point* arlCore::SmartPointList::findNearPoint(const  arlCore::Point &pt, double &distance) const
{
    if(m_listsByCam.size()<1) return 0;
    const arlCore::Point* r=0;
    double dist;
    unsigned int pos;
    if(arlCore::findNearPoint(m_listsByCam[0], pt, pos, dist))
    {
        r=m_listsByCam[0][pos];
        distance=dist;
    }
    unsigned int i;
    for( i=1 ; i<m_listsByCam.size() ; ++i )
    {
        if(arlCore::findNearPoint(m_listsByCam[i], pt, pos, dist))
            if(r==0)
            {
                r=m_listsByCam[i][pos];
                distance=dist;
            }else
            {
                if(dist<distance)
                {
                    r=m_listsByCam[i][pos];
                    distance=dist;
                }
            }
    }
    return 0;
}*/

arlCore::Point::csptr arlCore::SmartPointList::push_back( const arlCore::Point& pt, unsigned int cam, void* tag, int fringe )
{
    if(cam<1) return 0;
    arlCore::Point *p=new Point(pt);
    privateGetList(cam).push_back(p);
    if(tag!=0)
    {
        privateGetList(cam,tag).push_back(p);
        if(fringe!=0) privateGetList(cam,tag,fringe).push_back(p);
    }
    return p;
}

unsigned int arlCore::SmartPointList::push_back( const std::vector< arlCore::Point::csptr >& ptList, unsigned int cam, void* tag, int fringe )
{
    unsigned int i, n = 0;
    if(cam<1) return n;
    std::vector< arlCore::Point::csptr >& camList = privateGetList(cam);
    std::vector< arlCore::Point::csptr >& tagList = privateGetList(cam, tag);
    std::vector< arlCore::Point::csptr >& frgList = privateGetList(cam, tag, fringe);
    for( i=0 ; i<ptList.size() ; ++i )
    {
        if(ptList[i]!=0)
        {
            arlCore::Point *p=new Point(*(ptList[i]));
            ++n;
            camList.push_back(p);
            if(tag!=0)
            {
                tagList.push_back(p);
                if(fringe!=0) frgList.push_back(p);
            }
        }
    }
    return n;
}

bool arlCore::SmartPointList::addPoint( const arlCore::Point& pt, unsigned int cam, void* tag, unsigned int tagNo )
{
    if(cam<1 || tag==0) return false;
    unsigned int i;
    std::vector< arlCore::Point::csptr > &pointList = privateGetList( cam, tag );
    for( i=(unsigned int)pointList.size() ; i<tagNo+1 ; ++i )
        pointList.push_back(0);
    if(pointList[tagNo]==0)
    {
        arlCore::Point *p=new Point(pt);
        privateGetList(cam).push_back(p);
        pointList[tagNo]=p;
    } else ((arlCore::Point*)pointList[tagNo])->copy(pt);
    return true;
}

const double MaxFringes=100.0, MaxCams=10.0, HalfMaxFringes=MaxFringes/2.0;

double arlCore::SmartPointList::hash( unsigned int cam, void* tag, int fringe ) const
{
    long int p=(long int)tag;
    double key=(double)p*MaxCams*MaxFringes+(HalfMaxFringes+(double)fringe)*MaxCams+(double)cam;
    unsigned int c;
    void* t;
    int f;
    assert(unhash( key, c, t, f ));
    assert(cam==c && tag==t && fringe==f);
    // FIXME Test only in Debug mode
    unhash( key, c, t, f );
    if(cam!=c || tag!=t || fringe!=f)
        std::cout<<"*****UNHASH ERROR***** "<<cam<<","<<tag<<","<<fringe<<" => "<<c<<","<<t<<","<<f<<" ****************\n";
    return key;
}

bool arlCore::SmartPointList::unhash( double key, unsigned int &cam, void* &tag, int &fringe ) const
{
    long int t = (long int)(key/(MaxCams*MaxFringes));
    tag = (void*)t;
    key = key-t*MaxCams*MaxFringes;
    int f = (int)(key/MaxCams);
    fringe = f-(int)HalfMaxFringes;
    cam = (int)key-f*(int)MaxCams;
    return true;
}

std::vector< arlCore::Point::csptr >& arlCore::SmartPointList::privateGetList( unsigned int cam, void* tag, int fringe )
{
    if(tag==0 && fringe==0)
    {
        assert(cam>0);
        if(cam>m_listsByCam.size()) m_listsByCam.resize(cam);
        assert(cam<=m_listsByCam.size());
        //unsigned int n = (unsigned int)m_listsByCam[cam-1].size();
        return m_listsByCam[cam-1];
    }
    unsigned int n;
    double key = hash(cam, tag, fringe);
    std::map< double, unsigned int, mapOrder >::iterator it;
    it = m_mapOfLists.find(key);
    if(it==m_mapOfLists.end())
    {
        n=(unsigned int)m_listOfLists.size();
        m_listOfLists.resize(n+1);
        m_mapOfLists[key]=n;
    } else n=it->second;
    assert(n<m_listOfLists.size());
    return m_listOfLists[n];
}

const std::vector< arlCore::Point::csptr >& arlCore::SmartPointList::getList( unsigned int cam, void* tag, int fringe ) const
{
    if(tag==0 && fringe==0 && cam>0 && cam <=m_listsByCam.size()) return m_listsByCam[cam-1];
    std::map< double, unsigned int, mapOrder >::const_iterator it;
    it = m_mapOfLists.find(hash(cam, tag, fringe));
    if(it!=m_mapOfLists.end()) return m_listOfLists[it->second];
    return m_listOfLists[0]; // Empty list
}

unsigned int arlCore::SmartPointList::getPointByCam( std::vector<arlCore::Point::csptr>&pl, const std::vector<arlCore::Camera> &cameras, void* tag, unsigned int no, std::vector< bool >&whichCams ) const
{
    std::vector<const arlCore::Camera*> cams;
    unsigned int i;
    for( i=0 ; i<cameras.size() ; ++i )
        cams.push_back(&cameras[i]);
    return getPointByCam( pl, cams, tag, no, whichCams );
}

unsigned int arlCore::SmartPointList::getPointByCam( std::vector<arlCore::Point::csptr>&pl, const std::vector<const arlCore::Camera*> &cams, void* tag, unsigned int no, std::vector< bool >&whichCams ) const
{
    pl.clear();
    whichCams.resize(cams.size());
    unsigned int cam;
    for( cam=0 ; cam<whichCams.size() ; ++cam )
    {
        whichCams[cam]=false;
        if(cams[cam]->isIntrinsicCalibrated())
        {
            const std::vector< arlCore::Point::csptr > &l=getList(cam+1,tag);
            if(no<l.size())
                if(l[no]!=0)
                    if(l[no]->isVisible())
                    {
                        whichCams[cam]=true;
                        pl.push_back((arlCore::Point*)l[no]);
                    }
        }
    }
    return (unsigned int)pl.size();
}

bool arlCore::SmartPointList::getPoint( arlCore::Point::csptr &p, unsigned int cam, void *t, unsigned int tagNo ) const
{
    if(cam<=0) return false;
    const std::vector< arlCore::Point::csptr > &l=getList(cam,t);
    if(tagNo>=l.size()) return false;
    p=l[tagNo];
    return p!=0;
}

unsigned int arlCore::SmartPointList::getPoints( std::vector< arlCore::Point::csptr >&pl, unsigned int cam, void* t ) const
{
    pl.clear();
    if(cam<=0) return 0;
    const std::vector< arlCore::Point::csptr > &l=getList(cam,t);
    unsigned int i;
    for( i=0 ; i<l.size() ; ++i )
        pl.push_back((arlCore::Point*)l[i]);
    return (unsigned int)pl.size();
}

unsigned int arlCore::SmartPointList::getPoints( arlCore::PointList &pl, unsigned int cam, void* t ) const
{
    pl.clear();
    if(cam<=0) return 0;
    const std::vector< arlCore::Point::csptr > &l=getList(cam,t);
    unsigned int i;
    for( i=0 ; i<l.size() ; ++i )
        pl.push_back((arlCore::Point)(*l[i]));
    return pl.size();
}

unsigned int arlCore::SmartPointList::getInvisiblePoints( std::vector< arlCore::Point::csptr >&pl, unsigned int cam, void* t ) const
{
    pl.clear();
    if(cam<=0) return 0;
    const std::vector< arlCore::Point::csptr > &l=getList(cam,t);
    unsigned int i;
    for( i=0 ; i<l.size() ; ++i )
        if(l[i]!=0)
            if(!l[i]->isVisible())
                pl.push_back((arlCore::Point*)l[i]);
    return (unsigned int)pl.size();
}

bool arlCore::SmartPointList::clear( void )
{
    unsigned int i,j;
    for( i=0 ; i<m_listsByCam.size() ; ++i )
        for( j=0 ; j<m_listsByCam[i].size() ; ++j )
            if(m_listsByCam[i][j]!=0) delete m_listsByCam[i][j];
    m_listsByCam.clear();
    m_mapOfLists.clear();
    m_listOfLists.clear();
    m_listOfLists.resize(1);
    return true;
}

unsigned int arlCore::SmartPointList::size( unsigned int cam, void* tag, int fringe ) const
{
    return (unsigned int)getList( cam, tag, fringe ).size();
}

unsigned int arlCore::epipolarMatching( const std::vector<Camera>& cameras, const SmartPointList &spl, std::vector< std::vector<arlCore::Point::csptr> >&matching, double gaussianNoise )
{
    unsigned int nbCameras = (unsigned int)cameras.size();
    if(nbCameras<2) return 0;
    if(nbCameras>2) nbCameras=2;
    // Bruit minimum lié aux incertitudes sur le calcul
    const double NumericNoise = 1e-3;
    // On retient que les points a une distance de la droite epipolaire < 'DistPixMax'
    // Cette valeur est fonction du bruit estimé sur les points 2D
    const double DistPixMax = 2*gaussianNoise+NumericNoise;
    //const double DistPixMax = sqrt(2*gaussianNoise*gaussianNoise)+NumericNoise;
    // Le second plus proche doit imperativement etre à plus de DistPixMin de la droite epipolaire
    // Les voisins doivent être à l'extérieur de la zone d'incertitude
    const double DistPixMin = DistPixMax;

    // Elagage des points trop eloignes de la droite epipolaire
    // Actuellement inutile car l'algo actuel prend en compte le premier et le second

    // On retient que les 'Best'*100% points les plus proches de la droite epipolaire
    const double Best = 0.1; //]0,1]
    // Garde au minimum les BestMin premiers quelque soit le pourcentage
    const unsigned int BestMin = 100;

    // Cas avec 2 cameras - TODO : avec n cameras
    // distanceTab[no point de la cam0][no point de la cam 1]
    // vnl_vector_fixed<double,4> ={
    // distance point cam1 a droite du point cam0 ,
    // distance du second plus proche dans la cam1,
    // distance point cam0 a droite du point cam1,
    // distance du second plus proche dans la cam0
    std::vector< std::vector< vnl_vector_fixed<double,4> > > distanceTab;
    const vnl_vector_fixed<double,4> InitVector(DBL_MAX);

    unsigned int i,j,k,l,n;
    Point point3D(3);
    // Passage dans le plan de la focale
    const bool PerfectDisto = true;
    std::vector< PointList > focalPlanePoints(cameras.size());
    std::vector< std::vector<arlCore::Point::csptr> >pixelPoints(cameras.size());
    for( i=0 ; i<nbCameras ; ++i )
    {
        n = spl.getPoints( pixelPoints[i], i+1 );
        // Cas avec 2 cameras - TODO : avec n cameras
        if(i==0) distanceTab.resize(n);
        if(i==1)
            for( j=0 ; j<distanceTab.size() ; ++j )
            {
                distanceTab[j].resize(n);
                for( k=0 ; k<n ; ++k )
                    distanceTab[j][k]=InitVector;
            }
        for( j=0 ; j<n ; ++j )
        {
            ((Point::sptr)pixelPoints[i][j])->setScalar(-1); // Numero appariement pour visu
            if(cameras[i].pixelPlaneToUnitFocalPlane( *(pixelPoints[i][j]), point3D, PerfectDisto ))
                focalPlanePoints[i].push_back(point3D);
        }
    }
    // Calcul des droites epipolaires et des distances
    // Constitution du graphe des appariements potentiels
    double distance;
    for( i=0 ; i<nbCameras ; ++i )
        for( j=0 ; j<nbCameras ; ++j )
            if(i!=j /*&& cameras[i]!=0 && cameras[j]!=0*/ )
            {
                vnl_matrix_fixed<double,4,4> PMi_j = cameras[j].getExtrinsic() * cameras[i].getInvExtrinsic();
                for( k=0 ; k<focalPlanePoints[i].size() ; ++k )
                {
                    if(focalPlanePoints[j].size()>0)
                    {
                        std::vector< std::pair<double,unsigned int> > distances(focalPlanePoints[j].size());
                        for( l=0 ; l<focalPlanePoints[j].size() ; ++l )
                        {
                            double a,b,c;
                            if(getEpipolar(*(focalPlanePoints[i][k]), PMi_j, a, b, c))
                            {
                                distance = (a * focalPlanePoints[j][l]->x() + b * focalPlanePoints[j][l]->y() + c) / sqrt(a*a +b*b);
                                distance = sqrt(distance*distance)*cameras[j].getfx();
                                distances[l]=std::make_pair(distance,l);
                            }else distances[l]=std::make_pair(DBL_MAX,l);
                        }
                        std::sort( distances.begin(), distances.end() );
                        // On retient les Best*100% meilleurs points
/*                      for( l=0 ; l<distances.size() ; ++l )
                            if(l+1>BestMin && (l+1)/distances.size()>Best)
                                distances[l].first = DBL_MAX;
*/                      // Cas avec 2 cameras - TODO : avec n cameras                       
                        if(i<2 && j<2)
                        {
                            unsigned int cam0,cam1,index;
                            if(i==0) {cam0=k; cam1=distances[0].second; index=0;}
                            if(i==1) {cam1=k; cam0=distances[0].second; index=2;}
                            distanceTab[cam0][cam1](index)=distances[0].first;
                            if(distances.size()>1)
                                distanceTab[cam0][cam1](index+1)=distances[1].first;
                            else distanceTab[cam0][cam1](index+1)=DBL_MAX;
                            // Creer un arc de pixelPoints[i][k] vers pixelPoints[j][distance[l].second]
                            // avec comme poids distance[l].first
                        }
                    }
                }
            }
    // Elimination pour chaque point de chaque camera des liens multiples vers une autre meme camera
    // Elimination prioritaire des points ayant le moins bon score
    matching.clear();
    for( i=0 ; i<distanceTab.size() ; ++i )
    {   // Appariement Point i de la camera 0
        double maxSecondDistance = 0; // Indice de confiance
        int chosenPoint = -1;
        for( j=0 ; j<distanceTab[i].size() ; ++j )
        {   
            double minDistofsecond, meanDistToLine = (distanceTab[i][j](0)+distanceTab[i][j](2))/2;
            if(distanceTab[i][j](1) < distanceTab[i][j](3))
                minDistofsecond = distanceTab[i][j](1);
            else minDistofsecond = distanceTab[i][j](3);
            if(meanDistToLine<=DistPixMax)
                if(minDistofsecond>DistPixMin && minDistofsecond>maxSecondDistance)
                {
                    maxSecondDistance=minDistofsecond;
                    chosenPoint = j;
                }
        }
        if(chosenPoint>=0)
        {
            matching.resize(matching.size()+1);
            matching.back().push_back(pixelPoints[0][i]);
            matching.back().push_back(pixelPoints[1][chosenPoint]);
        }
    }
    // TODO Trier matching en fonction de la confiance : Les plus surs en premiers.
    return (unsigned int)matching.size();
}
