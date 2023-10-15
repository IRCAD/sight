/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "camera_set.hpp"

#include "data/container.hxx"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::camera_set);

namespace sight::data
{

camera_set::camera_set()
{
    new_signal<added_camera_signal_t>(ADDED_CAMERA_SIG);
    new_signal<removed_camera_signal_t>(REMOVED_CAMERA_SIG);
    new_signal<extrinsic_calibrated_signal_t>(EXTRINSIC_CALIBRATED_SIG);
}

//------------------------------------------------------------------------------

void camera_set::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const camera_set>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

bool camera_set::operator==(const camera_set& other) const noexcept
{
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool camera_set::operator!=(const camera_set& other) const noexcept
{
    return base_class::operator!=(other);
}

//------------------------------------------------------------------------------

void camera_set::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const camera_set>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    clear();

    std::transform(
        other->cbegin(),
        other->cend(),
        std::back_inserter(*this),
        [&](const auto& value)
        {
            return std::make_pair(object::copy(value.first, cache), value.second);
        });

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void camera_set::add_camera(camera::sptr camera)
{
    if(empty())
    {
        // Add camera with identity matrix
        push_back({camera, std::make_shared<matrix4>()});
    }
    // Check if the camera is already in the set before adding it
    else if(cend() == std::find_if(cbegin(), cend(), [&](const auto& value){return value.first == camera;}))
    {
        // Add camera with nullptr matrix
        push_back({camera, nullptr});
    }
    else
    {
        SIGHT_THROW("Camera already in set");
    }
}

//------------------------------------------------------------------------------

camera::csptr camera_set::get_camera(std::size_t index) const
{
    return at(index).first;
}

//------------------------------------------------------------------------------

camera::sptr camera_set::get_camera(std::size_t index)
{
    return at(index).first;
}

//------------------------------------------------------------------------------

void camera_set::remove_camera(camera::sptr camera)
{
    const auto& it = std::find_if(cbegin(), cend(), [&](const auto& value){return value.first == camera;});

    SIGHT_THROW_IF("Camera not found.", it == cend());

    erase(it);
}

//------------------------------------------------------------------------------

void camera_set::set_extrinsic_matrix(std::size_t index, matrix4::sptr matrix)
{
    at(index).second = matrix;
}

//------------------------------------------------------------------------------

DATA_API matrix4::csptr camera_set::get_extrinsic_matrix(std::size_t index) const
{
    return at(index).second;
}

//------------------------------------------------------------------------------

DATA_API matrix4::sptr camera_set::get_extrinsic_matrix(std::size_t index)
{
    return at(index).second;
}

} // namespace sight::data
