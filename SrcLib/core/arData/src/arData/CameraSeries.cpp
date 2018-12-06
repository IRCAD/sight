/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#include "arData/CameraSeries.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Exception.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/registry/macros.hpp>

namespace arData
{

fwDataRegisterMacro( ::arData::CameraSeries );

const ::fwCom::Signals::SignalKeyType CameraSeries::s_ADDED_CAMERA_SIG         = "addedCamera";
const ::fwCom::Signals::SignalKeyType CameraSeries::s_REMOVED_CAMERA_SIG       = "removedCamera";
const ::fwCom::Signals::SignalKeyType CameraSeries::s_EXTRINSIC_CALIBRATED_SIG = "extrinsicCalibrated";

//------------------------------------------------------------------------------

CameraSeries::CameraSeries(::fwData::Object::Key key) :
    Series(key)
{
    m_sigAddedCamera         = AddedCameraSignalType::New();
    m_sigRemovedCamera       = RemovedCameraSignalType::New();
    m_sigExtrinsicCalibrated = ExtrinsicCalibratedSignalType::New();

    ::fwCom::HasSignals::m_signals(s_ADDED_CAMERA_SIG, m_sigAddedCamera)
        (s_REMOVED_CAMERA_SIG, m_sigRemovedCamera)
        (s_EXTRINSIC_CALIBRATED_SIG, m_sigExtrinsicCalibrated);

}

//------------------------------------------------------------------------------

CameraSeries::~CameraSeries()
{
}

//------------------------------------------------------------------------------

void CameraSeries::shallowCopy( const ::fwData::Object::csptr& _source )
{
    CameraSeries::csptr other = CameraSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_cameras           = other->m_cameras;
    m_extrinsicMatrices = other->m_extrinsicMatrices;
}

//------------------------------------------------------------------------------

void CameraSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    CameraSeries::csptr other = CameraSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy( _source, cache );

    m_cameras.clear();
    m_extrinsicMatrices.clear();

    for(CameraContainerType::value_type elt : other->m_cameras )
    {
        ::arData::Camera::sptr newCamera;
        newCamera = ::fwData::Object::copy(elt, cache);
        m_cameras.push_back(newCamera);
    }

    for(MatricesContainer::value_type elt : other->m_extrinsicMatrices)
    {
        ::fwData::TransformationMatrix3D::sptr matrix;
        matrix = ::fwData::Object::copy(elt, cache);
        m_extrinsicMatrices.push_back(matrix);
    }
}

//------------------------------------------------------------------------------

void CameraSeries::addCamera(const ::arData::Camera::sptr& camera)
{
    FW_RAISE_IF("Camera already exists in CameraSeries.",
                std::find(m_cameras.begin(), m_cameras.end(), camera) != m_cameras.end());
    m_cameras.push_back(camera);
    ::fwData::TransformationMatrix3D::sptr matrix;
    if (m_extrinsicMatrices.empty())
    {
        matrix = ::fwData::TransformationMatrix3D::New();
    }
    m_extrinsicMatrices.push_back(matrix);
}

//------------------------------------------------------------------------------

::arData::Camera::sptr CameraSeries::getCamera(size_t index) const
{
    FW_RAISE_IF("Number of cameras is less than " << index, index >= m_cameras.size() );

    return m_cameras[index];
}

//------------------------------------------------------------------------------

void CameraSeries::removeCamera(const ::arData::Camera::sptr& camera)
{
    CameraContainerType::iterator iter = std::find(m_cameras.begin(), m_cameras.end(), camera);
    FW_RAISE_IF("Camera not found in CameraSeries.", iter == m_cameras.end());

    size_t index                        = std::distance(m_cameras.begin(), iter);
    MatricesContainer::iterator matIter = m_extrinsicMatrices.begin() + index;
    m_extrinsicMatrices.erase(matIter);
    m_cameras.erase(iter);
}

//------------------------------------------------------------------------------

void CameraSeries::setExtrinsicMatrix(size_t index, ::fwData::TransformationMatrix3D::sptr matrix)
{
    FW_RAISE_IF("Number of cameras is less than " << index, index >= m_cameras.size() );
    m_extrinsicMatrices[index] = matrix;
}

//------------------------------------------------------------------------------

::fwData::TransformationMatrix3D::sptr CameraSeries::getExtrinsicMatrix(size_t index) const
{
    FW_RAISE_IF("Number of cameras is less than " << index, index >= m_cameras.size() );
    return m_extrinsicMatrices[index];
}

//------------------------------------------------------------------------------

} //namespace arData
