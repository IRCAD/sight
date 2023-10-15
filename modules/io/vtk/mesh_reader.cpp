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

#include "modules/io/vtk/mesh_reader.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/jobs/base.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <io/vtk/MeshReader.hpp>
#include <io/vtk/ObjMeshReader.hpp>
#include <io/vtk/PlyMeshReader.hpp>
#include <io/vtk/StlMeshReader.hpp>
#include <io/vtk/VtpMeshReader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

sight::io::service::IOPathType mesh_reader::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

mesh_reader::mesh_reader() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void mesh_reader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to load Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialogFile.addFilter("OBJ File(.obj)", "*.obj");
    dialogFile.addFilter("PLY File(.ply)", "*.ply");
    dialogFile.addFilter("STL File(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void mesh_reader::starting()
{
}

//------------------------------------------------------------------------------

void mesh_reader::stopping()
{
}

//------------------------------------------------------------------------------

void mesh_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void mesh_reader::info(std::ostream& _sstream)
{
    _sstream << "mesh_reader::info";
}

//------------------------------------------------------------------------------

template<typename READER>
typename READER::sptr configureReader(const std::filesystem::path& _file)
{
    typename READER::sptr reader = std::make_shared<READER>();
    reader->set_file(_file);
    return reader;
}

//------------------------------------------------------------------------------

bool mesh_reader::loadMesh(const std::filesystem::path& vtkFile)
{
    bool ok = true;

    // Retrieve dataStruct associated with this service
    const auto locked = m_data.lock();
    const auto mesh   = std::dynamic_pointer_cast<data::mesh>(locked.get_shared());

    SIGHT_ASSERT(
        "The object is not a '"
        + data::mesh::classname()
        + "' or '"
        + sight::io::service::s_DATA_KEY
        + "' is not correctly set.",
        mesh
    );

    // Test extension to provide the reader
    sight::io::reader::object_reader::sptr meshReader;

    if(vtkFile.extension() == ".vtk")
    {
        meshReader = configureReader<sight::io::vtk::MeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".vtp")
    {
        meshReader = configureReader<sight::io::vtk::VtpMeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".obj")
    {
        meshReader = configureReader<sight::io::vtk::ObjMeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".stl")
    {
        meshReader = configureReader<sight::io::vtk::StlMeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".ply")
    {
        meshReader = configureReader<sight::io::vtk::PlyMeshReader>(vtkFile);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Extension '" + vtkFile.extension().string()
                + "' is not managed by module::io::vtk::mesh_reader."
            )
        );
    }

    m_sigJobCreated->emit(meshReader->getJob());

    meshReader->set_object(mesh);

    try
    {
        meshReader->read();
    }
    catch(core::tools::failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        ok = false;

        // Raise exception  for superior level
        SIGHT_THROW_EXCEPTION(e);
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        ok = false;
    }
    catch(...)
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading.",
            sight::ui::dialog::message::WARNING
        );
        ok = false;
    }

    return ok;
}

//------------------------------------------------------------------------------

void mesh_reader::updating()
{
    if(this->hasLocationDefined())
    {
        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        this->loadMesh(this->get_file());
        this->notificationOfUpdate();

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void mesh_reader::notificationOfUpdate()
{
    const auto locked = m_data.lock();
    const auto mesh   = std::dynamic_pointer_cast<data::mesh>(locked.get_shared());

    data::object::ModifiedSignalType::sptr sig;
    sig = mesh->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
