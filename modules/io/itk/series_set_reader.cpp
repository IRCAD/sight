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

#include "series_set_reader.hpp"

#include "modules/io/itk/image_reader.hpp"

#include <core/base.hpp>
#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/uuid.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/series_set.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

sight::io::service::path_type_t series_set_reader::get_path_type() const
{
    return sight::io::service::files;
}

//------------------------------------------------------------------------------

void series_set_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void series_set_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose an image file" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialog_file.add_filter("Inr (.inr.gz)", "*.inr.gz");
    dialog_file.set_type(ui::dialog::location::multi_files);
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

void series_set_reader::updating()
{
    // Do it right here in case of exception...
    m_read_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto data       = m_data.lock();
        const auto series_set = std::dynamic_pointer_cast<data::series_set>(data.get_shared());
        SIGHT_ASSERT("The inout key '" + sight::io::service::DATA_KEY + "' is not correctly set.", series_set);

        // Set cursor busy
        sight::ui::busy_cursor cursor;

        bool read_failed               = false;
        auto local_set                 = std::make_shared<data::series_set>();
        const std::string instance_uid = core::tools::uuid::generate();

        for(const std::filesystem::path& path : this->get_files())
        {
            auto img_series = std::make_shared<data::image_series>();
            series_set_reader::init_series(img_series, instance_uid);

            if(!image_reader::load_image(path, img_series))
            {
                read_failed = true;
            }

            local_set->push_back(img_series);
        }

        if(!local_set->empty())
        {
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
            series_set->insert(series_set->begin(), local_set->cbegin(), local_set->cend());
        }

        m_read_failed = read_failed;
    }
}

//------------------------------------------------------------------------------

void series_set_reader::init_series(data::series::sptr _series, const std::string& _instance_uid)
{
    _series->set_modality("OT");
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::get_date(now);
    const std::string time       = core::tools::get_time(now);
    _series->set_series_date(date);
    _series->set_series_time(time);

    _series->set_study_instance_uid(_instance_uid);
    _series->set_study_date(date);
    _series->set_study_time(time);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
