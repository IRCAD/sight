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

#include "fiducials_series.hpp"

namespace sight::data::helper
{

//------------------------------------------------------------------------------

data::point_list::sptr fiducials_series::to_point_list(const data::fiducials_series::fiducial& _fiducial)
{
    data::point_list::sptr res;
    if(_fiducial.fiducial_uid.has_value())
    {
        core::object::sptr o = core::id::get_object(*_fiducial.fiducial_uid);
        if(o == nullptr)
        {
            res = std::make_shared<data::point_list>();
            res->set_id(*_fiducial.fiducial_uid);
        }
        else
        {
            res = std::dynamic_pointer_cast<data::point_list>(o);
            SIGHT_ASSERT(
                "The ID " << *_fiducial.fiducial_uid << " is already set to an object which isn't a point list.",
                res
            );
        }
    }
    else
    {
        // The fiducial doesn't have a meaningful way to uniquely identify it, ignore it.
        return nullptr;
    }

    if(_fiducial.shape_type == sight::data::fiducials_series::shape::ruler && _fiducial.contour_data.size() < 2)
    {
        return nullptr;
    }

    if(_fiducial.shape_type == sight::data::fiducials_series::shape::shape && _fiducial.contour_data.size() < 3)
    {
        return nullptr;
    }

    if(!_fiducial.contour_data.empty())
    {
        for(const auto& contour_data : _fiducial.contour_data)
        {
            res->push_back(
                std::make_shared<data::point>(
                    contour_data.x,
                    contour_data.y,
                    contour_data.z
                )
            );
        }
    }
    else
    {
        // Position with Graphic Coordinates Data Sequence isn't supported
        return nullptr;
    }

    return res;
}

} // namespace sight::data::helper
