/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <stdio.h>
#include <string.h>

#include <fwServices/helper.hpp>
#include <fwDataIO/writer/IObjectWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include "fwXML/XML/ImageXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Image.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/IFileFormatService.hpp"

namespace fwXML
{

std::string ImageXMLTranslator::s_noFileProtocol = "NoFile";

ImageXMLTranslator::ImageXMLTranslator() {};

//------------------------------------------------------------------------------

ImageXMLTranslator::~ImageXMLTranslator() {};

//------------------------------------------------------------------------------

void ImageXMLTranslator::manageSavingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::fwData::Image::sptr img )
{
    // get XML node related to Buffer //FIXMEXPATH
    if ( img->getSize().size()!=0 && img->getSize().front()!=0  )
    {
        std::vector< ::fwXML::IFileFormatService::sptr > filesSrv = ::fwServices::OSR::getServices< ::fwXML::IFileFormatService >(img);
        ::fwXML::IFileFormatService::sptr binSaver;
        if( filesSrv.empty() )
        {
            binSaver = ::fwServices::add< ::fwXML::IFileFormatService >(img, "::fwXML::ImageFileFormatService");
        }
        else
        {
            binSaver = filesSrv.at(0);
        }
        std::string path;
        path = ( binSaver->localFolder() / binSaver->getFullFilename() ).string();
        XMLTH::addProp( boostXMLBuffer, "filename",  path );
        XMLTH::addProp( boostXMLBuffer, "protocol",  binSaver->getWriter()->getClassname() );
        // to Implement in session information ?
    }
    else
    {
        XMLTH::addProp( boostXMLBuffer, "filename",  "" );
        XMLTH::addProp( boostXMLBuffer, "protocol",  ImageXMLTranslator::s_noFileProtocol );
    }
}

//------------------------------------------------------------------------------

void ImageXMLTranslator::manageLoadingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::fwData::Image::sptr img )
{
    // find and update IO Protocol
    std::string protocol = XMLTH::getProp<std::string>(boostXMLBuffer,"protocol");

    if ( protocol != ImageXMLTranslator::s_noFileProtocol )
    {
        // get XML node related to Buffer //FIXMEXPATH
        std::vector< ::fwXML::IFileFormatService::sptr > filesSrv = ::fwServices::OSR::getServices< ::fwXML::IFileFormatService >(img);
        ::fwXML::IFileFormatService::sptr binLoader;
        if( filesSrv.empty() )
        {
            binLoader = ::fwServices::add< ::fwXML::IFileFormatService >(img, "::fwXML::ImageFileFormatService");
        }
        else
        {
            binLoader = filesSrv.at(0);
        }
        OSLM_DEBUG( "ImageXMLTranslator::manageLoadingBuffer :: READED FILENAME " << XMLParser::getAttribute(boostXMLBuffer,"filename") );
        ::boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
        binLoader->filename() = ::boost::filesystem::basename( fileLocation.leaf() );
        binLoader->extension()   = ::boost::filesystem::extension( fileLocation.leaf() );
        binLoader->localFolder() = fileLocation.branch_path();

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
        OSLM_DEBUG("ImageXMLTranslator::manageLoadingBuffer initial protocol="<< protocol << " final loading protocol=" << pseudoReader)
        reader = ::fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader >(pseudoReader);
        assert(reader);

        // assign to FileFormatService
        binLoader->setReader( reader );
    }
}

//------------------------------------------------------------------------------

xmlNodePtr ImageXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    // call default xmtl representation
    GenericXMLTranslator< ::fwData::Image > img2xmlbase;
    xmlNodePtr node = img2xmlbase.getXMLFrom(obj);
    assert(node);

    // search empty "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( node, std::string("Buffer") );
    assert( bufferNode ); // bufferNode must be found !!!

    // delegate process
    manageSavingBuffer( bufferNode, ::fwData::Image::dynamicCast(obj) );
    return node;
}

//------------------------------------------------------------------------------

void ImageXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    // TODO assertion xmlNode.name() == RealData.className();
    //return new ::fwData::Image();

    OSLM_DEBUG("ImageXMLTranslator::updateDataFromXML( obj " << toUpdate->className() << " XMLNode source " << source->name << "calling Generic" );
    GenericXMLTranslator< ::fwData::Image > img2xmlbase;
    img2xmlbase.updateDataFromXML(toUpdate,source);

    // search empty "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( source, std::string("Buffer") );
    assert( bufferNode ); // bufferNode must be found !!!
    manageLoadingBuffer( bufferNode , ::fwData::Image::dynamicCast(toUpdate) );
}

//------------------------------------------------------------------------------

}
