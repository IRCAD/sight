/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/PlaneSystem.h>

#include <fstream>
#include <algorithm>
#include <math.h>

#include <arlcore/Misc.h>

const unsigned int arlCore::PlaneSystem::NoPlane = 0;

arlCore::PlaneSystem::PlaneSystem( const std::string &name ):
Object(ARLCORE_CLASS_PLANESYSTEM, name)
{
    setOK(true);
}

arlCore::PlaneSystem::~PlaneSystem( void )
{
    m_trfTable.clear();
    m_trfState.clear();
    m_trfWeight.clear();
    m_lstName.clear();
    m_status.clear();
}

bool arlCore::PlaneSystem::setPlaneName( unsigned int plane, const std::string &name )
{
//  assert(!outOfRange(plane));
//  if(outOfRange(plane)) return false;
    assert(plane>0 && plane<=m_lstName.size());
    if(plane<1 || plane>m_lstName.size()) return false;
    m_lstName[plane-1]=name;
    Object::update();
    return true;
}

std::string arlCore::PlaneSystem::getString( void ) const   
{
    std::stringstream s;
    unsigned int i, j, index;
    s<<Object::getString();
    s<<"   ";
    for( i=0 ; i<m_status.size() ; ++i )
        if(m_status[i]) s<<" "<<i+1<<" "; else s<<"X"<<i+1<<"X";
    s<<"\n";
    for( i=0 ; i<m_status.size() ; ++i )
    {
        if(m_status[i]) s<<" "<<i+1<<" "; else s<<"X"<<i+1<<"X";
            for( j=0 ; j<m_lstName.size() ; ++j )
            {
                index=getIndex(j+1,i+1);
                switch(m_trfState[index])
                {
                case STATE_UNDEFINED:   s<<" ? ";break;
                case STATE_SET:         s<<" O ";break;
                case STATE_CALIBRATION: s<<" C ";break;
                case STATE_IDENTITY:    s<<" 1 ";break;
                case STATE_COMPUTED:    s<<" = ";break;
                }
            }
        s<<"\n";
    }
    // TODO : Fill the planeSystem description
    return s.str();
}

bool arlCore::PlaneSystem::load( const std::string &fileName )
{
    if(fileName=="") return false;
    //Object::update();
    return false;
}

bool arlCore::PlaneSystem::save( const std::string &fileName, bool overwrite ) const
{
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    // TODO : Save the planesystem in an appropriate format
    return false;
}

unsigned int arlCore::PlaneSystem::printGraph( void ) const
{
    unsigned int n=saveGraph(GRAPHVIZ_TMP);
    if(n>0)
    {
        std::string exe = GRAPHVIZ_EXE;
        std::string tmp = GRAPHVIZ_TMP;
        std::string cmd = exe + " ";
        cmd = cmd +tmp;
        system(cmd.c_str());
    }
    return n;
}

unsigned int arlCore::PlaneSystem::saveGraph( const std::string &name ) const
{
    const std::string ARL_PLANE_STATE_NAMES[NBSTATES]={ "Undefined","Identity","Calibration","Measure","Computed" };
    long int date, time;
    getTime(date, time);
    std::fstream s;
    s.open (name.c_str(), std::fstream::out);
    unsigned int i,j,size=(unsigned int)m_lstName.size();
    s<<"/*\n";
    s<<" PlaneSystem Graph - Use Graphviz for display it\n";
    s<<"http://www.graphviz.org\n";
    s<<"*/\n";
    s<<"digraph \""<<getName()<<"\" {labelloc =\"t\" label = \""<<getName()<<" at "<<date<<"-"<<time<<"\" node [fontname = \"Arial\" label = \"\\N\" shape = \"circle\" width = \"0.50000\" height = \"0.500000\" color = \"black\"]\n";
    for( i=0 ; i<size ; ++i )
    {
        if(m_status[i])
        {
            s<<"\""<<i+1<<"\\n"<<m_lstName[i]<<"\"[style=filled, fillcolor=green];\n";
            for( j=0 ; j<size ; ++j )
            {
                if(m_status[j] && j!=i)
                {
                    const unsigned int Index = getIndex(i+1, j+1);
                    const unsigned int State = m_trfState[Index];
                    if( State!=STATE_UNDEFINED )
                    {
                        const double Weight = m_trfWeight[Index];
                        if(m_trfTable[Index].isEquivalent(date,time)) s<<"edge [color = \"black\"]\n";
                        else s<<"edge [color = \"yellow\"]\n";
                        s<<"\""<<i+1<<"\\n"<<m_lstName[i]<<"\" -> \""<<j+1<<"\\n"<<m_lstName[j]<<"\"";
                        s<<" [label=\""<<ARL_PLANE_STATE_NAMES[State]<<" Weight="<<Weight<<" "<<m_trfTable[Index].getText()<<"\"];\n";
                    }
                }
            }
        } else s<<"\""<<i+1<<"\\n"<<m_lstName[i]<<"\"[style=filled, fillcolor=red];\n";
    }
    s<<"\n}\n";
    s.close();
    return size;
}

