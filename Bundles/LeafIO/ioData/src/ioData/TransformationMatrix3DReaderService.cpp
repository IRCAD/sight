/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <iostream>
#include <boost/filesystem/operations.hpp>

#include <fwDataIO/reader/TransformationMatrix3DReader.hpp>
#include <io/IReader.hpp>

#include <fwServices/macros.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwCore/base.hpp>


#include "ioData/TransformationMatrix3DReaderService.hpp"

namespace ioData
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::io::IReader, ::ioData::TransformationMatrix3DReaderService,
                         ::fwData::TransformationMatrix3D );

//------------------------------------------------------------------------------

::io::IOPathType TransformationMatrix3DReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::info(std::ostream &_sstream )
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

void TransformationMatrix3DReaderService::starting( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load a transformation matrix");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files","*.trf");
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

void TransformationMatrix3DReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::updating() throw(::fwTools::Failed)
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
