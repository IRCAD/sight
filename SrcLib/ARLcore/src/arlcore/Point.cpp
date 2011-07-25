/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Point.h>

#include <fstream>
#include <iomanip>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_cross_product_matrix.h>

#include <arlcore/Misc.h>
#include <arlcore/PointsList.h>

arlCore::Point::Point( unsigned int dim, ARLCORE_POINT_TYPE type, long int date, long int time ):
Object(ARLCORE_CLASS_POINT),
m_type(type),
m_confidence(0),
m_visibility(true),
m_status(ARLCORE_POINT_STATUS_UNKNOWN),
m_isColored(false),
m_colour(0,0,0),
m_scalar(-1.0),
m_ponderation(1.0),
m_error(-1.0)
{
    unsigned int i;
    setOK(true);
    setTime(date,time);
    m_coordinates.set_size(dim);
    m_coordinates.fill(0.0);
    m_stat.resize(size());
    for( i=0 ; i<size() ; ++i )
        m_stat[i].fill(0.0);
    initUncertainty();
}

arlCore::Point::Point( double x, double y, ARLCORE_POINT_TYPE type, long int date, long int time ):
Object(ARLCORE_CLASS_POINT),
m_type(type),
m_confidence(0),
m_visibility(true),
m_status(ARLCORE_POINT_STATUS_UNKNOWN),
m_isColored(false),
m_colour(0,0,0),
m_scalar(-1.0),
m_ponderation(1.0),
m_error(-1.0)
{
    unsigned int i;
    setTime(date,time);
    m_coordinates.set_size(2);
    m_coordinates.put(0,x);
    m_coordinates.put(1,y);
    m_stat.resize(size());
    for( i=0 ; i<size() ; ++i )
        m_stat[i].fill(0.0);
    initUncertainty();
    setOK(true);
}

arlCore::Point::Point( double x, double y, double z, ARLCORE_POINT_TYPE type, long int date, long int time ):
Object(ARLCORE_CLASS_POINT),
m_type(type),
m_confidence(0),
m_visibility(true),
m_status(ARLCORE_POINT_STATUS_UNKNOWN),
m_isColored(false),
m_colour(0,0,0),
m_scalar(-1.0),
m_ponderation(1.0),
m_error(-1.0)
{
    unsigned int i;
    setTime(date,time);
    m_coordinates.set_size(3);
    m_coordinates.put(0,x);
    m_coordinates.put(1,y);
    m_coordinates.put(2,z);
    m_stat.resize(size());
    for( i=0 ; i<size() ; ++i )
        m_stat[i].fill(0.0);
    initUncertainty();
    setOK(true);
}

void arlCore::Point::init( unsigned int dim )
{
    Object::update();
    long int date=0,time=0;
    unsigned int i;
    setTime(date,time);
    m_type=ARLCORE_POINT_TYPE_UNKNOWN;
    m_confidence=0;
    m_visibility=true;
    m_status=ARLCORE_POINT_STATUS_UNKNOWN;
    m_isColored=false;
    m_colour.setColour(0,0,0);
    m_scalar=-1.0;
    m_coordinates.set_size(dim);
    m_coordinates.fill(0.0);
    m_covMatrix.set_size(dim,dim);
    m_covMatrix.fill(0.0);
    m_ponderation=1.0;
    m_stat.resize(size());
    for( i=0 ; i<size() ; ++i )
        m_stat[i].fill(0.0);
    m_error=-1.0;
}

arlCore::Point::Point( const Point& p ):
Object(ARLCORE_CLASS_POINT)
{
    copy(p);
}

arlCore::Point& arlCore::Point::operator=(const Point& p)
{
    copy(p);
    return *this;
}

void arlCore::Point::copy(const Point& p)
{
    if(this==&p) return;
    arlCore::Object *a=this;
    const arlCore::Object *b=&p;
    *a = *b;
    unsigned int i,j,dim=p.size();
    m_type = p.m_type;
    m_confidence = p.m_confidence;
    m_visibility = p.m_visibility;
    m_status = p.m_status;
    m_isColored = p.m_isColored;
    m_colour = p.m_colour;
    m_scalar = p.m_scalar;
    m_ponderation = p.m_ponderation;
    m_stat = p.m_stat;
    m_error = p.m_error;
    m_coordinates.set_size(dim);
    m_stat.resize(size());
    for( i=0 ; i<size() ; ++i )
    {
        m_coordinates.put(i,p.m_coordinates.get(i));
        m_stat[i].fill(0.0);
    }
    m_covMatrix.set_size(dim,dim);
    for( i=0 ; i<size() ; ++i)
        for( j=0 ; j<size() ; ++j)
            m_covMatrix.put(i,j,p.m_covMatrix.get(i,j));
    Object::update();
    return;
}

