/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <data/landmarks.hpp>

#include <optional>
#include <set>
#include <string>

namespace sight::viz::scene3d
{

/// Landmarks configuration used in signals/slots
/// Parameter with `std::nullopt`, means "no change".
struct VIZ_SCENE3D_CLASS_API LandmarksConfiguration
{
    /// Group name of the landmarks to configure.
    std::optional<std::string> group {std::nullopt};

    /// Color of the landmarks.
    std::optional<sight::data::landmarks::ColorType> color {std::nullopt};

    /// Size of the landmarks.
    std::optional<sight::data::landmarks::SizeType> size {std::nullopt};

    /// Shape of the landmarks.
    std::optional<sight::data::landmarks::Shape> shape {std::nullopt};

    /// Maximum number of landmark in the group. Value < 0 means "no limit".
    std::optional<int> groupMax {std::nullopt};

    /// Maximum number of visible landmark. Value < 0 means "no limit".
    std::optional<int> visibleMax {std::nullopt};

    /// Maximum number of total landmark. Value < 0 means "no limit".
    std::optional<int> totalMax {std::nullopt};

    /// List of movable groups if `modify="group"` is used. If empty, only current group is movable.
    std::optional<std::set<std::string> > movableGroups {std::nullopt};
};

} // namespace sight::viz::scene3d
