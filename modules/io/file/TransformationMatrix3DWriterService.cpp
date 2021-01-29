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

#include "modules/io/file/TransformationMatrix3DWriterService.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <services/macros.hpp>

#include <io/base/writer/TransformationMatrix3DWriter.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace sight::modules::io::file
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::io::base::services::IWriter,
                         ::sight::modules::io::file::TransformationMatrix3DWriterService,
                         data::TransformationMatrix3D )

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " TransformationMatrix3D object writer";
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::starting( )
{
}

//------------------------------------------------------------------------------

sight::io::base::services::IOPathType TransformationMatrix3DWriterService::getIOPathType() const
{
    return sight::io::base::services::FILE;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configuring()
{
    sight::io::base::services::IWriter::configuring();
}

//------------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ui::base::dialog::LocationDialog dialogFile;
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

void TransformationMatrix3DWriterService::stopping()
{
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve object
        data::TransformationMatrix3D::csptr matrix =
            this->getInput< data::TransformationMatrix3D >(sight::sight::io::base::services::s_DATA_KEY);
        SLM_ASSERT("The input key '" + sight::io::base::services::s_DATA_KEY + "' is not correctly set.", matrix);

        sight::io::base::writer::TransformationMatrix3DWriter::sptr writer =
            sight::io::base::writer::TransformationMatrix3DWriter::New();
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
