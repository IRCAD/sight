/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/String.hpp>

#include "fwXML/XML/CompositeXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

CompositeXMLTranslator::CompositeXMLTranslator()
{};

//------------------------------------------------------------------------------

CompositeXMLTranslator::~CompositeXMLTranslator()
{};

//------------------------------------------------------------------------------

xmlNodePtr CompositeXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Composite::sptr  cmp= ::fwData::Composite::dynamicCast(obj);
    SLM_ASSERT("cmp not instanced", cmp);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    ::fwData::Composite::Container::iterator iter;
    for (iter = cmp->getRefMap().begin() ; iter != cmp->getRefMap().end() ; ++iter)
    {
        xmlNodePtr elementNode = xmlNewNode(NULL, BAD_CAST "element");
        xmlAddChild(node, elementNode);

        xmlNodePtr keyNode = xmlNewNode(NULL, BAD_CAST "key");
        xmlNodeAddContent( keyNode,  xmlStrdup( BAD_CAST iter->first.c_str() ) );
        xmlAddChild(elementNode, keyNode);

        xmlNodePtr valueNode = xmlNewNode(NULL, BAD_CAST "value");
        // xmlNodePtr trueValueNode = XMLTranslatorHelper::toXML( iter->second );
        xmlNodePtr trueValueNode = XMLTranslatorHelper::toXMLRecursive( iter->second );
        xmlAddChild(elementNode, valueNode);
        xmlAddChild(valueNode, trueValueNode);
    }

    return node;
}

//------------------------------------------------------------------------------

void CompositeXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate); // object should exist
    //get its label
    ::fwData::Composite::sptr cmp= ::fwData::Composite::dynamicCast(toUpdate);

    xmlNodePtr elementNode = ::fwXML::XMLParser::getChildrenXMLElement(source);
    while (elementNode )
    {
        std::string nodeName = (const char *) elementNode->name;
        if ( nodeName == "element" )
        {
            xmlNodePtr keyNode   = XMLParser::findChildNamed( elementNode, "key");
            xmlNodePtr valueNode = XMLParser::findChildNamed( elementNode, "value");
            SLM_ASSERT("keyNode not instanced", keyNode);
            SLM_ASSERT("valueNode not instanced", valueNode);
            OSLM_INFO( "CompositeXMLTranslator::updateDataFromXML"  << BAD_CAST xmlNodeGetContent(keyNode) );

            std::string key ( (char *)xmlNodeGetContent(keyNode)) ;

            xmlNodePtr concreteValueNode = ::fwXML::XMLParser::getChildrenXMLElement(valueNode);
            SLM_ASSERT("concreteValueNode not instanced", concreteValueNode);

            ::fwData::Object::sptr valueObj;
            valueObj = Serializer().ObjectsFromXml( concreteValueNode, true );

            SLM_ASSERT("valueObj not instanced", valueObj);
            assert( ::fwData::Object::dynamicCast( valueObj ));
            cmp->getRefMap()[key] = ::fwData::Object::dynamicCast( valueObj );
        }
        elementNode = ::fwXML::XMLParser::nextXMLElement(elementNode->next);
    }
}

//------------------------------------------------------------------------------

}
