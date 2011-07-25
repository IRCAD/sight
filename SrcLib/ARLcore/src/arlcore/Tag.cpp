/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Tag.h>

#include <arlcore/PlaneSystem.h>

arlCore::Tag::Tag( arlCore::PlaneSystem &universe, const std::string &name ):
Object(arlCore::ARLCORE_CLASS_TAG, name),
Particle( universe, name ),
Parameters("Tag"),
m_measuresPlane( 0 ),
m_registrationType( ARLCORE_TAG_REGISTRATION_UNKNOWN ),
m_registrationMaxError(-1.0),
m_persistence(-1)
{
    init();
}

arlCore::Tag::Tag( PlaneSystem &universe, const PointList& pl ):
Object(arlCore::ARLCORE_CLASS_TAG, "Points cloud"),
Particle( universe, "Points cloud" ),
Parameters("Tag"),
m_measuresPlane( 0 ),
m_registrationType( ARLCORE_TAG_REGISTRATION_3D3D ),
m_registrationMaxError(-1.0),
m_persistence(-1)
{
    init();
    arlCore::Point p0(0.0, 0.0, 0.0);
    p0.setVisible(false);
    unsigned int i;
    for(i=0 ; i<pl.size() ; ++i)
    {
        m_geometry.push_back(*(pl[i]));
        m_measures.push_back(p0);
    }
    getPlaneSystem().setPlaneName(getPlane(), Object::getName());
    setOK();
}

bool arlCore::Tag::init( void )
{
    Parameters::init( ARLCORE_TAGPARAMETERS_NAMES, ARLCORE_TAGPARAMETERS_DEFAULTS, ARLCORE_TAG_NBPARAMETERS );
    return true;
}

arlCore::Tag::~Tag( void )
{}

std::string arlCore::Tag::getString( void ) const   
{
    std::stringstream s;
    s<<this->Object::getString();
    s<<this->Particle::getString();
    if(m_registrationMaxError<0)
        s<<"No registration max error\n";
    else s<<"Registration max error = "<<m_registrationMaxError<<"\n";
    s<<"Measures plane "<<m_measuresPlane<<"\n";
    return s.str();
}

unsigned int arlCore::Tag::getMeasuresPlane() const
{
    return m_measuresPlane;
}

unsigned int arlCore::Tag::getNbPoints() const
{
    return m_geometry.size();
}

const arlCore::PointList& arlCore::Tag::getGeometry() const
{
    return m_geometry;
}

arlCore::PointList& arlCore::Tag::getGeometry()
{
    return m_geometry;
}

const arlCore::PointList& arlCore::Tag::getMeasures() const
{
    return m_measures;
}

arlCore::PointList& arlCore::Tag::getMeasures()
{
    return m_measures;
}

arlCore::Tag::ARLCORE_TAG_REGISTRATION_TYPE arlCore::Tag::getRegistrationType( void )
{
    return m_registrationType;
}

double arlCore::Tag::getRegistrationMaxError( void )
{
    return m_registrationMaxError;
}

void arlCore::Tag::setRegistrationMaxError( double error )
{
    m_registrationMaxError=error;
}

void arlCore::Tag::reset( void )
{
    m_measuresPlane=0;
    m_measures.clear();
}

bool arlCore::Tag::setRegistration( unsigned int plane, arlCore::vnl_rigid_matrix &T, long int date, long int time, bool reset )
{
    setTime(date,time);
    //std::vector<double> errors;
    //T.RMS3D3D( m_geometry, m_measures, errors);
    const double Error = T.getRMS();
    if(m_registrationMaxError>=0 && Error>m_registrationMaxError)
    {
        m_measuresPlane = arlCore::PlaneSystem::NoPlane;
        getPlaneSystem().resetTrf( getPlane(), plane );
        m_log<<"REGISTRATION FAILED : RMS="<<Error<<" > RMSMax="<<m_registrationMaxError<<"\n";
        log(ARLCORE_LOG_WARNING);
        return false;
    }
    if(reset) this->reset();
    unsigned int i;
    for( i=0 ; i<m_geometry.size() ; ++i )
    {
        if(i>=m_measures.size())
        {
            m_measures.push_back(*m_geometry[i]);
            m_measures[i]->setVisible(false);
        }
        if(!m_measures[i]->isVisible())
        {
            assert(m_measures.size()>=i);
            T.trf(*m_geometry[i],*m_measures[i]);
            m_measures[i]->setVisible(true);
        }
    }
    bool b = getPlaneSystem().setTrf( getPlane(), plane, T, date,time );
    if(b) m_measuresPlane=plane;
    return b;
}