bool arlCore::PlaneSystem::getPlaneID( std::string name, unsigned int &ID ) const
{
    unsigned int i;
    for( i=0 ; i<m_lstName.size() ; ++i )
        if(m_lstName[i]==name && m_status[i])
        {
            ID=i+1;
            return true;
        }
    return false;
}

bool arlCore::PlaneSystem::getPlaneName( unsigned int ID, std::string &name) const
{
    assert(!outOfRange(ID));
    if(outOfRange(ID)) return false;
    name = m_lstName[ID-1];
    return true;
}

unsigned int arlCore::PlaneSystem::add3DPlane ( const Object &o )
{   // ID [1,..[
    return add3DPlane( o.getName() );
}

unsigned int arlCore::PlaneSystem::add3DPlane ( const std::string &name )
{   // ID [1,..[
    const bool FillReleasedPlanes = true;
    unsigned int plane = 0;
    Object::update();
    if(FillReleasedPlanes && m_releasedPlanes.size()>0)
    {
        plane = m_releasedPlanes.back();
        m_releasedPlanes.pop_back();
        m_lstName[plane-1] = name;
        m_status[plane-1] = true;
    }else
    {
        m_lstName.push_back(name);
        m_status.push_back(true);
        unsigned int i;
        plane = (unsigned int)m_lstName.size();
        m_trfTable.resize(plane*plane);
        for( i=(plane-1)*(plane-1) ; i<m_trfTable.size() ; ++i)
        {
            m_trfState.push_back(STATE_UNDEFINED);
            m_trfWeight.push_back(-1);
        }
    }
    assert(m_trfTable.size()==m_trfState.size());
    assert(m_trfTable.size()==m_trfWeight.size());
    return plane;
}

bool arlCore::PlaneSystem::getPlaneStatus( unsigned int plane ) const
{
    if(plane<1 || plane>m_status.size()) return false;
    return m_status[plane-1];
}

unsigned int arlCore::PlaneSystem::getNbPlanes( void ) const
{
    /*
    unsigned int i, n=0;
    for( i=0 ; i<m_status.size() ; ++i )
        if(m_status[i]) ++n
    return n;
    */
    return (unsigned int)m_lstName.size();
}

bool arlCore::PlaneSystem::getTrf( unsigned int plane1, unsigned int plane2, vnl_rigid_matrix &T, bool verbose )
{
//  verbose = false;
    // TODO : Manage the cases when there are many paths to reach plane2
    // TODO : Prendre uniquement le chemin le plus récent, le plus court, celui de poids minimum
    // TODO : Ne pas poursuivre dans les chemins incompatible avec la date
//  assert(!outOfRange(plane1, plane2));
    if(outOfRange(plane1, plane2))
    {
        T.setIdentity();
        T.setTime(0, 0);
        return false;
    }
//  Object::update(); // ?
    const long int Date = 0, Time = 0; // FIXME
    if(plane1 != plane2)
    {
        if(isConnected( plane1, plane2, Date, Time, true, false )) 
        {
            if(verbose) std::cout<<"From "<<plane1<<" to "<<plane2<<"\n";
            T = m_trfTable[getIndex(plane1, plane2)];
            return true;
        }
        //std::vector< unsigned int >path;
        PlaneSystem::Path path(*this);
        bool b=findPath( plane1, plane2, path, Date, Time );
        if(b)
        {
            if(verbose) path.print();
            setTrf(path);
            T = m_trfTable[getIndex(plane1, plane2)];
        }else
        {
            T.setIdentity();
            T.setTime(0, 0);
        }
        untagAll();
        return b;
    }
    T.setIdentity();
    T.setTime(getDate(), getTime());
    return true;
}

