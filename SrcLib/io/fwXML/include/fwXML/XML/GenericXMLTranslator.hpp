/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GENERICXMLTRANSLATOR_HPP_
#define GENERICXMLTRANSLATOR_HPP_

#include <iostream>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include <libxml/tree.h>

#include <fwCore/base.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"


namespace fwXML
{

template<class RealData>
class FWXML_CLASS_API GenericXMLTranslator : public XMLTranslator
{
public:

    FWXML_API GenericXMLTranslator() {};

    FWXML_API virtual ~GenericXMLTranslator() {};

    xmlNodePtr getBoostRootNode( xmlNodePtr xmlFileNodeObject )
    {
        // find all children of xmlFileNodeObject which are inserted during boost::serialization.
        // The purpose of this function is to retrieve boost node from dynamic node
        // this node will be used with fw_xmliarchive which update object information
        // by default all node which are not a labeledObject are considered as boostnode
        // we create a DummyRoot node and copy all boost node as children of DummyRoot then return this node
        // useFreeNode to release

        xmlNodePtr dummyRoot = xmlNewNode(NULL, xmlStrdup( BAD_CAST xmlFileNodeObject->name ) );

        xmlNodePtr child = xmlFileNodeObject->children;
        while ( child!=NULL )
        {
                // if a true element and ,ot a labeledObject
                if ( child->type == XML_ELEMENT_NODE &&  xmlStrcmp( child->name, BAD_CAST "Field" )!=0 )
                {
                    xmlAddChild( dummyRoot, xmlCopyNode(child,1) );
                }
                child = child->next;
        }
        return dummyRoot;
    }

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
    {
        OSLM_DEBUG("GenericXMLTranslator for " << obj->className() << "-" <<  obj.get() );

        // check obj is well realated to RealObject
        ::boost::shared_ptr<RealData> robj = boost::dynamic_pointer_cast<RealData >(obj);
        SLM_ASSERT("robj not instanced", robj);

        // create master node with className+id
        xmlNodePtr node = XMLTranslatorHelper::MasterNode( robj );


        // create data information using boost tweaked archive

        ::boost::archive::fw_xml_oarchive ao_xml(0);
        {
            ao_xml.operator&( ::boost::serialization::make_nvp("BoostManagedObject", *robj ) ); // Serialization with pointer generate an NVP (.first with NULL name !!!)
        }

        xmlNodePtr boostXML =  ao_xml.getXMLNode();
        SLM_ASSERT("boostXML not instanced", boostXML);
        boostXML = boostXML->children;
        while ( boostXML!=NULL )
        {
            if ( boostXML->type == XML_ELEMENT_NODE )
            {
                xmlAddChild( node, xmlCopyNode(boostXML,1) );
            }
            boostXML =  boostXML->next;
        }

        xmlFree( ao_xml.getXMLNode() );
        return node;
    }

    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
    {
        // TODO assertion xmlNode.name() == RealData.className();
        OSLM_DEBUG("GenericXMLTranslator::updateDataFromXML( obj " << toUpdate->className() << " XMLNode source " << source->name );

        xmlNodePtr boostRoot = getBoostRootNode(source);

        SLM_DEBUG( XMLParser::toString(boostRoot) );
        assert( dynamic_cast<RealData *>(toUpdate.get()) );
        RealData *robj=dynamic_cast<RealData *>(toUpdate.get());

        // get data information using boost tweaked archive
        ::boost::archive::fw_xml_iarchive ai_xml(0);
        ai_xml.setXMLNode(boostRoot);
        ai_xml.operator&( ::boost::serialization::make_nvp("DummyRootFORBoostNode", *robj ) );
        SLM_DEBUG("GenericXMLTranslator::updateDataFromXML DONE");
    }
};

}

#endif /*GENERICXMLTRANSLATOR_HPP_*/
