/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/location/ILocation.hpp>

#include "fwDataIO/reader/IObjectReader.hpp"

namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

IObjectReader::IObjectReader()
{}

//------------------------------------------------------------------------------

IObjectReader::~IObjectReader()
{}

//------------------------------------------------------------------------------

void IObjectReader::setObject( ::fwTools::Object::sptr _pObject )
{
    m_object = _pObject;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr IObjectReader::getObject()
{
    assert( !m_object.expired() );
    return m_object.lock();
}

//------------------------------------------------------------------------------

void IObjectReader::setLocation( const ::fwData::location::ILocation::sptr _location )
{
    m_location = _location;
}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr IObjectReader::getLocation()
{
    return m_location;
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace fwDataIO
