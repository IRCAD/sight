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

#pragma once

#include "data/camera.hpp"
#include "data/container.hpp"
#include "data/matrix4.hpp"

#include <vector>

namespace sight::data
{

/// This class is Camera container
class SIGHT_DATA_CLASS_API camera_set final : public data::container<std::vector<std::pair<std::shared_ptr<camera>,
                                                                                           std::shared_ptr<matrix4> > > >
{
public:

    SIGHT_DECLARE_CLASS(camera_set, container<camera_set::container_t>);

    /// Constructors / Destructor / Assignment operators
    /// @{
    SIGHT_DATA_API camera_set();
    SIGHT_DATA_API ~camera_set() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using container<camera_set::container_t>::container;
    using container<camera_set::container_t>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const camera_set& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const camera_set& _other) const noexcept;
    /// @}

    /// Signals
    /// @{
    using added_camera_signal_t = core::com::signal<void (camera::sptr)>;
    SIGHT_DATA_API inline static const core::com::signals::key_t ADDED_CAMERA_SIG = "added_camera";

    using removed_camera_signal_t = core::com::signal<void (camera::sptr)>;
    SIGHT_DATA_API inline static const core::com::signals::key_t REMOVED_CAMERA_SIG = "removedCamera";

    using extrinsic_calibrated_signal_t = core::com::signal<void ()>;
    SIGHT_DATA_API inline static const core::com::signals::key_t EXTRINSIC_CALIBRATED_SIG = "extrinsicCalibrated";
    /// @}

    /// Adds a camera
    /// @throws core::exception if the camera is already present.
    /// @param[in] _camera the camera to add
    SIGHT_DATA_API void add_camera(camera::sptr _camera);

    /// Returns the camera at the index.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] _index the index of the camera to return
    /// @return the camera at the index
    /// @{
    SIGHT_DATA_API camera::csptr get_camera(std::size_t _index) const;
    SIGHT_DATA_API camera::sptr get_camera(std::size_t _index);
    /// @}

    /// Remove the given camera
    /// @throws core::exception if the camera is not found
    /// @param[in] _camera the camera to remove
    SIGHT_DATA_API void remove_camera(camera::sptr _camera);

    /// Sets the extrinsic matrix.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] _index index of the camera associated to this extrinsic matrix. The matrix correspond to the
    ///                   transformation from camera[0] to camera[index].
    /// @param[in] _matrix the extrinsic matrix
    SIGHT_DATA_API void set_extrinsic_matrix(std::size_t _index, matrix4::sptr _matrix);

    /// Gets the extrinsic matrix.
    /// @throws std::out_of_range if the index is out of range
    /// @param[in] _index index of the camera associated to this extrinsic matrix. The matrix correspond to the
    ///                   transformation from camera[0] to camera[index].
    /// @return Returns the extrinsic transformation matrix, or null if not defined.
    ///
    /// @note By default, the first matrix (index=0) is initialized to identity, the other are nullptr.
    /// @{
    SIGHT_DATA_API matrix4::csptr get_extrinsic_matrix(std::size_t _index) const;
    SIGHT_DATA_API matrix4::sptr get_extrinsic_matrix(std::size_t _index);
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache  cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /**
     * @brief set the stereo calibration error
     * @param _error double (default 0.0)
     */
    void set_calibration_error(double _error);
    /**
     * @brief get the calibration error of the stereo calibration
     * @return the calibration error in double.
     */
    double calibration_error() const;

private:

    /// Error of the computed calibration
    double m_calibration_error {0.};
};

//-----------------------------------------------------------------------------

inline void camera_set::set_calibration_error(double _calibration_error)
{
    m_calibration_error = _calibration_error;
}

//-----------------------------------------------------------------------------

inline double camera_set::calibration_error() const
{
    return m_calibration_error;
}

} // namespace sight::data
