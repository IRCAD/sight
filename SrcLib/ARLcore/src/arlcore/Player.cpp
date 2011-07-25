/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Player.h>

arlCore::Player::Player( void ):
m_playerDirectory(""),
m_playerFullName(""),
m_isReady(false),
m_loop(false),
m_eof(false),
m_afterHeader(0)
{}

arlCore::Player::~Player( void )
{
    m_playerHandle.close();
}

bool arlCore::Player::readyPlayer( void )
{
    return m_isReady;
}

bool arlCore::Player::getPlayerFilename( std::string &fileName )
{
    if(m_isReady) fileName = m_playerFullName;
    else fileName = "";
    return m_isReady;
}

bool arlCore::Player::setLoop( bool loop )
{
    m_loop=loop;
    return m_loop;
}

bool arlCore::Player::getEOF( void ) const
{
    return m_eof;
}

unsigned int arlCore::Player::initPlayer( const std::string &fileName )
{   
    m_playerFullName = fileName;
    m_afterHeader = 0;
    if(!readHeader(fileName)) return 0;
    m_afterHeader = m_playerHandle.tellg();
    unsigned int n = playNext();
    m_isReady = n>0;
    m_eof = !m_isReady;
    return n;
}

unsigned int arlCore::Player::setEOF( unsigned int n )
{
    m_eof = true;
    return n;
}

unsigned int arlCore::Player::playEnd( void )
{
    unsigned int n = 0;
    m_eof = !m_loop;
    if(m_loop)
    {
        if(!goAfterHeader()) return 0;
// ?        n = playNext();
    }
    return n;
}

bool arlCore::Player::goAfterHeader( void )
{
    if(m_afterHeader==0) return false;
    m_playerHandle.clear();
    m_playerHandle.seekg(0, std::ios::beg);
    m_playerHandle.seekg(m_afterHeader);
    return true;
}
