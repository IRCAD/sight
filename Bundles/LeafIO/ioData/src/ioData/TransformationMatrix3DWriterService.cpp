/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioData/TransformationMatrix3DWriterService.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataIO/writer/TransformationMatrix3DWriter.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <iostream>

namespace ioData
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::io::IWriter, ::ioData::TransformationMatrix3DWriterService,
                         ::fwData::TransformationMatrix3D );

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " TransformationMatrix3D object writer";
}

//-----------------------------------------------------------------------------

std::vector< std::string > TransformationMatrix3DWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".trf");
    return extensions;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::starting( )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::io::IOPathType TransformationMatrix3DWriterService::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configuring()
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save a transformation matrix" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files", "*.trf");
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

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::stopping()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::updating()
{
    SLM_TRACE_FUNC();
    if(this->hasLocationDefined())
    {
        // Retrieve object
        ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D >( );
        SLM_ASSERT("matrix not instanced", matrix);

        ::fwDataIO::writer::TransformationMatrix3DWriter::sptr writer =
            ::fwDataIO::writer::TransformationMatrix3DWriter::New();
        writer->setObject( matrix );
        writer->setFile(this->getFile());
        writer->write();

        // Notify writing
        auto sig = this->getObject()->signal< ::fwData::Object::ModifiedSignalType >(
            ::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

}
