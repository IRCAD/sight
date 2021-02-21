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

#include "modules/io/matrix/Matrix4ReaderService.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/Matrix4.hpp>

#include <service/macros.hpp>

#include <io/base/reader/Matrix4Reader.hpp>
#include <io/base/service/IReader.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------

sight::io::base::service::IOPathType Matrix4ReaderService::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//-----------------------------------------------------------------------------

void Matrix4ReaderService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " Matrix4 object reader";
}

//-----------------------------------------------------------------------------

std::vector< std::string > Matrix4ReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".trf");
    return extensions;
}

//-----------------------------------------------------------------------------

void Matrix4ReaderService::starting( )
{
}

//-----------------------------------------------------------------------------

void Matrix4ReaderService::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void Matrix4ReaderService::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void Matrix4ReaderService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load a transformation matrix" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files", "*.trf");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);

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

void Matrix4ReaderService::stopping()
{
}

//-----------------------------------------------------------------------------

void Matrix4ReaderService::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve object
        data::Matrix4::sptr matrix =
            this->getInOut< data::Matrix4 >(sight::io::base::service::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", matrix);

        sight::io::base::reader::Matrix4Reader::sptr reader =
            sight::io::base::reader::Matrix4Reader::New();
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
