/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
//#include <fwData/String.hpp>

#include "fwXML/XML/PointListXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

PointListXMLTranslator::PointListXMLTranslator() {};

//------------------------------------------------------------------------------

PointListXMLTranslator::~PointListXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr PointListXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{

    ::fwData::PointList::sptr pPointList = ::fwData::PointList::dynamicCast(obj);
    SLM_ASSERT("pPointList not instanced", pPointList);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    // Save points
    for (   std::vector< ::fwData::Point::sptr >::iterator pointIter = pPointList->getRefPoints().begin();
            pointIter != pPointList->getRefPoints().end();
            ++pointIter )
    {
        xmlNodePtr elementNode = xmlNewNode(NULL, BAD_CAST "element");
        xmlAddChild(masterNode, elementNode);

        xmlNodePtr pointNode = XMLTranslatorHelper::toXMLRecursive( *pointIter );
        xmlAddChild(elementNode, pointNode);
    }

    return masterNode;

}

//------------------------------------------------------------------------------

void PointListXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate); // object should exist
    //get its label
    ::fwData::PointList::sptr pPointList = ::fwData::PointList::dynamicCast(toUpdate);
    pPointList->getRefPoints().clear();

    // If the point list is not empty
    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "element" )
            {
                xmlNodePtr ConcretevalueNode = XMLParser::nextXMLElement( elementNode->children );
                SLM_ASSERT("ConcretevalueNode not instanced", ConcretevalueNode);

                // Load Point
                ::fwTools::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( ConcretevalueNode, true );

                // Add point in the vector
                SLM_ASSERT("valueObj not instanced", valueObj);
                assert( ::fwData::Point::dynamicCast( valueObj ) );
                pPointList->getRefPoints().push_back( ::fwData::Point::dynamicCast( valueObj ) );
            }

            // Element ++
            elementNode = XMLParser::nextXMLElement( elementNode->next );
        }
    }

}


}
