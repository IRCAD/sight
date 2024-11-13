/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/data/config.hpp>

#include <core/tools/dispatcher.hpp>
#include <core/tools/numeric_round_cast.hxx>

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/thread/region_threader.hpp>
#include <data/transfer_function.hpp>
#include <data/vector.hpp>

#include <optional>

namespace sight::data::helper
{

namespace id
{

// Note: keeping old name to preserve compatibility, should be harmonized in the future.
static constexpr std::string_view AXIAL_SLICE_INDEX    = "Axial Slice Index";
static constexpr std::string_view FRONTAL_SLICE_INDEX  = "Frontal Slice Index";
static constexpr std::string_view SAGITTAL_SLICE_INDEX = "Sagittal Slice Index";
static constexpr std::string_view LANDMARKS            = "m_imageLandmarksId";
static constexpr std::string_view DISTANCES            = "m_imageDistancesId";
static constexpr std::string_view DISTANCE_VISIBILITY  = "ShowDistances";
static constexpr std::string_view TRANSFER_FUNCTION    = "m_transferFunctionCompositeId";
static constexpr std::string_view LANDMARKS_VISIBILITY = "ShowLandmarks";
static constexpr std::string_view DIRECTION            = "direction";

} // namespace id

namespace medical_image
{

using index_t = std::array<int, 3>;
using vec3_t  = std::array<double, 3>;

enum orientation_t
{
    /// Directions.
    x_axis = 0,
    y_axis,
    z_axis,
    /// Planar definitions.
    sagittal = x_axis,
    frontal  = y_axis,
    axial    = z_axis
};

/**
 * @brief       Check if the image is valid.
 *
 * The image size must be > 1 for each dimension.
 *
 * @param[in]   _p_img   image checked.
 * @return      true if the image is valid.
 */
SIGHT_DATA_API bool check_image_validity(data::image::csptr _p_img);

/**
 * @brief       Check if the image is valid.
 *
 * The image size must be > 1 for each dimension.
 *
 * @param[in]   _p_img   image checked.
 * @return      true if the image is valid.
 */
SIGHT_DATA_API bool check_image_validity(const data::image& _p_img);

/**
 * @brief       Check if the image has slice index fields.
 *
 * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
 *
 * @param       _p_img   image which must contains the field.
 * @return      true if the fields are added.
 */
SIGHT_DATA_API bool check_image_slice_index(data::image::sptr _p_img);

/**
 * @brief Checks if image buffer contains zero, from begin to begin + len
 *
 * @param _buffer data::image::buffer_t* begin of the buffer.
 * @param _len unsigned int length, as begin+len.
 * @return boolean, true if null, false otherwise.
 */
SIGHT_DATA_API bool is_buf_null(const data::image::buffer_t* _buffer, unsigned int _len);

/**
 * @brief Return a buffer of image type's size, containing 'value' casted to image data type
 * @param[in] _image : reference image
 * @param[in] _value : value to map
 */
template<typename T>
SPTR(data::image::buffer_t) get_pixel_in_image_space(data::image::sptr _image, T & _value);

/**
 * @brief Return minimum and maximum values contained in image. If image
 * min or max value is out of T range, they are clamped to T capacity
 * @param[in] _img : image
 * @return: minimum value and maximum value
 */
template<typename T>
[[nodiscard]] std::pair<T, T> get_min_max(data::image::csptr _img);

/**
 * @brief Compute the indices of a 3D position inside an image. Beware, to be as generic as possible, no boundary check
 * is performed so it can lie outside the image. If you want to use it to access a voxel inside the image, be sure to
 * make these checks first and handle what you want to do in this case.
 * @param[in] _image : input image
 * @param[in] _pos : the 3D coordinates of the voxel
 * @return: the indices of the voxel
 */
SIGHT_DATA_API index_t compute_voxel_indices(const data::image& _image, const vec3_t& _pos);

/**
 * @brief Return the bounding box surrounding an image.
 * @param[in] _image : input image
 * @return: the bounding box as the minimum and a maximum coordinates
 */
SIGHT_DATA_API std::pair<vec3_t, vec3_t> compute_bounding_box(const data::image& _image);

// Getter/Setter for specific image fields

/**
 * @brief Helper function to get current slice index on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation
 * @return the current index as a std::int64_t.
 */
SIGHT_DATA_API std::optional<std::int64_t> get_slice_index(
    const data::image& _image,
    const orientation_t& _orientation
);

/**
 * @brief Helper function to get current slice position on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation
 * @return the current position as a double_t.
 */

SIGHT_DATA_API std::optional<double_t> get_slice_position(const data::image& _image, const orientation_t& _orientation);
/**
 * @brief Helper function to set current slice index on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation.
 * @param _slice_count : current slice index to set as std::int64_t.
 */
SIGHT_DATA_API void set_slice_index(
    data::image& _image,
    const orientation_t& _orientation,
    std::int64_t _slice_count
);

/**
 * @brief Helper function to set current slice position on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation.
 * @param _position  : current slice position to set as double_t.
 */

SIGHT_DATA_API void set_slice_position(
    data::image& _image,
    const orientation_t& _orientation,
    const double_t& _position
);

/**
 * @brief Helper function to get landmarks of a medical image.
 *
 * @param _image : input image reference.
 * @return a data::point_list::sptr containing landmarks, pointer can be null.
 */
SIGHT_DATA_API data::point_list::sptr get_landmarks(const data::image& _image);

/**
 * @brief Helper function to set landmarks in a medical image.
 *
 * @param _image : input image reference.
 * @param _landmarks : landmarks pointer as data::point_list::sptr.
 */
SIGHT_DATA_API void set_landmarks(data::image& _image, const data::point_list::sptr& _landmarks);

/**
 * @brief Helper function to calculate the slice index of a given fiducial point in a specified orientation within a
 * medical image.
 *
 * @param _image : The input image as a constant reference to data::image.
 * @param _point : The coordinates of the fiducial point as a std::array of three doubles.
 * @param _orientation : The orientation (axial, sagittal, or frontal) to calculate the slice index.
 * @return std::optional<std::int64_t> : The calculated slice index as an integer.
 */

SIGHT_DATA_API std::optional<std::int64_t> get_fiducial_slice_index(
    const data::image& _image,
    const std::array<double,
                     3>& _point,
    orientation_t _orientation
);

/**
 * @brief Helper function to get distances vector of a medical image.
 *
 * @param _image : input image reference.
 * @return a data::vector::sptr containing distances, pointer can be null.
 */
SIGHT_DATA_API data::vector::sptr get_distances(const data::image& _image);

/**
 * @brief Helper function to set distances in a medical image as a data::vector::sptr.
 *
 * @param _image : input image reference.
 * @param _distances : data::vector::sptr containing distances.
 */
SIGHT_DATA_API void set_distances(data::image& _image, const data::vector::sptr& _distances);

/**
 * @brief Helper function to get distance visibility on a medical image.
 *
 * @param _image : input image reference.
 * @return boolean value (visible or not).
 */
SIGHT_DATA_API bool get_distance_visibility(const data::image& _image);

/**
 * @brief Helper function to set distance visibility on a medical image.
 *
 * @param _image : input image reference.
 * @param _visibility : boolean to represents if distance is visible or not.
 */
SIGHT_DATA_API void set_distance_visibility(data::image& _image, bool _visibility);

/**
 * @brief Helper function to get landmarks visibility on a medical image.
 *
 * @param _image : input image reference.
 * @return boolean value (visible or not).
 */
SIGHT_DATA_API bool get_landmarks_visibility(const data::image& _image);

/**
 * @brief Helper function to set landmarks visibility on a medical image.
 *
 * @param _image : input image reference.
 * @param _visibility : boolean to represents if landmarks are visible or not.
 */
SIGHT_DATA_API void set_landmarks_visibility(data::image& _image, bool _visibility);

/**
 * @brief Helper to get comment on medical image.
 *
 * @param _image : input image reference.
 * @return std::string the comment, can be empty.
 */
SIGHT_DATA_API std::string get_comment(const data::image& _image);

/**
 * @brief Helper function to set comment on medical image.
 *
 * @param _image : input image reference.
 * @param _comment : std::string comment to add.
 */
SIGHT_DATA_API void set_comment(data::image& _image, const std::string& _comment);

/**
 * @brief Helper function to get label on a medical image.
 *
 * @param _image : input image reference.
 * @return std::string the label, can be empty.
 */
SIGHT_DATA_API std::string get_label(const data::image& _image);

/**
 * @brief Helper function to set label on medical image.
 *
 * @param _image : input image reference.
 * @param _label : std::string the label.
 */
SIGHT_DATA_API void set_label(data::image& _image, const std::string& _label);

/**
 * @brief Helper function to get the direction field from an image data.
 *
 * @param _image : input image reference.
 */
SIGHT_DATA_API data::matrix4::sptr get_direction(const data::image& _image);

/**
 * @brief Helper function to set the direction field from an image data.
 *
 * @param _image : input image reference.
 */
SIGHT_DATA_API void set_direction(data::image& _image, data::matrix4::sptr _direction);

// ------------------------------------------------------------------------------

template<typename VALUE>
class pixel_cast_and_set_functor
{
public:

