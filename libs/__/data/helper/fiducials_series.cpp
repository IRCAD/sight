/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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
        core::tools::object::sptr o = core::tools::id::get_object(*_fiducial.fiducial_uid);
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

//------------------------------------------------------------------------------

std::vector<data::fiducials_series::fiducial> fiducials_series::filter_fiducials(
    const data::fiducials_series::fiducials_series::fiducial_set& _fiducial_set,
    const std::optional<data::fiducials_series::shape> _shape,
    const std::optional<std::int32_t> _referenced_frame_number
)
{
    std::vector<data::fiducials_series::fiducial> fiducials;

    if(!_shape.has_value() && !_referenced_frame_number.has_value())
    {
        std::ranges::copy(
            _fiducial_set.fiducial_sequence,
            std::back_inserter(fiducials)
        );
    }
    else
    {
        for(std::size_t i = 0 ; i < _fiducial_set.fiducial_sequence.size() ; i++)
        {
            // Filter by shape
            bool shape_ok = false;
            // If we have no value, then we don't want to filter by shape
            if(!_shape.has_value())
            {
                shape_ok = true;
            }
            // If we have a value we must ensure that it matches the argument
            else
            {
                if(_fiducial_set.fiducial_sequence[i].shape_type == _shape)
                {
                    shape_ok = true;
                }
            }

            // Filter by referenced frame number (slice index)
            bool referenced_frame_number_ok = false;
            if(!_referenced_frame_number.has_value())
            {
                referenced_frame_number_ok = true;
            }
            else
            {
                const auto& sequence = _fiducial_set.referenced_image_sequence;
                if(sequence.has_value()
                   && !sequence->empty()
                   && !(sequence->at(i).referenced_frame_number.empty()))
                {
                    for(const auto& frame_number : sequence->at(i).referenced_frame_number)
                    {
                        if(frame_number == _referenced_frame_number)
                        {
                            referenced_frame_number_ok = true;
                        }
                    }
                }
            }

            // Perform the copy if the current entry matched
            if(shape_ok && referenced_frame_number_ok)
            {
                fiducials.push_back(_fiducial_set.fiducial_sequence[i]);
            }
        }
    }

    return fiducials;
}

} // namespace sight::data::helper
