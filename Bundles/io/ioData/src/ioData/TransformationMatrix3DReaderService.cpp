/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioData/TransformationMatrix3DReaderService.hpp"

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataIO/reader/TransformationMatrix3DReader.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <iostream>

namespace ioData
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwIO::IReader, ::ioData::TransformationMatrix3DReaderService,
                         ::fwData::TransformationMatrix3D );

//------------------------------------------------------------------------------

::fwIO::IOPathType TransformationMatrix3DReaderService::getIOPathType() const
{
    return ::fwIO::FILE;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " TransformationMatrix3D object reader";
}

//-----------------------------------------------------------------------------

std::vector< std::string > TransformationMatrix3DReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".trf");
    return extensions;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::starting( )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load a transformation matrix" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files", "*.trf");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

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

void TransformationMatrix3DReaderService::stopping()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::updating()
{
    SLM_TRACE_FUNC();
    if(this->hasLocationDefined())
    {
        // Retrieve object
        ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D >( );
        SLM_ASSERT("matrix not instanced", matrix);

        ::fwDataIO::reader::TransformationMatrix3DReader::sptr reader =
            ::fwDataIO::reader::TransformationMatrix3DReader::New();
        reader->setObject( matrix );
        reader->setFile(this->getFile());
        reader->read();

        // Notify reading
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
