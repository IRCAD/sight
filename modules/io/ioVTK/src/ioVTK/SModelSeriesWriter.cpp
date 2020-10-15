/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "ioVTK/SModelSeriesWriter.hpp"

#include "ioVTK/SMeshWriter.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwJobs/IJob.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/UUID.hpp>

#include <fwVtkIO/MeshWriter.hpp>
#include <fwVtkIO/ObjMeshWriter.hpp>
#include <fwVtkIO/PlyMeshWriter.hpp>
#include <fwVtkIO/StlMeshWriter.hpp>
#include <fwVtkIO/VtpMeshWriter.hpp>

#include <filesystem>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioVTK::SModelSeriesWriter, ::fwMedData::ModelSeries )

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesWriter::SModelSeriesWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SModelSeriesWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
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

    ::fwGui::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialog.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialog.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;

    while ((result = ::fwData::location::Folder::dynamicCast( dialog.show() )))
    {
        if( std::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == ::fwGui::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialog.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
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
            ::fwGui::dialog::SelectorDialog extensionDialog;
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
    ::fwIO::IWriter::configuring();
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    auto ext = config.get<std::string>("extension", "");

    if(!ext.empty())
    {
        // Make sure to lowercase extension
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        if(ext != "vtk" && ext != "vtp" && ext != "stl" && ext != "ply" && ext != "obj")
        {
            SLM_ERROR("Extensions '" + ext + "' isn't managed by ioVTK::SModelSeriesWriter");
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

void SModelSeriesWriter::writeMesh(const std::filesystem::path& _filename, const ::fwData::Mesh::csptr _mesh)
{
    ::fwDataIO::writer::IObjectWriter::sptr meshWriter;
    const auto ext = _filename.extension();
    if(ext == ".vtk")
    {
        meshWriter = configureWriter< ::fwVtkIO::MeshWriter >(_filename);
    }
    else if(ext == ".vtp")
    {
        meshWriter = configureWriter< ::fwVtkIO::VtpMeshWriter >(_filename);
    }
    else if(ext == ".obj")
    {
        meshWriter = configureWriter< ::fwVtkIO::ObjMeshWriter >(_filename);
    }
    else if(ext == ".stl")
    {
        meshWriter = configureWriter< ::fwVtkIO::StlMeshWriter >(_filename);
    }
    else if(ext == ".ply")
    {
        meshWriter = configureWriter< ::fwVtkIO::PlyMeshWriter >(_filename);
    }
    else
    {
        FW_RAISE_EXCEPTION(::fwTools::Failed("Extension '"+ ext.string() +
                                             "' is not managed by ::ioVTK::SModelSeriesWriter."));
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
        const auto modelSeriesLockedPtr = this->getLockedInput< const ::fwMedData::ModelSeries >(::fwIO::s_DATA_KEY);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        const auto modelSeries                                         = modelSeriesLockedPtr.get_shared();
        const ::fwMedData::ModelSeries::ReconstructionVectorType& recs = modelSeries->getReconstructionDB();
        for(const ::fwData::Reconstruction::csptr& rec :  recs)
        {
            SLM_ASSERT("Reconstruction from model series is not instanced", rec);
            ::fwData::Mesh::sptr mesh = rec->getMesh();
            SLM_ASSERT("Mesh from reconstruction is not instanced", mesh);

            const std::filesystem::path filename = this->getFolder() /
                                                   (rec->getOrganName() + "_" + ::fwTools::UUID::get(mesh) +
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

                ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    ss.str(),
                    ::fwGui::dialog::IMessageDialog::WARNING);
            }
            catch( ... )
            {
                m_writeFailed = true;
                ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    "Warning during saving",
                    ::fwGui::dialog::IMessageDialog::WARNING);
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
