/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Composite.hpp>

#include "fwXML/XML/StructureTraitsDictionaryXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

StructureTraitsDictionaryXMLTranslator::StructureTraitsDictionaryXMLTranslator()
{}

//------------------------------------------------------------------------------

StructureTraitsDictionaryXMLTranslator::~StructureTraitsDictionaryXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr StructureTraitsDictionaryXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::StructureTraitsDictionary::sptr structureDico = ::fwData::StructureTraitsDictionary::dynamicCast(obj);
    SLM_ASSERT("StructureTraitsDictionary not instanced", structureDico);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );


    ::fwData::Composite::NewSptr compo;
    std::vector<std::string> structureTypes = structureDico->getStructureTypeNames();
    BOOST_FOREACH(std::string type, structureTypes)
    {
        ::fwData::StructureTraits::sptr structure = structureDico->getStructure(type);
        (*compo)[type] = structure;
    }

    xmlNodePtr compositeNode = XMLTH::toXMLRecursive(compo);
    xmlAddChild( masterNode, compositeNode);

    return masterNode;
}

//------------------------------------------------------------------------------

void StructureTraitsDictionaryXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);

    //get its label
    ::fwData::StructureTraitsDictionary::sptr structureDico = ::fwData::StructureTraitsDictionary::dynamicCast(toUpdate);

    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;

            if ( nodeName == "Composite" )
            {
                ::fwTools::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( elementNode, true );
                SLM_ASSERT("valueObj not instanced", valueObj);
                ::fwData::Composite::sptr compo = ::fwData::Composite::dynamicCast(valueObj);
                SLM_ASSERT("composite not instanced", compo);

                BOOST_FOREACH(::fwData::Composite::value_type item, *compo)
                {
                    ::fwData::StructureTraits::sptr structure = ::fwData::StructureTraits::dynamicCast(item.second);
                    structureDico->addStructure(structure);
                }
            }
            // Element ++
            elementNode = ::fwXML::XMLParser::nextXMLElement(elementNode->next);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace fwXML
