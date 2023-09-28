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

#include "CameraSet.hpp"

#include "data/container.hxx"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::CameraSet);

namespace sight::data
{

CameraSet::CameraSet()
{
    new_signal<added_camera_signal_t>(ADDED_CAMERA_SIG);
    new_signal<removed_camera_signal_t>(REMOVED_CAMERA_SIG);
    new_signal<extrinsic_calibrated_signal_t>(EXTRINSIC_CALIBRATED_SIG);
}

//------------------------------------------------------------------------------

void CameraSet::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const CameraSet>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

bool CameraSet::operator==(const CameraSet& other) const noexcept
{
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool CameraSet::operator!=(const CameraSet& other) const noexcept
{
    return base_class::operator!=(other);
}

//------------------------------------------------------------------------------

void CameraSet::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const CameraSet>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
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
            return std::make_pair(Object::copy(value.first, cache), value.second);
        });

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void CameraSet::add_camera(Camera::sptr camera)
{
    if(empty())
    {
        // Add camera with identity matrix
        push_back({camera, std::make_shared<Matrix4>()});
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

Camera::csptr CameraSet::get_camera(std::size_t index) const
{
    return at(index).first;
}

//------------------------------------------------------------------------------

Camera::sptr CameraSet::get_camera(std::size_t index)
{
    return at(index).first;
}

//------------------------------------------------------------------------------

void CameraSet::remove_camera(Camera::sptr camera)
{
    const auto& it = std::find_if(cbegin(), cend(), [&](const auto& value){return value.first == camera;});

    SIGHT_THROW_IF("Camera not found.", it == cend());

    erase(it);
}

//------------------------------------------------------------------------------

void CameraSet::set_extrinsic_matrix(std::size_t index, Matrix4::sptr matrix)
{
    at(index).second = matrix;
}

//------------------------------------------------------------------------------

DATA_API Matrix4::csptr CameraSet::get_extrinsic_matrix(std::size_t index) const
{
    return at(index).second;
}

//------------------------------------------------------------------------------

DATA_API Matrix4::sptr CameraSet::get_extrinsic_matrix(std::size_t index)
{
    return at(index).second;
}

} // namespace sight::data
