/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>
#include <libxml/xmlversion.h>
#ifndef LIBXML_SCHEMAS_ENABLED
	#warning "Error libxml schemas disabled"
#endif
#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/io/Validator.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
{

namespace io
{



Validator::Validator( const std::string & buffer )
{
	m_xsd_content = buffer ;
}



Validator::Validator( const boost::filesystem::path & path )
{
	// Checks the path validity.
	if( boost::filesystem::exists(path) == false || boost::filesystem::is_directory(path) )
	{
		throw RuntimeException( path.native_file_string() + ": is not a valid path to an xml schema file." );
	}

	m_xsd_content = path.native_file_string() ;
}



Validator::~Validator()
{
}


void Validator::clearErrorLog()
{
	m_errorLog.str( std::string() );
}


const std::string Validator::getErrorLog() const
{
	return m_errorLog.str();
}



const bool Validator::validate( const boost::filesystem::path & xmlFile )
{
	int result;
	xmlSchemaParserCtxtPtr schemaParserContext;
	xmlSchemaPtr schema;
	xmlSchemaValidCtxtPtr schemaValidContext;

	// Open XML schema file
	if ((schemaParserContext = xmlSchemaNewParserCtxt(m_xsd_content.c_str())) == NULL)
	{
		return false;
	}

	// Load XML schema content
	schema = xmlSchemaParse(schemaParserContext);
	if (schema == NULL)
	{
		xmlSchemaFreeParserCtxt( schemaParserContext );
		return false;
	}

	// Create XML schemas validation context
	if ((schemaValidContext = xmlSchemaNewValidCtxt(schema)) == NULL)
	{
		xmlSchemaFree(schema);
		xmlSchemaFreeParserCtxt( schemaParserContext );
		return false;
	}

	 // Set the structured error callback
	xmlSchemaSetParserStructuredErrors(schemaParserContext, Validator::ErrorHandler, this );
	xmlSchemaSetValidStructuredErrors( schemaValidContext, Validator::ErrorHandler, this );

	result = xmlSchemaValidateFile(schemaValidContext, xmlFile.native_file_string().c_str(), 0 );

	if ( result !=0 )
	{
		OSLM_WARN("Validator::validation NOK, error log = " << getErrorLog() ) ;
		OSLM_WARN("Validator::validation NOK, xml = " << xmlFile.native_file_string() ) ;
		OSLM_WARN("Validator::validation NOK, xsd = " << getXsdContent() ) ;
	}

	xmlSchemaFreeValidCtxt(schemaValidContext);
	xmlSchemaFree( schema );
	xmlSchemaFreeParserCtxt( schemaParserContext );

	return result == 0;
}


const bool Validator::validate( xmlNodePtr node )
{
	int result;
	xmlSchemaParserCtxtPtr schemaParserContext;
	xmlSchemaPtr schema;
	xmlSchemaValidCtxtPtr schemaValidContext;

	// Open XML schema file
	if ((schemaParserContext = xmlSchemaNewParserCtxt(m_xsd_content.c_str())) == NULL)
	{
		return false;
	}

	// Load XML schema content
	schema = xmlSchemaParse(schemaParserContext);
	if (schema == NULL)
	{
		xmlSchemaFreeParserCtxt( schemaParserContext );
		return false;
	}

	// Create XML schemas validation context
	if ((schemaValidContext = xmlSchemaNewValidCtxt(schema)) == NULL)
	{
		xmlSchemaFree(schema);
		xmlSchemaFreeParserCtxt( schemaParserContext );
		return false;
	}

	 // Set the structured error callback
	xmlSchemaSetParserStructuredErrors(schemaParserContext, Validator::ErrorHandler, this );
	xmlSchemaSetValidStructuredErrors( schemaValidContext, Validator::ErrorHandler, this );


	result = xmlSchemaValidateOneElement( schemaValidContext, node );

	if ( result !=0 )
	{
		OSLM_WARN("Validator::validation NOK, error log = " << getErrorLog() ) ;
		OSLM_WARN("Validator::validation NOK, xsd = " << getXsdContent() ) ;
	}

	xmlSchemaFreeValidCtxt(schemaValidContext);
	xmlSchemaFree( schema );
	xmlSchemaFreeParserCtxt( schemaParserContext );

	return result == 0;
}


void Validator::ErrorHandler( void * userData, xmlErrorPtr error )
{
	Validator	* validator = (Validator*) userData;

	validator->m_errorLog << "At line " << error->line << ": " << error->message;
}

std::string Validator::getXsdContent()
{
	return m_xsd_content ;
}

} // namespace io

} // namespace fwRuntime
