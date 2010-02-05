#include "fwXML/XML/UniqueXMLAggregator.hpp"

#include <iostream>


namespace fwXML
{

UniqueXMLAggregator::UniqueXMLAggregator()
{
}

UniqueXMLAggregator::~UniqueXMLAggregator()
{
}



void UniqueXMLAggregator::append( xmlNodePtr  node)
{
    assert(m_xmlDoc);

    if ( xmlDocGetRootElement( m_xmlDoc) == NULL )
    {
        xmlDocSetRootElement ( m_xmlDoc , node );
    }
    else
    {
        xmlAddChild( xmlDocGetRootElement(m_xmlDoc), node );
    }

    assert(m_xmlDoc);
    assert( xmlDocGetRootElement( m_xmlDoc) );
}



void UniqueXMLAggregator::append( xmlNodePtr parent, xmlNodePtr  node)
{

    xmlAddChild( parent, node );
}






}

