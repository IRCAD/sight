/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#pragma once

#include <data/fiducials_series.hpp>
#include <data/point_list.hpp>

namespace sight::data::helper
{

class SIGHT_DATA_CLASS_API fiducials_series
{
public:

    /**
     * @brief Generates a point list from a fiducial.
     * @param _fiducial fiducial to convert.
     * @return a point list.
     */
    SIGHT_DATA_API static data::point_list::sptr to_point_list(const data::fiducials_series::fiducial& _fiducial);

    /**
     * Get all the fiducials satisfying one or several features.
     * @param _fiducial_set The fiducial set whose fiducials must be filtered
     * @return The list of fiducial whose shape type is point
     */
    [[nodiscard]] SIGHT_DATA_API static std::vector<data::fiducials_series::fiducial> filter_fiducials(
        const data::fiducials_series::fiducial_set& _fiducial_set,
        const std::optional<data::fiducials_series::shape> _shape  = std::nullopt,
        const std::optional<std::int32_t> _referenced_frame_number = std::nullopt
    );
};

} // namespace sight::data::helper
