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

#include "ioVTK/SMeshReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/IJob.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/MeshReader.hpp>
#include <fwVtkIO/ObjMeshReader.hpp>
#include <fwVtkIO/PlyMeshReader.hpp>
#include <fwVtkIO/StlMeshReader.hpp>
#include <fwVtkIO/VtpMeshReader.hpp>

#include <filesystem>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVTK::SMeshReader, ::fwData::Mesh )

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

::fwIO::IOPathType SMeshReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

SMeshReader::SMeshReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void SMeshReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SMeshReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to load Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialogFile.addFilter("OBJ File(.obj)", "*.obj");
    dialogFile.addFilter("PLY File(.ply)", "*.ply");
    dialogFile.addFilter("STL File(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }

}

//------------------------------------------------------------------------------

void SMeshReader::starting()
{
}

//------------------------------------------------------------------------------

void SMeshReader::stopping()
{
}

//------------------------------------------------------------------------------

void SMeshReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SMeshReader::info(std::ostream& _sstream )
{
    _sstream << "SMeshReader::info";
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

bool SMeshReader::loadMesh( const std::filesystem::path& vtkFile )
{
    bool ok = true;
    // Retrieve dataStruct associated with this service
    const auto meshlockedPtr = this->getLockedInOut< ::fwData::Mesh >(::fwIO::s_DATA_KEY);

    // Test extension to provide the reader

    ::fwDataIO::reader::IObjectReader::sptr meshReader;

    if(vtkFile.extension() == ".vtk")
    {
        meshReader = configureReader< ::fwVtkIO::MeshReader >(vtkFile);
    }
    else if(vtkFile.extension() == ".vtp")
    {
        meshReader = configureReader< ::fwVtkIO::VtpMeshReader >(vtkFile);
    }
    else if(vtkFile.extension() == ".obj")
    {
        meshReader = configureReader< ::fwVtkIO::ObjMeshReader >(vtkFile);
    }
    else if(vtkFile.extension() == ".stl")
    {
        meshReader = configureReader< ::fwVtkIO::StlMeshReader >(vtkFile);
    }
    else if(vtkFile.extension() == ".ply")
    {
        meshReader = configureReader< ::fwVtkIO::PlyMeshReader >(vtkFile);
    }
    else
    {
        FW_RAISE_EXCEPTION(::fwTools::Failed("Extension '"+ vtkFile.extension().string() +
                                             "' is not managed by ::ioVTK::SMeshReader."));
    }

    m_sigJobCreated->emit(meshReader->getJob());

    meshReader->setObject(meshlockedPtr.get_shared());

    try
    {
        meshReader->read();
    }
    catch(::fwTools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
        // Raise exception  for superior level
        FW_RAISE_EXCEPTION(e);
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        ::fwGui::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }

    return ok;
}

//------------------------------------------------------------------------------

void SMeshReader::updating()
{
    if( this->hasLocationDefined() )
    {
        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        this->loadMesh(this->getFile());
        this->notificationOfUpdate();

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SMeshReader::notificationOfUpdate()
{
    const auto meshLockedPtr = this->getLockedInOut< ::fwData::Mesh >(::fwIO::s_DATA_KEY);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = meshLockedPtr.get_shared()->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
