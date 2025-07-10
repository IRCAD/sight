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

#pragma once

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <geometry/__/matrix.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vec_swizzle.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace sight::geometry::data
{

/**
 * @brief Return the transform matrix that convert image voxels in world coordinates, taking into account origin,
 * orientation and spacing.
 *
 * @param _image reference image
 * @return transform matrix
 */
template<typename M = glm::dmat4>
requires std::is_same_v<M, glm::mat4>|| std::is_same_v<M, glm::dmat4>
inline M image_to_world_transform(const sight::data::image& _image)
{
    const auto origin  = glm::make_vec3(_image.origin().data());
    const auto spacing = glm::make_vec3(_image.spacing().data());

    const auto orientation = _image.orientation();
    const M transform {
        orientation[0], orientation[3], orientation[6], 0.,
        orientation[1], orientation[4], orientation[7], 0.,
        orientation[2], orientation[5], orientation[8], 0.,
        origin[0], origin[1], origin[2], 1
    };

    return glm::scale(transform, glm::vec<3, typename M::value_type>(spacing));
}

/**
 * @brief Return the transform matrix that convert world coordinates into image voxels coordinates, taking into account
 * origin, orientation and spacing.
 *
 * @param _image reference image
 * @return transform matrix
 */
template<typename M = glm::dmat4>
requires std::is_same_v<M, glm::mat4>|| std::is_same_v<M, glm::dmat4>
inline M world_to_image_transform(const sight::data::image& _image)
{
    const auto origin      = _image.origin();
    const auto orientation = _image.orientation();

    const M transform {
        orientation[0], orientation[3], orientation[6], 0.,
        orientation[1], orientation[4], orientation[7], 0.,
        orientation[2], orientation[5], orientation[8], 0.,
        origin[0], origin[1], origin[2], 1
    };

    const auto spacing = glm::make_vec3(_image.spacing().data());
    SIGHT_ASSERT("Spacing is null", spacing[0] != 0. && spacing[1] != 0. && spacing[2] != 0.);

    const auto scale = glm::scale(glm::identity<M>(), glm::vec<3, typename M::value_type>(1.0 / spacing));
    return scale * geometry::inverse_translation_rotation(transform);
}

/**
 * @brief Convert to image voxel coordinates
 *
 * @tparam W input container type (default glm::dvec3), I output container type (default glm::ivec3)
 * @param _image reference image
 * @param _world image coordinates
 * @param _round if true, round final computed values using std::round so 0.9877.. will be 1.
 *               This mitigated floating point precision issues
 * @param _clamp if true, returns the closest valid image coordinates, if the world coordinates are out of bounds
 * @return image coordinates
 */
template<typename I = glm::ivec3, typename W = glm::dvec3>
I world_to_image(const sight::data::image& _image, const W& _world, bool _round = false, bool _clamp = false)
{
    const glm::dvec4 world_vector {
        glm::dvec4::value_type(_world[0]),
        glm::dvec4::value_type(_world[1]),
        glm::dvec4::value_type(_world[2]),
        glm::dvec4::value_type(1)
    };

    const glm::dmat4 transform = world_to_image_transform(_image);
    const auto image_vector    = transform * world_vector;

    auto voxel = glm::xyz(_round ? glm::round(image_vector) : image_vector);

    if(_clamp)
    {
        const glm::dvec3 size = glm::make_vec3(_image.size().data());
        voxel = glm::clamp(voxel, glm::dvec3(0.0), size - 1.0);
    }

    const I res = {
        typename I::value_type(voxel[0]),
        typename I::value_type(voxel[1]),
        typename I::value_type(voxel[2])
    };

    return res;
}

/**
 * @brief Convert to world coordinates
 *
 * @tparam I input container type (default glm::ivec3), W output container type (default glm::dvec3)
 * @param _image reference image
 * @param _voxel image coordinates
 * @return world coordinates
 */
template<typename W = glm::dvec3, typename I = glm::ivec3>
W image_to_world(const sight::data::image& _image, const I& _voxel)
{
    const glm::dvec4 image_vector = {
        glm::dvec4::value_type(_voxel[0]),
        glm::dvec4::value_type(_voxel[1]),
        glm::dvec4::value_type(_voxel[2]),
        glm::dvec4::value_type(1)
    };

    const glm::dmat4 transform = image_to_world_transform(_image);
    const auto world_vector    = transform * image_vector;

    return {world_vector.x, world_vector.y, world_vector.z};
}

/**
 * @brief Helper function to calculate the slice index of a given fiducial point in a specified orientation within a
 * medical image.
 *
 * @param _image : The input image as a constant reference to data::image.
 * @param _point : The coordinates of the fiducial point as a std::array of three doubles.
 * @param _axis : The orientation (axial, sagittal, or frontal) to calculate the slice index.
 * @return std::optional<std::int64_t> : The calculated slice index as an integer.
 */
template<typename V>
std::optional<std::int64_t> get_fiducial_slice_index(
    const sight::data::image& _image,
    const V& _point,
    sight::data::helper::medical_image::axis_t _axis
)
{
    const auto point_in_image = world_to_image(_image, _point, true);
    const auto slice_index    = point_in_image[_axis];

    if(slice_index < 0 || slice_index >= std::int64_t(_image.size()[_axis]))
    {
        return std::nullopt;
    }

    return slice_index;
}

} // namespace sight::geometry::data
