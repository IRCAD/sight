#include <fwCore/base.hpp>
//#include <fwData/String.hpp>

#include "fwXML/XML/PlaneXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

PlaneXMLTranslator::PlaneXMLTranslator() {};

//------------------------------------------------------------------------------

PlaneXMLTranslator::~PlaneXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr PlaneXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{

    ::fwData::Plane::sptr pPlane = ::fwData::Plane::dynamicCast(obj);
    assert( pPlane );

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr isIntersectionNode = XMLTH::newElement("isIntersection", pPlane->getIsIntersection() );
    xmlAddChild( masterNode , isIntersectionNode );
    xmlNodePtr pointsNode =
        XMLTH::homogeneousContainerToXml("points", pPlane->getCRefPoints().begin(),  pPlane->getCRefPoints().end() );
    xmlAddChild( masterNode , pointsNode);

    return masterNode;

}

//------------------------------------------------------------------------------

void PlaneXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( toUpdate ); // object should exist
    //get its label
    ::fwData::Plane::sptr pPlane = ::fwData::Plane::dynamicCast(toUpdate);

    // If the point list is not empty
    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "isIntersection" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pPlane->setIsIntersection( ::boost::lexical_cast< bool >(  litteralValue ));
            }
            else if ( nodeName == "points" )
            {
                xmlNodePtr pointsNode = XMLParser::nextXMLElement( elementNode->children );
                int p = 0;
                while ( pointsNode )
                {
                    std::string pointName = (const char *) pointsNode->name;
                    if ( pointName == "Point" )
                    {
                        // Load Point
                        ::fwTools::Object::sptr valueObj;
                        valueObj = Serializer().ObjectsFromXml( pointsNode, true, true );

                        // Add point in the vector
                        assert( valueObj );
                        assert( ::fwData::Point::dynamicCast( valueObj ) );
                        assert(p < pPlane->getRefPoints().size());
                        pPlane->getRefPoints()[p] = ::fwData::Point::dynamicCast( valueObj ) ;
                        p++;
                    }
                    pointsNode = XMLParser::nextXMLElement( pointsNode->next );
                }
            }

            // Element ++
            elementNode = XMLParser::nextXMLElement( elementNode->next );
        }
    }
}


}
