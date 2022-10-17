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

#pragma once

#include "data/Camera.hpp"
#include "data/IContainer.hpp"
#include "data/Matrix4.hpp"

#include <vector>

namespace sight::data
{

/// This class is Camera container
class DATA_CLASS_API CameraSet final : public IContainer<std::vector<std::pair<std::shared_ptr<Camera>,
                                                                               std::shared_ptr<Matrix4> > > >
{
public:

    SIGHT_DECLARE_CLASS(CameraSet, IContainer<CameraSet::container_type>, factory::New<CameraSet>);

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API CameraSet(Object::Key key);
    DATA_API ~CameraSet() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using IContainer<CameraSet::container_type>::IContainer;
    using IContainer<CameraSet::container_type>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const CameraSet& other) const noexcept;
    DATA_API bool operator!=(const CameraSet& other) const noexcept;
    /// @}

    /// Signals
    /// @{
    using added_camera_signal_t = core::com::Signal<void (Camera::sptr)>;
    DATA_API inline static const core::com::Signals::SignalKeyType s_ADDED_CAMERA_SIG = "addedCamera";

    using removed_camera_signal_t = core::com::Signal<void (Camera::sptr)>;
    DATA_API inline static const core::com::Signals::SignalKeyType s_REMOVED_CAMERA_SIG = "removedCamera";

    using extrinsic_calibrated_signal_t = core::com::Signal<void ()>;
    DATA_API inline static const core::com::Signals::SignalKeyType s_EXTRINSIC_CALIBRATED_SIG = "extrinsicCalibrated";
    /// @}

    /// Adds a camera
    /// @throws core::Exception if the camera is already present.
    /// @param[in] camera the camera to add
    DATA_API void add_camera(Camera::sptr camera);

    /// Returns the camera at the index.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] index the index of the camera to return
    /// @return the camera at the index
    /// @{
    DATA_API Camera::csptr get_camera(std::size_t index) const;
    DATA_API Camera::sptr get_camera(std::size_t index);
    /// @}

    /// Remove the given camera
    /// @throws core::Exception if the camera is not found
    /// @param[in] camera the camera to remove
    DATA_API void remove_camera(Camera::sptr camera);

    /// Sets the extrinsic matrix.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
    ///                  transformation from camera[0] to camera[index].
    /// @param[in] matrix the extrinsic matrix
    DATA_API void set_extrinsic_matrix(std::size_t index, Matrix4::sptr matrix);

    /// Gets the extrinsic matrix.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
    ///                  transformation from camera[0] to camera[index].
    /// @return Returns the extrinsic transformation matrix, or null if not defined.
    ///
    /// @note By default, the first matrix (index=0) is initialized to identity, the other are nullptr.
    /// @{
    DATA_API Matrix4::csptr get_extrinsic_matrix(std::size_t index) const;
    DATA_API Matrix4::sptr get_extrinsic_matrix(std::size_t index);
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;
};

} // namespace sight::data
