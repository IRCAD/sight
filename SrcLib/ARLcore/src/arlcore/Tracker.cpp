/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Tracker.h>

arlCore::Tracker::Tracker( const std::string &name ):
Object(arlCore::ARLCORE_CLASS_TRACKER, name)
{}

arlCore::Tracker::~Tracker( void )
{}

std::string arlCore::Tracker::getString( void ) const
{
    std::stringstream s;
    s<<this->Object::getString();
    return s.str();
}
