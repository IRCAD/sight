/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioData/MeshWriterService.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataIO/writer/MeshWriter.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

fwServicesRegisterMacro( ::io::IWriter, ::ioData::MeshWriterService, ::fwData::Mesh );

namespace ioData
{

MeshWriterService::MeshWriterService()
{
}

//-----------------------------------------------------------------------------

void MeshWriterService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " Mesh writer";
}

//-----------------------------------------------------------------------------

std::vector< std::string > MeshWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".trian");
    return extensions;
}

//-----------------------------------------------------------------------------

MeshWriterService::~MeshWriterService() noexcept
{
}

//------------------------------------------------------------------------------

::io::IOPathType MeshWriterService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void MeshWriterService::configuring()
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void MeshWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a TrianMesh file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TrianMesh", "*.trian");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
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

void MeshWriterService::updating()
{
    SLM_TRACE_FUNC();
    if(this->hasLocationDefined())
    {
        // Retrieve object
        ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >( );
        SLM_ASSERT("Mesh not instanced", mesh);

        ::fwDataIO::writer::MeshWriter::sptr writer = ::fwDataIO::writer::MeshWriter::New();
        writer->setObject( mesh );
        writer->setFile(this->getFile());

        try
        {
            writer->write();
        }
        catch (const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during writing Mesh : " << e.what();

            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
        }

    }
}

//------------------------------------------------------------------------------

}
