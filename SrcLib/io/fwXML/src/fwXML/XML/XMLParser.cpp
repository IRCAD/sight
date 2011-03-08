/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <limits>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

// chdir management
#ifdef _MSC_VER
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <sstream>

#include <boost/cstdint.hpp>
#include <fwCore/base.hpp>

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLStream.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

void XMLParser::validateDoc (xmlDocPtr xmlDoc) throw (::fwTools::Failed)
{
    SLM_ASSERT("xmlDoc not instanced", xmlDoc);
    SLM_DEBUG( "checking schema ...." );

    xmlSchemaValidCtxtPtr validationCtxt = xmlSchemaNewValidCtxt(NULL);
    if (validationCtxt == NULL)
    {
        throw ::fwTools::Failed (std::string ("Failed to create the validation context"));
    }

    //validationCtxt
    ::boost::uint32_t  ui32Res = xmlSchemaValidateDoc (validationCtxt, xmlDoc);
    xmlSchemaFreeValidCtxt(validationCtxt);

    if (ui32Res > 0)
    {
        throw ::fwTools::Failed (std::string ("XML file not valid against the XML schema"));
    }
    else if (ui32Res == std::numeric_limits<boost::uint32_t> ::max() )
    {
        throw ::fwTools::Failed (std::string ("XML validation internal error"));
    }
}

//------------------------------------------------------------------------------

xmlNodePtr XMLParser::nextXMLElement (xmlNodePtr pNode)
{
    SLM_ASSERT("pNode not instanced", pNode);
    xmlNodePtr pChild;

    for (pChild = pNode; pChild != 0 ; pChild = pChild->next)
    {
        if ( pChild->type == XML_ELEMENT_NODE )
        {
            return pChild;
        }
    }
    return NULL; // not found !!
}

//------------------------------------------------------------------------------

std::string XMLParser::getTextValue (xmlNodePtr pNode) throw (::fwTools::Failed)
{
    SLM_ASSERT("pNode not instanced", pNode);
    xmlNodePtr pChild;
    std::string value = "";
    for (pChild = pNode->children; pChild != 0 && pChild->type != XML_TEXT_NODE; pChild = pChild->next);

    if (pChild)
    {
        if (pChild->type != XML_TEXT_NODE)
        {
            throw ::fwTools::Failed (std::string ("The current node is not a TEXT node !"));
        }
        else
        {
            value = std::string ((const char*)pChild->content);
        }
    }
    return value;
}

//------------------------------------------------------------------------------

std::string XMLParser::getAttribute (xmlNodePtr pNode, const std::string& _sAttrName) throw (::fwTools::Failed)
{
    SLM_ASSERT("pNode not instanced", pNode);
    xmlChar* psAttr = xmlGetProp (pNode, xmlCharStrdup (_sAttrName.c_str ()));

    if (psAttr == NULL)
    {
        throw ::fwTools::Failed (std::string ("The attribute ") + _sAttrName +
            std::string (" does not exist in the node ") + (const char*)pNode->name);
    }
    const std::string sAttr = (const char*)psAttr;
    xmlFree (psAttr);
    return sAttr;
}

//------------------------------------------------------------------------------

xmlDocPtr XMLParser::getXmlDocFromFile(boost::filesystem::path rootFile) throw (::fwTools::Failed)
{
    xmlDocPtr xmlDoc = NULL;
    xmlNodePtr xmlRoot = NULL;

    // save previous workingDirectory
    char workingDirectorySaved[1024];
    getcwd (workingDirectorySaved, 1024);

    // set new working directory
    std::string rootFolder = rootFile.branch_path().string();
    chdir (rootFolder.c_str ());
    OSLM_DEBUG( "change working dir to " <<   rootFolder << "...." );

    OSLM_DEBUG( "parsing XML file " <<   rootFile.string() << "...." );
    xmlDoc = xmlParseFile ( rootFile.leaf().c_str () );
    if (xmlDoc == NULL)
    {
        xmlCleanupParser ();
        throw ::fwTools::Failed("Unable to parse the XML file " + rootFile.string() );
    }

    OSLM_DEBUG( "managing XInclude ...." );
    xmlRoot = xmlDocGetRootElement (xmlDoc);
    if (xmlXIncludeProcessTree (xmlRoot) == -1)
    {
        throw ::fwTools::Failed(std::string ("Unable to manage xinclude !"));
    }

    // check validation
    //validateDoc(xmlDoc);

    // restore old working directory
    chdir (workingDirectorySaved);

    // memory cleanup
    xmlCleanupParser();

    return xmlDoc;
}

//------------------------------------------------------------------------------

xmlNodePtr XMLParser::findChildNamed(xmlNodePtr start, std::string value)
{
    SLM_ASSERT("start not instanced", start);
    xmlNodePtr elt = start->children; // we parse elements
    while( elt != NULL)
    {
        if (  !xmlStrcmp( elt->name, BAD_CAST value.c_str() ) )
        {
            return elt;
        }
        elt=elt->next;
    }
    return elt; // node not found

}

//------------------------------------------------------------------------------

std::string XMLParser::toString(xmlNodePtr node)
{
    SLM_ASSERT("node not instanced", node);

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlDocSetRootElement ( doc , xmlCopyNode( node, 1 ) );

    std::stringstream ss;
    XMLStream::toStream( doc, ss );

    // cleaning
    xmlFreeDoc( doc );
    return ss.str();
}

//------------------------------------------------------------------------------

}
