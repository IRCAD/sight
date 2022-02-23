/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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
#include "data/Matrix4.hpp"
#include "data/Object.hpp"
#include "data/Series.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/macros.hpp>

#include <data/Matrix4.hpp>
#include <data/Object.hpp>
#include <data/Series.hpp>

#include <map>
#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (CameraSeries));

namespace sight::data
{

/**
 * @brief  This class contains the information about cameras (container of camera and extrinsic matrix).
 */

class DATA_CLASS_API CameraSeries : public Series
{
public:

    SIGHT_DECLARE_CLASS(CameraSeries, Object, factory::New<CameraSeries>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (CameraSeries));

    typedef std::vector<Camera::sptr> CameraContainerType;

    /**
     *@brief Constructor
     *@param key Private construction key
     */
    DATA_API CameraSeries(Object::Key key);

    /// Destructor
    DATA_API virtual ~CameraSeries();

    /**
     * @brief Defines shallow copy
     * @throws data::Exception if an errors occurs during copy
     */
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /**@name Signals API
     * @{
     */
    DATA_API static const core::com::Signals::SignalKeyType s_ADDED_CAMERA_SIG;
    typedef core::com::Signal<void (Camera::sptr)> AddedCameraSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_REMOVED_CAMERA_SIG;
    typedef core::com::Signal<void (Camera::sptr)> RemovedCameraSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_EXTRINSIC_CALIBRATED_SIG;
    typedef core::com::Signal<void ()> ExtrinsicCalibratedSignalType;
    /** @} */

    typedef std::vector<Matrix4::sptr> MatricesContainer;

    /**
     * @brief Adds a camera in the cameraSeries.
     * @throws core::Exception if the camera is already present in the `CameraSeries`
     */
    DATA_API void addCamera(const Camera::sptr& camera);

    /**
     * @brief Returns the camera at the index.
     * @throws core::Exception if the index is out of range
     */
    DATA_API Camera::csptr getCamera(std::size_t index) const;

    /**
     * @brief Returns the camera at the index.
     * @throws core::Exception if the index is out of range
     */
    DATA_API Camera::sptr getCamera(std::size_t index);

    /**
     * @brief Remove the given camera from the series
     * @throws core::Exception if the camera is not found in the series
     */
    DATA_API void removeCamera(const Camera::sptr& camera);

    /// Returns the number of cameras
    inline std::size_t numCameras() const
    {
        return m_cameras.size();
    }

    /// Clear all cameras
    inline void clearCameras()
    {
        m_cameras.clear();
    }

    /**
     * @brief Set the extrinsic matrix.
     * @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
     *                  transformation from camera[0] to camera[index].
     * @param[in] matrix the extrinsic matrix
     * @note By default, the first matrix (index=0) is initialized to identity.
     * @throws core::Exception if the index is out of range
     */
    DATA_API void setExtrinsicMatrix(std::size_t index, Matrix4::sptr matrix);

    /**
     * @brief Gets the extrinsic matrix.
     * @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
     *                  transformation from camera[0] to camera[index].
     * @return Returns the extrinsic transformation matrix, or null if not defined.
     * @note By default, the first matrix (index=0) is initialized to identity, the other are nullptr.
     * @throws core::Exception if the index is out of range
     *
     * @{
     */
    DATA_API Matrix4::csptr getExtrinsicMatrix(std::size_t index) const;
    DATA_API Matrix4::sptr getExtrinsicMatrix(std::size_t index);
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const CameraSeries& other) const noexcept;
    DATA_API bool operator!=(const CameraSeries& other) const noexcept;
    /// @}

protected:

    /**
     * @brief Defines deep copy
     * @throws data::Exception if an errors occurs during copy
     */
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

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
