/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwDataIO/writer/IObjectWriter.hpp"

namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

IObjectWriter::IObjectWriter()
{}

//------------------------------------------------------------------------------

IObjectWriter::~IObjectWriter()
{}

//------------------------------------------------------------------------------

void IObjectWriter::setObject( ::fwTools::Object::sptr _pObject )
{
    m_object = _pObject;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr  IObjectWriter::getObject()
{
    assert( !m_object.expired() );
    return m_object.lock();
}

//------------------------------------------------------------------------------

void IObjectWriter::setLocation( const ::fwData::location::ILocation::sptr _location )
{
    m_location = _location;
}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr IObjectWriter::getLocation()
{
    return m_location;
}

//------------------------------------------------------------------------------

std::string  IObjectWriter::extension()
{
    return std::string();
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
