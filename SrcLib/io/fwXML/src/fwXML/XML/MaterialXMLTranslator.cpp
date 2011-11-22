/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <stdio.h>
#include <string.h>

#include "fwXML/Serializer.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/IFileFormatService.hpp"

#include "fwXML/boostSerializer/Material.hpp"

#include "fwXML/XML/MaterialXMLTranslator.hpp"

namespace fwXML
{

MaterialXMLTranslator::MaterialXMLTranslator()
{}

//------------------------------------------------------------------------------

MaterialXMLTranslator::~MaterialXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr MaterialXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::Material::sptr mat = ::fwData::Material::dynamicCast(obj);

    GenericXMLTranslator< ::fwData::Material > material2xmlbase;
    SLM_ASSERT("Material not instanced", mat);

    xmlNodePtr masterNode = material2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("node not instanced", masterNode);

    xmlNodePtr ambient = XMLTH::toXMLRecursive(mat->ambient());
    xmlNodePtr ambientNode = xmlNewNode(NULL, BAD_CAST "ambient");
    xmlAddChild( ambientNode, ambient);

    xmlNodePtr diffuse = XMLTH::toXMLRecursive(mat->diffuse());
    xmlNodePtr diffuseNode = xmlNewNode(NULL, BAD_CAST "diffuse");
    xmlAddChild( diffuseNode, diffuse);

    xmlAddChild( masterNode , ambientNode);
    xmlAddChild( masterNode , diffuseNode);

    return masterNode;
}

//------------------------------------------------------------------------------

void MaterialXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::Material::sptr mat = ::fwData::Material::dynamicCast(toUpdate);

    GenericXMLTranslator< ::fwData::Material > material2xmlbase;
    material2xmlbase.updateDataFromXML(toUpdate, source);

    xmlNodePtr ambientNode = XMLParser::findChildNamed( source, std::string("ambient") );
    SLM_ASSERT("ambientNode not instanced", ambientNode);
    xmlNodePtr diffuseNode = XMLParser::findChildNamed( source, std::string("diffuse") );
    SLM_ASSERT("diffuseNode not instanced", diffuseNode);
    xmlNodePtr cAmbientNode = xmlNextElementSibling( ambientNode->children );
    SLM_ASSERT("cAmbientNode not instanced", cAmbientNode);
    xmlNodePtr cDiffuseNode = xmlNextElementSibling( diffuseNode->children );
    SLM_ASSERT("cDiffuseNode not instanced", cDiffuseNode);

    ::fwTools::Object::sptr valueObj;
    valueObj = Serializer().ObjectsFromXml( cAmbientNode, true );
    mat->setAmbient(::fwData::Color::dynamicCast(valueObj));
    valueObj = Serializer().ObjectsFromXml( cDiffuseNode, true );
    mat->setDiffuse(::fwData::Color::dynamicCast(valueObj));
}

//------------------------------------------------------------------------------

}
