/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/io/vtk/mesh_writer.hpp"

#include <core/base.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/mesh.hpp>

#include <io/vtk/MeshWriter.hpp>
#include <io/vtk/ObjMeshWriter.hpp>
#include <io/vtk/PlyMeshWriter.hpp>
#include <io/vtk/StlMeshWriter.hpp>
#include <io/vtk/VtpMeshWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

mesh_writer::mesh_writer() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType mesh_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void mesh_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to save Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("OBJ File(.obj)", "*.obj");
    dialogFile.addFilter("PLY File(.ply)", "*.ply");
    dialogFile.addFilter("STL File(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialogFile.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        this->set_file(result->get_file());
        m_selectedExtension = dialogFile.getSelectedExtensions().front();
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void mesh_writer::starting()
{
}

//------------------------------------------------------------------------------

void mesh_writer::stopping()
{
}

//------------------------------------------------------------------------------

void mesh_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void mesh_writer::info(std::ostream& _sstream)
{
    _sstream << "mesh_writer::info";
}

//------------------------------------------------------------------------------

template<typename WRITER>
typename WRITER::sptr configureWriter(const std::filesystem::path& _file)
{
    typename WRITER::sptr writer = std::make_shared<WRITER>();
    writer->set_file(_file);
    return writer;
}

//------------------------------------------------------------------------------

void mesh_writer::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked = m_data.lock();
        const auto mesh   = std::dynamic_pointer_cast<const data::mesh>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::mesh::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            mesh
        );

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        std::filesystem::path fileToWrite   = this->get_file();
        const std::string providedExtension = fileToWrite.extension().string();
        std::string extensionToUse;

        // Check if file has an extension.
        if(providedExtension.empty())
        {
            // No extension provided, add extension of selected filter.
            extensionToUse = m_selectedExtension;
            fileToWrite   += extensionToUse;
        }
        else
        {
            extensionToUse = providedExtension;
        }

        sight::io::writer::object_writer::sptr meshWriter;

        if(extensionToUse == ".vtk")
        {
            meshWriter = configureWriter<sight::io::vtk::MeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".vtp")
        {
            meshWriter = configureWriter<sight::io::vtk::VtpMeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".stl")
        {
            meshWriter = configureWriter<sight::io::vtk::StlMeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".ply")
        {
            meshWriter = configureWriter<sight::io::vtk::PlyMeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".obj")
        {
            meshWriter = configureWriter<sight::io::vtk::ObjMeshWriter>(fileToWrite);
        }
        else
        {
            SIGHT_THROW_EXCEPTION(
                core::tools::failed(
                    "Extension '" + fileToWrite.extension().string()
                    + "' is not managed by module::io::vtk::mesh_writer."
                )
            );
        }

        m_sigJobCreated->emit(meshWriter->getJob());

        meshWriter->set_object(mesh);

        try
        {
            meshWriter->write();
            m_writeFailed = false;
        }
        catch(core::tools::failed& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            sight::ui::dialog::message::show(
                "Warning",
                ss.str(),
                sight::ui::dialog::message::WARNING
            );

            // Raise exception  for superior level
            SIGHT_THROW_EXCEPTION(e);
        }
        catch(const std::exception& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            sight::ui::dialog::message::show(
                "Warning",
                ss.str(),
                sight::ui::dialog::message::WARNING
            );
        }
        catch(...)
        {
            m_writeFailed = true;
            sight::ui::dialog::message::show(
                "Warning",
                "Warning during saving",
                sight::ui::dialog::message::WARNING
            );
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
