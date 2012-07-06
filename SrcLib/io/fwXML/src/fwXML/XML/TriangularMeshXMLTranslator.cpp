/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <string.h>

#include <fwTools/UUID.hpp>

#include <fwServices/Base.hpp>
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

void TriangularMeshXMLTranslator::manageSavingBuffer( xmlNodePtr boostXMLBuffer, ::fwData::TriangularMesh::sptr mesh )
{
    // get XML node related to Buffer
    ::fwXML::IFileFormatService::sptr binSaver;
    binSaver = ::fwServices::add< ::fwXML::IFileFormatService >(mesh, "::fwXML::TriangularMeshFileFormatService");
    SLM_ASSERT("TriangularMeshFileFormatService not found", binSaver);

    binSaver->filename() = mesh->getLeafClassname() + "_" + ::fwTools::UUID::get(mesh);
    binSaver->extension() = binSaver->getWriter()->extension();
    binSaver->rootFolder() = ::fwXML::Serializer::rootFolder();

    ::boost::filesystem::path fileLocation = binSaver->localFolder() / binSaver->getFullFilename();
    std::string path = fileLocation.string();
    XMLTH::addProp( boostXMLBuffer, "filename",  path );
    XMLTH::addProp( boostXMLBuffer, "protocol",  binSaver->getWriter()->getClassname() );

    binSaver->save();
    ::fwServices::OSR::unregisterService(binSaver);
}

//------------------------------------------------------------------------------

void TriangularMeshXMLTranslator::manageLoadingBuffer( xmlNodePtr boostXMLBuffer, ::fwData::TriangularMesh::sptr mesh )
{
    // get XML node related to Buffer
    ::fwXML::IFileFormatService::sptr binLoader;
    binLoader = ::fwServices::add< ::fwXML::IFileFormatService >(mesh, "::fwXML::TriangularMeshFileFormatService");
    SLM_ASSERT("TriangularMeshFileFormatService not found", binLoader);

    ::boost::filesystem::path fileLocation(  XMLParser::getAttribute(boostXMLBuffer,"filename") );
    binLoader->filename() = ::boost::filesystem::basename( fileLocation.leaf() );
    binLoader->extension()   = ::boost::filesystem::extension( fileLocation.leaf() );
    binLoader->localFolder() = fileLocation.parent_path();
    binLoader->rootFolder() = ::fwXML::Serializer::rootFolder();

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
    ::fwDataIO::reader::IObjectReader::sptr reader;
    reader = ::fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader >(pseudoReader);
    SLM_ASSERT("reader not instanced", reader);

    // assign to FileFormatService
    binLoader->setReader( reader );
    binLoader->load();
    ::fwServices::OSR::unregisterService(binLoader);
}

//------------------------------------------------------------------------------

xmlNodePtr TriangularMeshXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );
    xmlNodePtr bufferNode = xmlAddChild( node, xmlNewNode(NULL, BAD_CAST "Buffer"));
    SLM_ASSERT("bufferNode not instanced", bufferNode); // bufferNode must be found !!!

    // delegate process
    manageSavingBuffer( bufferNode, ::fwData::TriangularMesh::dynamicCast(obj) );
    return node;
}

//------------------------------------------------------------------------------

void TriangularMeshXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::TriangularMesh >(toUpdate,source) );

    // search "<Buffer/>" node
    xmlNodePtr bufferNode = XMLParser::findChildNamed( source, std::string("Buffer") );
    SLM_ASSERT("bufferNode not instanced", bufferNode); // bufferNode must be found !!!
    manageLoadingBuffer( bufferNode , ::fwData::TriangularMesh::dynamicCast(toUpdate) );
}

//------------------------------------------------------------------------------

}
