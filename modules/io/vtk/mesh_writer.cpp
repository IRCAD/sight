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
    m_sig_job_created = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t mesh_writer::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

void mesh_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a vtk file to save Mesh" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("OBJ File(.obj)", "*.obj");
    dialog_file.add_filter("PLY File(.ply)", "*.ply");
    dialog_file.add_filter("STL File(.stl)", "*.stl");
    dialog_file.add_filter("VTK Legacy File(.vtk)", "*.vtk");
    dialog_file.add_filter("VTK Polydata File(.vtp)", "*.vtp");
    dialog_file.set_option(ui::dialog::location::write);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        this->set_file(result->get_file());
        m_selected_extension = dialog_file.get_selected_extensions().front();
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
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
typename WRITER::sptr configure_writer(const std::filesystem::path& _file)
{
    typename WRITER::sptr writer = std::make_shared<WRITER>();
    writer->set_file(_file);
    return writer;
}

//------------------------------------------------------------------------------

void mesh_writer::updating()
{
    m_write_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked = m_data.lock();
        const auto mesh   = std::dynamic_pointer_cast<const data::mesh>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::mesh::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            mesh
        );

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        std::filesystem::path file_to_write  = this->get_file();
        const std::string provided_extension = file_to_write.extension().string();
        std::string extension_to_use;

        // Check if file has an extension.
        if(provided_extension.empty())
        {
            // No extension provided, add extension of selected filter.
            extension_to_use = m_selected_extension;
            file_to_write   += extension_to_use;
        }
        else
        {
            extension_to_use = provided_extension;
        }

        sight::io::writer::object_writer::sptr mesh_writer;

        if(extension_to_use == ".vtk")
        {
            mesh_writer = configure_writer<sight::io::vtk::mesh_writer>(file_to_write);
        }
        else if(extension_to_use == ".vtp")
        {
            mesh_writer = configure_writer<sight::io::vtk::vtp_mesh_writer>(file_to_write);
        }
        else if(extension_to_use == ".stl")
        {
            mesh_writer = configure_writer<sight::io::vtk::stl_mesh_writer>(file_to_write);
        }
        else if(extension_to_use == ".ply")
        {
            mesh_writer = configure_writer<sight::io::vtk::ply_mesh_writer>(file_to_write);
        }
        else if(extension_to_use == ".obj")
        {
            mesh_writer = configure_writer<sight::io::vtk::obj_mesh_writer>(file_to_write);
        }
        else
        {
            SIGHT_THROW_EXCEPTION(
                core::tools::failed(
                    "Extension '" + file_to_write.extension().string()
                    + "' is not managed by module::io::vtk::mesh_writer."
                )
            );
        }

        m_sig_job_created->emit(mesh_writer->get_job());

        mesh_writer->set_object(mesh);

        try
        {
            mesh_writer->write();
            m_write_failed = false;
        }
        catch(core::tools::failed& e)
        {
            m_write_failed = true;
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            sight::ui::dialog::message::show(
                "Warning",
                ss.str(),
                sight::ui::dialog::message::warning
            );

            // Raise exception  for superior level
            SIGHT_THROW_EXCEPTION(e);
        }
        catch(const std::exception& e)
        {
            m_write_failed = true;
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            sight::ui::dialog::message::show(
                "Warning",
                ss.str(),
                sight::ui::dialog::message::warning
            );
        }
        catch(...)
        {
            m_write_failed = true;
            sight::ui::dialog::message::show(
                "Warning",
                "Warning during saving",
                sight::ui::dialog::message::warning
            );
        }

        cursor.set_default_cursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
