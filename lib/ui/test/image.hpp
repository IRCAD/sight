/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/ui/test/config.hpp>

#include <string>
#include <string_view>

namespace sight::ui::test
{

struct image_threshold
{
    int channel {16};
    double max_bad_ratio {0.001};
    std::string reason;
};

/// Overrides the RGB thresholds with a required explanation, recorded in the step log.
[[nodiscard]] SIGHT_UI_TEST_API image_threshold threshold(int _channel, double _max_bad_ratio, std::string_view _why);

} // namespace sight::ui::test
