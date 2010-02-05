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
    assert( pPointList );

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
    assert( toUpdate ); // object should exist
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
                assert( ConcretevalueNode );

                // Load Point
                ::fwTools::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( ConcretevalueNode, true, true );

                // Add point in the vector
                assert( valueObj );
                assert( ::fwData::Point::dynamicCast( valueObj ) );
                pPointList->getRefPoints().push_back( ::fwData::Point::dynamicCast( valueObj ) );
            }

            // Element ++
            elementNode = XMLParser::nextXMLElement( elementNode->next );
        }
    }

}


}
