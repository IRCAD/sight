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

#include "modules/io/vtk/model_series_reader.hpp"

#include "modules/io/vtk/mesh_writer.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/uuid.hpp>

#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <io/vtk/mesh_reader.hpp>
#include <io/vtk/obj_mesh_reader.hpp>
#include <io/vtk/ply_mesh_reader.hpp>
#include <io/vtk/stl_mesh_reader.hpp>
#include <io/vtk/vtp_mesh_reader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

model_series_reader::model_series_reader() noexcept
{
    m_sig_job_created = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t model_series_reader::get_path_type() const
{
    return sight::io::service::files;
}

//-----------------------------------------------------------------------------

void model_series_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_default_location(default_directory);
    dialog_file.set_type(ui::dialog::location::multi_files);
    dialog_file.set_title(m_window_title.empty() ? "Choose vtk files to load Series" : m_window_title);
    dialog_file.add_filter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialog_file.add_filter("OBJ Files(.obj)", "*.obj");
    dialog_file.add_filter("PLY Files(.ply)", "*.ply");
    dialog_file.add_filter("STL Files(.stl)", "*.stl");
    dialog_file.add_filter("VTK Legacy Files(.vtk)", "*.vtk");
    dialog_file.add_filter("VTK Polydata Files(.vtp)", "*.vtp");
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_option(ui::dialog::location::file_must_exist);

    auto result = std::dynamic_pointer_cast<core::location::multiple_files>(dialog_file.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->get_files();
        if(!paths.empty())
        {
            default_directory->set_folder(paths[0].parent_path());
            dialog_file.save_default_location(default_directory);
        }

        this->set_files(paths);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void model_series_reader::starting()
{
}

//------------------------------------------------------------------------------

void model_series_reader::stopping()
{
}

//------------------------------------------------------------------------------

void model_series_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void model_series_reader::info(std::ostream& _sstream)
{
    _sstream << "model_series_reader::info";
}

//------------------------------------------------------------------------------

void model_series_reader::updating()
{
    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked       = m_data.lock();
        const auto model_series = std::dynamic_pointer_cast<data::model_series>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::model_series::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            model_series
        );

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        data::model_series::reconstruction_vector_t rec_db = model_series->get_reconstruction_db();
        data::model_series::reconstruction_vector_t added_recs;
        for(const auto& file : this->get_files())
        {
            auto mesh = std::make_shared<data::mesh>();
            this->load_mesh(file, mesh);

            data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
            rec->set_mesh(mesh);
            rec->set_is_visible(true);
            rec->set_organ_name(file.stem().string());
            rec_db.push_back(rec);
            added_recs.push_back(rec);
        }

        cursor.set_default_cursor();
        model_series->set_reconstruction_db(rec_db);

        auto sig = model_series->signal<data::model_series::reconstructions_added_signal_t>(
            data::model_series::RECONSTRUCTIONS_ADDED_SIG
        );
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit(added_recs);
        }
    }
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

void model_series_reader::load_mesh(const std::filesystem::path& _file, data::mesh::sptr _mesh)
{
    // Test extension to provide the reader

    sight::io::reader::object_reader::sptr mesh_reader;

    if(_file.extension() == ".vtk")
    {
        mesh_reader = configure_reader<sight::io::vtk::mesh_reader>(_file);
    }
    else if(_file.extension() == ".vtp")
    {
        mesh_reader = configure_reader<sight::io::vtk::vtp_mesh_reader>(_file);
    }
    else if(_file.extension() == ".obj")
    {
        mesh_reader = configure_reader<sight::io::vtk::obj_mesh_reader>(_file);
    }
    else if(_file.extension() == ".stl")
    {
        mesh_reader = configure_reader<sight::io::vtk::stl_mesh_reader>(_file);
    }
    else if(_file.extension() == ".ply")
    {
        mesh_reader = configure_reader<sight::io::vtk::ply_mesh_reader>(_file);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Extension '" + _file.extension().string()
                + "' is not managed by module::io::vtk::mesh_reader."
            )
        );
    }

    m_sig_job_created->emit(mesh_reader->get_job());

    mesh_reader->set_object(_mesh);

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
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
