/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "data/Camera.hpp"
#include "data/config.hpp"
#include <data/Matrix4.hpp>
#include <data/Object.hpp>
#include <data/Series.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/macros.hpp>

#include <map>
#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (CameraSeries));

namespace sight::data
{

/**
 * @brief  This class contains the information about cameras (container of camera and extrinsec matrix).
 */

class DATA_CLASS_API CameraSeries : public data::Series
{
public:

    SIGHT_DECLARE_CLASS(CameraSeries, data::Object, data::factory::New<CameraSeries>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (CameraSeries));

    typedef std::vector<data::Camera::sptr> CameraContainerType;

    /**
     *@brief Constructor
     *@param key Private construction key
     */
    DATA_API CameraSeries(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~CameraSeries();

    /**
     * @brief Defines shallow copy
     * @throws data::Exception if an errors occurs during copy
     */
    DATA_API void shallowCopy(const data::Object::csptr& _source) override;

    /**
     * @brief Defines deep copy
     * @throws data::Exception if an errors occurs during copy
     */
    DATA_API void cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**@name Signals API
     * @{
     */
    DATA_API static const core::com::Signals::SignalKeyType s_ADDED_CAMERA_SIG;
    typedef core::com::Signal<void (data::Camera::sptr)> AddedCameraSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_REMOVED_CAMERA_SIG;
    typedef core::com::Signal<void (data::Camera::sptr)> RemovedCameraSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_EXTRINSIC_CALIBRATED_SIG;
    typedef core::com::Signal<void ()> ExtrinsicCalibratedSignalType;
    /** @} */

    typedef std::vector<data::Matrix4::sptr> MatricesContainer;

    /**
     * @brief Adds a camera in the cameraSeries.
     * @throws core::Exception if the camera is already present in the `CameraSeries`
     */
    DATA_API void addCamera(const data::Camera::sptr& camera);

    /**
     * @brief Returns the camera at the index.
     * @throws core::Exception if the index is out of range
     */
    DATA_API data::Camera::sptr getCamera(size_t index) const;

    /**
     * @brief Remove the given camera from the series
     * @throws core::Exception if the camera is not found in the series
     */
    DATA_API void removeCamera(const data::Camera::sptr& camera);

    /// Returns the number of cameras
    size_t getNumberOfCameras() const
    {
        return m_cameras.size();
    }

    /**
     * @brief Set the extrinsic matrix.
     * @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
     *                  transformation from camera[0] to camera[index].
     * @param[in] matrix the extrinsic matrix
     * @note By default, the first matrix (index=0) is initialized to identity.
     * @throws core::Exception if the index is out of range
     */
    DATA_API void setExtrinsicMatrix(size_t index, data::Matrix4::sptr matrix);

    /**
     * @brief Gets the extrinsic matrix.
     * @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
     *                  transformation from camera[0] to camera[index].
     * @return Returns the extrinsic transformation matrix, or null if not defined.
     * @note By default, the first matrix (index=0) is initialized to identity, the other are nullptr.
     * @throws core::Exception if the index is out of range
     */
    DATA_API data::Matrix4::sptr getExtrinsicMatrix(size_t index) const;

    /**
     * @brief Gets the extrinsic matrix corresponding to the transformation from camera[0] to camera[1].
     * @return Returns the extrinsic transformation matrix, or null if not defined.
     * @deprecated Use getExtrinsicMatrix(1) instead of this method
     */
    data::Matrix4::sptr getExtrinsicMatrix() const
    {
        return this->getExtrinsicMatrix(1);
    }

protected:

    /// Contains camera
    CameraContainerType m_cameras;

    /// Signal when camera is added
    AddedCameraSignalType::sptr m_sigAddedCamera;

    /// Signal when camera is removed
    RemovedCameraSignalType::sptr m_sigRemovedCamera;

    /// Signal when cameras are calibrated (extrinsic matrix is computed)
    ExtrinsicCalibratedSignalType::sptr m_sigExtrinsicCalibrated;

    /// extrinsic matrices
    MatricesContainer m_extrinsicMatrices;
};

} //namespace sight::data
