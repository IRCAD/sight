/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Colour.h>
#include <arlcore/Misc.h>

const arlCore::Colour arlCore::Colour::Black( 0, 0, 0 );
const arlCore::Colour arlCore::Colour::Red( 255, 0, 0 );
const arlCore::Colour arlCore::Colour::Green( 0, 255, 0 );
const arlCore::Colour arlCore::Colour::Blue( 0, 0, 255 );
const arlCore::Colour arlCore::Colour::ElectricBlue( 0, 255, 255 );
const arlCore::Colour arlCore::Colour::Yellow( 255, 255, 0 );
const arlCore::Colour arlCore::Colour::Violet( 255, 0, 255 );
const arlCore::Colour arlCore::Colour::White( 255, 255, 255 );

arlCore::Colour::Colour( const Colour &c ):
m_R(c.m_R),
m_G(c.m_G),
m_B(c.m_B)
{}

arlCore::Colour::Colour( unsigned int R, unsigned int G, unsigned int B ):
m_R(R),
m_G(G),
m_B(B)
{}

arlCore::Colour::~Colour( void )
{}

arlCore::Colour& arlCore::Colour::operator=(const Colour& c)
{
    if(this==&c) return *this;
    m_R = c.m_R;
    m_G = c.m_G;
    m_B = c.m_B;
    return *this;
}

std::string arlCore::Colour::getString( void ) const    
{
    std::stringstream s;
    s<<m_R<<","<<m_G<<","<<m_B;
    return s.str();
}

void arlCore::Colour::setColour( const Colour &c )
{
    c.getColour(m_R,m_G,m_B);
}

void arlCore::Colour::setColour( unsigned int R, unsigned int G, unsigned int B )
{
    m_R=R;
    m_G=G;
    m_B=B;
}

void arlCore::Colour::setMapColour( long int n )
{
    static std::vector<arlCore::Colour> colours;
    if(colours.size()==0)
    {
        colours.push_back(arlCore::Colour::Red);
        colours.push_back(arlCore::Colour::Green);
        colours.push_back(arlCore::Colour::Blue);
        colours.push_back(arlCore::Colour::ElectricBlue);
        colours.push_back(arlCore::Colour::Yellow);
        colours.push_back(arlCore::Colour::Violet);
        colours.push_back(arlCore::Colour::White);
    }
    long int i;
    if(n<0) i = (-n)%colours.size();
    else i = n%colours.size();
    *this = colours[i];
/*  int step = 30;
    m_R=0,m_G=0,m_B=0;
    if(n<0) m_B=255+step*n;
    else m_R=255-step*n;
    if(arlCore::even(n)) m_G=m_R, m_R=m_B, m_B=m_G;*/
}

void arlCore::Colour::getColour( unsigned int &R, unsigned int &G, unsigned int &B ) const
{
    R=m_R;
    G=m_G;
    B=m_B;
}
