/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/VectorXMLTranslator.hpp"
#include <fwData/String.hpp>

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwCore/base.hpp"

#include "fwXML/Serializer.hpp"

namespace fwXML
{

//-----------------------------------------------------------------------------

VectorXMLTranslator::VectorXMLTranslator() {};

//-----------------------------------------------------------------------------

VectorXMLTranslator::~VectorXMLTranslator() {};

//-----------------------------------------------------------------------------

xmlNodePtr VectorXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    ::fwData::Vector::sptr myVector = ::fwData::Vector::dynamicCast(obj);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    // create node for container
    xmlNodePtr containerNode = XMLTH::homogeneousContainerToXml("container", myVector->getRefContainer().begin(),  myVector->getRefContainer().end() );
    xmlAddChild( masterNode , containerNode);

    return masterNode;
}

//-----------------------------------------------------------------------------

void VectorXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( toUpdate ); // object should exist

    //get its label
    ::fwData::Vector::sptr myVector = ::fwData::Vector::dynamicCast(toUpdate);
    myVector->getRefContainer().clear();

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
                        ::fwTools::Object::sptr valueObj = Serializer().ObjectsFromXml( containerNode, true  );
                        assert( valueObj );
                        assert( ::fwData::Object::dynamicCast( valueObj ) );
                        myVector->getRefContainer().push_back( ::fwData::Object::dynamicCast( valueObj ) );

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
