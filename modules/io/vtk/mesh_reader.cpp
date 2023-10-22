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
    m_sigJobCreated = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void mesh_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a vtk file to load Mesh" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialog_file.addFilter("OBJ File(.obj)", "*.obj");
    dialog_file.addFilter("PLY File(.ply)", "*.ply");
    dialog_file.addFilter("STL File(.stl)", "*.stl");
    dialog_file.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialog_file.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
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
typename READER::sptr configure_reader(const std::filesystem::path& _file)
{
    typename READER::sptr reader = std::make_shared<READER>();
    reader->set_file(_file);
    return reader;
}

//------------------------------------------------------------------------------

bool mesh_reader::loadMesh(const std::filesystem::path& _vtk_file)
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
    sight::io::reader::object_reader::sptr mesh_reader;

    if(_vtk_file.extension() == ".vtk")
    {
        mesh_reader = configure_reader<sight::io::vtk::MeshReader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".vtp")
    {
        mesh_reader = configure_reader<sight::io::vtk::VtpMeshReader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".obj")
    {
        mesh_reader = configure_reader<sight::io::vtk::ObjMeshReader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".stl")
    {
        mesh_reader = configure_reader<sight::io::vtk::StlMeshReader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".ply")
    {
        mesh_reader = configure_reader<sight::io::vtk::PlyMeshReader>(_vtk_file);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Extension '" + _vtk_file.extension().string()
                + "' is not managed by module::io::vtk::mesh_reader."
            )
        );
    }

    m_sigJobCreated->emit(mesh_reader->getJob());

    mesh_reader->set_object(mesh);

    try
    {
        mesh_reader->read();
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

    data::object::modified_signal_t::sptr sig;
    sig = mesh->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