arlCore::Point::~Point( void )
{}

std::string arlCore::Point::getString( void ) const 
{
    std::string t;
    switch(m_type)
    {
    case ARLCORE_POINT_TYPE_UNKNOWN : t="UNKNOWN";break;
    case ARLCORE_POINT_TYPE_ARTK : t="ARTK";break;
    case ARLCORE_POINT_TYPE_MRO : t="MRO";break;
    case ARLCORE_POINT_TYPE_CHESS : t="CHESS";break;
    case ARLCORE_POINT_TYPE_ROI : t="ROI";break;
    case ARLCORE_POINT_TYPE_CLOUD : t="CLOUD";break;
    case ARLCORE_POINT_TYPE_TIP : t="TIP";break;
    case ARLCORE_POINT_TYPE_LINE : t="LINE";break;
    case ARLCORE_POINT_TYPE_SIFT : t="SIFT";break;
    }   
    unsigned int i;
    std::stringstream s;
    s<<std::fixed<<std::setprecision(2);
    for( i=0 ; i<size() ; ++i )
        s<<get(i)<<" ";
    if(m_visibility) s<<"Visible";
    else s<<"Unvisible";
    s<<" "<<t<<" Conf.="<<m_confidence;
    if(m_isColored) s<<" colour("<<m_colour.getString()<<")";
    else s<<" No colour";
    if(m_scalar>0) s<<" scalar="<<m_scalar<<"mm";
    switch(m_status)
    {
    case ARLCORE_POINT_STATUS_UNKNOWN : s<<" (Unknown)";break;
    case ARLCORE_POINT_STATUS_DETECTED : s<<" (Detected)";break;
    case ARLCORE_POINT_STATUS_SUBPIXEL : s<<" (Subpixel)";break;
    case ARLCORE_POINT_STATUS_ESTIMATE : s<<" (Estimate)";break;
    case ARLCORE_POINT_STATUS_CLOUD : s<<" (Cloud)";break;
    case ARLCORE_POINT_REPROJECTION : s<<" (Reproj)";break;
    }
    s<<" Error="<<m_error<<"\n";
    //TODO Display m_covMatrix
    return s.str();
}

bool arlCore::Point::save( const std::string &fileName, bool overwrite ) const
{
    if(!isOK()) return false;
    std::fstream f;
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    f.open(fileName.c_str(), std::fstream::out);
    if(f.is_open()) f<<"Dimension "<<size()<<"\n";
    bool b=save(f);
    f.close();
    f.flush();
    return b;
}

bool arlCore::Point::save( std::fstream &f, unsigned int cam, void* tag, int fringe ) const
{
    f<<"Dimension "<<size()<<"\n";
    return save( f, true, cam, tag, fringe, 0, ARLCORE_POINT_SAVE_FULL );
}

bool arlCore::Point::save( std::fstream &f, unsigned int no, ARLCORE_POINT_SAVE_TYPE type ) const
{
    return save( f, false, 0, 0, 0, no, type );
}

