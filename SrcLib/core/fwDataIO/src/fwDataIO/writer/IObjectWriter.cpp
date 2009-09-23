/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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



void IObjectWriter::setObject( ::boost::shared_ptr< ::fwTools::Object > _pObject )
{
	m_object = _pObject;
}



::boost::shared_ptr< ::fwTools::Object >  IObjectWriter::getObject()
{
	assert( !m_object.expired() );
	return m_object.lock();
}



void IObjectWriter::setLocation( ::boost::shared_ptr< ::fwData::location::ILocation > _location )
{
	m_location = _location;
}

::boost::shared_ptr< ::fwData::location::ILocation > IObjectWriter::getLocation()
{
	return m_location;
}



std::string  IObjectWriter::extension()
{
	return std::string();
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
