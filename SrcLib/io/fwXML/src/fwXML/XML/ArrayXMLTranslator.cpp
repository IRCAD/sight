/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <string.h>

#include <fwTools/UUID.hpp>

#include <fwServices/Base.hpp>

#include <fwDataIO/writer/IObjectWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>
#include <fwDataIO/reader/factory/new.hpp>

#include <fwMemory/BufferManager.hpp>

#include "fwXML/XML/ArrayXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/ArrayFileFormatService.hpp"
#include "fwXML/IFileFormatService.hpp"
#include "fwXML/boostSerializer/Array.hpp"
#include "fwXML/Serializer.hpp"

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
        ::fwXML::IFileFormatService::sptr binSaver;
        binSaver = ::fwServices::add< ::fwXML::IFileFormatService >(array, "::fwXML::ArrayFileFormatService");
        SLM_ASSERT("ArrayFileFormatService not found", binSaver);

        binSaver->filename() = array->getLeafClassname() + "_" + ::fwTools::UUID::get(array);
        binSaver->extension() = binSaver->getWriter()->extension();
        binSaver->rootFolder() = ::fwXML::Serializer::rootFolder();

        ::boost::filesystem::path fileLocation = binSaver->localFolder() / binSaver->getFullFilename();
        std::string path = fileLocation.string();
        XMLTH::addProp( boostXMLBuffer, "filename",  path );
        XMLTH::addProp( boostXMLBuffer, "protocol",  binSaver->getWriter()->getClassname() );


        bool saveIsRequired = true;

        // Test if buffer is not already dumped
        ::fwMemory::BufferManager::sptr manager;
        manager = ::boost::dynamic_pointer_cast< ::fwMemory::BufferManager >( ::fwTools::IBufferManager::getCurrent() );
        if( manager )
        {
            if( manager->isDumped( (void ** ) array->getBufferObject()->getBufferPointer() ) )
            {
                saveIsRequired = false;

                ::boost::filesystem::path fileDest = binSaver->getFullPath();
                if (! ::boost::filesystem::exists(fileDest))
                {
                    ::boost::filesystem::path fileSrc = manager->getDumpedFilePath( (void ** ) array->getBufferObject()->getBufferPointer() );
                    ::boost::system::error_code err;
                    ::boost::filesystem::create_hard_link( fileSrc, fileDest, err );
                    if (err.value() != 0)
                    {
                        ::boost::filesystem::copy_file( fileSrc, fileDest );
                    }
                }
            }
        }

        if( saveIsRequired )
        {
            binSaver->save();
        }

        ::fwServices::OSR::unregisterService(binSaver);
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
        ::fwXML::IFileFormatService::sptr binLoader;
        binLoader = ::fwServices::add< ::fwXML::IFileFormatService >(array, "::fwXML::ArrayFileFormatService");
        SLM_ASSERT("ArrayFileFormatService not found", binLoader);

        ::boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
        binLoader->filename() = ::boost::filesystem::basename( fileLocation.leaf() );
        binLoader->extension()   = ::boost::filesystem::extension( fileLocation.leaf() );
        binLoader->localFolder() = fileLocation.parent_path();
        binLoader->rootFolder() = ::fwXML::Serializer::rootFolder();

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
        reader = ::fwDataIO::reader::factory::New(pseudoReader);
        SLM_ASSERT("reader not instanced", reader);

        // assign to FileFormatService
        binLoader->setReader( reader );
        binLoader->load();
        ::fwServices::OSR::unregisterService(binLoader);
    }
}

//------------------------------------------------------------------------------

xmlNodePtr ArrayXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    // call default xml representation
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

void ArrayXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
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
