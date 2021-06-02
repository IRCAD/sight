/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "utest/config.hpp"

namespace sight::utest
{

/// This allows filtering unstable or too slow unit tests that may give accurate results
/// when executed manually but inaccurate in a CI context.
/// This should be considered as a last ressort workaround, if the test could not be fixed
class UTEST_CLASS_API Filter
{
public:

    /// Returns true if we should ignore "slow" tests
    /// Use SIGHT_IGNORE_SLOW_TESTS environment variable
    UTEST_API static bool ignoreSlowTests();

    /// Returns true if we should ignore "unstable" tests
    /// Use SIGHT_IGNORE_UNSTABLE_TESTS environment variable
    UTEST_API static bool ignoreUnstableTests();
};

} // namespace sight::utest
