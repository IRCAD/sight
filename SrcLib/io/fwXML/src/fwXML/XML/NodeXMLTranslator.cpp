/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwData/visitor/accept.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>

#include "fwXML/XML/NodeXMLTranslator.hpp"
#include "fwXML/visitor/SerializeXML.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLParser.hpp"

namespace fwXML
{

NodeXMLTranslator::NodeXMLTranslator()
{};

//------------------------------------------------------------------------------

NodeXMLTranslator::~NodeXMLTranslator()
{};

//------------------------------------------------------------------------------

xmlNodePtr NodeXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::Node::sptr graphNode = ::fwData::Node::dynamicCast(obj);
    assert(graphNode);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr objectXMLNode;
    if (graphNode->getObject())
    {
        ::visitor::SerializeXML visitor;
        ::fwData::visitor::accept( graphNode->getObject(), &visitor );
        objectXMLNode = visitor.m_correspondance[graphNode->getObject()];
    }
    else
    {
        objectXMLNode = xmlNewNode(NULL,  BAD_CAST "nullObject");
    }
    xmlAddChild(node,objectXMLNode);

    // append input ports
    xmlNodePtr inputsList = XMLTranslatorHelper::homogeneousContainerToXml("inputs",
                                                                            graphNode->getInputPorts().begin(),
                                                                            graphNode->getInputPorts().end()    );
    xmlAddChild(node,inputsList);

    // append output ports
    xmlNodePtr outputsList = XMLTranslatorHelper::homogeneousContainerToXml("outputs",
                                                                            graphNode->getOutputPorts().begin(),
                                                                            graphNode->getOutputPorts().end()   );
    xmlAddChild(node,outputsList);

    return node;
}

//------------------------------------------------------------------------------

void NodeXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate, xmlNodePtr source)
{
    xmlNodePtr nodeObject = XMLParser::nextXMLElement(source->children);
    OSLM_DEBUG("NodeXMLTranslator::updateDataFromXML first XML child" << (const char *) nodeObject->name );

    ::fwData::Node::sptr graphNode = ::fwData::Node::dynamicCast(toUpdate);
    assert(graphNode);

    xmlNodePtr nullObj = XMLParser::findChildNamed(source,"nullObject");
    if ( ! nullObj)
    {
        ::fwData::Object::sptr subObject = ::fwData::Object::dynamicCast( XMLTH::fromXML(nodeObject) );
        assert(subObject);
        graphNode->setObject( subObject );
    }

    xmlNodePtr inputs = XMLParser::findChildNamed(source,"inputs");
    XMLTH::containerFromXml(inputs, back_inserter( graphNode->getInputPorts() ) );

    xmlNodePtr outputs = XMLParser::findChildNamed(source,"outputs");
    XMLTH::containerFromXml(outputs, back_inserter( graphNode->getOutputPorts() ) );
}

//------------------------------------------------------------------------------

}
