/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ioVTK/SModelSeriesReader.hpp"

#include "ioVTK/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/tools/UUID.hpp>

#include <data/location/Folder.hpp>
#include <data/location/ILocation.hpp>
#include <data/location/MultiFiles.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/Reconstruction.hpp>

#include <fwVtkIO/MeshReader.hpp>
#include <fwVtkIO/ObjMeshReader.hpp>
#include <fwVtkIO/PlyMeshReader.hpp>
#include <fwVtkIO/StlMeshReader.hpp>
#include <fwVtkIO/VtpMeshReader.hpp>

#include <services/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <filesystem>

namespace ioVTK
{

fwServicesRegisterMacro( io::base::services::IReader, ::ioVTK::SModelSeriesReader, ::sight::data::ModelSeries )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesReader::SModelSeriesReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::io::base::services::IOPathType SModelSeriesReader::getIOPathType() const
{
    return io::base::services::FILES;
}

//------------------------------------------------------------------------------

void SModelSeriesReader::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void SModelSeriesReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(ui::base::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialogFile.addFilter("OBJ Files(.obj)", "*.obj");
    dialogFile.addFilter("PLY Files(.ply)", "*.ply");
    dialogFile.addFilter("STL Files(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    data::location::MultiFiles::sptr result;
    result = data::location::MultiFiles::dynamicCast( dialogFile.show() );
    if (result)
    {
        const data::location::ILocation::VectPathType paths = result->getPaths();
        if(!paths.empty())
        {
            _sDefaultPath = paths[0].parent_path();
            dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        }
        this->setFiles(paths);
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
    io::base::services::IReader::configuring();
}

//------------------------------------------------------------------------------

void SModelSeriesReader::info(std::ostream& _sstream )
{
    _sstream << "SModelSeriesReader::info";
}

//------------------------------------------------------------------------------

void SModelSeriesReader::updating()
{
    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        const auto modelSeriesLockedPtr = this->getLockedInOut< data::ModelSeries >(io::base::services::s_DATA_KEY);
        const auto modelSeries          = modelSeriesLockedPtr.get_shared();

        ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        data::ModelSeries::ReconstructionVectorType recDB = modelSeries->getReconstructionDB();
        data::ModelSeries::ReconstructionVectorType addedRecs;
        for(const data::location::ILocation::PathType& file :  this->getFiles())
        {
            data::Mesh::sptr mesh = data::Mesh::New();
            this->loadMesh(file, mesh);

            data::Reconstruction::sptr rec = data::Reconstruction::New();
            rec->setMesh(mesh);
            rec->setIsVisible(true);
            rec->setOrganName(file.stem().string());
            recDB.push_back(rec);
            addedRecs.push_back(rec);
        }
        cursor.setDefaultCursor();
        modelSeries->setReconstructionDB(recDB);

        auto sig = modelSeries->signal< data::ModelSeries::ReconstructionsAddedSignalType >(
            data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit(addedRecs);
        }
    }
}

//------------------------------------------------------------------------------

template< typename READER >
typename READER::sptr configureReader(const std::filesystem::path& _file )
{
    typename READER::sptr reader = READER::New();
    reader->setFile(_file);
    return reader;
}

//------------------------------------------------------------------------------

void SModelSeriesReader::loadMesh( const std::filesystem::path& _file, data::Mesh::sptr _mesh)
{
    // Test extension to provide the reader

    io::base::reader::IObjectReader::sptr meshReader;

    if(_file.extension() == ".vtk")
    {
        meshReader = configureReader< ::fwVtkIO::MeshReader >(_file);
    }
    else if(_file.extension() == ".vtp")
    {
        meshReader = configureReader< ::fwVtkIO::VtpMeshReader >(_file);
    }
    else if(_file.extension() == ".obj")
    {
        meshReader = configureReader< ::fwVtkIO::ObjMeshReader >(_file);
    }
    else if(_file.extension() == ".stl")
    {
        meshReader = configureReader< ::fwVtkIO::StlMeshReader >(_file);
    }
    else if(_file.extension() == ".ply")
    {
        meshReader = configureReader< ::fwVtkIO::PlyMeshReader >(_file);
    }
    else
    {
        FW_RAISE_EXCEPTION(core::tools::Failed("Extension '"+ _file.extension().string() +
                                               "' is not managed by ::ioVTK::SMeshReader."));
    }

    m_sigJobCreated->emit(meshReader->getJob());

    meshReader->setObject(_mesh);

    try
    {
        meshReader->read();
    }
    catch(core::tools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ui::base::dialog::IMessageDialog::WARNING);
        // Raise exception  for superior level
        FW_RAISE_EXCEPTION(e);
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ui::base::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading.",
            ui::base::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
