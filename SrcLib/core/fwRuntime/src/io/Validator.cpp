/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

//#include <ios_base.h> not necessary on win32
#include <boost/filesystem/operations.hpp>
#include <libxml/xmlversion.h>
#ifndef LIBXML_SCHEMAS_ENABLED
    #warning "Error libxml schemas disabled"
#endif
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/io/Validator.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
{
namespace io
{

//------------------------------------------------------------------------------

Validator::Validator( const Validator &validator )
{
    m_xsd_content = validator.m_xsd_content;
    m_schemaParserContext = validator.m_schemaParserContext;
    m_schema = validator.m_schema;
}

//------------------------------------------------------------------------------

Validator::Validator( const std::string & buffer )
{
    m_xsd_content = buffer ;
}

//------------------------------------------------------------------------------

Validator::Validator( const boost::filesystem::path & path )
{
#if BOOST_FILESYSTEM_VERSION > 2
    std::string strPath( path.string() );
#else
    std::string strPath( path.native_file_string() );
#endif
    // Checks the path validity.
    if( ::boost::filesystem::exists(path) == false || ::boost::filesystem::is_directory(path) )
    {
        throw RuntimeException( strPath + ": is not a valid path to an xml schema file." );
    }
    m_xsd_content = strPath;
}

//------------------------------------------------------------------------------

Validator::~Validator()
{
}

//------------------------------------------------------------------------------

void Validator::clearErrorLog()
{
    m_errorLog.str( std::string() );
}

//------------------------------------------------------------------------------

const std::string Validator::getErrorLog() const
{
    return m_errorLog.str();
}

//------------------------------------------------------------------------------

void Validator::initializeContext()
{
    if(m_schemaValidContext)
    {
        return;
    }

    if ( !m_schemaParserContext )
    {
        if (!(m_schemaParserContext = SchemaParserCtxtSptr (
                  xmlSchemaNewParserCtxt(m_xsd_content.c_str()),
                  xmlSchemaFreeParserCtxt)
              ) )
        {
            return ;
        }
        // Set the structured error callback
        xmlSchemaSetParserStructuredErrors(m_schemaParserContext.get(), Validator::ErrorHandler, this );
    }

    // Load XML schema content
    if (!m_schema)
    {
        m_schema = SchemaSptr ( xmlSchemaParse(m_schemaParserContext.get()), xmlSchemaFree );
    }
    if (!m_schema)
    {
        return ;
    }

    // Create XML schemas validation context
    if ( m_schemaValidContext = SchemaValidCtxtSptr( xmlSchemaNewValidCtxt(m_schema.get()), xmlSchemaFreeValidCtxt) )
    {
        // Set the structured error callback
        xmlSchemaSetValidStructuredErrors( m_schemaValidContext.get(), Validator::ErrorHandler, this );
    }
}

//------------------------------------------------------------------------------

const bool Validator::validate( const boost::filesystem::path & xmlFile )
{
    int result;

    initializeContext();

    xmlDocPtr xmlDoc = xmlParseFile ( xmlFile.string().c_str () );
    if (xmlDoc == NULL)
    {
        throw std::ios_base::failure("Unable to parse the XML file " + xmlFile.string() );
    }
    xmlNodePtr xmlRoot = xmlDocGetRootElement (xmlDoc);
    if (xmlXIncludeProcessTreeFlags (xmlRoot,XML_PARSE_NOBASEFIX) == -1)
    {
        xmlFreeDoc(xmlDoc);
        throw std::ios_base::failure(std::string ("Unable to manage xinclude !"));
    }

    if(!m_schemaValidContext)
    {
        return false;
    }

    result = xmlSchemaValidateDoc(m_schemaValidContext.get(), xmlDoc );

    xmlFreeDoc(xmlDoc);

    if ( result !=0 )
    {
        OSLM_WARN("Validator::validation NOK, xml = " << xmlFile.string() ) ;
        OSLM_WARN("Validator::validation NOK, xsd = " << getXsdContent() ) ;
        OSLM_ERROR("Validator::validation NOK, error log = " << getErrorLog() ) ;
    }

    return result == 0;
}

//------------------------------------------------------------------------------

const bool Validator::validate( xmlNodePtr node )
{
    int result;

    initializeContext();

    if(!m_schemaValidContext)
    {
        return false;
    }

    result = xmlSchemaValidateOneElement( m_schemaValidContext.get(), node );

    if ( result !=0 )
    {
        xmlBufferPtr buffer = xmlBufferCreate();
        xmlNodeDump( buffer, node->doc, node, 1, 1 );
        xmlBufferFree( buffer );
        OSLM_WARN("Validator::validation NOK, node :\n " << buffer->content) ;
        OSLM_WARN("Validator::validation NOK, xsd = " << getXsdContent() ) ;
        OSLM_ERROR("Validator::validation NOK, error log = " << getErrorLog() ) ;
    }

    return result == 0;
}

//------------------------------------------------------------------------------

void Validator::ErrorHandler( void * userData, xmlErrorPtr error )
{
    Validator   * validator = (Validator*) userData;

    validator->m_errorLog << "At line " << error->line << ": " << error->message;
}

//------------------------------------------------------------------------------

std::string Validator::getXsdContent()
{
    return m_xsd_content ;
}

//------------------------------------------------------------------------------

} // namespace io

} // namespace fwRuntime