bool arlCore::PlaneSystem::getTrf( unsigned int plane1, unsigned int plane2, vnl_rigid_matrix &T, long int date, long int time, double ageTolerance, bool verbose )
{
    if(!getTrf(plane1, plane2, T, verbose)) return false;
    long int d,t;
    T.getTime(d,t);
    if(date!=d) return false;
    return fabs((double)(time-t))<=ageTolerance*10;
}

bool arlCore::PlaneSystem::getTrf( unsigned int plane1, unsigned int plane2, vnl_rigid_matrix &T, double ageTolerance, bool verbose )
{
    if(!getTrf(plane1, plane2, T, verbose)) return false;
    if(ageTolerance<0.0) return true;
    long int d,t;
    T.getTime(d,t);
    if(d==0 && t==0) return true;
    if(getDate()!=d) return false;
    return fabs((double)(getTime()-t))<=ageTolerance*10;
}

bool arlCore::PlaneSystem::setTrf( unsigned int index, const vnl_rigid_matrix& T, long int date, long int time )
{
    const double CALIBRATION_WEIGHT = 1;
    const double MEASURE_WEIGHT = 5;

    assert( index<m_trfTable.size() );
    Object::update();
    m_trfTable[index].copy(T);
    m_trfTable[index].setTime(date,time);
    if(date==0 && time==0)
    {   // TODO : Calculez la transfo inverse lorsque date=0 et time=0 : Validité permanente
        m_trfState[index] = STATE_CALIBRATION;
        m_trfWeight[index] = CALIBRATION_WEIGHT;
    }else
    {   // TODO : Calculez la transfo inverse en même temps ?
        m_trfState[index] = STATE_SET;
        m_trfWeight[index] = MEASURE_WEIGHT;
    }
    eraseComputedTrf(index);
    return true;
}

bool arlCore::PlaneSystem::setTrf( unsigned int plane1, unsigned int plane2, const vnl_rigid_matrix& T )
{
    assert(!outOfRange(plane1, plane2) && plane1!=plane2);
    if(outOfRange(plane1, plane2) || plane1==plane2) return false;
    m_trfState[getIndex(plane2, plane1)] = STATE_UNDEFINED;
    return setTrf(getIndex(plane1, plane2), T, T.getDate(), T.getTime());
}

bool arlCore::PlaneSystem::setTrf( unsigned int plane1, unsigned int plane2, const vnl_rigid_matrix& T, long int date, long int time )
{
    assert(!outOfRange(plane1, plane2) && plane1!=plane2);
    if(outOfRange(plane1, plane2) || plane1==plane2) return false;
    m_trfState[getIndex(plane2, plane1)] = STATE_UNDEFINED;
    return setTrf(getIndex(plane1, plane2), T, date, time);
}

bool arlCore::PlaneSystem::resetTrf( unsigned int plane1, unsigned int plane2 )
{
    assert(!outOfRange(plane1, plane2) && plane1!=plane2);
    if(outOfRange(plane1, plane2) || plane1==plane2) return false;
    unsigned int i = getIndex(plane1, plane2);
    Object::update();
    m_trfState[i] = STATE_UNDEFINED;
    m_trfState[getIndex(plane2, plane1)] = STATE_UNDEFINED;
    eraseComputedTrf(i);
    return true;
}

bool arlCore::PlaneSystem::resetTrf( unsigned int plane )
{
    assert(!outOfRange(plane));
    if(outOfRange(plane)) return false;
    Object::update();
    unsigned int i;
    for( i=0 ; i<m_status.size() ; ++i )
        if(m_status[i])
        {
            m_trfState[getIndex(plane,i+1)] = STATE_UNDEFINED;
            m_trfState[getIndex(i+1,plane)] = STATE_UNDEFINED;
        }
    // FIXME eraseComputedTrf(?);
    return true;
}