bool arlCore::Point::save( std::fstream &f, bool ctf, unsigned int cam, void* tag, int fringe, unsigned no, ARLCORE_POINT_SAVE_TYPE type ) const
{
    if(!f.is_open()) return false;
    unsigned int i,j,dim=size();
    if(type==ARLCORE_POINT_SAVE_FULL)
    {
        if(ctf)
        {
            f<<"Camera "<<cam;
            f<<"\nTag "<<tag;
            f<<"\nFringe "<<fringe;
        }
        else f<<"Numero "<<no<<"";
        f<<"\nVisibility "<<m_visibility;
        //f<<"Status "<<""; //TODO
        f<<"\nScalar "<<m_scalar;
        f<<"\nConfidence "<<m_confidence;
        f<<"\nError "<<m_error;
        if(m_isColored)
        {
            unsigned int R,G,B;
            m_colour.getColour(R,G,B);
            f<<"\nColour "<<R<<" "<<G<<" "<<B;
        }
        f<<"\nCoordinates\n";
        for( i=0 ; i<dim ; ++i )
            f<<get(i)<<"\t";
        f<<"\nUncertainty\n";
        for( i=0 ; i<dim ; ++i )
        {
            for( j=0 ; j<dim ; ++j )
                f<<m_covMatrix.get(i,j)<<"\t";
            f<<"\n";
        }
        f<<"End\n";
    }else
    {
        if(type==ARLCORE_POINT_SAVE_FULL ||type==ARLCORE_POINT_SAVE_LIGHT)
            f<<"Numero "<<no<<" Coordinates ";
        if(type==ARLCORE_POINT_SAVE_VTK)
            for( i=0 ; i<3 ; ++i )
                if(i<dim) f<<get(i)<<" "; else f<<"0.0 ";
        else
            for( i=0 ; i<dim ; ++i )
                f<<get(i)<<" ";
        if(type==ARLCORE_POINT_SAVE_FULL ||type==ARLCORE_POINT_SAVE_LIGHT)
            f<<"End";
        f<<"\n";
    }
    f.flush();
    return true;
}

bool arlCore::Point::load( const std::string &fileName )
{
    if(fileName=="") return false;
    std::ifstream f;
    f.open (fileName.c_str(), std::fstream::in);
    int no;
    bool b=load(f,no);
    f.close();
    return b;
}

bool arlCore::Point::load( std::ifstream &f, int &no, unsigned int dim )
{
    unsigned int cam;
    void* tag;
    int fringe;
    return load( f, cam, tag, fringe, no, dim );
}

bool arlCore::Point::load( std::ifstream &f, unsigned int &cam, void* &tag, int &fringe )
{
    int no;
    return load( f, cam, tag, fringe, no );
}

bool arlCore::Point::load( std::ifstream &f, unsigned int &cam, void* &tag, int &fringe, int &no, unsigned int dim )
{
    Object::update();
    setOK(false);
    if(!f.is_open()) return false;
    init(dim);
    no=-1;
    unsigned int i,j;
    std::string token,text;
    double val; //,version;
    bool b=false;
    do
    {
        f>>token;
        //if(token=="Version") f>>version;
        if(token=="Visibility") f>>m_visibility;
        //if(token=="Status") f>>""; //TODO
        if(token=="Radius") f>>m_scalar; // Deprecated
        if(token=="Scalar") f>>m_scalar;
        if(token=="Confidence") f>>m_confidence;
        if(token=="Error") f>>m_error;
        if(token=="Numero") f>>no;
        if(token=="Camera") f>>cam;
        if(token=="Tag") f>>tag;
        if(token=="Fringe") f>>fringe;
        if(token=="Name")
        {
            f>>text;
            setName(text);
        }
        if(token=="Dimension")
        {
            if(dim>0) f>>i;
            else
            {
                f>>dim;
                m_coordinates.set_size(dim);
                m_stat.resize(size());
                for( i=0 ; i<size() ; ++i )
                    m_stat[i].fill(0.0);
                m_coordinates.fill(0.0);
                m_covMatrix.set_size(dim,dim);
                m_covMatrix.fill(0.0);
            }
        }
        if(token=="Colour")
        {
            unsigned int R,G,B;
            f>>R>>G>>B;
            m_colour.setColour(R,G,B);          
            m_isColored=true;
        }
        if(token=="Coordinates" && dim>0)
        {
            for( i=0 ; i<dim ; ++i )
            {
                f>>val;
                m_coordinates.put(i,val);
            }
            b=true;
        }
        if(token=="Uncertainty" && dim>0)
        {
            for( i=0 ; i<dim ; ++i )
                for( j=0 ; j<dim ; ++j )
                {
                    f>>val;
                    m_covMatrix.put(i,j,val);
                }
        }
    }while(!f.eof() && token!="End");
    setOK(b);
    return b;
}

vnl_vector<double> arlCore::Point::getCoordinates() const
{   //
    return m_coordinates*m_ponderation;
}

vnl_vector<double> arlCore::Point::getHCoordinates() const
{
    unsigned int size = m_coordinates.size();
    vnl_vector<double> homogene(size+1);
    homogene.update(m_coordinates*m_ponderation);
    homogene.put(size,1.0);
    return homogene;
}

