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

#include "modules/io/vtk/SModelSeriesReader.hpp"

#include "modules/io/vtk/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/uuid.hpp>

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

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

SModelSeriesReader::SModelSeriesReader() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType SModelSeriesReader::getIOPathType() const
{
    return sight::io::service::FILES;
}

//-----------------------------------------------------------------------------

void SModelSeriesReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setType(ui::dialog::location::MULTI_FILES);
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialogFile.addFilter("OBJ Files(.obj)", "*.obj");
    dialogFile.addFilter("PLY Files(.ply)", "*.ply");
    dialogFile.addFilter("STL Files(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::multiple_files>(dialogFile.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->get_files();
        if(!paths.empty())
        {
            defaultDirectory->set_folder(paths[0].parent_path());
            dialogFile.saveDefaultLocation(defaultDirectory);
        }

        this->set_files(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesReader::starting()
{
}

//------------------------------------------------------------------------------

void SModelSeriesReader::stopping()
{
}

//------------------------------------------------------------------------------

void SModelSeriesReader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void SModelSeriesReader::info(std::ostream& _sstream)
{
    _sstream << "SModelSeriesReader::info";
}

//------------------------------------------------------------------------------

void SModelSeriesReader::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked      = m_data.lock();
        const auto modelSeries = std::dynamic_pointer_cast<data::ModelSeries>(locked.get_shared());

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

        data::ModelSeries::ReconstructionVectorType recDB = modelSeries->getReconstructionDB();
        data::ModelSeries::ReconstructionVectorType addedRecs;
        for(const auto& file : this->get_files())
        {
            auto mesh = std::make_shared<data::Mesh>();
            this->loadMesh(file, mesh);

            data::Reconstruction::sptr rec = std::make_shared<data::Reconstruction>();
            rec->setMesh(mesh);
            rec->setIsVisible(true);
            rec->setOrganName(file.stem().string());
            recDB.push_back(rec);
            addedRecs.push_back(rec);
        }

        cursor.setDefaultCursor();
        modelSeries->setReconstructionDB(recDB);

        auto sig = modelSeries->signal<data::ModelSeries::ReconstructionsAddedSignalType>(
            data::ModelSeries::RECONSTRUCTIONS_ADDED_SIG
        );
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit(addedRecs);
        }
    }
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

void SModelSeriesReader::loadMesh(const std::filesystem::path& _file, data::Mesh::sptr _mesh)
{
    // Test extension to provide the reader

    sight::io::reader::IObjectReader::sptr meshReader;

    if(_file.extension() == ".vtk")
    {
        meshReader = configureReader<sight::io::vtk::MeshReader>(_file);
    }
    else if(_file.extension() == ".vtp")
    {
        meshReader = configureReader<sight::io::vtk::VtpMeshReader>(_file);
    }
    else if(_file.extension() == ".obj")
    {
        meshReader = configureReader<sight::io::vtk::ObjMeshReader>(_file);
    }
    else if(_file.extension() == ".stl")
    {
        meshReader = configureReader<sight::io::vtk::StlMeshReader>(_file);
    }
    else if(_file.extension() == ".ply")
    {
        meshReader = configureReader<sight::io::vtk::PlyMeshReader>(_file);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Extension '" + _file.extension().string()
                + "' is not managed by module::io::vtk::SMeshReader."
            )
        );
    }

    m_sigJobCreated->emit(meshReader->getJob());

    meshReader->setObject(_mesh);

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
