/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <stdio.h>
#include <string.h>

#include "fwXML/XML/ImageXMLTranslator.hpp"

#include "fwXML/XML/XMLParser.hpp"

#include "fwXML/XML/GenericXMLTranslator.hpp" // WIN32 SUX header include dependant

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Image.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

#include <fwServices/helper.hpp>
#include "fwXML/IFileFormatService.hpp"
#include <fwDataIO/writer/IObjectWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

namespace fwXML
{


std::string ImageXMLTranslator::s_noFileProtocol = "NoFile";

ImageXMLTranslator::ImageXMLTranslator() {};

ImageXMLTranslator::~ImageXMLTranslator() {};

void ImageXMLTranslator::manageSavingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::Image> img )
{
    // get XML node related to Buffer //FIXMEXPATH

    if ( img->getSize().size()!=0 && img->getSize().front()!=0  )
    {
        ::boost::shared_ptr< IFileFormatService > binSaver = fwServices::get<  IFileFormatService >(img,0);
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

void ImageXMLTranslator::manageLoadingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::Image> img )
{
    // find and update IO Protocol
    std::string protocol = XMLTH::getProp<std::string>(boostXMLBuffer,"protocol");

    if ( protocol != ImageXMLTranslator::s_noFileProtocol )
    {
        // get XML node related to Buffer //FIXMEXPATH
        ::boost::shared_ptr< IFileFormatService > binLoader = fwServices::get<  IFileFormatService >(img,0);
        OSLM_DEBUG( "ImageXMLTranslator::manageLoadingBuffer :: READED FILENAME " << XMLParser::getAttribute(boostXMLBuffer,"filename") );
        boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
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
         ::boost::shared_ptr< ::fwDataIO::reader::IObjectReader > reader;
        OSLM_DEBUG("ImageXMLTranslator::manageLoadingBuffer initial protocol="<< protocol << " final loading protocol=" << pseudoReader)
        reader = ::fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader >(pseudoReader);
        assert(reader);

        // assign to FileFormatService
         ::boost::shared_ptr< IFileFormatService > binReader = fwServices::get<  IFileFormatService >(img,0);
        binReader->setReader( reader );
    }
}








xmlNodePtr ImageXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{

    // call default xmtl representation
    GenericXMLTranslator< ::fwData::Image > img2xmlbase;
    xmlNodePtr node = img2xmlbase.getXMLFrom(obj);
    assert(node);

    // search empty "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( node, std::string("Buffer") );
    assert( bufferNode ); // bufferNode must be found !!!

    // delegate process
    manageSavingBuffer( bufferNode ,boost::dynamic_pointer_cast< ::fwData::Image >(obj) );
    return node;
}


void ImageXMLTranslator::updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
{
    // TODO assertion xmlNode.name() == RealData.className();
    //return new ::fwData::Image();

    OSLM_DEBUG("ImageXMLTranslator::updateDataFromXML( obj " << toUpdate->className() << " XMLNode source " << source->name << "calling Generic" );
    GenericXMLTranslator< ::fwData::Image > img2xmlbase;
    img2xmlbase.updateDataFromXML(toUpdate,source);

    // search empty "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( source, std::string("Buffer") );
    assert( bufferNode ); // bufferNode must be found !!!
    manageLoadingBuffer( bufferNode , ::boost::dynamic_pointer_cast< ::fwData::Image >(toUpdate) );




}


}