arlCore::vnl_covariance_matrix& arlCore::Point::getCovMatrix()
{
    Object::update(); //?
    return m_covMatrix;
}

const arlCore::vnl_covariance_matrix& arlCore::Point::getCovMatrix() const
{
    return m_covMatrix;
}

bool arlCore::Point::isVisible() const
{
    return m_visibility;
}

bool arlCore::Point::getVisible() const
{
    return m_visibility;
}

bool arlCore::Point::setVisible(bool b)
{
    if(m_visibility!=b)
    {
        Object::update();
        m_visibility=b;
    }
    return m_visibility;
}

arlCore::Point::ARLCORE_POINT_STATUS arlCore::Point::getStatus() const
{
    return m_status;
}

bool arlCore::Point::setStatus(ARLCORE_POINT_STATUS status)
{
    if(m_status!=status)
    {
        Object::update();
        m_status=status;
    }
    return true;
}

bool arlCore::Point::setColour(unsigned int R, unsigned int G, unsigned int B)
{
    Object::update();
    m_isColored=true;
    m_colour.setColour(R,G,B);
    return true;
}

bool arlCore::Point::setColour(const Colour &c)
{
    Object::update();
    m_isColored=true;
    m_colour.setColour(c);
    return true;
}

bool arlCore::Point::getColour(unsigned int &R, unsigned int &G, unsigned int &B) const
{
    m_colour.getColour(R,G,B);
    return m_isColored;
}

const arlCore::Colour & arlCore::Point::getColour( void ) const
{
    return m_colour;
}

bool arlCore::Point::isColored() const
{
    return m_isColored;
}

bool arlCore::Point::initUncertainty( void )
{
    unsigned int dim = size();
    Object::update();
    m_covMatrix.set_size(dim,dim);
    m_covMatrix.fill(0.0);
    return true;
}

double arlCore::Point::operator[]( unsigned int i ) const
{
    assert(i<size());
    if(i<size()) return get(i);
    else return 0.0;
}

/*double& arlCore::Point::operator []( unsigned int i )
{
    if(i<size()) return m_coordinates[i]*m_ponderation;
    else throw(-1);
}*/

double arlCore::Point::get( unsigned int i ) const
{
    assert(i<size());
    if(i<size()) return m_coordinates.get(i)*m_ponderation;
    else return 0.0;
}

bool arlCore::Point::set( unsigned int i, double a )
{
    assert(i<size());
    if(i>=size()) return false;
    Object::update();
    m_coordinates.put(i,a/m_ponderation); // FIXME
    return true;
}

bool arlCore::Point::set( const Point& p )
{
    assert(p.size()==size());
    if(p.size()!=size()) return false;
    unsigned int i;
    Object::update();
    m_ponderation=1.0;
    for( i=0 ; i<size() ; ++i )
    {
        m_coordinates.put(i,p.get(i));
        m_stat[i].fill(0.0);
    }
    return true;
}

void arlCore::Point::fill( double a )
{
    unsigned int i;
    Object::update();
    m_coordinates.fill(a);
//  for( i=0 ; i<m_coordinates.size() ; ++i )
//      m_coordinates.put(i,a);
    m_ponderation=1.0;
    for( i=0 ; i<size() ; ++i )
        m_stat[i].fill(0.0);
}

bool arlCore::Point::pond( const Point& p )
{
    if(p.size()!=size()) return false;
    unsigned int i;
    Object::update();
    for( i=0 ; i<size() ; ++i )
    {
        m_coordinates.put(i,m_coordinates.get(i)+p.get(i));
        arlCore::addValue(m_stat[i], p.get(i));
    }
    m_ponderation /= 1.0 + m_ponderation;       
    return true;
}

const std::vector< vnl_vector_fixed<double,5> >& arlCore::Point::getStatistic( void )
{
    return m_stat;
}

bool arlCore::Point::add( const Point& p )
{
    assert(p.size()==size());
    if(p.size()!=size()) return false;
    unsigned int i;
    Object::update();
    for( i=0 ; i<size() ; ++i )
    {
        m_coordinates.put(i,get(i)+p.get(i));
        m_stat[i].fill(0.0);
    }
    m_ponderation = 1.0;
    return true;
}

