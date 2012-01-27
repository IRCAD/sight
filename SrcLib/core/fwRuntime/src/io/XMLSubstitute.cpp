/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>

#include <libxml/xpath.h>
#include <iostream>

#include <fwCore/base.hpp>
#include <fwRuntime/io/XMLSubstitute.hpp>

namespace fwRuntime
{
namespace io
{

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::io::XMLSubstitute > XMLSubstitute::m_instance;

//------------------------------------------------------------------------------

XMLSubstitute::XMLSubstitute()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

XMLSubstitute::~XMLSubstitute()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

std::map< std::string, std::string > &XMLSubstitute::getRefDictionary()
{
    return m_dictionary;
}
//------------------------------------------------------------------------------

::boost::shared_ptr<XMLSubstitute> XMLSubstitute::getDefault()
{
    if ( m_instance == 0 )
    {
        SLM_TRACE_FUNC();
        m_instance = ::boost::shared_ptr< ::fwRuntime::io::XMLSubstitute >( new XMLSubstitute() );
    }
    return m_instance;
}

//------------------------------------------------------------------------------


xmlDocPtr XMLSubstitute::load( const ::boost::filesystem::path& xmlFile)
{
#if BOOST_FILESYSTEM_VERSION > 2
    xmlDocPtr doc = xmlParseFile(  xmlFile.string().c_str() );
#else
    xmlDocPtr doc = xmlParseFile(  xmlFile.native_file_string().c_str() );
#endif

    if ( ! m_dictionary.empty() ) // dictionary empty => classic xmlParseFile()
    {
        xmlNodePtr original = xmlDocGetRootElement(doc);
        // create the context for xpath
        xmlXPathContextPtr xpathCtx;
        xpathCtx = xmlXPathNewContext(doc);
        SLM_ASSERT("xpathCtx not instanced", xpathCtx);

        xmlChar *xpathExpr= BAD_CAST "//Substitutions";
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);

        if(xpathObj->nodesetval->nodeNr > 0)
        {
            // substitution node exists perform the translation
            SLM_ASSERT("XMLSubstitute::load manage only one xmlNode substitution", xpathObj->nodesetval->nodeNr == 1);
            xmlNodePtr substitutionRules = xpathObj->nodesetval->nodeTab[0];
            substitute(original, substitutionRules, m_dictionary);
        }
    }

    return doc;
}

//------------------------------------------------------------------------------

void XMLSubstitute::substitute( xmlNodePtr original, xmlNodePtr substitutionRules, std::map< std::string, std::string> &dictionary)
{
    std::list< ::fwRuntime::io::Substitute > substitutions = getSubstitutions( substitutionRules );

    for ( std::list< ::fwRuntime::io::Substitute >::iterator iter = substitutions.begin(); iter != substitutions.end(); ++iter )
    {
        std::string xpath = iter->xpath;
        std::string dictEntry = iter->dictEntry;
        std::string status = iter->status;
        bool entryInDictionary = dictionary.find(dictEntry) != dictionary.end();

        if ( status=="required" && !entryInDictionary )
        {
            OSLM_FATAL("XML substitution required dictEntry [" << dictEntry << "] missing for xpath " << xpath );
        }
        // optional and not in dictionary
        if ( status=="optional" && !entryInDictionary )
        {
            OSLM_INFO("XML substitution optional dictEntry [" << dictEntry << "] not modified for xpath " << xpath );
            continue;
        }

        OSLM_INFO("XML substitution dictEntry [" << dictEntry << "] modified with xpath " << xpath << " with the value : " <<  dictionary[dictEntry] );

        // create the context for xpath
        xmlXPathContextPtr xpathCtx;
        xpathCtx = xmlXPathNewContext(original->doc);
        SLM_ASSERT("xpathCtx not instanced", xpathCtx);
        // search
        xmlChar *xpathExpr= BAD_CAST xpath.c_str();
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
        SLM_ASSERT("xpathObj not instanced", xpathObj);

        int NbNodesFound = xpathObj->nodesetval->nodeNr;
        for (int i=NbNodesFound-1; i >= 0; --i )
        {
            xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
            // substitution
            if (node->type == XML_ATTRIBUTE_NODE )
            {
                xmlSetProp( node->parent, node->name, BAD_CAST dictionary[dictEntry].c_str() );
            }
            if (node->type == XML_ELEMENT_NODE )
            {
                xmlNodeSetName( node , BAD_CAST dictionary[dictEntry].c_str() );
            }
            if (node->type == XML_TEXT_NODE )
            {
                xmlNodeSetContent( node , BAD_CAST dictionary[dictEntry].c_str() );
            }
        }
        xmlXPathFreeObject(xpathObj );
    }
}

//------------------------------------------------------------------------------

std::list< ::fwRuntime::io::Substitute > XMLSubstitute::getSubstitutions( xmlNodePtr substitutionRules )
{
    // create the context for xpath
    xmlXPathContextPtr xpathCtx;
    xpathCtx = xmlXPathNewContext(substitutionRules->doc);
    SLM_ASSERT("xpathCtx not instanced", xpathCtx);

    std::list< ::fwRuntime::io::Substitute > result;

    xmlChar *xpathExpr= BAD_CAST "//Substitutions/substitute";
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    // xmlXPathObjectPtr::nodesetval : get set of node
    // xmlNodeSetPtr::nodeNr  = nb element
    // xmlNodeSetPtr::nodeTab[i] : ith element : must be processed in reverse order
    SLM_ASSERT("xpathObj not instanced", xpathObj);

    int NbNodesFound = xpathObj->nodesetval->nodeNr;

    for (int i=0; i < NbNodesFound ; ++i )
    {
        Substitute s;
        xmlNodePtr subNode = xpathObj->nodesetval->nodeTab[i];
        xmlNodePtr element = xmlNextElementSibling(subNode->children);
        while (element )
        {
            if ( xmlStrcmp( element->name, BAD_CAST "nodePath")==0 )
            {
                s.xpath = (const char *)xmlNodeGetContent( element );
            }
            if ( xmlStrcmp( element->name, BAD_CAST "replace")==0 )
            {
                s.dictEntry = (const char *)xmlGetProp( element, BAD_CAST "dictEntry" );
                s.status    = (const char *)xmlGetProp( element, BAD_CAST "status" );
            }
            element = xmlNextElementSibling(element);
        }
        assert( s.xpath.size()  && s.dictEntry.size() && s.status.size() );
        result.push_back( s );
    }

    xmlXPathFreeObject(xpathObj );

    return result;
}


} //namespace fwRuntime
} //namespace io

