/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "io/dicom/helper/series.hpp"

#include <core/os/temp_path.hpp>
#include <core/spy_log.hpp>

#include <data/map.hpp>
#include <data/string.hpp>

#include <memory>
#include <string>

namespace sight::io::dicom::helper
{

const std::string FIELD_DOWNLOADED_INSTANCES = "downloaded_instances";

//------------------------------------------------------------------------------

std::filesystem::path series::get_path(
    const sight::data::series& _series,
    std::optional<std::size_t> _instance_number
)
{
    const auto root_path = core::os::temp_dir::shared_directory() / "dicom/";

    auto series_path = std::filesystem::path(root_path / _series.get_series_instance_uid());

    if(!std::filesystem::exists(series_path))
    {
        std::filesystem::create_directories(series_path);
    }

    if(_instance_number.has_value())
    {
        auto downloaded_instances = _series.get_field<sight::data::map>(FIELD_DOWNLOADED_INSTANCES);
        if(downloaded_instances)
        {
            if(auto it = downloaded_instances->find(std::to_string(_instance_number.value()));
               it != downloaded_instances->end())
            {
                series_path /= std::dynamic_pointer_cast<sight::data::string>(it->second)->to_string();
                return series_path;
            }
        }
    }

    return series_path;
}

//------------------------------------------------------------------------------

std::filesystem::path series::get_path(const std::string& _series_uid, const std::string& _instance_uid)
{
    const auto root_path = core::os::temp_dir::shared_directory() / "dicom/";

    auto series_path = std::filesystem::path(root_path / _series_uid);

    if(!std::filesystem::exists(series_path))
    {
        std::filesystem::create_directories(series_path);
    }

    series_path /= _instance_uid;

    return series_path;
}

//------------------------------------------------------------------------------

void series::add_instance(
    sight::data::series& _series,
    std::size_t _instance_number,
    const std::string& _sop_instance_uid
)
{
    // Add the instance number as a field in the DICOM container of the series
    auto downloaded_instances = _series.get_field(FIELD_DOWNLOADED_INSTANCES, std::make_shared<sight::data::map>());
    auto sop_instance_uid     = std::make_shared<data::string>(_sop_instance_uid);
    (*downloaded_instances)[std::to_string(_instance_number)] = sop_instance_uid;
    _series.set_field(FIELD_DOWNLOADED_INSTANCES, downloaded_instances);
}

//------------------------------------------------------------------------------

bool series::is_instance_available(const sight::data::series& _series, std::size_t _instance_number)
{
    auto downloaded_instances = _series.get_field<sight::data::map>(FIELD_DOWNLOADED_INSTANCES);
    if(downloaded_instances == nullptr)
    {
        return false;
    }

    return downloaded_instances->contains(std::to_string(_instance_number));
}

} // namespace sight::io::dicom::helper