double arlCore::Point::x( void ) const
{
    assert(this->size()>0);
    return get(0);
}

double arlCore::Point::y( void ) const
{
    assert(this->size()>1);
    return get(1);
}

double arlCore::Point::z( void ) const
{
    assert(this->size()>2);
    return get(2);
}

bool arlCore::Point::x( double a )
{
    assert(this->size()>0);
    return set(0,a/m_ponderation); // FIXME
}

bool arlCore::Point::y( double a )
{
    assert(this->size()>1);
    return set(1,a/m_ponderation); // FIXME
}

bool arlCore::Point::z( double a )
{
    assert(this->size()>2);
    return set(2,a/m_ponderation); // FIXME
}

void arlCore::Point::normalize( void )
{
    unsigned int i;
    const Point Origin(size());
    const double Norm = distance(Origin);
    for( i=0 ; i<size() ; ++i )
    {
        m_coordinates.put(i,get(i)/Norm);
        m_stat[i].fill(0.0);
    }
    m_ponderation = 1.0;
    Object::update();
}

unsigned int arlCore::Point::size( void ) const
{
    return m_coordinates.size();
}

arlCore::Point::ARLCORE_POINT_TYPE arlCore::Point::getType( void ) const
{
    return m_type;
}

bool arlCore::Point::setType( ARLCORE_POINT_TYPE type )
{
    if(type<ARLCORE_POINT_NBTYPES)
    {
        Object::update();
        m_type=type;
    }
    return (type<ARLCORE_POINT_NBTYPES);
}

double arlCore::Point::getScalar( void ) const
{
    return m_scalar;
}

void arlCore::Point::setScalar( double scalar )
{
    Object::update();
    m_scalar=scalar;
}

bool arlCore::Point::addGaussianNoise( const unsigned int index, const double std )
{
    if(index>=size()) return false;
    set(index, get(index) + arlRandom::Random::gaussianRnd(std));
    return true;
}

bool arlCore::Point::addGaussianNoise( const double std )
{
    unsigned int i;
    for( i=0 ; i<size() ; ++i )
        set(i, get(i) + arlRandom::Random::gaussianRnd(std));
    return true;
}

void arlCore::Point::addUniformNoise( const unsigned int index, const double range )
{
    set(index, get(index)+arlRandom::Random::uniformDoubleRnd(-range, range));  
}
/*
void arlCore::Point::cubicRandom( const double side )
{
    unsigned int i;
    for( i=0 ; i<size() ; ++i )
        set(i, get(i)+arlRandom::Random::uniformDoubleRnd(-side/2,side/2));
}

void arlCore::Point::sphericRandom( const double radius )
{
    unsigned int i;
    vnl_vector<double> vec(size());
    do
    {
        for ( i=0 ; i<size() ; i++)
            vec[i] = arlRandom::Random::uniformDoubleRnd(-1.0,1.0);
    }while(vec.two_norm()>1.0);
    for( i=0 ; i<size() ; ++i )
        set(i, get(i)+vec[i]*radius);
}*/

