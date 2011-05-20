/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwXML/XML/IXMLAggregator.hpp"

#include <iostream>

namespace fwXML
{

IXMLAggregator::IXMLAggregator()
{
    reset();
    SLM_ASSERT("m_xmlDoc not instanced", m_xmlDoc);
}

//------------------------------------------------------------------------------

IXMLAggregator::~IXMLAggregator()
{}

//------------------------------------------------------------------------------

void IXMLAggregator::reset()
{
    m_xmlDoc = xmlNewDoc(BAD_CAST "1.0");
    SLM_ASSERT("m_xmlDoc not instanced", m_xmlDoc);
    // FIXME destroy old tree ????
}

//------------------------------------------------------------------------------

xmlDocPtr IXMLAggregator::getXMLDoc()
{
    return m_xmlDoc;
}

//------------------------------------------------------------------------------

}
