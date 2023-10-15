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

#include "modules/io/vtk/series_set_reader.hpp"

#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>

#include <data/series_set.hpp>

#include <io/vtk/SeriesSetReader.hpp>

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

series_set_reader::series_set_reader() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType series_set_reader::getIOPathType() const
{
    return sight::io::service::FILES;
}

//------------------------------------------------------------------------------

void series_set_reader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setType(ui::dialog::location::MULTI_FILES);
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl");
    dialogFile.addFilter("MetaImage files", "*.mhd");
    dialogFile.addFilter("OBJ Files(.obj)", "*.obj");
    dialogFile.addFilter("PLY Files(.ply)", "*.ply");
    dialogFile.addFilter("STL Files(.stl)", "*.stl");
    dialogFile.addFilter("VTI image files", "*.vti");
    dialogFile.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialogFile.addFilter("VTU image files", "*.vtu");
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

void series_set_reader::starting()
{
}

//------------------------------------------------------------------------------

void series_set_reader::stopping()
{
}

//------------------------------------------------------------------------------

void series_set_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void series_set_reader::info(std::ostream& _sstream)
{
    _sstream << "series_set_reader::info";
}

//------------------------------------------------------------------------------

void series_set_reader::loadSeriesSet(
    const std::vector<std::filesystem::path>& vtkFiles,
    const data::series_set::sptr& series_set
)
{
    m_readFailed = true;

    auto reader = std::make_shared<sight::io::vtk::SeriesSetReader>();
    reader->set_object(series_set);
    reader->set_files(vtkFiles);

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

void series_set_reader::updating()
{
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked     = m_data.lock();
        const auto series_set = std::dynamic_pointer_cast<data::series_set>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::series_set::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            series_set
        );

        auto local_series_set = std::make_shared<data::series_set>();

        sight::ui::BusyCursor cursor;

        this->loadSeriesSet(this->get_files(), local_series_set);

        if(!m_readFailed)
        {
            const auto scoped_emitter = series_set->scoped_emit();

            series_set->clear();
            series_set->shallow_copy(local_series_set);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