bool arlCore::Point::shapeRandom( arlCore::ARLCORE_SHAPE type, const double size, const double angle )
{
    const unsigned int Dim = this->size();
    vnl_vector<double> vec(Dim);
    unsigned int i;
    double a, theta, phi, fi_max;
    switch(type)
    {
    case arlCore::ARLCORE_SHAPE_CUBE : // Size is the side of cube
        for( i=0 ; i<Dim ; ++i )
            set(i, get(i)+arlRandom::Random::uniformDoubleRnd(-size/2,size/2));
        return true;
    case arlCore::ARLCORE_SHAPE_SPHERE : // Size is the diameter
        if(this->size()!=Dim) return false;
        do
        {
            for ( i=0 ; i<Dim ; i++)
                vec[i] = arlRandom::Random::uniformDoubleRnd(-1.0,1.0);
        }while(vec.two_norm()>1.0);
        for( i=0 ; i<Dim ; ++i )
            set(i, get(i)+vec[i]*size/2);
        return true;
    case arlCore::ARLCORE_SHAPE_SPHERESURFACE :
        if(this->size()!=Dim) return false;
        theta = arlRandom::Random::uniformDoubleRnd(0,2*vnl_math::pi);
        phi   = arlRandom::Random::uniformDoubleRnd(0, vnl_math::pi);
        set(0, get(0) + size/2*sin(phi)*cos(theta) );
        set(1, get(1) + size/2*sin(phi)*sin(theta) );
        set(2, get(2) + size/2*cos(phi));
        return true;
    case arlCore::ARLCORE_SHAPE_SPHERE_CAP :
        if(this->size()!=Dim) return false;
        fi_max = acos(1-angle/2/vnl_math::pi);
        theta = arlRandom::Random::uniformDoubleRnd(0,2*vnl_math::pi);
        do
        {
            phi = arlRandom::Random::uniformDoubleRnd( 0, vnl_math::pi );
        }while( phi > fi_max );
        set(0, get(0) + size/2*sin(phi)*cos(theta) );
        set(1, get(1) + size/2*sin(phi)*sin(theta) );
        set(2, get(2) + size/2*cos(phi));
        return true;
    case arlCore::ARLCORE_SHAPE_DISC : 
        vec[Dim-1]=0.0;
        do //le point tire appartient au plan xOy i.e. point.z() = 0.O
        { //on ne boucle pas sur la derniere coordonnee
            for ( i=0 ; i<Dim-1 ; i++)
                vec[i] = arlRandom::Random::uniformDoubleRnd(-1.0,1.0);
        }while(vec.two_norm()>1.0);
        for( i=0 ; i<Dim ; ++i )
            set(i, get(i)+vec[i]*size/2);
        return true;
    case arlCore::ARLCORE_SHAPE_PLAINSQUARE :
        for( i=0 ; i<Dim-1 ; ++i )
            set(i, get(i)+arlRandom::Random::uniformDoubleRnd(-size/2,size/2));
        set(i, get(i) );
        return true;
    case arlCore::ARLCORE_SHAPE_CIRCLE :
        if( Dim!=3 ) return false;
        a = arlRandom::Random::uniformDoubleRnd(-vnl_math::pi,vnl_math::pi);
        set(0, get(0) + cos(a)*size/2 );
        set(1, get(1) + sin(a)*size/2 );
        set(2, get(2));
        return true;
    case arlCore::ARLCORE_SHAPE_EDGESQUARE :
        if( Dim!=3 ) return false;
        for ( i=0 ; i<Dim ; i++)
            vec[i] = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);
        if(vec[1]<0.0) a=-1; else a=1;
        if(vec[2] < 0.0)
        {   
            set(0, get(0) + a*size/2 );
            set(1, get(1) + vec[0] );
        }
        else
        {   
            set(0, get(0) + vec[0] );
            set(1, get(1) + a*size/2 );
        }
        set(2, get(2));
        return true;
    case arlCore::ARLCORE_SHAPE_SOLIDANGLE :
        if( Dim!=3 ) return false;
        double x,y,z,r;
        double phi_max;
        phi_max = acos(1.0-angle/2.0/vnl_math::pi);
        do
        {
            x = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);
            y = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);
            z = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);
            r = sqrt(x*x + y*y + z*z);
            phi = acos(z/r);
        }while( r>size/2 || phi > phi_max );
        set(0, get(0) + x);
        set(1, get(1) + y);
        set(2, get(2) + z);
        return true;
    case arlCore::ARLCORE_SHAPE_SOLIDANGLE_SURFACE :
        if( Dim!=3 ) return false;
        fi_max = acos(1.0-angle/2.0/vnl_math::pi);
        double xx,yy,zz,rr,fi;
        do
        {
            xx = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);
            yy = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);
            zz = arlRandom::Random::uniformDoubleRnd(-size/2,size/2);

            rr = sqrt(xx*xx + yy*yy + zz*zz);
            fi = acos(zz/rr);
        }while( rr>size/2 || fi > fi_max || fi < (fi_max - 0.01 ) );
        set(0, get(0) + xx);
        set(1, get(1) + yy);
        set(2, get(2) + zz);
        return true;
    }
    return false;
}

double arlCore::Point::distance(const Point &pt) const
{   // Calcule la distance sur les dimensions communes
    return ( sqrt(this->distance2(pt) ) );
}

double arlCore::Point::distance2(const Point &pt) const
{
    unsigned int i,dim=size();
    if(dim<pt.size()) dim=pt.size();
    double dist = 0;
    for(i=0 ; i<dim ; ++i)
        dist += ((*this)[i]-pt[i])*((*this)[i]-pt[i]);
    assert(dist>=0);
    return (dist);
}

