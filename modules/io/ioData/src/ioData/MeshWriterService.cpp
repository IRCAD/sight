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

#include "ioData/MeshWriterService.hpp"

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/Mesh.hpp>

#include <services/macros.hpp>

#include <io/base/writer/MeshWriter.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <filesystem>

fwServicesRegisterMacro( io::base::services::IWriter, ::ioData::MeshWriterService, ::sight::data::Mesh )

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

MeshWriterService::~MeshWriterService() noexcept
{
}

//------------------------------------------------------------------------------

::io::base::services::IOPathType MeshWriterService::getIOPathType() const
{
    return io::base::services::FILE;
}

//------------------------------------------------------------------------------

void MeshWriterService::configuring()
{
    io::base::services::IWriter::configuring();
}

//------------------------------------------------------------------------------

void MeshWriterService::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void MeshWriterService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a TrianMesh file" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TrianMesh", "*.trian");
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

//------------------------------------------------------------------------------

void MeshWriterService::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve object
        data::Mesh::csptr mesh = this->getInput< data::Mesh >(io::base::services::s_DATA_KEY);
        SLM_ASSERT("The input key '" + io::base::services::s_DATA_KEY + "' is not correctly set.", mesh);

        io::base::writer::MeshWriter::sptr writer = io::base::writer::MeshWriter::New();
        writer->setObject( mesh );
        writer->setFile(this->getFile());

        try
        {
            writer->write();
        }
        catch (const std::exception& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during writing Mesh : " << e.what();

            ui::base::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                ui::base::dialog::IMessageDialog::WARNING);
        }

    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

}
