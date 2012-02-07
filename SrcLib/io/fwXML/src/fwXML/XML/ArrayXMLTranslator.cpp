/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <stdio.h>
#include <string.h>

#include <fwServices/Base.hpp>

#include <fwDataIO/writer/IObjectWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include "fwXML/XML/ArrayXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/ArrayFileFormatService.hpp"
#include "fwXML/IFileFormatService.hpp"
#include "fwXML/boostSerializer/Array.hpp"

namespace fwXML
{

std::string ArrayXMLTranslator::S_NOFILEPROTOCOL = "NoFile";

ArrayXMLTranslator::ArrayXMLTranslator()
{}

//------------------------------------------------------------------------------

ArrayXMLTranslator::~ArrayXMLTranslator()
{}

//------------------------------------------------------------------------------

void ArrayXMLTranslator::manageSavingBuffer( xmlNodePtr boostXMLBuffer, ::fwData::Array::sptr array )
{
    // get XML node related to Buffer
    if ( array->getSizeInBytes() != 0  )
    {
        std::vector< ::fwXML::IFileFormatService::sptr > filesSrv = ::fwServices::OSR::getServices< ::fwXML::IFileFormatService >(array);
        ::fwXML::IFileFormatService::sptr binSaver;
        if( filesSrv.empty() )
        {
            binSaver = ::fwXML::ArrayFileFormatService::sptr( new ::fwXML::ArrayFileFormatService());
            ::fwServices::registry::ObjectService::registerService(array, binSaver);
        }
        else
        {
            binSaver = filesSrv.at(0);
        }

        std::string path;
        path = ( binSaver->localFolder() / binSaver->getFullFilename() ).string();
        XMLTH::addProp( boostXMLBuffer, "filename",  path );
        XMLTH::addProp( boostXMLBuffer, "protocol",  binSaver->getWriter()->getClassname() );
    }
    else
    {
        XMLTH::addProp( boostXMLBuffer, "filename",  "" );
        XMLTH::addProp( boostXMLBuffer, "protocol",  ArrayXMLTranslator::S_NOFILEPROTOCOL );
    }
}

//------------------------------------------------------------------------------

void ArrayXMLTranslator::manageLoadingBuffer( xmlNodePtr boostXMLBuffer, ::fwData::Array::sptr array )
{
    // find and update IO Protocol
    std::string protocol = XMLTH::getProp<std::string>(boostXMLBuffer,"protocol");

    if ( protocol != ArrayXMLTranslator::S_NOFILEPROTOCOL )
    {
        // get XML node related to Buffer
        std::vector< ::fwXML::IFileFormatService::sptr > filesSrv = ::fwServices::OSR::getServices< ::fwXML::IFileFormatService >(array);
        ::fwXML::IFileFormatService::sptr binLoader;
        if( filesSrv.empty() )
        {
            binLoader = ::fwXML::ArrayFileFormatService::sptr( new ::fwXML::ArrayFileFormatService());
            ::fwServices::registry::ObjectService::registerService(array, binLoader);
        }
        else
        {
            binLoader = filesSrv.at(0);
        }
        OSLM_DEBUG( "ArrayXMLTranslator::manageLoadingBuffer :: READED FILENAME " << XMLParser::getAttribute(boostXMLBuffer,"filename") );
        ::boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
        binLoader->filename() = ::boost::filesystem::basename( fileLocation.leaf() );
        binLoader->extension()   = ::boost::filesystem::extension( fileLocation.leaf() );
        binLoader->localFolder() = fileLocation.parent_path();

        std::string pseudoReader = protocol;
        if (  protocol.find("Writer") != std::string::npos )
        {
            pseudoReader.replace(  protocol.find("Writer"), strlen("Writer") ,"Reader");
        }
        if (  protocol.find("writer") != std::string::npos )
        {
            pseudoReader.replace(  protocol.find("writer"), strlen("writer") ,"reader");
        }
        assert( protocol != pseudoReader );

        // get new reader
        ::fwDataIO::reader::IObjectReader::sptr reader;
        OSLM_DEBUG("ArrayXMLTranslator::manageLoadingBuffer initial protocol="<< protocol << " final loading protocol=" << pseudoReader)
        reader = ::fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader >(pseudoReader);
        SLM_ASSERT("reader not instanced", reader);

        // assign to FileFormatService
        binLoader->setReader( reader );
    }
}

//------------------------------------------------------------------------------

xmlNodePtr ArrayXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    // call default xmtl representation
    GenericXMLTranslator< ::fwData::Array > array2xmlbase;
    xmlNodePtr node = array2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("node not instanced", node);

    // search empty "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( node, std::string("Buffer") );
    SLM_ASSERT("bufferNode not instanced", bufferNode); // bufferNode must be found !!!

    // delegate process
    this->manageSavingBuffer( bufferNode, ::fwData::Array::dynamicCast(obj) );
    return node;
}

//------------------------------------------------------------------------------

void ArrayXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    GenericXMLTranslator< ::fwData::Array > array2xmlbase;
    array2xmlbase.updateDataFromXML(toUpdate,source);

    // search empty "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( source, std::string("Buffer") );
    SLM_ASSERT("bufferNode not instanced", bufferNode); // bufferNode must be found !!!
    this->manageLoadingBuffer( bufferNode , ::fwData::Array::dynamicCast(toUpdate) );
}

//------------------------------------------------------------------------------

}
