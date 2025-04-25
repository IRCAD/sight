/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/helper/fiducial.hpp"

#include "io/dicom/helper/dicom_data_tools.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/point_list.hpp>
#include <data/series_set.hpp>
#include <data/vector.hpp>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

bool fiducial::contains_landmarks(const SPTR(data::series_set)& _series_set)
{
    // Let's find if a series contains distances
    return std::any_of(
        _series_set->begin(),
        _series_set->end(),
        [](const data::series::sptr& _series)
        {
            if(auto image_series = std::dynamic_pointer_cast<data::image_series>(_series))
            {
                if(data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*image_series))
                {
                    return !point_list->get_points().empty();
                }
            }

            return false;
        });
}

//------------------------------------------------------------------------------

bool fiducial::contains_distances(const SPTR(data::series_set)& _series_set)
{
    return std::any_of(
        _series_set->begin(),
        _series_set->end(),
        [](const data::series::sptr& _series)
        {
            if(auto image_series = std::dynamic_pointer_cast<data::image_series>(_series))
            {
                if(auto distance_vector = data::helper::medical_image::get_distances(*image_series))
                {
                    return !distance_vector->empty();
                }
            }

            return false;
        });
}

//------------------------------------------------------------------------------

bool fiducial::contains_3d_distances(const SPTR(data::series_set)& _series_set)
{
    // Let's find if a series contains distances
    for(const auto& series : *_series_set)
    {
        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);
        if(image_series)
        {
            const auto& distance_vector = data::helper::medical_image::get_distances(*image_series);
            if(distance_vector && !distance_vector->empty())
            {
                for(const auto& object : *distance_vector)
                {
                    const auto& point_list = std::dynamic_pointer_cast<data::point_list>(object);
                    if(point_list && point_list->get_points().size() >= 2)
                    {
                        const auto& point1              = *point_list->get_points().begin();
                        const auto& point2              = *(++point_list->get_points().begin());
                        const std::size_t frame_number1 =
                            io::dicom::helper::dicom_data_tools::convert_point_to_frame_number(image_series, point1);
                        const std::size_t frame_number2 =
                            io::dicom::helper::dicom_data_tools::convert_point_to_frame_number(image_series, point2);
                        if(frame_number1 != frame_number2)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
