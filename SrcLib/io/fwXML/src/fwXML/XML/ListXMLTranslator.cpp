/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/String.hpp>

#include "fwXML/XML/ListXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//-----------------------------------------------------------------------------

ListXMLTranslator::ListXMLTranslator() {};

//-----------------------------------------------------------------------------

ListXMLTranslator::~ListXMLTranslator() {};

//-----------------------------------------------------------------------------

xmlNodePtr ListXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::List::sptr myList = ::fwData::List::dynamicCast(obj);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    // create node for container
    xmlNodePtr containerNode = XMLTH::homogeneousContainerToXml("container", myList->getRefContainer().begin(),  myList->getRefContainer().end() );
    xmlAddChild( masterNode , containerNode);

    return masterNode;
}

//-----------------------------------------------------------------------------

void ListXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate); // object should exist

    //get its label
    ::fwData::List::sptr myList = ::fwData::List::dynamicCast(toUpdate);
    myList->getRefContainer().clear();

    // If the point list is not empty
    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "container" )
            {
                if ( elementNode->children != NULL )
                {
                    xmlNodePtr containerNode = XMLParser::nextXMLElement( elementNode->children );
                    while ( containerNode )
                    {
                        // Load Object
                        ::fwTools::Object::sptr valueObj = Serializer().ObjectsFromXml( containerNode, true );
                        SLM_ASSERT("valueObj not instanced", valueObj);
                        assert( ::fwData::Object::dynamicCast( valueObj ) );
                        myList->getRefContainer().push_back( ::fwData::Object::dynamicCast( valueObj ) );

                        containerNode = XMLParser::nextXMLElement( containerNode->next );
                    }
                }
            }

            // Element ++
            elementNode = XMLParser::nextXMLElement( elementNode->next );
        }
    }
}

//-----------------------------------------------------------------------------

}