bool arlCore::PlaneSystem::setIdentity ( unsigned int plane1, unsigned int plane2, const long int date, const long int time )
{
    if(outOfRange(plane1, plane2) || plane1==plane2) return false;
    assert(!outOfRange(plane1, plane2) && plane1!=plane2);
    unsigned int index;
    vnl_rigid_matrix T;
    T.setIdentity();
    T.setTime(date, time);
    index = getIndex(plane1, plane2);
    eraseComputedTrf(index);
    Object::update();
    m_trfTable[index].copy(T);
    m_trfState[index] = STATE_IDENTITY;
    m_trfWeight[index] = 0;
    index = getIndex(plane2, plane1);
    m_trfTable[index].copy(T);
    m_trfState[index] = STATE_IDENTITY;
    m_trfWeight[index] = 0;
    return true;
}

void arlCore::PlaneSystem::eraseComputedTrf( unsigned int index )
{   //TODO : [Optimisation] Passer à STATE_UNDEFINED uniquement les transfos déduites de la transfo[index] modifiée
    unsigned int i;
    for( i=0 ; i<m_trfState.size() ; ++i )
        if(m_trfState[i]==STATE_COMPUTED || m_trfState[i]==STATE_COMPUTED+NBSTATES)
            m_trfState[i] = STATE_UNDEFINED;
}

bool arlCore::PlaneSystem::releasePlane( unsigned int plane )
{
    if(!resetTrf( plane )) return false;
    m_status[plane-1]=false;
    m_releasedPlanes.push_back(plane);
    return true;
}

bool arlCore::PlaneSystem::distance( unsigned int plane1, unsigned int plane2, double &dist/*, long int &date, long int &time*/ )
{
    dist=0;
    vnl_rigid_matrix T;
    if(!getTrf ( plane1, plane2, T/*, date, time*/ )) return false; // FIXME
    dist=sqrt(T(0,3)*T(0,3)+T(1,3)*T(1,3)+T(2,3)*T(2,3));
//  date = T.getDate();
//  time = T.getTime();
    return true;
}

bool arlCore::PlaneSystem::chgPlane( unsigned int plane1, const Point &pt1, unsigned int plane2, Point &pt2)
{   // Set pt2 with the coordinates in Plane2 of pt1 set in Plane1
    arlCore::vnl_rigid_matrix T;
    if(!getTrf( plane1, plane2, T)) return false;
    return T.trf(pt1,pt2);
}

bool arlCore::PlaneSystem::getOrigin( unsigned int plane1, unsigned int plane2, Point &pt)
{   // Set pt with the origin of Plane1 in the Plane2
    arlCore::vnl_rigid_matrix T;
    if(!getTrf( plane1, plane2, T)) return false;
    arlCore::Point origin(0.0, 0.0, 0.0);
    return T.trf(origin, pt);
}

// PRIVATE FUNCTIONS
bool arlCore::PlaneSystem::outOfRange( unsigned int plane1, unsigned int plane2 ) const
{   // True if plane 1 or 2 is out of range
    return (outOfRange(plane1) || outOfRange(plane2));
}

bool arlCore::PlaneSystem::outOfRange( unsigned int planeNo ) const
{   // ID=[1,n]
    if(planeNo<1 || planeNo>m_lstName.size()) return true;
    return !m_status[planeNo-1];
}

unsigned int arlCore::PlaneSystem::getIndex( unsigned int plane1, unsigned int plane2 ) const
{   // Organisation concentrique des index :
    //    |  1  2  3  4 <- plane1
    //    |===================
    //  1 | 00 01 04 09 ...
    //  2 | 03 02 05 10 ...
    //  3 | 08 07 06 11 ...
    //  4 | 15 14 13 12 ...
    //  ^plane2
    assert(!outOfRange(plane1, plane2));
    unsigned int index;
    if(plane2<=plane1)
        index = (plane1-1)*(plane1-1)+(plane2-1);
    else
        index = (plane2*plane2)-plane1;
    assert(index<m_trfState.size());
    assert(index==getInvIndex(getInvIndex(index)));
    return index;
}

unsigned int arlCore::PlaneSystem::getInvIndex( unsigned int index ) const
{
    unsigned int alpha = (unsigned int)sqrt((double)index);
    ++alpha;
    const unsigned int Diagonal = alpha*alpha-alpha;
    return index+2*(Diagonal-index);
}

