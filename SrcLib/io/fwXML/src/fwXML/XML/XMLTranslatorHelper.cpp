/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwData/Object.hpp>
#include <fwXML/visitor/accept.hpp>

#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/visitor/SerializeXML.hpp"

namespace fwXML
{

XMLTranslatorHelper::XMLTranslatorHelper()
{
    // TODO Auto-generated constructor stub
}

//------------------------------------------------------------------------------

XMLTranslatorHelper::~XMLTranslatorHelper()
{
    // TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------

xmlNodePtr XMLTranslatorHelper::MasterNode( ::fwData::Object::sptr obj )
{
    // create node with className
    xmlNodePtr node = xmlNewNode(NULL, xmlStrdup( BAD_CAST obj->getLeafClassname().c_str() ) );

    // append an unique id to its objects
    std::string id = ::fwTools::UUID::get(obj);

    xmlNewProp( node, BAD_CAST "id", xmlStrdup( BAD_CAST  id.c_str() ));
    xmlNewProp( node, BAD_CAST "class", xmlStrdup( BAD_CAST  obj->getRootedClassname().c_str() ));

    return node;
}

//------------------------------------------------------------------------------

xmlNodePtr XMLTranslatorHelper::toXML( ::fwData::Object::sptr obj )
{
    ::fwXML::XMLTranslator::sptr translator;
    translator = ::fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator  >(  obj->getRootedClassname() );

    if (translator)
    {
        return translator->getXMLFrom(obj);
    }
    else
    {
        return TrivialXMLTranslator().getXMLFrom(obj);
    }
}

//------------------------------------------------------------------------------

xmlNodePtr XMLTranslatorHelper::toXMLRecursive( ::fwData::Object::sptr obj )
{
    ::visitor::SerializeXML visitor;
    ::fwData::visitor::accept( obj , &visitor );
    return visitor.m_correspondance[ obj ];
}

//------------------------------------------------------------------------------

void XMLTranslatorHelper::fromXML( ::fwData::Object::sptr toUpdate, xmlNodePtr source )
{
    const std::string nameInXML = (const char*)source->name;
    SLM_ASSERT("XML node not correspond to object classname", toUpdate->getLeafClassname() ==  nameInXML );

    ::fwXML::XMLTranslator::sptr translator;
    translator = ::fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator  >(  toUpdate->getRootedClassname() );

    if ( translator.get() )
    {
        translator->updateDataFromXML(toUpdate,source);
    }
    else
    {
        OSLM_WARN("No XML Translator for " << toUpdate->getLeafClassname() << " Object UnModified");
    }

//    if (translator.get() )
//    {
//        translator->updateDataFromXML(toUpdate,source);
//        xmlNodePtr child = source->children;
//        bool classicObject = ( xmlStrcmp( source->name, BAD_CAST "Field" ) != 0 ) ;
//        while ( child!=NULL )
//        {
//            if ( child->type == XML_ELEMENT_NODE )
//            {
//                // normal parent object ignore children which are not Field
//                if ( classicObject &&  xmlStrcmp( child->name, BAD_CAST "Field" ) )
//                {
//                    OSLM_DEBUG( "XMLTranslatorHelper::fromXML : " << source->name << " ignoring " << child->name );
//                }
//                else
//                {
//                    OSLM_DEBUG( "XMLTranslatorHelper::fromXML : " <<  source->name << " accept " << child->name );
//                    ::fwData::Object::sptr newChild = XMLTranslatorHelper::fromXML( child );
//                    assert (newChild);
//                    toUpdate->children().push_back( newChild );
//                }
//            }
//            child = child->next;
//        }
//    }
//    else
//    {
//        OSLM_WARN("No XML Translator for " << toUpdate->getLeafClassname() << " Object UnModified");
//    }
}

//------------------------------------------------------------------------------

::fwData::Object::sptr XMLTranslatorHelper::fromXML( xmlNodePtr source )
{
    OSLM_TRACE(" fromXML(xmlNode) with xmlNode->name=" << (const char*)source->name << " addr=" << source );

    std::string className = ObjectTracker::getClassname(source);
    std::string id        = ObjectTracker::getID(source);

    // !!! NOTE HERE WE DO NOT PERFORME TRANSLATION ID OLD -> NEW !!!

    // dot not create duplicate object
    ::fwData::Object::sptr obj;
    if ( ObjectTracker::isAlreadyInstanciated( id ) )
    {
        obj = ObjectTracker::buildObject( className, id ); // use previous one
    }
    else
    {
        obj = ObjectTracker::buildObject( className, id ); // create one with a new id
        fromXML(obj,source); //then fill with xml
    }
    return obj;
}

//------------------------------------------------------------------------------

xmlNodePtr XMLTranslatorHelper::newElement( const  std::string &name, bool value )
{
    xmlNodePtr result = xmlNewNode( NULL, xmlStrdup( BAD_CAST name.c_str() )  );
    xmlNodeAddContent( result,   BAD_CAST (value?"1":"0")  );
    return result;
}


//------------------------------------------------------------------------------

bool XMLTranslatorHelper::getElement( xmlNodePtr node )
{
    std::string str = XMLParser::getTextValue (node);
    return (str == "1");
}

//------------------------------------------------------------------------------

} // end namespace
