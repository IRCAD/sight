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

#include "modules/io/matrix/Matrix4WriterService.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/Matrix4.hpp>

#include <io/base/writer/Matrix4Writer.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void Matrix4WriterService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " Matrix4 object writer";
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::starting( )
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType Matrix4WriterService::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void Matrix4WriterService::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save a transformation matrix" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files", "*.trf");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

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

void Matrix4WriterService::stopping()
{
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve object
        data::Matrix4::csptr matrix =
            this->getInput< data::Matrix4 >(sight::io::base::service::s_DATA_KEY);
        SIGHT_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", matrix);

        sight::io::base::writer::Matrix4Writer::sptr writer =
            sight::io::base::writer::Matrix4Writer::New();
        writer->setObject( matrix );
        writer->setFile(this->getFile());
        writer->write();
    }
    else
    {
        m_writeFailed = true;
    }
}

//-----------------------------------------------------------------------------

}
