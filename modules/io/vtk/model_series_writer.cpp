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

#include "modules/io/vtk/model_series_writer.hpp"

#include "modules/io/vtk/mesh_writer.hpp"

#include <core/base.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/uuid.hpp>

#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <io/vtk/MeshWriter.hpp>
#include <io/vtk/ObjMeshWriter.hpp>
#include <io/vtk/PlyMeshWriter.hpp>
#include <io/vtk/StlMeshWriter.hpp>
#include <io/vtk/VtpMeshWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/algorithm/string.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

model_series_writer::model_series_writer() noexcept
{
    m_sigJobCreated = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType model_series_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void model_series_writer::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation(default_directory);
    dialog.setOption(ui::dialog::location::WRITE);
    dialog.setType(ui::dialog::location::FOLDER);

    core::location::single_folder::sptr result;

    while((result = std::dynamic_pointer_cast<core::location::single_folder>(dialog.show())))
    {
        if(std::filesystem::is_empty(result->get_folder()))
        {
            break;
        }

        // message box
        sight::ui::dialog::message message_box;
        message_box.setTitle("Overwrite confirmation");
        message_box.setMessage("The selected directory is not empty. Write anyway ?");
        message_box.setIcon(ui::dialog::message::QUESTION);
        message_box.addButton(ui::dialog::message::YES);
        message_box.addButton(ui::dialog::message::CANCEL);
        if(message_box.show() == sight::ui::dialog::message::YES)
        {
            break;
        }
    }

    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder().parent_path());
        dialog.saveDefaultLocation(default_directory);

        if(m_selectedExtension.empty())
        {
            // Ask user to select extension
            // Create a map with description that will be displayed to user, and extensions.
            std::map<std::string, std::string> description_to_extension;
            description_to_extension["VTK Legacy (*.vtk)"]                = ".vtk";
            description_to_extension["VTK Polydata (*.vtp"]               = ".vtp";
            description_to_extension["OBJ Wavefront Object (*.obj)"]      = ".obj";
            description_to_extension["PLY Polygonal File Format (*.ply)"] = ".ply";
            description_to_extension["STL StereoLithograpy (*.stl)"]      = ".stl";

            // Fill the descriptions vector with map keys.
            std::vector<std::string> descriptions;
            std::transform(
                std::begin(description_to_extension),
                std::end(description_to_extension),
                std::back_inserter(descriptions),
                [](auto const& _pair)
                {
                    return _pair.first;
                });
            sight::ui::dialog::selector extension_dialog;
            extension_dialog.setTitle("Extensions");
            extension_dialog.setMessage("Choose the extensions: ");
            extension_dialog.set_choices(descriptions);

            if(const auto& choices = extension_dialog.show(); !choices.empty())
            {
                m_selectedExtension = description_to_extension[choices.front()];
            }
        }
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void model_series_writer::starting()
{
}

//------------------------------------------------------------------------------

void model_series_writer::stopping()
{
}

//------------------------------------------------------------------------------

void model_series_writer::configuring()
{
    sight::io::service::writer::configuring();
    service::config_t config = this->get_config();

    auto ext = config.get<std::string>("extension", "");

    if(!ext.empty())
    {
        // Make sure to lowercase extension
        boost::to_lower(ext);

        if(ext != "vtk" && ext != "vtp" && ext != "stl" && ext != "ply" && ext != "obj")
        {
            SIGHT_ERROR("Extensions '" + ext + "' isn't managed by module::io::vtk::model_series_writer");
        }
        else
        {
            m_selectedExtension = "." + ext;
        }
    }
}

//------------------------------------------------------------------------------

void model_series_writer::info(std::ostream& _sstream)
{
    _sstream << "model_series_writer::info";
}

//------------------------------------------------------------------------------

template<typename WRITER>
typename WRITER::sptr configure_writer(const std::filesystem::path& _filename)
{
    typename WRITER::sptr writer = std::make_shared<WRITER>();
    writer->set_file(_filename);
    return writer;
}

//------------------------------------------------------------------------------

void model_series_writer::writeMesh(const std::filesystem::path& _filename, const data::mesh::csptr _mesh)
{
    sight::io::writer::object_writer::sptr mesh_writer;
    const auto ext = _filename.extension();
    if(ext == ".vtk")
    {
        mesh_writer = configure_writer<sight::io::vtk::MeshWriter>(_filename);
    }
    else if(ext == ".vtp")
    {
        mesh_writer = configure_writer<sight::io::vtk::VtpMeshWriter>(_filename);
    }
    else if(ext == ".obj")
    {
        mesh_writer = configure_writer<sight::io::vtk::ObjMeshWriter>(_filename);
    }
    else if(ext == ".stl")
    {
        mesh_writer = configure_writer<sight::io::vtk::StlMeshWriter>(_filename);
    }
    else if(ext == ".ply")
    {
        mesh_writer = configure_writer<sight::io::vtk::PlyMeshWriter>(_filename);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Extension '" + ext.string()
                + "' is not managed by module::io::vtk::model_series_writer."
            )
        );
    }

    m_sigJobCreated->emit(mesh_writer->getJob());

    mesh_writer->set_object(_mesh);
    mesh_writer->write();
}

//------------------------------------------------------------------------------

void model_series_writer::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked       = m_data.lock();
        const auto model_series = std::dynamic_pointer_cast<const data::model_series>(locked.get_shared());

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

        for(const auto& rec : model_series->getReconstructionDB())
        {
            SIGHT_ASSERT("Reconstruction from model series is not instanced", rec);
            data::mesh::sptr mesh = rec->getMesh();
            SIGHT_ASSERT("Mesh from reconstruction is not instanced", mesh);

            const std::filesystem::path filename = this->get_folder()
                                                   / (rec->getOrganName() + "_" + mesh->get_uuid()
                                                      + m_selectedExtension);
            try
            {
                this->writeMesh(filename, mesh);
                m_writeFailed = false;
            }
            catch(const std::exception& e)
            {
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
                sight::ui::dialog::message::show(
                    "Warning",
                    "Warning during saving",
                    sight::ui::dialog::message::WARNING
                );
            }
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
