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

#pragma once

#include "data/camera.hpp"
#include "data/container.hpp"
#include "data/matrix4.hpp"

#include <vector>

namespace sight::data
{

/// This class is Camera container
class DATA_CLASS_API camera_set final : public data::container<std::vector<std::pair<std::shared_ptr<camera>,
                                                                                     std::shared_ptr<matrix4> > > >
{
public:

    SIGHT_DECLARE_CLASS(camera_set, container<camera_set::container_t>);

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API camera_set();
    DATA_API ~camera_set() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using container<camera_set::container_t>::container;
    using container<camera_set::container_t>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const camera_set& _other) const noexcept;
    DATA_API bool operator!=(const camera_set& _other) const noexcept;
    /// @}

    /// Signals
    /// @{
    using added_camera_signal_t = core::com::signal<void (camera::sptr)>;
    DATA_API inline static const core::com::signals::key_t ADDED_CAMERA_SIG = "addedCamera";

    using removed_camera_signal_t = core::com::signal<void (camera::sptr)>;
    DATA_API inline static const core::com::signals::key_t REMOVED_CAMERA_SIG = "removedCamera";

    using extrinsic_calibrated_signal_t = core::com::signal<void ()>;
    DATA_API inline static const core::com::signals::key_t EXTRINSIC_CALIBRATED_SIG = "extrinsicCalibrated";
    /// @}

    /// Adds a camera
    /// @throws core::exception if the camera is already present.
    /// @param[in] camera the camera to add
    DATA_API void add_camera(camera::sptr _camera);

    /// Returns the camera at the index.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] index the index of the camera to return
    /// @return the camera at the index
    /// @{
    DATA_API camera::csptr get_camera(std::size_t _index) const;
    DATA_API camera::sptr get_camera(std::size_t _index);
    /// @}

    /// Remove the given camera
    /// @throws core::exception if the camera is not found
    /// @param[in] camera the camera to remove
    DATA_API void remove_camera(camera::sptr _camera);

    /// Sets the extrinsic matrix.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
    ///                  transformation from camera[0] to camera[index].
    /// @param[in] matrix the extrinsic matrix
    DATA_API void set_extrinsic_matrix(std::size_t _index, matrix4::sptr _matrix);

    /// Gets the extrinsic matrix.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] index index of the camera associated to this extrinsic matrix. The matrix correspond to the
    ///                  transformation from camera[0] to camera[index].
    /// @return Returns the extrinsic transformation matrix, or null if not defined.
    ///
    /// @note By default, the first matrix (index=0) is initialized to identity, the other are nullptr.
    /// @{
    DATA_API matrix4::csptr get_extrinsic_matrix(std::size_t _index) const;
    DATA_API matrix4::sptr get_extrinsic_matrix(std::size_t _index);
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;
};

} // namespace sight::data
