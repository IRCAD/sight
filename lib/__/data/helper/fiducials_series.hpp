/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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
};

} // namespace sight::data::helper