bool arlCore::Point::mult( double scalaire )
{
    Object::update();
    m_coordinates *= scalaire;
    return true;
}

bool arlCore::Point::cross_3D(const Point &vec1, const Point &vec2 )
{   
    unsigned int i,j;
    assert(this->size()==3 && vec1.size()==3 && vec2.size()==3);
    vnl_vector_fixed<double,3> vnl_vec1(vec1[0], vec1[1],vec1[2]), vnl_vec2(vec2[0], vec2[1],vec2[2]), cross_prod;
    vnl_matrix_fixed<double,3,3> Sx, Sy, res;
    cross_prod = vnl_cross_3d(vnl_vec1, vnl_vec2);
    for(i=0;i<3;i++)
        this->set(i,cross_prod(i));
    Sx = vnl_cross_product_matrix(vnl_vec1);
    Sy = vnl_cross_product_matrix(vnl_vec2);
    res = Sy * vec1.getCovMatrix() * vnl_transpose(Sy) + Sx * vec2.getCovMatrix() * vnl_transpose(Sx);
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            m_covMatrix.put(i, j, res(i,j) );
    return true;
}

bool arlCore::Point::dotProduct(const Point& vec1, const Point& vec2)
{
    assert(this->size()==1);    
    assert(vec1.size()== vec2.size());
    vnl_matrix<double> vnl_vec1(vec1.size(),1), vnl_vec2(vec1.size(),1);
    vnl_matrix<double> res(1,1);
    double dot = 0;
    unsigned int i;
    for( i=0 ; i<vec1.size( ); i++ )
    {
        dot+=vec1[i]*vec2[i];
        vnl_vec1[i][0] = vec1[i];
        vnl_vec2[i][0] = vec2[i];
    }
    this->set(0, dot);
    res  = vnl_transpose(vnl_vec2) * vec1.getCovMatrix() * vnl_vec2
         + vnl_transpose(vnl_vec1) * vec2.getCovMatrix() * vnl_vec1;
    m_covMatrix.put(0, 0, res(0,0));
    return true;
}

bool arlCore::Point::divide(const Point& var1, const Point& var2)
{
    assert(this->size()==1 && var1.size() ==1 && var2.size() ==1);  
    vnl_matrix<double> res(1,1);
    //res = [1/y -x/y^2] * [sigma_x^2        0 ] * [  1/y ] = sigma_x^2 / y^2 + x^2/y^4 *sigma_y^2
    //                     [0         sigma_y^2]   [-x/y^2]
    res  = var1.getCovMatrix() / (var2[0] * var2[0]) +  var2.getCovMatrix()*(var1[0] * var1[0])/(var2[0] * var2[0]* var2[0]* var2[0]);
    m_covMatrix.put(0, 0, res(0,0));
    this->set(0, var1[0]/var2[0]);
    return true;
}

bool arlCore::Point::multiply(const Point& scalar, const Point& vec)
{
    assert( this->size()== vec.size() );
    assert( scalar.size()==1 ); 
    vnl_matrix<double> res(vec.size(),vec.size()), jacobien(vec.size(),vec.size()+1), sigma(vec.size()+1,vec.size()+1);
    unsigned int i,j;
    for( i=0 ; i<vec.size() ; ++i )
        this->set(i, scalar[0] * vec[i]);
    sigma.fill(0);
    sigma[0][0] = scalar.getCovMatrix()[0][0];
    for( i=1 ; i<vec.size()+1 ; ++i )
        for( j=1 ; j<vec.size()+1 ; ++j )
        sigma[i][j] =  vec.getCovMatrix()[i-1][j-1];
    //std::cerr<< "MATRIX\n"<<sigma;
    jacobien.fill(0);
    for( i=0 ; i<vec.size() ; ++i )
        jacobien[i][0] = vec[i];
    for( i=0 ; i<vec.size() ; ++i )
        jacobien[i][i+1] = scalar[0];
    //std::cerr<< "jacobien\n"<<jacobien;
    res = jacobien * sigma * vnl_transpose(jacobien);
    for( i=0 ; i<vec.size() ; ++i )
        for( j=0 ; j<vec.size() ; ++j )
            m_covMatrix.put(i, j, res(i,j));
    return true;
}

