/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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
#include <data/string.hpp>
#include <data/vector.hpp>

#include <cmath>
#include <numeric>
#include <utility> // std::pair

namespace sight::data::helper
{

namespace id
{

// Note: keeping old name to preserve compatibility, should be harmonized in the future.
static constexpr std::string_view axial_slice_index    = "Axial Slice Index";
static constexpr std::string_view frontal_slice_index  = "Frontal Slice Index";
static constexpr std::string_view sagittal_slice_index = "Sagittal Slice Index";
static constexpr std::string_view landmarks            = "m_imageLandmarksId";
static constexpr std::string_view distances            = "m_imageDistancesId";
static constexpr std::string_view distance_visibility  = "ShowDistances";
static constexpr std::string_view transferFunction     = "m_transferFunctionCompositeId";
static constexpr std::string_view landmarks_visibility = "ShowLandmarks";

} // namespace id

namespace medical_image
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
    bool data_image_is_allocated = (_image.getAllocatedSizeInBytes() > 0);

    if(data_image_is_allocated)
    {
        std::size_t nb_dim = _image.numDimensions();
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

    const data::image::Size& image_size = _p_img->size();

    const auto axial_idx    = get_slice_index(*_p_img, orientation_t::AXIAL);
    const auto frontal_idx  = get_slice_index(*_p_img, orientation_t::FRONTAL);
    const auto sagittal_idx = get_slice_index(*_p_img, orientation_t::SAGITTAL);

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
        set_slice_index(*_p_img, orientation_t::AXIAL, index_values[orientation_t::AXIAL]);
        set_slice_index(*_p_img, orientation_t::FRONTAL, index_values[orientation_t::FRONTAL]);
        set_slice_index(*_p_img, orientation_t::SAGITTAL, index_values[orientation_t::SAGITTAL]);
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

std::optional<std::int64_t> get_slice_index(
    const data::image& _image,
    const orientation_t& _orientation
)
{
    std::string orientation_index;
    switch(_orientation)
    {
        case orientation_t::AXIAL:
            orientation_index = std::string(id::axial_slice_index);
            break;

        case orientation_t::SAGITTAL:
            orientation_index = std::string(id::sagittal_slice_index);
            break;

        case orientation_t::FRONTAL:
            orientation_index = std::string(id::frontal_slice_index);
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

//------------------------------------------------------------------------------

void set_slice_index(
    data::image& _image,
    const orientation_t& _orientation,
    std::int64_t _slice_idx
)
{
    data::integer::sptr value = std::make_shared<data::integer>();
    value->setValue(_slice_idx);

    std::string orientation_index;
    switch(_orientation)
    {
        case orientation_t::AXIAL:
            orientation_index = std::string(id::axial_slice_index);
            break;

        case orientation_t::SAGITTAL:
            orientation_index = std::string(id::sagittal_slice_index);
            break;

        case orientation_t::FRONTAL:
            orientation_index = std::string(id::frontal_slice_index);
            break;

        default:
            SIGHT_THROW_EXCEPTION(data::exception("Wrong orientation type."));
            break;
    }

    _image.set_field(orientation_index, value);
}

//------------------------------------------------------------------------------

data::point_list::sptr get_landmarks(const data::image& _image)
{
    return _image.get_field<data::point_list>(std::string(id::landmarks));
}

//------------------------------------------------------------------------------

void set_landmarks(data::image& _image, const data::point_list::sptr& _landmarks)
{
    if(_landmarks)
    {
        _image.set_field(std::string(id::landmarks), _landmarks);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(data::exception("Trying to set nullptr as landmark field."));
    }
}

//------------------------------------------------------------------------------

data::vector::sptr get_distances(const data::image& _image)
{
    return _image.get_field<data::vector>(std::string(id::distances));
}

//------------------------------------------------------------------------------

void set_distances(data::image& _image, const data::vector::sptr& _distances)
{
    if(_distances)
    {
        _image.set_field(std::string(id::distances), _distances);
    }
    else
    {
        SIGHT_ERROR("Trying to set nullptr to distances field.");
    }
}

//------------------------------------------------------------------------------

bool get_distance_visibility(const data::image& _image)
{
    const auto visibility = _image.get_field<boolean>(std::string(id::distance_visibility));

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
    _image.set_field(std::string(id::distance_visibility), std::make_shared<data::boolean>(_visibility));
}

//------------------------------------------------------------------------------

bool get_landmarks_visibility(const data::image& _image)
{
    const auto visibility = _image.get_field<boolean>(std::string(id::landmarks_visibility));

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
    _image.set_field(std::string(id::landmarks_visibility), std::make_shared<data::boolean>(_visibility));
}

//------------------------------------------------------------------------------

data::transfer_function::sptr get_transfer_function(const data::image& _image)
{
    return _image.get_field<data::transfer_function>(std::string(id::transferFunction));
}

//------------------------------------------------------------------------------

void set_transfer_function(data::image& _image, const data::transfer_function::sptr& _cmp)
{
    _image.set_field(std::string(id::transferFunction), _cmp);
}

//------------------------------------------------------------------------------

} //namespace medical_image

} // namespace sight::data::helper
