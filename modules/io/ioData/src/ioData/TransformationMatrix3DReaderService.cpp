/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ioData/TransformationMatrix3DReaderService.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <fwDataIO/reader/TransformationMatrix3DReader.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwIO/IReader.hpp>

#include <services/macros.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace ioData
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwIO::IReader, ::io::sight::data::TransformationMatrix3DReaderService,
                         data::TransformationMatrix3D )

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
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load a transformation matrix" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files", "*.trf");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
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
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve object
        data::TransformationMatrix3D::sptr matrix =
            this->getInOut< data::TransformationMatrix3D >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", matrix);

        ::fwDataIO::reader::TransformationMatrix3DReader::sptr reader =
            ::fwDataIO::reader::TransformationMatrix3DReader::New();
        reader->setObject( matrix );
        reader->setFile(this->getFile());
        reader->read();

        // Notify reading
        auto sig = matrix->signal< data::Object::ModifiedSignalType >(
            data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//-----------------------------------------------------------------------------

}
