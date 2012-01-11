/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Composite.hpp>

#include "fwXML/XML/ROITraitsXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

ROITraitsXMLTranslator::ROITraitsXMLTranslator()
{}

//------------------------------------------------------------------------------

ROITraitsXMLTranslator::~ROITraitsXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr ROITraitsXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::ROITraits::sptr rec = ::fwData::ROITraits::dynamicCast(obj);
    SLM_ASSERT("ROITraits not instanced", rec);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr idNode = XMLTH::newElement("identifier", rec->getIdentifier() );
    xmlAddChild( masterNode , idNode );
    xmlNodePtr expNode = XMLTH::newElement("evaluatedExp", rec->getEvaluatedExp() );
    xmlAddChild( masterNode , expNode );

    ::fwData::Composite::NewSptr compo;
    (*compo)["structureTraits"] = rec->getStructureTraits();

    if (rec->getMaskOpNode())
    {
        (*compo)["maskOpNode"] = rec->getMaskOpNode();
    }



    xmlNodePtr compositeNode = XMLTH::toXMLRecursive(compo);
    xmlAddChild( masterNode, compositeNode);

    return masterNode;
}

//------------------------------------------------------------------------------

void ROITraitsXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);

    //get its label
    ::fwData::ROITraits::sptr rec = ::fwData::ROITraits::dynamicCast(toUpdate);

    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "identifier" )
            {
                rec->setIdentifier( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "evaluatedExp" )
            {
                rec->setEvaluatedExp( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "Composite" )
            {
                ::fwTools::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( elementNode, true );
                SLM_ASSERT("valueObj not instanced", valueObj);
                ::fwData::Composite::sptr compo = ::fwData::Composite::dynamicCast(valueObj);
                SLM_ASSERT("composite not instanced", compo);

                rec->setStructureTraits(::fwData::StructureTraits::dynamicCast((*compo)["structureTraits"]));

                if (compo->find("maskOpNode") != compo->end())
                {
                    rec->setMaskOpNode(::fwData::Node::dynamicCast((*compo)["maskOpNode"]));
                }

            }
            // Element ++
            elementNode = ::fwXML::XMLParser::nextXMLElement(elementNode->next);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace fwXML
