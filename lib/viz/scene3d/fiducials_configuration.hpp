/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <data/fiducials_series.hpp>

#include <optional>
#include <set>
#include <string>

namespace sight::viz::scene3d
{

/// Fiducials configuration used in signals/slots
/// Parameter with `std::nullopt`, means "no change".
struct SIGHT_VIZ_SCENE3D_CLASS_API fiducials_configuration
{
    /// Group name of the fiducials to configure.
    std::optional<std::string> group {std::nullopt};

    /// Color of the fiducials.
    std::optional<std::array<float, 4> > color {std::nullopt};

    /// Size of the fiducials.
    std::optional<float> size {std::nullopt};

    /// Shape of the fiducials.
    std::optional<sight::data::fiducials_series::private_shape> shape {std::nullopt};

    /// Maximum number of fiducials in the group. Value < 0 means "no limit".
    std::optional<int> group_max {std::nullopt};

    /// Maximum number of fiducials in the current slice. Value < 0 means "no limit".
    std::optional<int> slice_max {std::nullopt};

    /// Maximum number of total fiducials. Value < 0 means "no limit".
    std::optional<int> total_max {std::nullopt};

    /// List of movable groups if `modify="group"` is used. If empty, only current group is movable.
    std::optional<std::set<std::string> > movable_groups {std::nullopt};
};

} // namespace sight::viz::scene3d
