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
    m_sigJobCreated = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType series_set_reader::getIOPathType() const
{
    return sight::io::service::FILES;
}

//------------------------------------------------------------------------------

void series_set_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setType(ui::dialog::location::MULTI_FILES);
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialog_file.addFilter("All supported files", "*.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl");
    dialog_file.addFilter("MetaImage files", "*.mhd");
    dialog_file.addFilter("OBJ Files(.obj)", "*.obj");
    dialog_file.addFilter("PLY Files(.ply)", "*.ply");
    dialog_file.addFilter("STL Files(.stl)", "*.stl");
    dialog_file.addFilter("VTI image files", "*.vti");
    dialog_file.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialog_file.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialog_file.addFilter("VTU image files", "*.vtu");
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::multiple_files>(dialog_file.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->get_files();
        if(!paths.empty())
        {
            default_directory->set_folder(paths[0].parent_path());
            dialog_file.saveDefaultLocation(default_directory);
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
    const std::vector<std::filesystem::path>& _vtk_files,
    const data::series_set::sptr& _series_set
)
{
    m_readFailed = true;

    auto reader = std::make_shared<sight::io::vtk::SeriesSetReader>();
    reader->set_object(_series_set);
    reader->set_files(_vtk_files);

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