    class param
    {
    public:

        using value_t          = VALUE;
        using buffer_type_sptr = std::shared_ptr<data::image::buffer_t>;

        param(value_t& _v) :
            value(_v)
        {
        }

        const value_t& value;
        buffer_type_sptr res;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(param& _param)
    {
        unsigned char image_type_size = sizeof(IMAGE);

        auto val = core::tools::numeric_round_cast<IMAGE>(_param.value);

        auto* buf = reinterpret_cast<data::image::buffer_t*>(&val);

        SPTR(data::image::buffer_t) res(new data::image::buffer_t[image_type_size]);
        std::copy(buf, buf + image_type_size, res.get());
        _param.res = res;
    }
};

// ------------------------------------------------------------------------------

template<typename VALUE, typename INT_INDEX>
class cast_and_set_functor
{
public:

    class param
    {
    public:

        using value_t = VALUE;
        using vec3_t  = INT_INDEX;

        param(vec3_t& _p, value_t& _v) :
            value(_v),
            point(_p)
        {
        }

        data::image::sptr image;
        const value_t& value;
        const vec3_t& point;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(param& _param)
    {
        auto* buffer                    = static_cast<IMAGE*>(_param.image->buffer());
        const INT_INDEX& p              = _param.point;
        const data::image::size_t& size = _param.image->size();
        const int& sx                   = size[0];
        const int& sy                   = size[1];
        const int& offset               = p[0] + sx * p[1] + p[2] * sx * sy;
        *(buffer + offset) = core::tools::numeric_round_cast<IMAGE>(_param.value);
    }
};

// ------------------------------------------------------------------------------

template<typename T>
SPTR(data::image::buffer_t) get_pixel_in_image_space(
    data::image::sptr _image,
    T & _value
)
{
    typename pixel_cast_and_set_functor<T>::param param(_value);

    core::type type = _image->type();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, pixel_cast_and_set_functor<T> >::invoke(
        type,
        param
    );
    return param.res;
}

// ------------------------------------------------------------------------------

template<typename T>
class min_max_functor
{
public:

