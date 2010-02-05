#include "fwXML/XML/IXMLAggregator.hpp"

#include <iostream>

namespace fwXML
{

IXMLAggregator::IXMLAggregator()
{
    reset();
    assert(m_xmlDoc);
}

IXMLAggregator::~IXMLAggregator()
{
}


void IXMLAggregator::reset()
{
    m_xmlDoc = xmlNewDoc(BAD_CAST "1.0");
    assert(m_xmlDoc);
    // FIXME destroy old tree ????
}


xmlDocPtr IXMLAggregator::getXMLDoc()
{
    return m_xmlDoc;
}



}
