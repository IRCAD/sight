/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include "module/io/vtk/image_series_reader.hpp"

#include "module/io/vtk/image_reader.hpp"

#include <core/tools/date_and_time.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <data/image_series.hpp>

#include <io/__/service/io_types.hpp>
#include <io/vtk/bitmap_image_reader.hpp>

#include <ui/__/cursor.hpp>

#include <filesystem>
#include <utility>

namespace sight::module::io::vtk
{

//------------------------------------------------------------------------------

image_series_reader::image_series_reader() noexcept :
    reader("Choose a file to load an ImageSeries")
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t image_series_reader::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

std::vector<std::pair<std::string, std::string> > image_series_reader::get_supported_extensions() const
{
    std::vector<std::string> extensions;
    sight::io::vtk::bitmap_image_reader::get_available_extensions(extensions);

    std::string bitmap_wildcards;
    for(const auto& extension : extensions)
    {
        if(!bitmap_wildcards.empty())
        {
            bitmap_wildcards += ' ';
        }

        bitmap_wildcards += "*" + extension;
    }

    std::vector<std::pair<std::string, std::string> > filters {
        {"Vtk", "*.vtk"},
        {"Vti", "*.vti"},
        {"MetaImage", "*.mhd"}
    };

    if(!bitmap_wildcards.empty())
    {
        filters.emplace_back("Bitmap image", std::move(bitmap_wildcards));
    }

    return filters;
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

static void init_series(data::series::sptr _series)
{
    const std::string instance_uid     = core::tools::uuid::generate();
    const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date             = core::tools::get_date(now);
    const std::string time             = core::tools::get_time(now);

    _series->set_modality(data::dicom::modality_t::ot);
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
            + sight::io::service::READER_DATA_KEY
            + "' is not correctly set.",
            image_series
        );

        sight::ui::busy_cursor cursor;

        const auto& file = this->get_file();

        auto observer = this->observe("Reading " + file.string() + " file");

        if(image_reader::load_image(file, image_series, observer))
        {
            init_series(image_series);

            image_series->async_emit(this, data::signals::MODIFIED);
            m_read_failed = false;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
