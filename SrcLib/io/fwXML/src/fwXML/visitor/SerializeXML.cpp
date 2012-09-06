/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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

namespace visitor
{

SerializeXML::SerializeXML()
{}

//-----------------------------------------------------------------------------

SerializeXML::~SerializeXML()
{}

//-----------------------------------------------------------------------------

void SerializeXML::visit( ::fwData::Object::sptr obj)
{
    SLM_ASSERT("Object is null", obj);
    OSLM_DEBUG( "SerializeXML Visitor Visiting : Class " << obj->className() <<
            "(" <<  ::fwTools::UUID::get(obj)    <<
            ") ParentClass: " <<  (m_source?m_source->className():"NULL")   <<
            "(" << (m_source?::fwTools::UUID::get(m_source):"NoSOURCENOUUID") << ")"
    );

    // get XMLTranslator
    ::fwXML::XMLTranslator::sptr translator;
    translator = ::fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator >( obj->className()  );
    if (!translator)
    {
        translator = ::fwXML::TrivialXMLTranslator::New();
    }

    // update XML
    xmlNodePtr objectXMLNode = translator->getXMLFrom(obj);

    // save fields
    if( ! obj->getFields().empty() )
    {
        xmlNodePtr attributeNode = xmlNewNode( NULL, xmlStrdup( BAD_CAST "Attributes" )  );
        xmlAddChild( objectXMLNode, attributeNode );

        BOOST_FOREACH( ::fwData::Object::FieldNameType name, obj->getFieldNames() )
        {
            ::fwData::Object::sptr objAttribute = obj->getField(name);
            if( objAttribute )
            {
                // <element key="" value="" />
                xmlNodePtr elementNode = xmlNewNode(NULL, BAD_CAST "element");
                xmlAddChild(attributeNode, elementNode);

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

    // save DynamicAttributes
    if( ! obj->getAttributeNames().empty() )
    {
        xmlNodePtr dynAttributeNode = xmlNewNode( NULL, xmlStrdup( BAD_CAST "DynamicAttributes" )  );
        xmlAddChild(objectXMLNode, dynAttributeNode);
        BOOST_FOREACH( ::fwData::Object::AttrNameType name, obj->getAttributeNames() )
        {
            ::fwData::Object::sptr objAttribute = obj->getAttribute(name);
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
    // keep correspondence
    m_correspondance[obj] = objectXMLNode;
}

//-----------------------------------------------------------------------------

}