void arlCore::PlaneSystem::tag( unsigned int plane1, unsigned int plane2 )
{
    assert(!outOfRange(plane1) && !outOfRange(plane2));
    const unsigned int Index = getIndex(plane1, plane2);
    if(!isTagged(Index))
    {
        //Object::update();
        m_trfState[Index] = (ARL_PLANE_STATE)(m_trfState[Index] + NBSTATES);
    }
    const unsigned int InvIndex = getIndex(plane2, plane1);
    if(!isTagged(InvIndex))
    {
        //Object::update();
        m_trfState[InvIndex] = (ARL_PLANE_STATE)(m_trfState[InvIndex] + NBSTATES);
    }
}

arlCore::PlaneSystem::ARL_PLANE_STATE arlCore::PlaneSystem::getStatus( unsigned int plane1, unsigned int plane2 ) const
{
    assert(!outOfRange(plane1) && !outOfRange(plane2));
    return getStatus( getIndex(plane1, plane2) );
}

arlCore::PlaneSystem::ARL_PLANE_STATE arlCore::PlaneSystem::getStatus( unsigned int index ) const
{
    assert( index<m_trfState.size() );
    if(!isTagged(index)) return m_trfState[index];
    return(ARL_PLANE_STATE)(m_trfState[index] - NBSTATES);
}

void arlCore::PlaneSystem::untag( unsigned int plane1, unsigned int plane2 )
{
    assert(!outOfRange(plane1) && !outOfRange(plane2));
    const unsigned int Index = getIndex(plane1, plane2);
    if(isTagged(Index))
    {
        //Object::update();
        m_trfState[Index] = (ARL_PLANE_STATE)(m_trfState[Index] - NBSTATES);
    }
    const unsigned int InvIndex = getIndex(plane1, plane2);
    if(isTagged(InvIndex))
    {
        //Object::update();
        m_trfState[InvIndex] = (ARL_PLANE_STATE)(m_trfState[InvIndex] - NBSTATES);
    }
}

bool arlCore::PlaneSystem::isTagged( unsigned int index ) const
{
    assert( index<m_trfState.size() );
    return( m_trfState[index]>=NBSTATES );
}

bool arlCore::PlaneSystem::isTagged( unsigned int plane1, unsigned int plane2 ) const
{
    assert(!outOfRange(plane1) && !outOfRange(plane2));
    return isTagged( getIndex(plane1, plane2) );
}

void arlCore::PlaneSystem::untagAll( void )
{
    unsigned int i;
    bool b = false;
    for( i=0 ; i<m_trfState.size() ; ++i )
        if(m_trfState[i]>=NBSTATES)
        {
            b = true;
            m_trfState[i] = (ARL_PLANE_STATE)(m_trfState[i] - NBSTATES);
        }
    //if(b) Object::update();
}

typedef std::pair<unsigned int, double> PlaneWeight;
bool sortPlanes(const PlaneWeight& left, const PlaneWeight& right)
{   // Ordre croissant des poids
    return left.second < right.second;
}

bool arlCore::PlaneSystem::findPath( unsigned int plane1, unsigned int plane2, Path &path, long int date, long int time )
{
    assert(!outOfRange(plane1) && !outOfRange(plane2));
    std::vector<PlaneWeight> Who;
    unsigned int i, n = whoIsConnected(plane1, plane2, Who, date, time, true, true);
    if(n==0) return false;
    path.push_back(plane1);
    for( i=0 ; i<n ; ++i )
    {
        if(Who[i].first==plane2)
        {
            path.push_back(plane2);
            return true;
        }
        if(findPath(Who[i].first,plane2,path,date,time)) return true;
    }
    path.pop_back();
    return false;
}

