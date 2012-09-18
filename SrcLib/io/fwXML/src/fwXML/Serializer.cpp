/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <iostream>

#include <iomanip> // for indentation
#include <exception>

#include <boost/filesystem/convenience.hpp>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwData/Object.hpp>

#include <fwTools/Failed.hpp>
#include <fwCore/Demangler.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/policy/NeverDump.hpp>
#include <fwMemory/policy/BarrierDump.hpp>
#include <fwMemory/tools/MemoryMonitorTools.hpp>

#include "fwXML/visitor/accept.hpp"

#include "fwXML/visitor/Serialize.hpp"
#include "fwXML/XML/XMLAggregator.hpp"
#include "fwXML/XML/XMLHierarchy.hpp"
#include "fwXML/XML/XMLStream.hpp"
#include "fwXML/XML/XMLPartitioner.hpp"
#include "fwXML/IFileFormatService.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/XML/XMLPatch.hpp"
#include "fwXML/ObjectTracker.hpp"
#include "fwXML/visitor/CollectFileFormatService.hpp"
#include "fwXML/validator/DataFolderValidator.hpp"

#include "fwXML/Serializer.hpp"

namespace fwXML
{

::boost::filesystem::path Serializer::m_rootFolder(".");

//------------------------------------------------------------------------------

Serializer::Serializer()
{
    ::fwMemory::BufferManager::sptr manager;
    manager = ::boost::dynamic_pointer_cast< ::fwMemory::BufferManager >( ::fwTools::IBufferManager::getCurrent() );

    if( manager )
    {
        ::fwMemory::IPolicy::sptr policy = manager->getDumpPolicy();
        if( ::boost::dynamic_pointer_cast< ::fwMemory::policy::NeverDump >( policy ) )
        {
            ::fwMemory::policy::BarrierDump::sptr newDumpPolicy = ::fwMemory::policy::BarrierDump::New();
            size_t aliveMemory = manager->getManagedBufferSize() - manager->getDumpedBufferSize();
            size_t freeMemory = ::fwMemory::tools::MemoryMonitorTools::estimateFreeMem() / 2;
            size_t barrier = std::max( aliveMemory, std::max( freeMemory, static_cast<size_t>(500L * 1024 * 1024) ) );

            newDumpPolicy->setBarrier( barrier );
            manager->setDumpPolicy( newDumpPolicy );
            m_oldPolicy = policy;
        }
    }
}

//------------------------------------------------------------------------------

Serializer::~Serializer()
{
    ::fwMemory::BufferManager::sptr manager;
    manager = ::boost::dynamic_pointer_cast< ::fwMemory::BufferManager >( ::fwTools::IBufferManager::getCurrent() );

    if( manager && m_oldPolicy )
    {
        manager->setDumpPolicy( m_oldPolicy );
        m_oldPolicy.reset();
    }
}

//------------------------------------------------------------------------------

void Serializer::setPathPolicy( IPathPolicy::sptr  newPathPolicy)
{
    SLM_TRACE_FUNC();
    XMLPartitioner::getDefault()->setPathPolicy( newPathPolicy );
}

//------------------------------------------------------------------------------

void Serializer::setSplitPolicy( ISplitPolicy::sptr  newSplitPolicy)
{
    SLM_TRACE_FUNC();
    XMLPartitioner::getDefault()->setSplitPolicy( newSplitPolicy );
}

//------------------------------------------------------------------------------

void Serializer::serialize( ::fwData::Object::sptr object, bool saveSchema) throw (::fwTools::Failed)
{
    // serialize
    std::ofstream ofs_xml( m_rootFolder.string().c_str() );

    SLM_ASSERT("object not instanced", object); // check if object is well instantiated
    OSLM_INFO( "Serializing to " <<   m_rootFolder.string().c_str() << "...." );
    m_processedXMLFile =  m_rootFolder.string();

    //reset all hierarchy informations
    ::fwXML::XMLHierarchy::getDefault()->clearAll();

    // recreate hierarchy
    ::visitor::Serialize visitor;
    ::fwData::visitor::accept( object , &visitor );

    std::set< ::fwXML::XMLAggregator::sptr > savedAggregator;
    ::fwXML::XMLHierarchy::ObjectAggregatorMap::iterator aggIter = ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().begin();

    while( aggIter != ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().end() )
    {
         ::fwData::Object::sptr obj =  aggIter->first.lock();
         ::fwXML::XMLAggregator::sptr aggregator =  aggIter->second;

        // save aggregator only once
        if ( savedAggregator.count( aggregator ) == 0 ) // not already saved
        {
            aggIter->second->rootFolder() = this->rootFolder();
            ::boost::filesystem::path filePath =  aggIter->second->getFullPath(); ;
            OSLM_DEBUG( "Saving XMLAggregator " << filePath.string() << "..." );
            m_processedXMLFile =  filePath.string();
            {
                // create directory structure if needed
                if ( filePath.parent_path().empty() == false )
                {
                    ::boost::filesystem::create_directories(filePath.parent_path());
                }
                std::ofstream outFile(  filePath.string().c_str(), std::ios::binary);
                ::fwXML::XMLStream::toStream(  aggIter->second->getXMLDoc() , outFile );
            }
            savedAggregator.insert( aggregator );
        }
        ++aggIter;
    }

    m_processedXMLFile =  m_rootFolder.string();

    if (saveSchema)// save schema if needed
    {
        DataFolderValidator::copySchema( m_rootFolder );
    }
    notifyProgress( 1.0, "Done" );
}

//------------------------------------------------------------------------------


// scan children of xmlNode :
// a) if not a Field ignore this child
// b) else createObject on this child

::fwData::Object::sptr Serializer::ObjectsFromXml( xmlNodePtr xmlNode )
{
    xmlNodePtr child = xmlNode->children;

    // the full named classname + ns
    std::string className = ObjectTracker::getClassname(xmlNode );
    if ( className.empty() )
    {
        std::string mesg = "Serializer::ObjectsFromXml : missing class attribute for xml node " + std::string( (const char*)xmlNode->name);
        mesg+= " from file " + m_processedXMLFile;
        throw ::fwTools::Failed(mesg);
    }

    SLM_ASSERT("Empty classname", !className.empty());
    std::string idXML = ObjectTracker::getID(xmlNode );

    OSLM_DEBUG("ObjectsFromXml : manage Object " << xmlNode->name );
    if ( ObjectTracker::isAlreadyInstanciated( idXML ) )
    {
        OSLM_DEBUG("ObjectsFromXml : manageObject " << xmlNode->name  << "already instantiated with idXML " << idXML);
        return ObjectTracker::buildObject( className , idXML  );
    }

     ::fwData::Object::sptr newObject = ObjectTracker::buildObject( className , idXML  );

    assert( newObject.get() );

    // classic object ignore all children expect Field
    // labeledObject manage all children
    while ( child!=NULL )
    {
        if ( child->type == XML_ELEMENT_NODE )
        {
            std::string nodeName((const char*)child->name );

            if ( nodeName == "Attributes" )
            {
                xmlNodePtr elementNode = XMLParser::getChildrenXMLElement( child );
                while (elementNode)
                {
                    std::string nodeName = (const char *) elementNode->name;
                    if ( nodeName == "element" )
                    {
                        xmlNodePtr keyNode   = XMLParser::findChildNamed( elementNode, "key");
                        xmlNodePtr valueNode = XMLParser::findChildNamed( elementNode, "value");
                        SLM_ASSERT("keyNode not instanced", keyNode);
                        SLM_ASSERT("valueNode not instanced", valueNode);
                        OSLM_INFO( "CompositeXMLTranslator::updateDataFromXML"  << BAD_CAST xmlNodeGetContent(keyNode) );

                        std::string key ( (char *)xmlNodeGetContent(keyNode)) ;

                        xmlNodePtr ConcretevalueNode = XMLParser::getChildrenXMLElement( valueNode );
                        SLM_ASSERT("ConcretevalueNode not instanced", ConcretevalueNode);

                        ::fwData::Object::sptr valueObj;
                        valueObj = this->ObjectsFromXml( ConcretevalueNode );
                        SLM_ASSERT("valueObj not instanced", valueObj);

                        OSLM_ASSERT("Sorry, attribute " << key << " already exists.", ! newObject->getField(key) );
                        newObject->setField( key, valueObj );
                    }
                    elementNode = XMLParser::nextXMLElement( elementNode->next );
                }
            }

            if ( nodeName == "DynamicAttributes" )
            {
                xmlNodePtr elementNode = XMLParser::getChildrenXMLElement( child );
                while (elementNode)
                {
                    std::string nodeName = (const char *) elementNode->name;
                    if ( nodeName == "element" )
                    {
                        xmlNodePtr keyNode   = XMLParser::findChildNamed( elementNode, "key");
                        xmlNodePtr valueNode = XMLParser::findChildNamed( elementNode, "value");
                        SLM_ASSERT("keyNode not instanced", keyNode);
                        SLM_ASSERT("valueNode not instanced", valueNode);
                        OSLM_INFO( "CompositeXMLTranslator::updateDataFromXML"  << BAD_CAST xmlNodeGetContent(keyNode) );

                        std::string key ( (char *)xmlNodeGetContent(keyNode)) ;

                        xmlNodePtr ConcretevalueNode = XMLParser::getChildrenXMLElement( valueNode );
                        SLM_ASSERT("ConcretevalueNode not instanced", ConcretevalueNode);

                        ::fwData::Object::sptr valueObj;
                        valueObj = this->ObjectsFromXml( ConcretevalueNode );
                        SLM_ASSERT("valueObj not instanced", valueObj);

                        if(newObject->hasAttribute(key))
                        {
                            newObject->setAttribute( key, valueObj );
                        }
                    }
                    elementNode = XMLParser::nextXMLElement( elementNode->next );
                }
            }

        }
        child = child->next;
    }

    // update data form XML
    ::fwXML::XMLTranslator::sptr translator = ::fwXML::factory::New( newObject->getClassname() );

    OSLM_DEBUG("ObjectsFromXml : Looking for XMLTranslator for "<< newObject->getClassname());
    if (translator.get() )
    {
        OSLM_DEBUG("ObjectsFromXml : XMLTranslator found "<< ::fwCore::Demangler(typeid(*translator)).getClassname() );
        translator->updateDataFromXML(newObject,xmlNode);
    }
    else
    {
        OSLM_ERROR("ObjectsFromXml : No XML Translator for " << newObject->className() << " use its default constructor");
    }

    OSLM_DEBUG(" ObjectsFromXml return " << newObject->className()  );

    return newObject;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr  Serializer::deSerialize( ::boost::filesystem::path filePath, bool validateWithSchema) throw (::fwTools::Failed)
{
    xmlDocPtr xmlDoc = NULL;
    xmlNodePtr xmlRoot = NULL;
    xmlKeepBlanksDefault(0);

    // get XMLDOC & root PTR
    this->rootFolder() = filePath.parent_path();
    m_processedXMLFile = filePath.string();

    xmlDoc = XMLParser::getXmlDocFromFile( filePath );
    xmlRoot = xmlDocGetRootElement (xmlDoc);

    if ( validateWithSchema )
    {
        xmlNodePtr rootToBeValidated = xmlCopyNode(xmlRoot,1);
        // validation
        DataFolderValidator validator;
        validator.collecteSchema( this->rootFolder() );
        bool validationOK = validator.validate( rootToBeValidated );
        OSLM_INFO("XML VALIDATION (1=OK) OF " <<  filePath.string() << " result=" << validationOK );
        OSLM_INFO("XML VALIDATION ERROR LOG ==BEGIN " << validator.getErrorLog() << " END==" );

        xmlFreeNode(rootToBeValidated);

        if ( validationOK == false )
        {
            std::string msg("XML Validation Failed " + filePath.string() + ": " );
            msg+= validator.getErrorLog();
            msg+= "\n NB: an apostrophe can cause problem to the XML library, so if you have one in your user name, change your TMP environment variable or contact your system administrator.";
            throw ::fwTools::Failed( msg );
        }
    }

    xmlNodePtr rootObject = NULL;
    xmlNodePtr archive =  !xmlStrcmp( xmlRoot->name, BAD_CAST "archive" )? xmlRoot: NULL;

    std::string vProtocol = "0";
    if (archive)
    {
        xmlNodePtr protocol = XMLParser::findChildNamed(archive,"protocol");
        OSLM_ASSERT( "No protocol version for archive : " << filePath.string(), protocol );
        vProtocol = XMLParser::getAttribute(protocol, "version");
        xmlNodePtr content = XMLParser::findChildNamed(archive,"content");
        OSLM_ASSERT( "No content for archive : " << filePath.string() , content );
        rootObject = XMLParser::nextXMLElement(content->children);
    }

    XMLAggregator::Version dataVersion(vProtocol);

    if (dataVersion > XMLAggregator::Version::current())
    {
        std::stringstream stream;
        stream << "Data version " << dataVersion.string()
                            << " is too recent compared to the software version ("
                            << XMLAggregator::Version::current().string() << ")\n"
                            <<"Contact rd-support@ircad.fr to have more information.";
        throw ::fwTools::Failed( stream.str() );
    }
    else if(dataVersion < XMLAggregator::Version::current())
    {
        if(dataVersion == XMLAggregator::Version(0)
            && XMLAggregator::Version::current() == XMLAggregator::Version(2) )
        {
            rootObject = xmlRoot;
            OSLM_INFO("Patch file "<<filePath.string()<<" from "<<dataVersion.string()<<" to v1");
            XMLPatch::PatchNoVersionToVersion1( rootObject );
            OSLM_INFO("Patch file "<<filePath.string()<<" from "<<dataVersion.string()<<" to v2");
            XMLPatch::PatchVersion1ToVersion2( rootObject );
        }
        else if(dataVersion == XMLAggregator::Version(1)
                && XMLAggregator::Version::current() == XMLAggregator::Version(2))
        {
            OSLM_INFO("Patch file "<<filePath.string()<<" from "<<dataVersion.string()<<" to v2");
            XMLPatch::PatchVersion1ToVersion2( rootObject );
        }
        else
        {
            std::stringstream stream;
            stream << "Data version " << dataVersion.string()
                                << " is too old compared to the software version ("
                                << XMLAggregator::Version::current().string() << ")\n"
                                <<"Contact rd-support@ircad.fr to have more information.";
            throw ::fwTools::Failed( stream.str() );
        }
    }

    ObjectTracker::clear();

    ::fwData::Object::sptr objRoot;
    objRoot = this->ObjectsFromXml( rootObject );

    // memory cleanup
    xmlFreeDoc (xmlDoc);
    ObjectTracker::clear();
    return objRoot;
}

//------------------------------------------------------------------------------

}
