/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/ExtensionPoint.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/io/Validator.hpp"



namespace fwRuntime
{



ExtensionPoint::ExtensionPoint( const ::boost::shared_ptr< Bundle > bundle, const std::string & id, const boost::filesystem::path & schema )
:	BundleElement	( bundle ),
	m_id			( id ),
	m_schema		( schema )
{}



const std::string & ExtensionPoint::getIdentifier() const
{
	return m_id;
}



::boost::shared_ptr< io::Validator > ExtensionPoint::getExtensionValidator() const
{
	if( m_schema.empty() == false && !m_validator )
	{
		try
		{
			const boost::filesystem::path	schemaPath = getBundle()->getLocation() / m_schema;

			m_validator = ::boost::shared_ptr< io::Validator >( new io::Validator(schemaPath) );
		}
		catch( const std::exception & e )
		{
			throw RuntimeException( "Error while creating a validator. " + std::string(e.what()) );
		}
	}

	return m_validator;
}



void ExtensionPoint::operator=( const ExtensionPoint & ) throw()
{}



} // namespace fwRuntime
