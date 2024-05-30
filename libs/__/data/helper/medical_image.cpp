/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include "data/helper/medical_image.hpp"

#include "data/helper/field.hpp"

#include <data/boolean.hpp>
#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <cmath>
#include <numeric>
#include <utility> // std::pair

namespace sight::data::helper::medical_image
{

//------------------------------------------------------------------------------

bool check_image_validity(data::image::csptr _p_img)
{
    return _p_img ? check_image_validity(*_p_img) : false;
}

//------------------------------------------------------------------------------

bool check_image_validity(const data::image& _image)
{
    // Test if the image is allocated
    bool data_image_is_allocated = (_image.allocated_size_in_bytes() > 0);

    if(data_image_is_allocated)
    {
        std::size_t nb_dim = _image.num_dimensions();
        data_image_is_allocated &= nb_dim > 1;

        for(std::size_t k = 0 ; data_image_is_allocated && k < nb_dim ; ++k)
        {
            data_image_is_allocated = data_image_is_allocated && (_image.size()[k] >= 1);
        }
    }

    return data_image_is_allocated;
}

//------------------------------------------------------------------------------

bool check_image_slice_index(data::image::sptr _p_img)
{
    SIGHT_ASSERT("_pImg pointer null", _p_img);

    bool field_is_modified = false;

    const data::image::size_t& image_size = _p_img->size();

    const auto axial_idx    = get_slice_index(*_p_img, orientation_t::axial);
    const auto frontal_idx  = get_slice_index(*_p_img, orientation_t::frontal);
    const auto sagittal_idx = get_slice_index(*_p_img, orientation_t::sagittal);

    std::array<std::int64_t, 3> index_values = {0, 0, 0};

    // Check if values are out of bounds
    if(!axial_idx.has_value()
       || (axial_idx.has_value() && image_size[2] > 0
           && image_size[2] < static_cast<std::size_t>(axial_idx.value())))
    {
        index_values[2]   = static_cast<std::int64_t>(image_size[2] / 2);
        field_is_modified = true;
    }

    if(!frontal_idx.has_value()
       || (axial_idx.has_value() && image_size[1] > 0
           && image_size[1] < static_cast<std::size_t>(frontal_idx.value())))
    {
        index_values[1]   = static_cast<std::int64_t>(image_size[1] / 2);
        field_is_modified = true;
    }

    if(!sagittal_idx.has_value()
       || (sagittal_idx.has_value() && image_size[0] > 0
           && image_size[0] < static_cast<std::size_t>(sagittal_idx.value())))
    {
        index_values[0]   = static_cast<std::int64_t>(image_size[0] / 2);
        field_is_modified = true;
    }

    // Update or create fields.
    if(field_is_modified)
    {
        set_slice_index(*_p_img, orientation_t::axial, index_values[orientation_t::axial]);
        set_slice_index(*_p_img, orientation_t::frontal, index_values[orientation_t::frontal]);
        set_slice_index(*_p_img, orientation_t::sagittal, index_values[orientation_t::sagittal]);
    }

    return field_is_modified;
}

//------------------------------------------------------------------------------

bool is_buf_null(const data::image::buffer_t* _buf, const unsigned int _len)
{
    bool is_null       = false;
    const auto* buffer = static_cast<const data::image::buffer_t*>(_buf);
    is_null = 0 == std::accumulate(
        buffer,
        buffer + _len,
        0,
        std::bit_or<>()
    );
    return is_null;
}

//------------------------------------------------------------------------------

index_t compute_voxel_indices(const data::image& _image, const vec3_t& _pos)
{
    const auto& spacing = _image.spacing();
    const auto& origin  = _image.origin();

    return {
        static_cast<index_t::value_type>(std::round((_pos[0] - origin[0]) / spacing[0])),
        static_cast<index_t::value_type>(std::round((_pos[1] - origin[1]) / spacing[1])),
        static_cast<index_t::value_type>(std::round((_pos[2] - origin[2]) / spacing[2]))
    };
}

//------------------------------------------------------------------------------

std::pair<vec3_t, vec3_t> compute_bounding_box(const data::image& _image)
{
    const auto& spacing = _image.spacing();
    const auto& origin  = _image.origin();
    const auto& size    = _image.size();

    const vec3_t max {origin[0] + spacing[0] * static_cast<double>(size[0]),
                      origin[1] + spacing[1] * static_cast<double>(size[1]),
                      origin[2] + spacing[2] * static_cast<double>(size[2])
    };

    return {origin, max};
}

//-------------------------------------------------------------------------------
std::optional<std::int64_t> get_slice_index(
    const data::image& _image,
    const orientation_t& _orientation
)
{
    std::string orientation_index;
    switch(_orientation)
    {
        case orientation_t::axial:
            orientation_index = std::string(id::AXIAL_SLICE_INDEX);
            break;

        case orientation_t::sagittal:
            orientation_index = std::string(id::SAGITTAL_SLICE_INDEX);
            break;

        case orientation_t::frontal:
            orientation_index = std::string(id::FRONTAL_SLICE_INDEX);
            break;

        default:
            SIGHT_THROW_EXCEPTION(data::exception("Wrong orientation type."));
            break;
    }

    // Test if field exists
    const auto field = _image.get_field(orientation_index);
    if(field)
    {
        // Test if the type is data::integer.
        const auto field_int = _image.get_field<data::integer>(orientation_index);
        if(field_int)
        {
            // Get value.
            return field_int->value();
        }
    }

    return {};
}

//-------------------------------------------------------------------------------
void set_slice_index(
    data::image& _image,
    const orientation_t& _orientation,
    std::int64_t _slice_idx
)
{
    data::integer::sptr value = std::make_shared<data::integer>();
    value->set_value(_slice_idx);

    std::string orientation_index;
    switch(_orientation)
    {
        case orientation_t::axial:
            orientation_index = std::string(id::AXIAL_SLICE_INDEX);
            break;

        case orientation_t::sagittal:
            orientation_index = std::string(id::SAGITTAL_SLICE_INDEX);
            break;

        case orientation_t::frontal:
            orientation_index = std::string(id::FRONTAL_SLICE_INDEX);
            break;

        default:
            SIGHT_THROW_EXCEPTION(data::exception("Wrong orientation type."));
            break;
    }

    _image.set_field(orientation_index, value);
}

//-------------------------------------------------------------------------------

std::optional<double_t> get_slice_position(
    const data::image& _image,
    const orientation_t& _orientation
)
{
    const auto slice_idx_opt = get_slice_index(_image, _orientation);
    if(!slice_idx_opt)
    {
        return {};
    }

    const auto& spacing          = _image.spacing();
    const auto& origin           = _image.origin();
    const std::int64_t slice_idx = slice_idx_opt.value();
    return origin[_orientation] + static_cast<double>(slice_idx) * spacing[_orientation];
}

//-----------------------------------------------------------------------------
void set_slice_position(
    data::image& _image,
    const orientation_t& _orientation,
    double& _position
)
{
    const auto& spacing = _image.spacing();
    const auto& origin  = _image.origin();

    const auto new_index =
        static_cast<std::int64_t>((_position - origin[_orientation]) / spacing[_orientation]);
    set_slice_index(_image, _orientation, new_index);
}

//------------------------------------------------------------------------------

data::point_list::sptr get_landmarks(const data::image& _image)
{
    return _image.get_field<data::point_list>(std::string(id::LANDMARKS));
}

//------------------------------------------------------------------------------

void set_landmarks(data::image& _image, const data::point_list::sptr& _landmarks)
{
    if(_landmarks)
    {
        _image.set_field(std::string(id::LANDMARKS), _landmarks);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(data::exception("Trying to set nullptr as landmark field."));
    }
}

//------------------------------------------------------------------------------

data::vector::sptr get_distances(const data::image& _image)
{
    return _image.get_field<data::vector>(std::string(id::DISTANCES));
}

//------------------------------------------------------------------------------

void set_distances(data::image& _image, const data::vector::sptr& _distances)
{
    if(_distances)
    {
        _image.set_field(std::string(id::DISTANCES), _distances);
    }
    else
    {
        SIGHT_ERROR("Trying to set nullptr to distances field.");
    }
}

//------------------------------------------------------------------------------

bool get_distance_visibility(const data::image& _image)
{
    const auto visibility = _image.get_field<boolean>(std::string(id::DISTANCE_VISIBILITY));

    if(visibility)
    {
        return visibility->value();
    }

    // default value is true.
    return true;
}

//------------------------------------------------------------------------------

void set_distance_visibility(data::image& _image, bool _visibility)
{
    _image.set_field(std::string(id::DISTANCE_VISIBILITY), std::make_shared<data::boolean>(_visibility));
}

//------------------------------------------------------------------------------

bool get_landmarks_visibility(const data::image& _image)
{
    const auto visibility = _image.get_field<boolean>(std::string(id::LANDMARKS_VISIBILITY));

    if(visibility)
    {
        return visibility->value();
    }

    // default value is true.
    return true;
}

//------------------------------------------------------------------------------

void set_landmarks_visibility(data::image& _image, bool _visibility)
{
    _image.set_field(std::string(id::LANDMARKS_VISIBILITY), std::make_shared<data::boolean>(_visibility));
}

//------------------------------------------------------------------------------

data::matrix4::sptr get_direction(const data::image& _image)
{
    if(auto direction = _image.get_field<data::matrix4>(std::string(id::DIRECTION)); direction)
    {
        return direction;
    }

    return std::make_shared<data::matrix4>();
}

//------------------------------------------------------------------------------

void set_direction(data::image& _image, data::matrix4::sptr _direction)
{
    if(_direction)
    {
        _image.set_field(std::string(id::DIRECTION), _direction);
    }
}

//------------------------------------------------------------------------------

data::transfer_function::sptr get_transfer_function(const data::image& _image)
{
    return _image.get_field<data::transfer_function>(std::string(id::TRANSFER_FUNCTION));
}

//------------------------------------------------------------------------------

void set_transfer_function(data::image& _image, const data::transfer_function::sptr& _cmp)
{
    _image.set_field(std::string(id::TRANSFER_FUNCTION), _cmp);
}

//------------------------------------------------------------------------------

} // namespace sight::data::helper::medical_image
