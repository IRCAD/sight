/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>

#include <io/IReader.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/reader/TriangularMeshReader.hpp>

#include <fwComEd/TriangularMeshMsg.hpp>

#include "ioData/TriangularMeshReaderService.hpp"

REGISTER_SERVICE( ::io::IReader , ::ioData::TriangularMeshReaderService , ::fwData::TriangularMesh ) ;

namespace ioData
{

TriangularMeshReaderService::TriangularMeshReaderService()
{
}

//-----------------------------------------------------------------------------

void TriangularMeshReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " Triangular Mesh reader" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > TriangularMeshReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trian");
    return extensions ;
}

//-----------------------------------------------------------------------------

TriangularMeshReaderService::~TriangularMeshReaderService() throw()
{
}

//------------------------------------------------------------------------------

::io::IOPathType TriangularMeshReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void TriangularMeshReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an triangular mesh file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TrianMesh","*.trian");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void TriangularMeshReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(this->hasLocationDefined())
    {
        // Retrieve object
        ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >( );
        SLM_ASSERT("mesh not instanced", mesh);

        ::fwDataIO::reader::TriangularMeshReader reader;
        reader.setObject( mesh );
        reader.setFile(this->getFile());
        reader.read();

        // Notify reading
        ::fwComEd::TriangularMeshMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH );
        ::fwServices::IEditionService::notify(this->getSptr(), mesh, msg);
    }
}

}
