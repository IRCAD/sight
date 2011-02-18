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

#include "fwXML/XML/TriangularMeshXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/XML/TriangularMeshXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/IFileFormatService.hpp"


namespace fwXML
{

TriangularMeshXMLTranslator::TriangularMeshXMLTranslator() {};

//------------------------------------------------------------------------------

TriangularMeshXMLTranslator::~TriangularMeshXMLTranslator() {};

//------------------------------------------------------------------------------

void TriangularMeshXMLTranslator::manageSavingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::TriangularMesh> mesh )
{
    // get XML node related to Buffer //FIXMEXPATH
    ::boost::shared_ptr< IFileFormatService > binSaver = fwServices::get<  IFileFormatService >(mesh);
    std::string path;
    path = ( binSaver->localFolder() / binSaver->getFullFilename() ).string();

    XMLTH::addProp( boostXMLBuffer, "filename",  path );
    XMLTH::addProp( boostXMLBuffer, "protocol",  binSaver->getWriter()->getClassname() );
}

//------------------------------------------------------------------------------

void TriangularMeshXMLTranslator::manageLoadingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::TriangularMesh> mesh )
{
    // get XML node related to Buffer //FIXMEXPATH
    ::boost::shared_ptr< IFileFormatService > binLoader = fwServices::get<  IFileFormatService >(mesh);
    OSLM_TRACE("READED FILENAME " << XMLParser::getAttribute(boostXMLBuffer,"filename"));
    ::boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
    binLoader->filename() = ::boost::filesystem::basename( fileLocation.leaf() );
    binLoader->extension()   = ::boost::filesystem::extension( fileLocation.leaf() );
    binLoader->localFolder() = fileLocation.branch_path();

    // find and update IO Protocol
    std::string protocol = XMLTH::getProp<std::string>(boostXMLBuffer,"protocol");
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
    reader = ::fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader >(pseudoReader);
    assert(reader);

    // assign to FileFormatService
     ::boost::shared_ptr< IFileFormatService > binReader = ::fwServices::get<  IFileFormatService >(mesh);
    binReader->setReader( reader );
}

//------------------------------------------------------------------------------

xmlNodePtr TriangularMeshXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );
    xmlNodePtr bufferNode = xmlAddChild( node, xmlNewNode(NULL, BAD_CAST "Buffer"));
    assert( bufferNode ); // bufferNode must be found !!!

    // delegate process
    manageSavingBuffer( bufferNode ,boost::dynamic_pointer_cast< ::fwData::TriangularMesh >(obj) );
    return node;
}

//------------------------------------------------------------------------------

void TriangularMeshXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::TriangularMesh >(toUpdate,source) );

    // search "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( source, std::string("Buffer") );
    assert( bufferNode ); // bufferNode must be found !!!
    manageLoadingBuffer( bufferNode , ::boost::dynamic_pointer_cast< ::fwData::TriangularMesh >(toUpdate) );
}

//------------------------------------------------------------------------------


}
