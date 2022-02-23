/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "data/CameraSeries.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Exception.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/registry/macros.hpp>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::CameraSeries);

const core::com::Signals::SignalKeyType CameraSeries::s_ADDED_CAMERA_SIG         = "addedCamera";
const core::com::Signals::SignalKeyType CameraSeries::s_REMOVED_CAMERA_SIG       = "removedCamera";
const core::com::Signals::SignalKeyType CameraSeries::s_EXTRINSIC_CALIBRATED_SIG = "extrinsicCalibrated";

//------------------------------------------------------------------------------

CameraSeries::CameraSeries(data::Object::Key key) :
    Series(key)
{
    m_sigAddedCamera         = AddedCameraSignalType::New();
    m_sigRemovedCamera       = RemovedCameraSignalType::New();
    m_sigExtrinsicCalibrated = ExtrinsicCalibratedSignalType::New();

    core::com::HasSignals::m_signals(s_ADDED_CAMERA_SIG, m_sigAddedCamera)
        (s_REMOVED_CAMERA_SIG, m_sigRemovedCamera)
        (s_EXTRINSIC_CALIBRATED_SIG, m_sigExtrinsicCalibrated);
}

//------------------------------------------------------------------------------

CameraSeries::~CameraSeries()
{
}

//------------------------------------------------------------------------------

void CameraSeries::shallowCopy(const data::Object::csptr& _source)
{
    CameraSeries::csptr other = CameraSeries::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->data::Series::shallowCopy(_source);

    m_cameras           = other->m_cameras;
    m_extrinsicMatrices = other->m_extrinsicMatrices;
}

//------------------------------------------------------------------------------

void CameraSeries::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    CameraSeries::csptr other = CameraSeries::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->data::Series::cachedDeepCopy(_source, cache);

    m_cameras.clear();
    m_extrinsicMatrices.clear();

    for(CameraContainerType::value_type elt : other->m_cameras)
    {
        data::Camera::sptr newCamera;
        newCamera = data::Object::copy(elt, cache);
        m_cameras.push_back(newCamera);
    }

    for(MatricesContainer::value_type elt : other->m_extrinsicMatrices)
    {
        data::Matrix4::sptr matrix;
        matrix = data::Object::copy(elt, cache);
        m_extrinsicMatrices.push_back(matrix);
    }
}

//------------------------------------------------------------------------------

void CameraSeries::addCamera(const data::Camera::sptr& camera)
{
    SIGHT_THROW_IF(
        "Camera already exists in CameraSeries.",
        std::find(m_cameras.begin(), m_cameras.end(), camera) != m_cameras.end()
    );
    m_cameras.push_back(camera);
    data::Matrix4::sptr matrix;
    if(m_extrinsicMatrices.empty())
    {
        matrix = data::Matrix4::New();
    }

    m_extrinsicMatrices.push_back(matrix);
}

//------------------------------------------------------------------------------

data::Camera::csptr CameraSeries::getCamera(std::size_t index) const
{
    SIGHT_THROW_IF("Number of cameras is less than " << index, index >= m_cameras.size());

    return m_cameras[index];
}

//------------------------------------------------------------------------------

data::Camera::sptr CameraSeries::getCamera(std::size_t index)
{
    SIGHT_THROW_IF("Number of cameras is less than " << index, index >= m_cameras.size());

    return m_cameras[index];
}

//------------------------------------------------------------------------------

void CameraSeries::removeCamera(const data::Camera::sptr& camera)
{
    CameraContainerType::iterator iter = std::find(m_cameras.begin(), m_cameras.end(), camera);
    SIGHT_THROW_IF("Camera not found in CameraSeries.", iter == m_cameras.end());

    const auto index                    = std::distance(m_cameras.begin(), iter);
    MatricesContainer::iterator matIter = m_extrinsicMatrices.begin() + index;
    m_extrinsicMatrices.erase(matIter);
    m_cameras.erase(iter);
}

//------------------------------------------------------------------------------

void CameraSeries::setExtrinsicMatrix(std::size_t index, data::Matrix4::sptr matrix)
{
    SIGHT_THROW_IF("Number of cameras is less than " << index, index >= m_cameras.size());
    m_extrinsicMatrices[index] = matrix;
}

//------------------------------------------------------------------------------

data::Matrix4::csptr CameraSeries::getExtrinsicMatrix(std::size_t index) const
{
    SIGHT_THROW_IF("Number of cameras is less than " << index, index >= m_cameras.size());
    return m_extrinsicMatrices[index];
}

//------------------------------------------------------------------------------

data::Matrix4::sptr CameraSeries::getExtrinsicMatrix(std::size_t index)
{
    SIGHT_THROW_IF("Number of cameras is less than " << index, index >= m_cameras.size());
    return m_extrinsicMatrices[index];
}

//------------------------------------------------------------------------------

bool CameraSeries::operator==(const CameraSeries& other) const noexcept
{
    if(!core::tools::is_equal(m_cameras, other.m_cameras)
       || !core::tools::is_equal(m_extrinsicMatrices, other.m_extrinsicMatrices))
    {
        return false;
    }

    // Super class last
    return Series::operator==(other);
}

//------------------------------------------------------------------------------

bool CameraSeries::operator!=(const CameraSeries& other) const noexcept
{
    return !(*this == other);
}

} //namespace sight::data
