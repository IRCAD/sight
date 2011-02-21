/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <iostream>

#include <iomanip> // for indentation

#include <boost/filesystem/convenience.hpp>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwData/Object.hpp>
#include <fwTools/Factory.hpp>
#include <fwTools/Failed.hpp>
#include <fwCore/Demangler.hpp>
#include <fwData/visitor/accept.hpp>

#include "fwXML/Serializer.hpp"
#include "fwXML/visitor/Serialize.hpp"
#include "fwXML/XML/XMLAggregator.hpp"
#include "fwXML/XML/XMLHierarchy.hpp"
#include "fwXML/XML/XMLStream.hpp"
#include "fwXML/XML/XMLPartitioner.hpp"
#include "fwXML/IFileFormatService.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/ObjectTracker.hpp"
#include "fwXML/visitor/CollectFileFormatService.hpp"
#include "fwXML/validator/DataFolderValidator.hpp"


namespace fwXML
{


std::string ProcessedXMLFile = std::string();

boost::filesystem::path Serializer::m_rootFolder(".");

//------------------------------------------------------------------------------

void PatchNoVersionToNewData( xmlNodePtr node )
{
    if (node &&  node->type == XML_ELEMENT_NODE)
    {
        std::string NodeName( (const char*)node->name );

        //////////////////////
        // renaming node name
        //////////////////////
        if ( NodeName == "StringField")  { xmlNodeSetName(node, BAD_CAST "String");  }
        if ( NodeName == "FloatField")   { xmlNodeSetName(node, BAD_CAST "Float");   }
        if ( NodeName == "IntegerField") { xmlNodeSetName(node, BAD_CAST "Integer"); }
        if ( NodeName == "BooleanField") { xmlNodeSetName(node, BAD_CAST "Boolean"); }


        ////////////////////////////////////////////////////////////////////////
        // manage extraXML : search for  <Buffer> sub node with attribute protocol="::DataIO:: -> ::fwDataIO::
        ///////////////////////////////////////////////////////////////////////
        std::string protocolValue;
        xmlNodePtr bufferNode;
        try
        {
            bufferNode = XMLParser::findChildNamed( node, "Buffer");
            if ( bufferNode )
            {
                protocolValue = XMLParser::getAttribute (bufferNode, "protocol");
            }
        }
        catch ( ::fwTools::Failed ef)
        {
            OSLM_TRACE(" no protocol class for node->name=" << NodeName )
        }
        // if protocol exist and contain ::dataIO:: replace it by fwDataIO
        if ( !protocolValue.empty() &&  ( protocolValue.find("::dataIO::") != std::string::npos ) )
        {
            protocolValue = protocolValue.replace(0,strlen("::dataIO::"),"::fwDataIO::");
            xmlSetProp( bufferNode , BAD_CAST "protocol",  xmlStrdup( BAD_CAST protocolValue.c_str() ) );
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // manage class attribut : adapt "::data::" to "::fwData::" and manage {String,Float,Interger}Field renaming
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string className,newClassName;
        try
        {
            className = XMLParser::getAttribute (node, "class");
        }
        catch ( ::fwTools::Failed ef)
        {
            OSLM_TRACE(" no attrib class for node->name=" << NodeName )
        }
        OSLM_INFO( "PatchNoVersionToNewData nodeName=" << NodeName << " className=" << className);




        // renaming class attribute if required
        if ( !className.empty() )
        {
            // special case for Object class
            if ( className.find("::data::Object") != std::string::npos )
            {
                newClassName = "::fwTools::Object";
            }
            else if ( className.find("::data::") != std::string::npos ) // general case (namespace)
            {
                newClassName = className.replace(0,strlen("::data::"),"::fwData::");
            }

            // specific className : StringField, FloatField, IntegerField
            if ( className.find("StringField") != std::string::npos )
            {
                newClassName = className.replace( className.find("StringField"), strlen("StringField"), "String" );
            }
            else if ( className.find("FloatField") != std::string::npos )
            {
                newClassName = className.replace( className.find("FloatField"), strlen("FloatField"), "Float" );
            }
            else if ( className.find("IntegerField") != std::string::npos )
            {
                newClassName = className.replace( className.find("IntegerField"), strlen("IntegerField"), "Integer" );
            }
            else if ( className.find("BooleanField") != std::string::npos )
            {
                newClassName = className.replace( className.find("BooleanField"), strlen("BooleanField"), "Boolean" );
            }
        }
        // change needed ?
        if ( !newClassName.empty() )
        {
            OSLM_DEBUG( "PatchNoVersionToNewData NEWclassName=" << newClassName );
            assert( xmlHasProp( node, BAD_CAST "class" ) );
            xmlSetProp( node , BAD_CAST "class",  xmlStrdup( BAD_CAST newClassName.c_str() ) );
        }

        // continue parsing to child
        node = node->children;
        while ( node )
        {
            PatchNoVersionToNewData(node);
            node = node->next;
        }
    }
}

//------------------------------------------------------------------------------

// an handler helper which transform a single file progress to a group of IO Operation
struct HandlerHelper : public boost::signals::trackable
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

    ::visitor::CollectFileFormatService::MapObjectFileFormatService::iterator iter;
    for ( iter= collector.m_objWithFileFormatService.begin(); iter != collector.m_objWithFileFormatService.end(); ++iter )
    {
         ::fwXML::IFileFormatService::sptr filedata = iter->second;
        OSLM_ASSERT("No IFileFormatService found for Object "<<iter->first->getID(), ::fwServices::OSR::has(iter->first, "::fwXML::IFileFormatService"));
        filedata->rootFolder() = this->rootFolder();
        boost::filesystem::path filePath =  filedata->getFullPath() ;
        std::string msg = savingMode?"saving":"loading";
        OSLM_DEBUG( msg<< " extraXML for " << iter->first->className() << "-" << object.get() << "filename=" << filePath.string() << std::endl );

        //notifyProgress( currentStep*1.0/nbSteps,  filePath.string() );

        filedata->addHandler( handlerHelper );
        savingMode ? filedata->save() : filedata->load() ;
        // remove IFileFormatService in OSR
        ::fwServices::ObjectServiceRegistry::unregisterService(filedata);
        handlerHelper.m_currentStep++;
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

void Serializer::setPathPolicy( ::boost::shared_ptr< IPathPolicy>  newPathPolicy)
{
    SLM_TRACE_FUNC();
    XMLPartitioner::getDefault()->setPathPolicy( newPathPolicy );
}

//------------------------------------------------------------------------------

void Serializer::setSplitPolicy( ::boost::shared_ptr< ISplitPolicy>  newSplitPolicy)
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
        if ( obj && ::fwServices::ObjectServiceRegistry::has(obj, "::fwXML::IFileFormatService") )
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

    assert(object); // check if object is well instanciated
    OSLM_INFO( "Serializing to " <<   m_rootFolder.string().c_str() << "...." );
    ProcessedXMLFile =  m_rootFolder.string();

    //reset all hierarchy informations
    ::fwXML::XMLHierarchy::getDefault()->clearAll();

    // recreate hierarchy
    ::visitor::Serialize visitor;
    ::fwData::visitor::accept( object , &visitor );

    std::set< ::fwXML::XMLAggregator::sptr > savedAggregator;
    ::fwXML::XMLHierarchy::ObjectAggregatorMap::iterator aggIter = ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().begin();

    while( aggIter != ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().end() )
    {
         ::boost::shared_ptr<fwTools::Object> obj =  aggIter->first.lock();
         ::fwXML::XMLAggregator::sptr aggregator =  aggIter->second;

        // save aggregator only once
        if ( savedAggregator.count( aggregator ) == 0 ) // not already saved
        {
            aggIter->second->rootFolder() = this->rootFolder();
            boost::filesystem::path filePath =  aggIter->second->getFullPath(); ;
            OSLM_DEBUG( "Saving XMLAggregator " << filePath.string() << "..." );
            ProcessedXMLFile =  filePath.string();
            {
                // create directory structure if needed
                if ( filePath.branch_path().empty() == false )
                {
                    ::boost::filesystem::create_directories(filePath.branch_path());
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

     ProcessedXMLFile =  m_rootFolder.string();

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
        mesg+= " from file " + ProcessedXMLFile;
        throw ::fwTools::Failed(mesg);
    }

    assert(className.size());
    std::string idXML = ObjectTracker::getID(xmlNode );

    OSLM_DEBUG("ObjectsFromXml : manage Object " << xmlNode->name );
    if ( ObjectTracker::isAlreadyInstanciated( idXML ) )
    {
        OSLM_DEBUG("ObjectsFromXml : manageObject " << xmlNode->name  << "already Instancited with idXML " << idXML);
        return ObjectTracker::buildObject( className , idXML  );
    }

     ::fwTools::Object::sptr newObject = ObjectTracker::buildObject( className , idXML  );
    // warning do not duplicate FIELDS
    newObject->children().clear();

    assert( newObject.get() );

    bool classicObject = ( xmlStrcmp( xmlNode->name, BAD_CAST "Field" ) != 0 ) ;

    // classic object ignore all children expect Field
    // labeledObject manage all children
    while ( child!=NULL )
    {
        if ( child->type == XML_ELEMENT_NODE )
        {
            // normal parent object ignore chlidren which are not Field
            if ( classicObject &&  xmlStrcmp( child->name, BAD_CAST "Field" ) )
            {
                OSLM_DEBUG( "ObjectsFromXml : " << xmlNode->name << " ignoring " << child->name );
            }
            else
            {
                OSLM_DEBUG( "ObjectsFromXml : " <<  xmlNode->name << " accept " << child->name );
                ::fwTools::Object::sptr newChild = ObjectsFromXml( child, loadExtraXML );
                assert (newChild);
                newObject->children().push_back( newChild );
            }
        }
        child = child->next;
    }

    // update data form XML
    ::fwXML::XMLTranslator::sptr translator = ::fwTools::ClassFactoryRegistry::create< fwXML::XMLTranslator  >(  newObject->getClassname()  );

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
    if (tmp)
    {
        OSLM_WARN(" with label= " << tmp->label());
    }
    return newObject;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr  Serializer::deSerialize( ::boost::filesystem::path filePath , bool loadExtraXML , bool validateWithSchema   ) throw (::fwTools::Failed)
{
    xmlDocPtr xmlDoc = NULL;
    xmlNodePtr xmlRoot = NULL;

    // get XMLDOC & root PTR
    this->rootFolder() = filePath.branch_path();
    ProcessedXMLFile = filePath.string();

    xmlDoc = XMLParser::getXmlDocFromFile( filePath );
    xmlRoot = xmlDocGetRootElement (xmlDoc);

    if ( validateWithSchema )
    {
        // validation
        DataFolderValidator validator;
        validator.collecteSchema( this->rootFolder() );
        bool validationOK = validator.validate( xmlRoot );
        OSLM_INFO("XML VALIDATION (1=OK) OF " <<  filePath.string() << " result=" << validationOK );
        OSLM_INFO("XML VALIDATION ERROR LOG ==BEGIN " << validator.getErrorLog() << " END==" );

        if ( validationOK == false )
        {
            std::string msg("XML Validation Failed " + filePath.string() + ": " );
            msg+= validator.getErrorLog();
            msg+= "\n NB: an apostrophe can cause problem to the XML library, so if you have one in your user name, change your TMP environment variable or contact your system administrator.";
            throw ::fwTools::Failed( msg );
            return ::boost::shared_ptr< fwTools::Object>();
        }
    }

    xmlNodePtr rootObject = NULL;
    xmlNodePtr archive =  !xmlStrcmp( xmlRoot->name, BAD_CAST "archive" )? xmlRoot: NULL;

    if (archive)
    {
        xmlNodePtr protocol = XMLParser::findChildNamed(archive,"protocol");
        OSLM_ASSERT( "No protocol version for archive : " << filePath.string(), protocol );
        xmlNodePtr content = XMLParser::findChildNamed(archive,"content");
        OSLM_ASSERT( "No content for archive : " << filePath.string() , content );
        rootObject = XMLParser::nextXMLElement(content->children);
    }
    else
    {
        OSLM_WARN( "No  versionned  archive : " << filePath.string() );
        rootObject = xmlRoot;
        PatchNoVersionToNewData( rootObject ); // internal function

    }

    ObjectTracker::clear();

    ::fwTools::Object::sptr objRoot = ObjectsFromXml( rootObject , loadExtraXML );

    if (loadExtraXML)
    {
        OSLM_INFO("Serializer::deSerialize also load extraXML");
        this->IOforExtraXML( objRoot, false );
    }
    else
    {
        OSLM_INFO("Serializer::deSerialize DO NOT load extraXML");
    }
    // memory cleanup
    xmlFreeDoc (xmlDoc);

    return objRoot;
}

//------------------------------------------------------------------------------

}
