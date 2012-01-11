/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Composite.hpp>

#include "fwXML/XML/ReconstructionTraitsXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

ReconstructionTraitsXMLTranslator::ReconstructionTraitsXMLTranslator()
{}

//------------------------------------------------------------------------------

ReconstructionTraitsXMLTranslator::~ReconstructionTraitsXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr ReconstructionTraitsXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::ReconstructionTraits::sptr roi = ::fwData::ReconstructionTraits::dynamicCast(obj);
    SLM_ASSERT("ReconstructionTraits not instanced", roi);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr idNode = XMLTH::newElement("identifier", roi->getIdentifier() );
    xmlAddChild( masterNode , idNode );

    ::fwData::Composite::NewSptr compo;
    (*compo)["structureTraits"] = roi->getStructureTraits();

    if (roi->getMaskOpNode())
    {
        (*compo)["maskOpNode"] = roi->getMaskOpNode();
    }
    if (roi->getMeshOpNode())
    {
        (*compo)["meshOpNode"] = roi->getMeshOpNode();
    }

    xmlNodePtr compositeNode = XMLTH::toXMLRecursive(compo);
    xmlAddChild( masterNode, compositeNode);

    return masterNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraitsXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);

    //get its label
    ::fwData::ReconstructionTraits::sptr roi = ::fwData::ReconstructionTraits::dynamicCast(toUpdate);

    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "identifier" )
            {
                roi->setIdentifier( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "Composite" )
            {
                ::fwTools::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( elementNode, true );
                SLM_ASSERT("valueObj not instanced", valueObj);
                ::fwData::Composite::sptr compo = ::fwData::Composite::dynamicCast(valueObj);
                SLM_ASSERT("composite not instanced", compo);

                roi->setStructureTraits(::fwData::StructureTraits::dynamicCast((*compo)["structureTraits"]));

                if (compo->find("maskOpNode") != compo->end())
                {
                    roi->setMaskOpNode(::fwData::Node::dynamicCast((*compo)["maskOpNode"]));
                }
                if (compo->find("meshOpNode") != compo->end())
                {
                    roi->setMeshOpNode(::fwData::Node::dynamicCast((*compo)["meshOpNode"]));
                }
            }
            // Element ++
            elementNode = ::fwXML::XMLParser::nextXMLElement(elementNode->next);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace fwXML
