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

#include "modules/io/vtk/SSeriesDBReader.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/helper/SeriesDB.hpp>
#include <data/SeriesDB.hpp>

#include <io/vtk/SeriesDBReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SSeriesDBReader::getIOPathType() const
{
    return sight::io::base::service::FILES;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setType(ui::base::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl");
    dialogFile.addFilter("MetaImage files", "*.mhd");
    dialogFile.addFilter("OBJ Files(.obj)", "*.obj");
    dialogFile.addFilter("PLY Files(.ply)", "*.ply");
    dialogFile.addFilter("STL Files(.stl)", "*.stl");
    dialogFile.addFilter("VTI Image files", "*.vti");
    dialogFile.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialogFile.addFilter("VTU Image files", "*.vtu");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    auto result = core::location::MultipleFiles::dynamicCast(dialogFile.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->getFiles();
        if(!paths.empty())
        {
            defaultDirectory->setFolder(paths[0].parent_path());
            dialogFile.saveDefaultLocation(defaultDirectory);
        }

        this->setFiles(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::info(std::ostream& _sstream)
{
    _sstream << "SSeriesDBReader::info";
}

//------------------------------------------------------------------------------

void SSeriesDBReader::loadSeriesDB(
    const std::vector<std::filesystem::path>& vtkFiles,
    const data::SeriesDB::sptr& seriesDB
)
{
    m_readFailed = true;

    auto reader = sight::io::vtk::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFiles(vtkFiles);

    m_sigJobCreated->emit(reader->getJob());

    try
    {
        reader->read();
        m_readFailed = false;
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
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked   = m_data.lock();
        const auto seriesDB = std::dynamic_pointer_cast<data::SeriesDB>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::SeriesDB::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            seriesDB
        );

        auto localSeriesDB = data::SeriesDB::New();

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        this->loadSeriesDB(this->getFiles(), localSeriesDB);

        if(!m_readFailed)
        {
            data::helper::SeriesDB sdbHelper(*seriesDB);
            sdbHelper.clear();

            // Notify removal.
            sdbHelper.notify();
            seriesDB->shallowCopy(localSeriesDB);

            auto sig = seriesDB->signal<data::SeriesDB::AddedSeriesSignalType>(
                data::SeriesDB::s_ADDED_SERIES_SIG
            );

            sig->asyncEmit(seriesDB->getContainer());
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
