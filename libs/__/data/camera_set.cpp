/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "data/container.hpp"
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

void camera_set::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const camera_set>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    m_calibration_error = other->m_calibration_error;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

bool camera_set::operator==(const camera_set& _other) const noexcept
{
    return base_class_t::operator==(_other) && m_calibration_error == _other.m_calibration_error;
}

//------------------------------------------------------------------------------

bool camera_set::operator!=(const camera_set& _other) const noexcept
{
    return base_class_t::operator!=(_other);
}

//------------------------------------------------------------------------------

void camera_set::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const camera_set>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    clear();

    std::transform(
        other->cbegin(),
        other->cend(),
        std::back_inserter(*this),
        [&](const auto& _value)
        {
            return std::make_pair(object::copy(_value.first, _cache), _value.second);
        });

    m_calibration_error = other->m_calibration_error;

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void camera_set::add_camera(camera::sptr _camera)
{
    if(empty())
    {
        // Add camera with identity matrix
        push_back({_camera, std::make_shared<matrix4>()});
    }
    // Check if the camera is already in the set before adding it
    else if(cend() == std::find_if(cbegin(), cend(), [&](const auto& _value){return _value.first == _camera;}))
    {
        // Add camera with nullptr matrix
        push_back({_camera, nullptr});
    }
    else
    {
        SIGHT_THROW("Camera already in set");
    }
}

//------------------------------------------------------------------------------

camera::csptr camera_set::get_camera(std::size_t _index) const
{
    return at(_index).first;
}

//------------------------------------------------------------------------------

camera::sptr camera_set::get_camera(std::size_t _index)
{
    return at(_index).first;
}

//------------------------------------------------------------------------------

void camera_set::remove_camera(camera::sptr _camera)
{
    const auto& it = std::find_if(cbegin(), cend(), [&](const auto& _value){return _value.first == _camera;});

    SIGHT_THROW_IF("Camera not found.", it == cend());

    erase(it);
}

//------------------------------------------------------------------------------

void camera_set::set_extrinsic_matrix(std::size_t _index, matrix4::sptr _matrix)
{
    at(_index).second = _matrix;
}

//------------------------------------------------------------------------------

matrix4::csptr camera_set::get_extrinsic_matrix(std::size_t _index) const
{
    return at(_index).second;
}

//------------------------------------------------------------------------------

matrix4::sptr camera_set::get_extrinsic_matrix(std::size_t _index)
{
    return at(_index).second;
}

} // namespace sight::data
