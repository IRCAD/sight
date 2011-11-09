/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>

#include <fwDataIO/writer/IObjectWriter.hpp>

#include <libxml/tree.h>

#include "fwXML/visitor/SerializeXML.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/IFileFormatService.hpp"

namespace visitor
{

SerializeXML::SerializeXML()
{}

//-----------------------------------------------------------------------------

SerializeXML::~SerializeXML()
{}

//-----------------------------------------------------------------------------

void SerializeXML::visit( ::fwTools::Object::sptr obj)
{
    SLM_ASSERT("Object is null", obj);
    bool supportFileFormatSrv =  ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),  "::fwXML::IFileFormatService");
    OSLM_DEBUG( "SerializeXML Visitor Visiting : Class " << obj->className() <<
            "(" <<  ::fwTools::UUID::get(obj)    <<
            ") Support<FileFormatService>" <<  (supportFileFormatSrv?"yes":"no") <<
            "ParentClass: " <<  (m_source?m_source->className():"NULL")   <<
            "(" << (m_source?::fwTools::UUID::get(m_source):"NoSOURCENOUUID") << ")"
    );

    // get XMLTranslator
    ::fwXML::XMLTranslator::sptr translator;
    translator = ::fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator >( obj->className()  );
    if (!translator)
    {
        translator = ::fwXML::TrivialXMLTranslator::New();
    }

    if ( supportFileFormatSrv )
    {
        ::fwXML::IFileFormatService::sptr  saver;
        std::vector< ::fwXML::IFileFormatService::sptr > filesSrv = ::fwServices::OSR::getServices< ::fwXML::IFileFormatService >(obj);
        if( filesSrv.empty() )
        {
            std::string defaultImpl = ::fwServices::registry::ServiceFactory::getDefault()->getDefaultImplementationIdFromObjectAndType(obj->getClassname(), "::fwXML::IFileFormatService");
            saver = ::fwServices::add< ::fwXML::IFileFormatService >(obj, defaultImpl);
        }
        else
        {
            saver = filesSrv.at(0);
        }
        saver->filename() = obj->getLeafClassname() + "_" + ::fwTools::UUID::get(obj);
        saver->extension() = saver->getWriter()->extension();
    }

    // update XML
    xmlNodePtr objectXMLNode = translator->getXMLFrom(obj);

    // save DynamicAttributes
    ::fwData::Object::sptr dataObject = ::fwData::Object::dynamicCast(obj);
    if(dataObject && !dataObject->getAttributeNames().empty())
    {
        xmlNodePtr dynAttributeNode = xmlNewNode( NULL, xmlStrdup( BAD_CAST "DynamicAttributes" )  );
        xmlAddChild(objectXMLNode, dynAttributeNode);
        BOOST_FOREACH( ::fwData::Object::AttrNameType name, dataObject->getAttributeNames() )
        {
            ::fwData::Object::sptr objAttribute = dataObject->getAttribute(name);
            if(objAttribute)
            {
                xmlNodePtr elementNode = xmlNewNode(NULL, BAD_CAST "element");
                xmlAddChild(dynAttributeNode, elementNode);

                xmlNodePtr keyNode = xmlNewNode(NULL, BAD_CAST "key");
                xmlNodeAddContent( keyNode,  xmlStrdup( BAD_CAST name.c_str() ) );
                xmlAddChild(elementNode, keyNode);

                xmlNodePtr valueNode = xmlNewNode(NULL, BAD_CAST "value");
                xmlNodePtr trueValueNode = ::fwXML::XMLTranslatorHelper::toXMLRecursive(objAttribute);
                xmlAddChild(elementNode, valueNode);
                xmlAddChild(valueNode, trueValueNode);
            }
        }
    }
    if ( m_correspondance[m_source] ) //manage the root
    {
        xmlAddChild(m_correspondance[m_source] , objectXMLNode );
    }
    // keep correspondance
    m_correspondance[obj] = objectXMLNode;
}

//-----------------------------------------------------------------------------

}

