/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/io/vtk/image_series_reader.hpp"

#include "modules/io/vtk/image_reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/failed.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <data/image.hpp>

#include <io/__/service/ioTypes.hpp>
#include <io/__/service/reader.hpp>
#include <io/vtk/BitmapImageReader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

image_series_reader::image_series_reader() noexcept
{
    m_sig_job_created = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t image_series_reader::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

void image_series_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    // Initialize the available extensions for BitmapImageReader
    std::vector<std::string> ext;
    sight::io::vtk::bitmap_image_reader::get_available_extensions(ext);
    std::string available_extensions;

    if(!ext.empty())
    {
        available_extensions = "*" + ext.at(0);
        for(std::size_t i = 1 ; i < ext.size() ; i++)
        {
            available_extensions.append(" *").append(ext.at(i));
        }
    }

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a file to load an ImageSeries" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("Vtk", "*.vtk");
    dialog_file.add_filter("Vti", "*.vti");
    dialog_file.add_filter("MetaImage", "*.mhd");
    dialog_file.add_filter("Bitmap image", available_extensions);
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_option(ui::dialog::location::file_must_exist);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void image_series_reader::starting()
{
}

//------------------------------------------------------------------------------

void image_series_reader::stopping()
{
}

//------------------------------------------------------------------------------

void image_series_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void image_series_reader::info(std::ostream& _sstream)
{
    _sstream << "image_series_reader::info";
}

//------------------------------------------------------------------------------

void init_series(data::series::sptr _series)
{
    const std::string instance_uid     = core::tools::uuid::generate();
    const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date             = core::tools::get_date(now);
    const std::string time             = core::tools::get_time(now);

    _series->set_modality("OT");
    _series->set_series_date(date);
    _series->set_series_time(time);
    _series->set_series_description("image imported with VTK");
    std::string physicians = _series->get_performing_physician_name();
    if(physicians.empty())
    {
        physicians = core::tools::os::get_env("USERNAME", core::tools::os::get_env("LOGNAME", "Unknown"));
    }

    _series->set_performing_physician_name(physicians);
    _series->set_study_instance_uid(instance_uid);
    _series->set_study_date(date);
    _series->set_study_time(time);
}

//------------------------------------------------------------------------------

void image_series_reader::updating()
{
    m_read_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked       = m_data.lock();
        const auto image_series = std::dynamic_pointer_cast<data::image_series>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::image_series::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            image_series
        );

        sight::ui::busy_cursor cursor;

        if(image_reader::load_image(this->get_file(), image_series, m_sig_job_created))
        {
            init_series(image_series);

            auto sig = image_series->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }

            m_read_failed = false;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
