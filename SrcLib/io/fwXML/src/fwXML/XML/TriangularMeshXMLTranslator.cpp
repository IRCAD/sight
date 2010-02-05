#include <boost/filesystem/convenience.hpp>

#include <stdio.h>
#include <string.h>

#include "fwXML/XML/TriangularMeshXMLTranslator.hpp"

#include "fwXML/XML/XMLParser.hpp"

#include "fwXML/XML/GenericXMLTranslator.hpp" // WIN32 SUX header include dependant

#include <fwXML/boostSerializer/IFSBoostSerialize.hpp>
#include "fwXML/XML/TriangularMeshXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

#include <fwServices/helper.hpp>
#include "fwXML/IFileFormatService.hpp"
#include <fwDataIO/writer/IObjectWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

namespace fwXML
{

TriangularMeshXMLTranslator::TriangularMeshXMLTranslator() {};

TriangularMeshXMLTranslator::~TriangularMeshXMLTranslator() {};

void TriangularMeshXMLTranslator::manageSavingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::TriangularMesh> mesh )
{
    // get XML node related to Buffer //FIXMEXPATH
    ::boost::shared_ptr< IFileFormatService > binSaver = fwServices::get<  IFileFormatService >(mesh,0);
    std::string path;
    path = ( binSaver->localFolder() / binSaver->getFullFilename() ).string();
    if ( mesh->points().size()!=0  )
    {
        XMLTH::addProp( boostXMLBuffer, "filename",  path );
        XMLTH::addProp( boostXMLBuffer, "protocol",  binSaver->getWriter()->getClassname() );
        // to Implement in session information ?
    }
    else
    {
        XMLTH::addProp( boostXMLBuffer, "filename",  "" );
    }



}

void TriangularMeshXMLTranslator::manageLoadingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::TriangularMesh> mesh )
{
    // get XML node related to Buffer //FIXMEXPATH
    ::boost::shared_ptr< IFileFormatService > binLoader = fwServices::get<  IFileFormatService >(mesh,0);
    std::cout << "READED FILENAME " << XMLParser::getAttribute(boostXMLBuffer,"filename");
    boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
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
     ::boost::shared_ptr< IFileFormatService > binReader = fwServices::get<  IFileFormatService >(mesh,0);
    binReader->setReader( reader );
}








xmlNodePtr TriangularMeshXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );
    xmlNodePtr bufferNode = xmlAddChild( node, xmlNewNode(NULL, BAD_CAST "Buffer"));
    assert( bufferNode ); // bufferNode must be found !!!

    // delegate process
    manageSavingBuffer( bufferNode ,boost::dynamic_pointer_cast< ::fwData::TriangularMesh >(obj) );
    return node;
}


void TriangularMeshXMLTranslator::updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::TriangularMesh >(toUpdate,source) );

    // search "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( source, std::string("Buffer") );
    assert( bufferNode ); // bufferNode must be found !!!
    manageLoadingBuffer( bufferNode , ::boost::dynamic_pointer_cast< ::fwData::TriangularMesh >(toUpdate) );




}


}
