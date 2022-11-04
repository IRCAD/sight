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

#include "modules/io/vtk/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Mesh.hpp>

#include <io/vtk/MeshWriter.hpp>
#include <io/vtk/ObjMeshWriter.hpp>
#include <io/vtk/PlyMeshWriter.hpp>
#include <io/vtk/StlMeshWriter.hpp>
#include <io/vtk/VtpMeshWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::io::vtk
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SMeshWriter::SMeshWriter() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SMeshWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SMeshWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to save Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("OBJ File(.obj)", "*.obj");
    dialogFile.addFilter("PLY File(.ply)", "*.ply");
    dialogFile.addFilter("STL File(.stl)", "*.stl");
    dialogFile.addFilter("VTK Legacy File(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata File(.vtp)", "*.vtp");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        this->setFile(result->getFile());
        m_selectedExtension = dialogFile.getSelectedExtensions().front();
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
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
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SMeshWriter::info(std::ostream& _sstream)
{
    _sstream << "SMeshWriter::info";
}

//------------------------------------------------------------------------------

template<typename WRITER>
typename WRITER::sptr configureWriter(const std::filesystem::path& _file)
{
    typename WRITER::sptr writer = WRITER::New();
    writer->setFile(_file);
    return writer;
}

//------------------------------------------------------------------------------

void SMeshWriter::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked = m_data.lock();
        const auto mesh   = std::dynamic_pointer_cast<const data::Mesh>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::Mesh::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            mesh
        );

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

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

        sight::io::base::writer::IObjectWriter::sptr meshWriter;

        if(extensionToUse == ".vtk")
        {
            meshWriter = configureWriter<sight::io::vtk::MeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".vtp")
        {
            meshWriter = configureWriter<sight::io::vtk::VtpMeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".stl")
        {
            meshWriter = configureWriter<sight::io::vtk::StlMeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".ply")
        {
            meshWriter = configureWriter<sight::io::vtk::PlyMeshWriter>(fileToWrite);
        }
        else if(extensionToUse == ".obj")
        {
            meshWriter = configureWriter<sight::io::vtk::ObjMeshWriter>(fileToWrite);
        }
        else
        {
            SIGHT_THROW_EXCEPTION(
                core::tools::Failed(
                    "Extension '" + fileToWrite.extension().string()
                    + "' is not managed by module::io::vtk::SMeshWriter."
                )
            );
        }

        m_sigJobCreated->emit(meshWriter->getJob());

        meshWriter->setObject(mesh);

        try
        {
            meshWriter->write();
            m_writeFailed = false;
        }
        catch(core::tools::Failed& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                sight::ui::base::dialog::IMessageDialog::WARNING
            );

            // Raise exception  for superior level
            SIGHT_THROW_EXCEPTION(e);
        }
        catch(const std::exception& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
        catch(...)
        {
            m_writeFailed = true;
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "Warning during saving",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
