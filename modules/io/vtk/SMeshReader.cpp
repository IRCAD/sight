/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/io/vtk/SMeshReader.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/jobs/IJob.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <io/vtk/MeshReader.hpp>
#include <io/vtk/ObjMeshReader.hpp>
#include <io/vtk/PlyMeshReader.hpp>
#include <io/vtk/StlMeshReader.hpp>
#include <io/vtk/VtpMeshReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SMeshReader::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

SMeshReader::SMeshReader() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void SMeshReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to load Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.obj *.ply *.stl");
    dialogFile.addFilter("OBJ File(.obj)", "*.obj");
    dialogFile.addFilter("PLY File(.ply)", "*.ply");
    dialogFile.addFilter("STL File(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->setFile(result->getFile());
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
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SMeshReader::info(std::ostream& _sstream)
{
    _sstream << "SMeshReader::info";
}

//------------------------------------------------------------------------------

template<typename READER>
typename READER::sptr configureReader(const std::filesystem::path& _file)
{
    typename READER::sptr reader = READER::New();
    reader->setFile(_file);
    return reader;
}

//------------------------------------------------------------------------------

bool SMeshReader::loadMesh(const std::filesystem::path& vtkFile)
{
    bool ok = true;

    // Retrieve dataStruct associated with this service
    const auto locked = m_data.lock();
    const auto mesh   = std::dynamic_pointer_cast<data::Mesh>(locked.get_shared());

    SIGHT_ASSERT(
        "The object is not a '"
        + data::Mesh::classname()
        + "' or '"
        + sight::io::base::service::s_DATA_KEY
        + "' is not correctly set.",
        mesh
    );

    // Test extension to provide the reader
    sight::io::base::reader::IObjectReader::sptr meshReader;

    if(vtkFile.extension() == ".vtk")
    {
        meshReader = configureReader<sight::io::vtk::MeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".vtp")
    {
        meshReader = configureReader<sight::io::vtk::VtpMeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".obj")
    {
        meshReader = configureReader<sight::io::vtk::ObjMeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".stl")
    {
        meshReader = configureReader<sight::io::vtk::StlMeshReader>(vtkFile);
    }
    else if(vtkFile.extension() == ".ply")
    {
        meshReader = configureReader<sight::io::vtk::PlyMeshReader>(vtkFile);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::Failed(
                "Extension '" + vtkFile.extension().string()
                + "' is not managed by module::io::vtk::SMeshReader."
            )
        );
    }

    m_sigJobCreated->emit(meshReader->getJob());

    meshReader->setObject(mesh);

    try
    {
        meshReader->read();
    }
    catch(core::tools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;

        // Raise exception  for superior level
        SIGHT_THROW_EXCEPTION(e);
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }
    catch(...)
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading.",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }

    return ok;
}

//------------------------------------------------------------------------------

void SMeshReader::updating()
{
    if(this->hasLocationDefined())
    {
        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        this->loadMesh(this->getFile());
        this->notificationOfUpdate();

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SMeshReader::notificationOfUpdate()
{
    const auto locked = m_data.lock();
    const auto mesh   = std::dynamic_pointer_cast<data::Mesh>(locked.get_shared());

    data::Object::ModifiedSignalType::sptr sig;
    sig = mesh->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