    class param
    {
    public:

        param(data::image::csptr _img, T& _min, T& _max) :
            image(_img),
            min(_min),
            max(_max)
        {
        }

        data::image::csptr image;
        T& min;
        T& max;
    };

    using result_vector_t = std::vector<T>;

    //------------------------------------------------------------------------------

    template<typename IMAGE>
    static void get_min_max(
        const data::image::const_iterator<IMAGE>& _img_begin,
        result_vector_t& _min_res,
        result_vector_t& _max_res,
        std::ptrdiff_t _region_min,
        std::ptrdiff_t _region_max,
        std::size_t _i
)
    {
        const data::image::const_iterator<IMAGE> begin = _img_begin + _region_min;
        const data::image::const_iterator<IMAGE> end   = _img_begin + _region_max;

        typedef std::numeric_limits<IMAGE> ImgLimits;
        IMAGE imin = ImgLimits::max();
        IMAGE imax = ImgLimits::lowest();

        for(auto itr = begin ; itr != end ; ++itr)
        {
            IMAGE current_voxel = *itr;

            if(current_voxel < imin)
            {
                imin = current_voxel;
            }
            else if(current_voxel > imax)
            {
                imax = current_voxel;
            }
        }

        typedef std::numeric_limits<T> TLimits;
        constexpr T min_t = TLimits::lowest();
        constexpr T max_t = TLimits::max();

        const T min = (static_cast<T>(imin) < min_t) ? min_t : static_cast<T>(imin);
        const T max = (static_cast<T>(imax) > max_t) ? max_t : static_cast<T>(imax);

        _min_res[_i] = min;
        _max_res[_i] = max;
    }

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(param& _param)
    {
        const data::image::csptr image = _param.image;
        const auto dump_lock           = image->dump_lock();

        result_vector_t min_result;
        result_vector_t max_result;

        sight::data::thread::region_threader rt;
        min_result.resize(rt.number_of_thread());
        max_result.resize(rt.number_of_thread());
        rt(
            [capture0 = image->cbegin<IMAGE>(), &min_result, &max_result](std::ptrdiff_t _p_h1, std::ptrdiff_t _p_h2,
                                                                          std::size_t _p_h3, auto&& ...)
                {
                    return min_max_functor::get_min_max<IMAGE>(
                        capture0,
                        min_result,
                        max_result,
                        _p_h1,
                        _p_h2,
                        _p_h3
                    );
                },
            image->cend<IMAGE>() - image->cbegin<IMAGE>()
        );

        _param.min = *std::min_element(min_result.begin(), min_result.end());
        _param.max = *std::max_element(max_result.begin(), max_result.end());
    }
};

// ------------------------------------------------------------------------------

template<typename T>
std::pair<T, T> get_min_max(const data::image::csptr _img)
{
    T min = std::numeric_limits<T>::max();
    T max = std::numeric_limits<T>::min();
    typename min_max_functor<T>::param param(_img, min, max);

    core::type type = _img->type();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, min_max_functor<T> >::invoke(type, param);

    return {min, max};
}

} // namespace sight::data::helper::medical_image

} // namespace sight::data::helper
