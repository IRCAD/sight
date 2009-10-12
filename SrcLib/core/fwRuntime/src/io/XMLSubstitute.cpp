#include <assert.h>

#include <libxml/xpath.h>
#include <iostream>

#include <fwRuntime/io/XMLSubstitute.hpp>

namespace fwRuntime {
namespace io {

void substitute( xmlNodePtr original, xmlNodePtr substitutionRules, std::map< std::string, std::string> &dictionary)
{
	std::list< Substitute > substitutions = getSubstitutions( substitutionRules );


	for ( std::list< Substitute >::iterator iter = substitutions.begin(); iter != substitutions.end(); ++iter )
	{
		std::string xpath = iter->xpath;
		std::string dictEntry = iter->dictEntry;

		// create the context for xpath
		xmlXPathContextPtr xpathCtx;
		xpathCtx = xmlXPathNewContext(original->doc);
		assert( xpathCtx );
		// search
		xmlChar *xpathExpr= BAD_CAST xpath.c_str();
		xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
		assert(xpathObj );

		int NbNodesFound = xpathObj->nodesetval->nodeNr;
		for (int i=NbNodesFound-1; i >= 0; --i )
		{
			xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
			std::cout << "SSS " << i << node->name << node->type << std::endl;
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




std::list< Substitute > getSubstitutions( xmlNodePtr substitutionRules )
{

	// create the context for xpath
	xmlXPathContextPtr xpathCtx;
	xpathCtx = xmlXPathNewContext(substitutionRules->doc);
	assert( xpathCtx );

	std::list< Substitute > result;

	xmlChar *xpathExpr= BAD_CAST "//Substitutions/substitute";
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
	// xmlXPathObjectPtr::nodesetval : get set of node
	// xmlNodeSetPtr::nodeNr  = nb element
	// xmlNodeSetPtr::nodeTab[i] : ith element : must be processed in reverse order
	assert(xpathObj );

	int NbNodesFound = xpathObj->nodesetval->nodeNr;
	assert( NbNodesFound == 2 );

	for (int i=0; i < NbNodesFound ; ++i )
	{
		Substitute s;
		xmlNodePtr subNode = xpathObj->nodesetval->nodeTab[i];
		xmlNodePtr element = xmlNextElementSibling(subNode->children);
		while (element )
		{
			std::cout << "EEE " << i << " " << element->name << std::endl;
			if ( xmlStrcmp( element->name, BAD_CAST "nodePath")==0 )
			{
				s.xpath = (const char *)xmlNodeGetContent( element );
			}
			if ( xmlStrcmp( element->name, BAD_CAST "replace")==0 )
			{
				s.dictEntry = (const char *)xmlGetProp( element, BAD_CAST "dictEntry" );
			}
			element = xmlNextElementSibling(element);
		}
		assert( s.xpath.size()  && s.dictEntry.size() );
		result.push_back( s );
	}

	xmlXPathFreeObject(xpathObj );

	return result;

}


} //namespace fwRuntime
} //namespace io

