/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Composite.hpp>

#include "fwXML/XML/StructureTraitsXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

StructureTraitsXMLTranslator::StructureTraitsXMLTranslator()
{}

//------------------------------------------------------------------------------

StructureTraitsXMLTranslator::~StructureTraitsXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr StructureTraitsXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::StructureTraits::sptr structure = ::fwData::StructureTraits::dynamicCast(obj);
    SLM_ASSERT("StructureTraits not instanced", structure);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr typeNode = XMLTH::newElement("type", structure->getType() );
    xmlAddChild( masterNode , typeNode );
    xmlNodePtr classNode = XMLTH::newElement("class", structure->getClass() );
    xmlAddChild( masterNode , classNode );
    xmlNodePtr nativeExpNode = XMLTH::newElement("nativeExp", structure->getNativeExp() );
    xmlAddChild( masterNode , nativeExpNode );
    xmlNodePtr nativeGeoExpNode = XMLTH::newElement("nativeGeometricExp", structure->getNativeGeometricExp() );
    xmlAddChild( masterNode , nativeGeoExpNode );
    xmlNodePtr attachmentNode = XMLTH::newElement("attachmentType", structure->getAttachmentType() );
    xmlAddChild( masterNode , attachmentNode );

    xmlNodePtr categoriesNode =
            XMLTH::PODContainerToXml("categories", structure->getCRefCategories().begin(), structure->getCRefCategories().end());
    xmlAddChild(masterNode, categoriesNode);

    ::fwData::Composite::NewSptr compo;
    (*compo)["color"] = structure->getColor();

    xmlNodePtr compositeNode = XMLTH::toXMLRecursive(compo);
    xmlAddChild( masterNode, compositeNode);

    return masterNode;
}

//------------------------------------------------------------------------------

void StructureTraitsXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);

    //get its label
    ::fwData::StructureTraits::sptr structure = ::fwData::StructureTraits::dynamicCast(toUpdate);

    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "type" )
            {
                structure->setType( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "class" )
            {
                int structClass = ::boost::lexical_cast< int >(XMLParser::getTextValue(elementNode) );
                structure->setClass( static_cast< ::fwData::StructureTraits::StructureClass >(structClass) );
            }
            else if ( nodeName == "nativeExp" )
            {
                structure->setNativeExp( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "nativeGeometricExp" )
            {
                structure->setNativeGeometricExp( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "attachmentType" )
            {
                structure->setAttachmentType( XMLParser::getTextValue(elementNode) );
            }
            else if ( nodeName == "categories" )
            {
                std::vector <int > vecCat;
                ::fwData::StructureTraits::CategoryContainer categories;
                XMLTH::PODcontainerFromXml(elementNode, std::back_inserter(vecCat));
                BOOST_FOREACH(int cat, vecCat )
                {
                    ::fwData::StructureTraits::Category category = static_cast< ::fwData::StructureTraits::Category >(cat);
                    categories.push_back(category);
                }
                structure->setCategories(categories);
            }
            else if ( nodeName == "Composite" )
            {
                ::fwData::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( elementNode, true );
                SLM_ASSERT("valueObj not instanced", valueObj);
                ::fwData::Composite::sptr compo = ::fwData::Composite::dynamicCast(valueObj);
                SLM_ASSERT("composite not instanced", compo);

                structure->setColor(::fwData::Color::dynamicCast((*compo)["color"]) );
            }
            // Element ++
            elementNode = ::fwXML::XMLParser::nextXMLElement(elementNode->next);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace fwXML
