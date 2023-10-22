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

sight::io::service::IOPathType series_set_reader::getIOPathType() const
{
    return sight::io::service::FILES;
}

//------------------------------------------------------------------------------

void series_set_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void series_set_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose an image file" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialog_file.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialog_file.setType(ui::dialog::location::MULTI_FILES);
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

void series_set_reader::updating()
{
    // Do it right here in case of exception...
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto data       = m_data.lock();
        const auto series_set = std::dynamic_pointer_cast<data::series_set>(data.get_shared());
        SIGHT_ASSERT("The inout key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", series_set);

        // Set cursor busy
        sight::ui::BusyCursor cursor;

        bool read_failed               = false;
        auto local_set                 = std::make_shared<data::series_set>();
        const std::string instance_uid = core::tools::UUID::generate();

        for(const std::filesystem::path& path : this->get_files())
        {
            auto img_series = std::make_shared<data::image_series>();
            series_set_reader::initSeries(img_series, instance_uid);

            if(!image_reader::loadImage(path, img_series))
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

        m_readFailed = read_failed;
    }
}

//------------------------------------------------------------------------------

void series_set_reader::initSeries(data::series::sptr _series, const std::string& _instance_uid)
{
    _series->setModality("OT");
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::get_date(now);
    const std::string time       = core::tools::get_time(now);
    _series->setSeriesDate(date);
    _series->setSeriesTime(time);

    _series->setStudyInstanceUID(_instance_uid);
    _series->setStudyDate(date);
    _series->setStudyTime(time);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
