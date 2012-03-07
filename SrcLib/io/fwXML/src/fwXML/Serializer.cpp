/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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
#include <fwTools/Factory.hpp>
#include <fwTools/Failed.hpp>
#include <fwCore/Demangler.hpp>
#include <fwData/visitor/accept.hpp>

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

// an handler helper which transform a single file progress to a group of IO Operation
struct HandlerHelper : public ::boost::signals::trackable
{
    void operator()(float OnFilePercent, std::string filename)
    {
        OSLM_DEBUG( "HandlerHelper filename=" << filename << " "<< OnFilePercent << "%" );

        m_serializer->notifyProgress( (OnFilePercent + m_currentStep)/m_nbSteps , filename );
    }

    Serializer *m_serializer;
    int m_currentStep;
    int m_nbSteps;
};

//------------------------------------------------------------------------------

void Serializer::IOforExtraXML( ::fwTools::Object::sptr object , bool savingMode)
{
    SLM_TRACE_FUNC();

    ::visitor::CollectFileFormatService collector;
    ::fwData::visitor::accept( object , &collector );

    HandlerHelper handlerHelper;
    handlerHelper.m_serializer = this;
    handlerHelper.m_currentStep = 0;
    handlerHelper.m_nbSteps = collector.m_objWithFileFormatService.size();

    try
    {
        BOOST_FOREACH(::visitor::CollectFileFormatService::MapObjectFileFormatService::value_type elem, collector.m_objWithFileFormatService)
        {
            ::fwXML::IFileFormatService::sptr filedata = elem.second;
            OSLM_ASSERT("No IFileFormatService found for Object "<<elem.first->getID(), ::fwServices::OSR::has(elem.first, "::fwXML::IFileFormatService"));
            filedata->rootFolder() = this->rootFolder();
            ::boost::filesystem::path filePath =  filedata->getFullPath() ;
            std::string msg = savingMode?"saving":"loading";
            OSLM_DEBUG( msg<< " extraXML for " << elem.first->className() << "-" << object.get() << "filename=" << filePath.string() << std::endl );

            //notifyProgress( currentStep*1.0/nbSteps,  filePath.string() );

            filedata->addHandler( handlerHelper );
            savingMode ? filedata->save() : filedata->load() ;
            // remove IFileFormatService in OSR
            ::fwServices::OSR::unregisterService(filedata);
            handlerHelper.m_currentStep++;
        }
    }
    catch (const std::exception & e)
    {
        OSLM_ERROR("Exception : " << e.what());
        BOOST_FOREACH(::visitor::CollectFileFormatService::MapObjectFileFormatService::value_type elem, collector.m_objWithFileFormatService)
        {
            ::fwXML::IFileFormatService::sptr filedata = elem.second;
            if (::fwServices::OSR::has(elem.first, "::fwXML::IFileFormatService"))
            {
                // remove IFileFormatService in OSR
                ::fwServices::OSR::unregisterService(filedata);
            }
        }
        throw;
    }

    notifyProgress(1.0,"Done");
}

//------------------------------------------------------------------------------

Serializer::Serializer()
{}

//------------------------------------------------------------------------------

Serializer::~Serializer()
{}

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

int nbObjectHavingFileFormatService()
{
    int nbObjects = 0;
    ::fwXML::XMLHierarchy::ObjectAggregatorMap::iterator aggIter;
    aggIter= ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().begin();
    while( aggIter != ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().end() )
    {
        ::fwTools::Object::sptr obj =  aggIter->first.lock();
        if ( obj && ::fwServices::OSR::has(obj, "::fwXML::IFileFormatService") )
        {
            nbObjects++;
        }
        ++aggIter;
    }
    return nbObjects;
}

//------------------------------------------------------------------------------

void Serializer::serialize( ::fwTools::Object::sptr object, bool saveSchema) throw (::fwTools::Failed)
{
    // serialize
    std::ofstream ofs_xml( m_rootFolder.string().c_str() );

    SLM_ASSERT("object not instanced", object); // check if object is well instanciated
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
         ::fwTools::Object::sptr obj =  aggIter->first.lock();
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

    // save extra XML
    IOforExtraXML( object, true );

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

::fwTools::Object::sptr Serializer::ObjectsFromXml( xmlNodePtr xmlNode, bool loadExtraXML  )
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

     ::fwTools::Object::sptr newObject = ObjectTracker::buildObject( className , idXML  );
    // warning do not duplicate FIELDS
    newObject->children().clear();

    assert( newObject.get() );

    bool classicObject = ( xmlStrcmp( xmlNode->name, BAD_CAST "Field" ) != 0 ) ;
    ::fwData::Object::sptr newDataObject = ::fwData::Object::dynamicCast( newObject );
    // classic object ignore all children expect Field
    // labeledObject manage all children
    while ( child!=NULL )
    {
        if ( child->type == XML_ELEMENT_NODE )
        {
            std::string nodeName((const char*)child->name );
            // normal parent object ignore chlidren which are not Field
            OSLM_DEBUG_IF( "ObjectsFromXml : " << xmlNode->name << " ignoring " << child->name, classicObject &&  nodeName != "Field" );
            if (!classicObject || nodeName == "Field")
            {
                OSLM_DEBUG( "ObjectsFromXml : " <<  xmlNode->name << " accept " << child->name );
                ::fwTools::Object::sptr newChild = this->ObjectsFromXml( child, loadExtraXML );
                assert (newChild);
                newObject->children().push_back( newChild );
            }

            if ( nodeName == "DynamicAttributes" && newDataObject)
            {
                xmlNodePtr elementNode = xmlNextElementSibling(child->children);
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

                        xmlNodePtr ConcretevalueNode = xmlNextElementSibling( valueNode->children );
                        SLM_ASSERT("ConcretevalueNode not instanced", ConcretevalueNode);

                        ::fwTools::Object::sptr valueObj;
                        valueObj = this->ObjectsFromXml( ConcretevalueNode, true );

                        SLM_ASSERT("valueObj not instanced", valueObj);
                        assert( ::fwData::Object::dynamicCast( valueObj ));

                        if(newDataObject->hasAttribute(key))
                        {
                            newDataObject->setAttribute(key, ::fwData::Object::dynamicCast( valueObj ));
                        }
                    }
                    elementNode = xmlNextElementSibling(elementNode->next);
                }
            }
        }
        child = child->next;
    }

    // update data form XML
    ::fwXML::XMLTranslator::sptr translator = ::fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator  >(  newObject->getClassname()  );

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

    ::fwTools::Field::sptr  tmp = ::fwTools::Field::dynamicCast(newObject);
    OSLM_DEBUG(" ObjectsFromXml return " << newObject->className()  );
    OSLM_DEBUG_IF(" with label= " << tmp->label(), tmp);

    return newObject;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr  Serializer::deSerialize( ::boost::filesystem::path filePath , bool loadExtraXML , bool validateWithSchema   ) throw (::fwTools::Failed)
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

    ::fwTools::Object::sptr objRoot;
    objRoot = this->ObjectsFromXml( rootObject, loadExtraXML );

    if (loadExtraXML)
    {
        try
        {
            this->IOforExtraXML( objRoot, false );
        }
        catch (const std::exception & e)
        {
            OSLM_ERROR("Exception : " << e.what());
            // memory cleanup
            xmlFreeDoc (xmlDoc);
            ObjectTracker::clear();
            throw;
        }
    }
    // memory cleanup
    xmlFreeDoc (xmlDoc);
    ObjectTracker::clear();
    return objRoot;
}

//------------------------------------------------------------------------------

}
