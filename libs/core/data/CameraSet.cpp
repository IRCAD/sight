/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "data/IContainer.hxx"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA(sight::data::CameraSet);

namespace sight::data
{

CameraSet::CameraSet(Object::Key key) :
    IContainer<CameraSet::container_type>(key)
{
    newSignal<extrinsic_calibrated_signal>(s_EXTRINSIC_CALIBRATED_SIG);
}

//------------------------------------------------------------------------------

void CameraSet::shallowCopy(const Object::csptr& source)
{
    const auto& other = CameraSet::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    IContainer<CameraSet::container_type>::shallowCopy(other);
}

//------------------------------------------------------------------------------

bool CameraSet::operator==(const CameraSet& other) const noexcept
{
    return IContainer<CameraSet::container_type>::operator==(other);
}

//------------------------------------------------------------------------------

bool CameraSet::operator!=(const CameraSet& other) const noexcept
{
    return IContainer<CameraSet::container_type>::operator!=(other);
}

//------------------------------------------------------------------------------

void CameraSet::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    const auto& other = CameraSet::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    fieldDeepCopy(other, cache);

    clear();

    std::transform(
        other->cbegin(),
        other->cend(),
        std::back_inserter(*this),
        [&](const auto& value)
        {
            return std::make_pair(Object::copy(value.first, cache), value.second);
        });
}

//------------------------------------------------------------------------------

void CameraSet::add_camera(Camera::sptr camera)
{
    if(empty())
    {
        // Add camera with identity matrix
        push_back({camera, Matrix4::New()});
    }
    // Check if the camera is already in the set before adding it
    else if(cend() == std::find_if(cbegin(), cend(), [&](const auto& value){return value.first == camera;}))
    {
        // Add camera with nullptr matrix
        push_back({camera, Matrix4::sptr()});
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
