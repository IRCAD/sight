/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Command.h>

/*#include <sstream>
#include <fstream>
#include <iostream>
*/
#ifdef _WIN32
#include <conio.h>
#endif // _WIN32

arlCore::Command::Command( const std::string &fileName )
{}

arlCore::Command::~Command( void )
{}

bool arlCore::Command::get( const std::string& commandName )
{
    std::map< std::string, unsigned int >::const_iterator it;
    it = m_byName.find(commandName);
    if( it==m_byName.end() ) return false;
    const bool Persistent = false;
    bool value = m_values[it->second];
    if(!Persistent) m_values[it->second] = false;
    return value;
}

bool arlCore::Command::refresh( void )
{
    char key;
#ifdef _WIN32
    if(kbhit()==0) return false;
    key = getch();
#else // _WIN32
    // TODO Test keys for Linux
    return false;
#endif // _WIN32
    std::map< char, unsigned int >::const_iterator it;
    it = m_byKey.find(key);
    if( it==m_byKey.end() ) return false;
    const bool Switch = true;
    bool value = m_values[it->second];
    if(Switch) value = !value; else value = true;
    m_values[it->second] = value;
    return true;
}