bool arlCore::PlaneSystem::setTrf( const Path &path )
{
    unsigned int i, n=path.size();
    if(n<3) return false;
//  Object::update();
    vnl_rigid_matrix T = m_trfTable[getIndex(path[0], path[1])];
    arlCore::Object O;
    O.setTime(T.getDate(), T.getTime());
    double RMSMax = T.getRMS();
    double stdDevMax = T.getStdDev();
    for( i=1 ; i<n-/*2*/1 ; ++i )
    {
        const vnl_rigid_matrix& M = m_trfTable[getIndex(path[i], path[i+1])];
        O.setMinTime( M.getDate(), M.getTime() );
        T.mult(M,T);
        if(M.getStdDev()>stdDevMax) stdDevMax = M.getStdDev();
        if(M.getRMS()>RMSMax) RMSMax = M.getRMS();
    }
/*  const vnl_rigid_matrix& M = m_trfTable[getIndex(path[n-2], path[n-1])];
    O.setMinTime( M.getDate(), M.getTime() );
    T.mult(M,T);
    if(M.getStdDev()>stdDevMax) stdDevMax = M.getStdDev();
    if(M.getRMS()>RMSMax) RMSMax = M.getRMS();*/
    T.setRMS(RMSMax);
    T.setStdDev(stdDevMax);
    const unsigned int Index = getIndex(path[0],path[n-1]);
    m_trfTable[Index].copy(T);
    m_trfTable[Index].setTime(O.getDate(), O.getTime());
    m_trfState[Index] = STATE_COMPUTED;
    m_trfWeight[Index] = path.weight();
    return true;
}

unsigned int arlCore::PlaneSystem::whoIsConnected( unsigned int connectedAt, unsigned int finalDestination, std::vector<PlaneWeight> &planes, long int date, long int time, bool SetInv, bool ToTag )
{
    assert(!outOfRange(connectedAt, finalDestination));
    planes.clear();
/*  if(isConnected(connectedAt, finalDestination, date, time, SetInv, ToTag))
    {
        planes.push_back(PlaneWeight(finalDestination, getWeight(connectedAt, finalDestination)));
        return 1;
    }*/
    unsigned int i;
    bool sort = false;
    double firstWeight = -1;
    for( i=1 ; i<=m_status.size() ; ++i )
        if(/*i!=finalDestination && */m_status[i-1])
            if(isConnected(connectedAt, i, date, time, SetInv, ToTag))
            {
                const double Weight = getWeight(connectedAt, i);
                planes.push_back(PlaneWeight(i, Weight));
                if(!sort)
                {// Sort weights only if it exists at least two different weights
                    if(firstWeight<0) firstWeight = Weight;
                    else if(Weight!=firstWeight) sort = true;
                }
            }
    if(sort) std::sort(planes.begin(), planes.end(), sortPlanes);
    return (unsigned int)planes.size();
}

bool arlCore::PlaneSystem::isConnected( unsigned int plane1, unsigned int plane2, long int date, long int time, bool SetInv, bool ToTag )
{
    assert(!outOfRange(plane1) && !outOfRange(plane2));
    if(outOfRange(plane1, plane2)) return false;
    if(plane1==plane2 || isTagged(plane1,plane2) || isTagged(plane2,plane1)) return false;
    unsigned int index = getIndex(plane1, plane2);
    if(getStatus(index)!=STATE_UNDEFINED && getStatus(index)!=STATE_COMPUTED/* && m_trfTable[index].isEquivalent(date,time)*/)
    {
        if(ToTag) tag(plane1, plane2);
        return true;
    }
    const unsigned int InvIndex = getIndex(plane2, plane1);
    if(getStatus(InvIndex)!=STATE_UNDEFINED && getStatus(InvIndex)!=STATE_COMPUTED/* && m_trfTable[inv].isEquivalent(date,time)*/)
    {
        if(SetInv)
        {
//          Object::update();
            if(m_trfTable[index].invert(m_trfTable[InvIndex]))
            {
                m_trfState[index] = getStatus(InvIndex);
                m_trfWeight[index] = getWeight(InvIndex);
            }
        }
        if(ToTag) tag(plane1,plane2);
        return true;
    }
    return false;
}

// PATH IN PLANESYSTEM
arlCore::PlaneSystem::Path::Path( const PlaneSystem& universe ):
m_universe(universe),
m_weight(0.0)
{}

arlCore::PlaneSystem::Path::Path( const Path& p ):
m_universe(p.m_universe),
m_weight(p.m_weight)
{
    unsigned int i;
    for( i=0 ; i<m_planes.size() ; ++i )
        m_planes.push_back(p.m_planes[i]);
    for( i=0 ; i<m_weights.size() ; ++i )
        m_weights.push_back(p.m_weights[i]);
}

arlCore::PlaneSystem::Path::~Path( void )
{}

unsigned int arlCore::PlaneSystem::Path::operator[]( unsigned int i ) const
{
    if( i<m_planes.size() )return m_planes[i];
    return 0;
}

