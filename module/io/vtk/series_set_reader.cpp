/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "module/io/vtk/series_set_reader.hpp"

#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>

#include <data/series_set.hpp>

#include <io/vtk/series_set_reader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "job_created";

//------------------------------------------------------------------------------

series_set_reader::series_set_reader() noexcept :
    reader("Choose vtk files to load Series"),
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t series_set_reader::get_path_type() const
{
    return sight::io::service::files;
}

//------------------------------------------------------------------------------

void series_set_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_default_location(default_directory);
    dialog_file.set_type(ui::dialog::location::multi_files);
    dialog_file.set_title(*m_window_title);
    dialog_file.add_filter("All supported files", "*.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl");
    dialog_file.add_filter("MetaImage files", "*.mhd");
    dialog_file.add_filter("OBJ Files(.obj)", "*.obj");
    dialog_file.add_filter("PLY Files(.ply)", "*.ply");
    dialog_file.add_filter("STL Files(.stl)", "*.stl");
    dialog_file.add_filter("VTI image files", "*.vti");
    dialog_file.add_filter("VTK Legacy Files(.vtk)", "*.vtk");
    dialog_file.add_filter("VTK Polydata Files(.vtp)", "*.vtp");
    dialog_file.add_filter("VTU image files", "*.vtu");
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_option(ui::dialog::location::file_must_exist);

    auto result = std::dynamic_pointer_cast<core::location::multiple_files>(dialog_file.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->get_files();
        if(!paths.empty())
        {
            default_directory->set_folder(paths[0].parent_path());
            dialog_file.save_default_location(default_directory);
        }

        this->set_files(paths);
    }
    else
    {
        this->clear_locations();
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

void series_set_reader::load_series_set(
    const std::vector<std::filesystem::path>& _vtk_files,
    const data::series_set::sptr& _series_set
)
{
    m_read_failed = true;

    auto reader = std::make_shared<sight::io::vtk::series_set_reader>();
    reader->set_object(_series_set);
    reader->set_files(_vtk_files);

    m_sig_job_created->emit(reader->get_job());

    try
    {
        reader->read();
        m_read_failed = false;
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::warning
        );
    }
    catch(...)
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading.",
            sight::ui::dialog::message::warning
        );
    }
}

//------------------------------------------------------------------------------

void series_set_reader::updating()
{
    m_read_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked     = m_data.lock();
        const auto series_set = std::dynamic_pointer_cast<data::series_set>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::series_set::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            series_set
        );

        auto local_series_set = std::make_shared<data::series_set>();

        sight::ui::busy_cursor cursor;

        this->load_series_set(this->get_files(), local_series_set);

        if(!m_read_failed)
        {
            const auto scoped_emitter = series_set->scoped_emit();

            series_set->clear();
            series_set->shallow_copy(local_series_set);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
