/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fwCore/base.hpp>

#include "fwXML/XML/UniqueXMLAggregator.hpp"

namespace fwXML
{

UniqueXMLAggregator::UniqueXMLAggregator()
{}

//------------------------------------------------------------------------------

UniqueXMLAggregator::~UniqueXMLAggregator()
{}

//------------------------------------------------------------------------------

void UniqueXMLAggregator::append( xmlNodePtr  node)
{
    SLM_ASSERT("m_xmlDoc not instanced", m_xmlDoc);

    if ( xmlDocGetRootElement( m_xmlDoc) == NULL )
    {
        xmlDocSetRootElement ( m_xmlDoc , node );
    }
    else
    {
        xmlAddChild( xmlDocGetRootElement(m_xmlDoc), node );
    }

    SLM_ASSERT("m_xmlDoc not instanced", m_xmlDoc);
    assert( xmlDocGetRootElement( m_xmlDoc) );
}

//------------------------------------------------------------------------------

void UniqueXMLAggregator::append( xmlNodePtr parent, xmlNodePtr  node)
{
    xmlAddChild( parent, node );
}

//------------------------------------------------------------------------------

}

