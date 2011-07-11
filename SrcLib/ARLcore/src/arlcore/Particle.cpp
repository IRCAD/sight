/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Particle.h>

arlCore::Particle::Particle( PlaneSystem &universe, const std::string &name ):
m_universe( universe ),
m_plane( universe.add3DPlane(name) )
{}


arlCore::Particle::Particle( const Particle& p ):
m_universe( p.m_universe ),
m_plane( p.m_plane )
{}

arlCore::Particle& arlCore::Particle::operator=( const Particle& p )
{
    m_universe = p.m_universe;;
    m_plane = p.m_plane;
    return *this;
}

arlCore::Particle::~Particle( void )
{   // The particle's plane is released by the Planesystem
    m_universe.releasePlane(m_plane);
}

std::string arlCore::Particle::getName( void ) const
{   // The name of the particle
    std::string name;
    if(m_universe.getPlaneName(getPlane(), name)) return name;
    else return "";
}

std::string arlCore::Particle::getString( void ) const
{   // Particle's description
    std::stringstream s;
    s<<"Universe : \""<<m_universe.getName()<<"\" Plane : "<<m_plane<<"\n";
    return s.str();
}

unsigned int arlCore::Particle::getPlane( void ) const
{   // ID of the Particle's plane in the Planesystem
    return m_plane;
}

arlCore::PlaneSystem& arlCore::Particle::getPlaneSystem( void ) const
{   // Planesystem referency where the particle is dived
    return m_universe;
}