void arlCore::PlaneSystem::Path::print( void ) const
{
    if(size()>0)
    {
        unsigned int i;
        std::cout<<"From "<<m_planes[0]<<" to "<<m_planes.back()<<"\n";
        std::cout<<m_planes[0];
        for( i=1 ; i<m_planes.size() ; ++i )
            std::cout<<" -> ("<<m_weights[i-1]<<") -> "<<m_planes[i];
        std::cout<<"\nSize = "<<size()<<" ; Weight = "<<weight()<<"\n";
    }else std::cout<<"Empty path\n";
}

unsigned int arlCore::PlaneSystem::Path::size( void ) const
{
    return (unsigned int)m_planes.size();
}

double arlCore::PlaneSystem::Path::weight( void ) const
{
    return m_weight;
}

double arlCore::PlaneSystem::getWeight( unsigned int index ) const
{
    assert(index<m_trfWeight.size());
    return m_trfWeight[index];
}

double arlCore::PlaneSystem::getWeight( unsigned int plane1, unsigned int plane2 ) const
{
    const double DefaultWeight = 1000.0;
    unsigned int index = getIndex(plane1, plane2);
    if(getStatus(index)!=STATE_UNDEFINED) return getWeight(index);
    index = getIndex(plane2, plane1);
    if(getStatus(index)!=STATE_UNDEFINED) return getWeight(index);
    return DefaultWeight;
}

unsigned int arlCore::PlaneSystem::Path::push_back( unsigned int plane )
{
    // TODO Calcul Weight de m_planes.back() -> plane dans m_universe
    m_planes.push_back(plane);
    if(size()==1) return 1;
    const unsigned int Previous = m_planes[size()-2];
    const double Weight = m_universe.getWeight(Previous, plane);
    m_weights.push_back(Weight);
    m_weight += Weight;
    return (unsigned int)m_planes.size();
}

bool arlCore::PlaneSystem::Path::pop_back( void )
{
    if(size()<1) return false;
    m_planes.pop_back();
    if(size()>0)
    {
        double weight = m_weights.back();
        m_weights.pop_back();
        m_weight -= weight;
    }
    return true;
}

// TRANSFORMATION FILTER
arlCore::TransformationFilter::TransformationFilter( const PlaneSystem& universe, long int duration, ARLCORE_TRF_FILTER_TYPE type ):
m_universe(universe),
m_duration(duration*10), //ms
m_filterType(type),
m_lastTime(0)
{}

arlCore::TransformationFilter::TransformationFilter( const TransformationFilter& TF ):
m_universe(TF.m_universe)
{
    unsigned int i;
    m_lastTime = TF.m_lastTime;
    m_duration = TF.m_duration;
    m_filterType = TF.m_filterType;
    for( i=0 ; i<TF.m_trfList.size() ; ++i )
        push_back(*TF.m_trfList[i]);
}

arlCore::TransformationFilter::~TransformationFilter( void )
{
    clear();
}

void arlCore::TransformationFilter::clear( void )
{
    unsigned int i;
    for( i=0 ; i<m_trfList.size() ; ++i )
        delete(m_trfList[i]);
    m_trfList.clear();
    m_lastTime = 0;
}

void arlCore::TransformationFilter::push_back( const vnl_rigid_matrix& T )
{   // Verifier continuite & croissance temporelle
    if(T.getTime()<m_lastTime) clear();
    m_lastTime = T.getTime();
    vnl_rigid_matrix* M = new vnl_rigid_matrix(T);
    m_trfList.push_back(M);
}

unsigned int arlCore::TransformationFilter::get( arlCore::vnl_rigid_matrix& T )
{
    timeCleaning();
    return filter( m_trfList, m_filterType, T );
}

void arlCore::TransformationFilter::timeCleaning( bool all )
{
    std::vector<const arlCore::vnl_rigid_matrix*>::iterator it=m_trfList.begin();
//  const long int date = m_universe.getDate();
    const long int time = m_universe.getTime() - m_duration;
    while( it!=m_trfList.end() )
    {
        if( (*it)->getTime()<time )
        {
            delete( *it );
            m_trfList.erase(it);
            ++it;
        }else if(!all) return;
    }
}
