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

#include "ioVTK/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/Mesh.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwVtkIO/MeshWriter.hpp>
#include <fwVtkIO/ObjMeshWriter.hpp>
#include <fwVtkIO/PlyMeshWriter.hpp>
#include <fwVtkIO/StlMeshWriter.hpp>
#include <fwVtkIO/VtpMeshWriter.hpp>

#include <services/macros.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioVTK::SMeshWriter, ::sight::data::Mesh )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SMeshWriter::SMeshWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SMeshWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SMeshWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SMeshWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to save Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("OBJ File(.obj)", "*.obj");
    dialogFile.addFilter("PLY File(.ply)", "*.ply");
    dialogFile.addFilter("STL File(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_selectedExtension = dialogFile.getCurrentSelection();
        _sDefaultPath       = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SMeshWriter::starting()
{
}

//------------------------------------------------------------------------------

void SMeshWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SMeshWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SMeshWriter::info(std::ostream& _sstream )
{
    _sstream << "SMeshWriter::info";
}

//------------------------------------------------------------------------------

template< typename WRITER >
typename WRITER::sptr configureWriter(const std::filesystem::path& _file )
{
    typename WRITER::sptr writer = WRITER::New();
    writer->setFile(_file);
    return writer;
}

//------------------------------------------------------------------------------

void SMeshWriter::updating()
{
    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        const auto meshlockedPtr = this->getLockedInput< const data::Mesh >(::fwIO::s_DATA_KEY);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        std::filesystem::path fileToWrite   = this->getFile();
        const std::string providedExtension = fileToWrite.extension().string();
        std::string extensionToUse;

        // Check if file has an extension.
        if(providedExtension.empty())
        {
            // No extension provided, add extension of selected filter.
            extensionToUse = m_selectedExtension;
            fileToWrite   += extensionToUse;
        }
        else
        {
            extensionToUse = providedExtension;
        }

        ::fwDataIO::writer::IObjectWriter::sptr meshWriter;

        if(extensionToUse == ".vtk")
        {
            meshWriter = configureWriter< ::fwVtkIO::MeshWriter >(fileToWrite);
        }
        else if(extensionToUse == ".vtp")
        {
            meshWriter = configureWriter< ::fwVtkIO::VtpMeshWriter >(fileToWrite);
        }
        else if(extensionToUse == ".stl")
        {
            meshWriter = configureWriter< ::fwVtkIO::StlMeshWriter >(fileToWrite);
        }
        else if(extensionToUse == ".ply")
        {
            meshWriter = configureWriter< ::fwVtkIO::PlyMeshWriter >(fileToWrite);
        }
        else if(extensionToUse == ".obj")
        {
            meshWriter = configureWriter< ::fwVtkIO::ObjMeshWriter >(fileToWrite);
        }
        else
        {
            FW_RAISE_EXCEPTION(core::tools::Failed("Extension '"+ fileToWrite.extension().string() +
                                                   "' is not managed by ::ioVTK::SMeshWriter."));
        }

        m_sigJobCreated->emit(meshWriter->getJob());

        meshWriter->setObject(meshlockedPtr.get_shared());

        try
        {
            meshWriter->write();
        }
        catch(core::tools::Failed& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            ::fwGui::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
            // Raise exception  for superior level
            FW_RAISE_EXCEPTION(e);
        }
        catch (const std::exception& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            ::fwGui::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
        catch( ... )
        {
            m_writeFailed = true;
            ::fwGui::dialog::MessageDialog::show(
                "Warning",
                "Warning during saving",
                ::fwGui::dialog::IMessageDialog::WARNING);
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
