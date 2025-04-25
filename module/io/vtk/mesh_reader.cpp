/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "module/io/vtk/mesh_reader.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/jobs/base.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <io/vtk/mesh_reader.hpp>
#include <io/vtk/obj_mesh_reader.hpp>
#include <io/vtk/ply_mesh_reader.hpp>
#include <io/vtk/stl_mesh_reader.hpp>
#include <io/vtk/vtp_mesh_reader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "job_created";

//------------------------------------------------------------------------------

sight::io::service::path_type_t mesh_reader::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

mesh_reader::mesh_reader() noexcept :
    reader("Choose a vtk file to load Mesh"),
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

void mesh_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(*m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialog_file.add_filter("OBJ File(.obj)", "*.obj");
    dialog_file.add_filter("PLY File(.ply)", "*.ply");
    dialog_file.add_filter("STL File(.stl)", "*.stl");
    dialog_file.add_filter("VTK Legacy File(.vtk)", "*.vtk");
    dialog_file.add_filter("VTK Polydata File(.vtp)", "*.vtp");
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_option(ui::dialog::location::file_must_exist);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clear_locations();
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

bool mesh_reader::load_mesh(const std::filesystem::path& _vtk_file)
{
    bool ok = true;

    // Retrieve dataStruct associated with this service
    const auto locked = m_data.lock();
    const auto mesh   = std::dynamic_pointer_cast<data::mesh>(locked.get_shared());

    SIGHT_ASSERT(
        "The object is not a '"
        + data::mesh::classname()
        + "' or '"
        + sight::io::service::DATA_KEY
        + "' is not correctly set.",
        mesh
    );

    // Test extension to provide the reader
    sight::io::reader::object_reader::sptr mesh_reader;

    if(_vtk_file.extension() == ".vtk")
    {
        mesh_reader = configure_reader<sight::io::vtk::mesh_reader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".vtp")
    {
        mesh_reader = configure_reader<sight::io::vtk::vtp_mesh_reader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".obj")
    {
        mesh_reader = configure_reader<sight::io::vtk::obj_mesh_reader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".stl")
    {
        mesh_reader = configure_reader<sight::io::vtk::stl_mesh_reader>(_vtk_file);
    }
    else if(_vtk_file.extension() == ".ply")
    {
        mesh_reader = configure_reader<sight::io::vtk::ply_mesh_reader>(_vtk_file);
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

    m_sig_job_created->emit(mesh_reader->get_job());

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
            sight::ui::dialog::message::warning
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
            sight::ui::dialog::message::warning
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
            sight::ui::dialog::message::warning
        );
        ok = false;
    }

    return ok;
}

//------------------------------------------------------------------------------

void mesh_reader::updating()
{
    if(this->has_location_defined())
    {
        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        this->load_mesh(this->get_file());
        this->notification_of_update();

        cursor.set_default_cursor();
    }
}

//------------------------------------------------------------------------------

void mesh_reader::notification_of_update()
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
