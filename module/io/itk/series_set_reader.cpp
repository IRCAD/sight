/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "module/io/itk/image_reader.hpp"

#include <core/tools/date_and_time.hpp>
#include <core/tools/uuid.hpp>

#include <data/image_series.hpp>
#include <data/series_set.hpp>

#include <ui/__/cursor.hpp>

namespace sight::module::io::itk
{

series_set_reader::series_set_reader() noexcept :
    reader("Choose an image file")
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t series_set_reader::get_path_type() const
{
    return sight::io::service::files;
}

//------------------------------------------------------------------------------

void series_set_reader::configuring()
{
    sight::io::service::reader::configuring();

    if(const auto config = this->get_config().get_child_optional("config.<xmlattr>"); config)
    {
        m_append = config->get("append", false);
    }
}

//------------------------------------------------------------------------------

std::vector<std::pair<std::string, std::string> > series_set_reader::get_supported_extensions() const
{
    return {
        {"NIfTI (.nii)", "*.nii *.nii.gz"},
        {"Inr (.inr)", "*.inr *.inr.gz"}
    };
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
        SIGHT_ASSERT("The inout key '" + sight::io::service::READER_DATA_KEY + "' is not correctly set.", series_set);

        // Set cursor busy
        sight::ui::busy_cursor cursor;

        bool read_failed               = false;
        auto local_set                 = std::make_shared<data::series_set>();
        const std::string instance_uid = core::tools::uuid::generate();

        for(const std::filesystem::path& path : this->get_files())
        {
            auto img_series = std::make_shared<data::image_series>();
            series_set_reader::init_series(img_series, instance_uid);
            img_series->set_series_description(path.filename().string());
            img_series->set_file(path);

            auto read_observer = this->observe("Loading images... ");

            if(!image_reader::load_image(path, img_series, read_observer))
            {
                read_failed = true;
            }

            local_set->push_back(img_series);
        }

        if(!local_set->empty())
        {
            const auto scoped_emitter = series_set->scoped_emit();
            if(!m_append)
            {
                series_set->clear();
                series_set->insert(series_set->cend(), local_set->cbegin(), local_set->cend());
            }
            else
            {
                const std::size_t duplicate_count = series_set->append_unique(*local_set);
                if(duplicate_count > 0)
                {
                    this->warn(
                        duplicate_count == 1
                        ? "This image is already loaded."
                        : std::to_string(duplicate_count) + " images are already loaded."
                    );
                }
            }
        }

        m_read_failed = read_failed;
    }
}

//------------------------------------------------------------------------------

void series_set_reader::init_series(data::series::sptr _series, const std::string& _instance_uid)
{
    _series->set_modality(data::dicom::modality_t::ot);
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
