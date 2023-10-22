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

#include <io/vtk/MeshReader.hpp>
#include <io/vtk/ObjMeshReader.hpp>
#include <io/vtk/PlyMeshReader.hpp>
#include <io/vtk/StlMeshReader.hpp>
#include <io/vtk/VtpMeshReader.hpp>

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
    m_sigJobCreated = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType model_series_reader::getIOPathType() const
{
    return sight::io::service::FILES;
}

//-----------------------------------------------------------------------------

void model_series_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setType(ui::dialog::location::MULTI_FILES);
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialog_file.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialog_file.addFilter("OBJ Files(.obj)", "*.obj");
    dialog_file.addFilter("PLY Files(.ply)", "*.ply");
    dialog_file.addFilter("STL Files(.stl)", "*.stl");
    dialog_file.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialog_file.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::multiple_files>(dialog_file.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->get_files();
        if(!paths.empty())
        {
            default_directory->set_folder(paths[0].parent_path());
            dialog_file.saveDefaultLocation(default_directory);
        }

        this->set_files(paths);
    }
    else
    {
        this->clearLocations();
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
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked       = m_data.lock();
        const auto model_series = std::dynamic_pointer_cast<data::model_series>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::model_series::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            model_series
        );

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        data::model_series::reconstruction_vector_t rec_db = model_series->getReconstructionDB();
        data::model_series::reconstruction_vector_t added_recs;
        for(const auto& file : this->get_files())
        {
            auto mesh = std::make_shared<data::mesh>();
            this->loadMesh(file, mesh);

            data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
            rec->setMesh(mesh);
            rec->setIsVisible(true);
            rec->setOrganName(file.stem().string());
            rec_db.push_back(rec);
            added_recs.push_back(rec);
        }

        cursor.setDefaultCursor();
        model_series->setReconstructionDB(rec_db);

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

void model_series_reader::loadMesh(const std::filesystem::path& _file, data::mesh::sptr _mesh)
{
    // Test extension to provide the reader

    sight::io::reader::object_reader::sptr mesh_reader;

    if(_file.extension() == ".vtk")
    {
        mesh_reader = configure_reader<sight::io::vtk::MeshReader>(_file);
    }
    else if(_file.extension() == ".vtp")
    {
        mesh_reader = configure_reader<sight::io::vtk::VtpMeshReader>(_file);
    }
    else if(_file.extension() == ".obj")
    {
        mesh_reader = configure_reader<sight::io::vtk::ObjMeshReader>(_file);
    }
    else if(_file.extension() == ".stl")
    {
        mesh_reader = configure_reader<sight::io::vtk::StlMeshReader>(_file);
    }
    else if(_file.extension() == ".ply")
    {
        mesh_reader = configure_reader<sight::io::vtk::PlyMeshReader>(_file);
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

    m_sigJobCreated->emit(mesh_reader->getJob());

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
            sight::ui::dialog::message::WARNING
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
            sight::ui::dialog::message::WARNING
        );
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
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
