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

#include "modules/io/vtk/SModelSeriesWriter.hpp"

#include "modules/io/vtk/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/uuid.hpp>

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

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

SModelSeriesWriter::SModelSeriesWriter() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType SModelSeriesWriter::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation(defaultDirectory);
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
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(ui::dialog::message::QUESTION);
        messageBox.addButton(ui::dialog::message::YES);
        messageBox.addButton(ui::dialog::message::CANCEL);
        if(messageBox.show() == sight::ui::dialog::message::YES)
        {
            break;
        }
    }

    if(result)
    {
        this->set_folder(result->get_folder());
        defaultDirectory->set_folder(result->get_folder().parent_path());
        dialog.saveDefaultLocation(defaultDirectory);

        if(m_selectedExtension.empty())
        {
            // Ask user to select extension
            // Create a map with description that will be displayed to user, and extensions.
            std::map<std::string, std::string> descriptionToExtension;
            descriptionToExtension["VTK Legacy (*.vtk)"]                = ".vtk";
            descriptionToExtension["VTK Polydata (*.vtp"]               = ".vtp";
            descriptionToExtension["OBJ Wavefront Object (*.obj)"]      = ".obj";
            descriptionToExtension["PLY Polygonal File Format (*.ply)"] = ".ply";
            descriptionToExtension["STL StereoLithograpy (*.stl)"]      = ".stl";

            // Fill the descriptions vector with map keys.
            std::vector<std::string> descriptions;
            std::transform(
                std::begin(descriptionToExtension),
                std::end(descriptionToExtension),
                std::back_inserter(descriptions),
                [](auto const& pair)
                {
                    return pair.first;
                });
            sight::ui::dialog::selector extensionDialog;
            extensionDialog.setTitle("Extensions");
            extensionDialog.setMessage("Choose the extensions: ");
            extensionDialog.set_choices(descriptions);

            const auto selected = extensionDialog.show()[0];
            m_selectedExtension = descriptionToExtension[selected];
        }
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::configuring()
{
    sight::io::service::writer::configuring();
    service::config_t config = this->getConfiguration();

    auto ext = config.get<std::string>("extension", "");

    if(!ext.empty())
    {
        // Make sure to lowercase extension
        boost::to_lower(ext);

        if(ext != "vtk" && ext != "vtp" && ext != "stl" && ext != "ply" && ext != "obj")
        {
            SIGHT_ERROR("Extensions '" + ext + "' isn't managed by module::io::vtk::SModelSeriesWriter");
        }
        else
        {
            m_selectedExtension = "." + ext;
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::info(std::ostream& _sstream)
{
    _sstream << "SModelSeriesWriter::info";
}

//------------------------------------------------------------------------------

template<typename WRITER>
typename WRITER::sptr configureWriter(const std::filesystem::path& _filename)
{
    typename WRITER::sptr writer = std::make_shared<WRITER>();
    writer->set_file(_filename);
    return writer;
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::writeMesh(const std::filesystem::path& _filename, const data::Mesh::csptr _mesh)
{
    sight::io::writer::IObjectWriter::sptr meshWriter;
    const auto ext = _filename.extension();
    if(ext == ".vtk")
    {
        meshWriter = configureWriter<sight::io::vtk::MeshWriter>(_filename);
    }
    else if(ext == ".vtp")
    {
        meshWriter = configureWriter<sight::io::vtk::VtpMeshWriter>(_filename);
    }
    else if(ext == ".obj")
    {
        meshWriter = configureWriter<sight::io::vtk::ObjMeshWriter>(_filename);
    }
    else if(ext == ".stl")
    {
        meshWriter = configureWriter<sight::io::vtk::StlMeshWriter>(_filename);
    }
    else if(ext == ".ply")
    {
        meshWriter = configureWriter<sight::io::vtk::PlyMeshWriter>(_filename);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Extension '" + ext.string()
                + "' is not managed by module::io::vtk::SModelSeriesWriter."
            )
        );
    }

    m_sigJobCreated->emit(meshWriter->getJob());

    meshWriter->setObject(_mesh);
    meshWriter->write();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked      = m_data.lock();
        const auto modelSeries = std::dynamic_pointer_cast<const data::ModelSeries>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::ModelSeries::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            modelSeries
        );

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        for(const auto& rec : modelSeries->getReconstructionDB())
        {
            SIGHT_ASSERT("Reconstruction from model series is not instanced", rec);
            data::Mesh::sptr mesh = rec->getMesh();
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