void arlCore::Point::setError( double error )
{
    m_error = error;
}

double arlCore::Point::getError( void ) const
{
    return m_error;
}

// FONCTIONS DIVERSES : A TRIER
/*unsigned int arlCore::load(PointsCont &list, const char *fileName, Point::ARLCORE_POINT_TYPE type, long int date)
{
    unsigned int i, j, nbPoints = 0, serie=0, dim=0;
    int n;
    double a;
    list.clear();
    std::ifstream pFile(fileName);
    std::string s;
    if( pFile != 0 )
    {
        std::getline (pFile, s);
        n = sscanf(s.c_str(),"%dDPoints\n",&dim);
        while(!pFile.eof() && dim>0 && n==1)
        {
            std::getline (pFile, s);
            n = sscanf(s.c_str(),"%d Points Serie %d\n",&nbPoints,&serie);
            Point point(dim, type);
            //TODO Lire tous les autres de la meme serie et moyenner
            for( i=0 ; i<nbPoints && n==2 ; ++i )
            {
                for( j=0 ; j<dim ; ++j )
                {
                    pFile >> a;
                    point.set(j,a);
                }
                point.setTime(serie,date);
                list.push_back(point);
            }
            std::getline (pFile, s);
        }
        pFile.close();
    }
    return nbPoints;
}
*/
double arlCore::distance(const arlCore::Point &pt1, const arlCore::Point &pt2)
{
    return pt1.distance(pt2);
}

double arlCore::surface( const std::vector< arlCore::Point::csptr >& ptList )
{
    //TODO : The next calculus is incorrect
    // It's the sum of distances between all points
    unsigned int i,j;
    double s=0;
    for( i=0 ; i<ptList.size() ; ++i )
        for( j=i+1 ; j<ptList.size() ; ++j )
            if(ptList[i]!=0 && ptList[j]!=0)
                s+=ptList[i]->distance(*(ptList[j]));
    return s;
}

unsigned int arlCore::closestY( const std::vector< arlCore::Point::csptr >&l, double y, unsigned int i1, unsigned int i2, int indexInit )
{   // Si l est trié par y croissant
    const bool Verbose = false;
/*#ifdef _DEBUG
//  arlCore::save(l,"c:/ccc.txt",arlCore::ARLCORE_POINT_SAVE_GNUPLOT);
    const bool lPasTrieCroissant = false;
    const bool y_de_l_pas_continue_dans_Z = false;
    unsigned int i;
    double y0 = -99999999.99;
    for( i=0 ; i<l.size() ; ++i )
    {
        // Verification de la croissance
        if(l[i]->y()<y0) assert( lPasTrieCroissant );
        // Verification de la continuite
        if(i>0 && l[i]->y()!=y0+1)
        {
            std::cout<<"Discontinuite de l : "<<y0<<" - "<<l[i]->y()<<"\n";
//          assert( y_de_l_pas_continue_dans_Z );
        }
        y0 = l[i]->y();
    }
#endif // _DEBUG*/
    assert(l.size()>0);
    if(Verbose) std::cout<<"Nb Points : "<<l.size();
    unsigned int index[2],centre,intervalle,iterations=0,r;
    index[0]=i1;
    if(i2<=i1) index[1]=l.size()-1;
    else index[1]=i2;
    if(y<=l[index[0]]->y()) return index[0];
    if(y>=l[index[1]]->y()) return index[1];
    //if(index[1]>=l.size()) index[1]=l.size()-1;
    intervalle=index[1]-index[0];
    while(intervalle>1)
    {
        centre=index[0]+intervalle/2;
        if(l[centre]->y()==y) return centre;
        if(l[centre]->y()<y) index[0]=centre;
        else index[1]=centre;
        intervalle=index[1]-index[0];
        ++iterations;
    }
    if(Verbose) std::cout<<" - ClosestY Nb iterations : "<<iterations<<"\n";
    if(y-l[index[0]]->y()>l[index[1]]->y()-y) r=index[1];
    else r=index[0];
#ifdef _DEBUG
    // Verification du bornage du resultat
    if(y<l[r]->y())
        if(r>0) assert(l[r-1]->y()<y);
    if(y>l[r]->y())
        if(r<l.size()-1) assert(l[r+1]->y()>y);
#endif // _DEBUG
    return r;
}
