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

#include "modules/io/vtk/SModelSeriesWriter.hpp"

#include "modules/io/vtk/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/tools/UUID.hpp>

#include <data/location/Folder.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <service/macros.hpp>

#include <io/vtk/MeshWriter.hpp>
#include <io/vtk/ObjMeshWriter.hpp>
#include <io/vtk/PlyMeshWriter.hpp>
#include <io/vtk/StlMeshWriter.hpp>
#include <io/vtk/VtpMeshWriter.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <filesystem>

namespace sight::modules::io::vtk
{

fwServicesRegisterMacro( ::sight::io::base::service::IWriter, ::sight::modules::io::vtk::SModelSeriesWriter,
                         ::sight::data::ModelSeries )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesWriter::SModelSeriesWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SModelSeriesWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::configureWithIHM()
{
    this->openLocationDialog();
}
//------------------------------------------------------------------------------

void SModelSeriesWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    sight::ui::base::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialog.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialog.setType(ui::base::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result;

    while ((result = data::location::Folder::dynamicCast( dialog.show() )))
    {
        if( std::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::YES);
        messageBox.addButton(ui::base::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == sight::ui::base::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialog.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());

        if(m_selectedExtension.empty())
        {
            // Ask user to select extension
            // Create a map with description that will be displayed to user, and extensions.
            std::map < std::string, std::string > descriptionToExtension;
            descriptionToExtension["VTK Legacy (*.vtk)"]                = ".vtk";
            descriptionToExtension["VTK Polydata (*.vtp"]               = ".vtp";
            descriptionToExtension["OBJ Wavefront Object (*.obj)"]      = ".obj";
            descriptionToExtension["PLY Polygonal File Format (*.ply)"] = ".ply";
            descriptionToExtension["STL StereoLithograpy (*.stl)"]      = ".stl";

            // Fill the descriptions vector with map keys.
            std::vector< std::string > descriptions;
            std::transform(std::begin(descriptionToExtension), std::end(descriptionToExtension),
                           std::back_inserter(descriptions),
                           [](auto const& pair)
                {
                    return pair.first;
                });
            sight::ui::base::dialog::SelectorDialog extensionDialog;
            extensionDialog.setTitle("Extensions");
            extensionDialog.setMessage("Choose the extensions: ");
            extensionDialog.setSelections(descriptions);

            const auto selected = extensionDialog.show();
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
    sight::io::base::service::IWriter::configuring();
    service::IService::ConfigType config = this->getConfigTree();

    auto ext = config.get<std::string>("extension", "");

    if(!ext.empty())
    {
        // Make sure to lowercase extension
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        if(ext != "vtk" && ext != "vtp" && ext != "stl" && ext != "ply" && ext != "obj")
        {
            SLM_ERROR("Extensions '" + ext + "' isn't managed by modules::io::vtk::SModelSeriesWriter");
        }
        else
        {
            m_selectedExtension = "." + ext;
        }
    }

}

//------------------------------------------------------------------------------

void SModelSeriesWriter::info(std::ostream& _sstream )
{
    _sstream << "SModelSeriesWriter::info";
}
//------------------------------------------------------------------------------

template< typename WRITER >
typename WRITER::sptr configureWriter(const std::filesystem::path& _filename)
{
    typename WRITER::sptr writer = WRITER::New();
    writer->setFile(_filename);
    return writer;
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::writeMesh(const std::filesystem::path& _filename, const data::Mesh::csptr _mesh)
{
    sight::io::base::writer::IObjectWriter::sptr meshWriter;
    const auto ext = _filename.extension();
    if(ext == ".vtk")
    {
        meshWriter = configureWriter< sight::io::vtk::MeshWriter >(_filename);
    }
    else if(ext == ".vtp")
    {
        meshWriter = configureWriter< sight::io::vtk::VtpMeshWriter >(_filename);
    }
    else if(ext == ".obj")
    {
        meshWriter = configureWriter< sight::io::vtk::ObjMeshWriter >(_filename);
    }
    else if(ext == ".stl")
    {
        meshWriter = configureWriter< sight::io::vtk::StlMeshWriter >(_filename);
    }
    else if(ext == ".ply")
    {
        meshWriter = configureWriter< sight::io::vtk::PlyMeshWriter >(_filename);
    }
    else
    {
        FW_RAISE_EXCEPTION(core::tools::Failed("Extension '"+ ext.string() +
                                               "' is not managed by modules::io::vtk::SModelSeriesWriter."));
    }

    m_sigJobCreated->emit(meshWriter->getJob());

    meshWriter->setObject(_mesh);
    meshWriter->write();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::updating()
{
    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        const auto modelSeriesLockedPtr =
            this->getLockedInput< const data::ModelSeries >(sight::io::base::service::s_DATA_KEY);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        const auto modelSeries                                  = modelSeriesLockedPtr.get_shared();
        const data::ModelSeries::ReconstructionVectorType& recs = modelSeries->getReconstructionDB();
        for(const data::Reconstruction::csptr& rec :  recs)
        {
            SLM_ASSERT("Reconstruction from model series is not instanced", rec);
            data::Mesh::sptr mesh = rec->getMesh();
            SLM_ASSERT("Mesh from reconstruction is not instanced", mesh);

            const std::filesystem::path filename = this->getFolder() /
                                                   (rec->getOrganName() + "_" + core::tools::UUID::get(mesh) +
                                                    m_selectedExtension);
            try
            {
                this->writeMesh(filename, mesh);
            }
            catch (const std::exception& e)
            {
                m_writeFailed = true;
                std::stringstream ss;
                ss << "Warning during saving : " << e.what();

                sight::ui::base::dialog::MessageDialog::show(
                    "Warning",
                    ss.str(),
                    sight::ui::base::dialog::IMessageDialog::WARNING);
            }
            catch( ... )
            {
                m_writeFailed = true;
                sight::ui::base::dialog::MessageDialog::show(
                    "Warning",
                    "Warning during saving",
                    sight::ui::base::dialog::IMessageDialog::WARNING);
            }
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
