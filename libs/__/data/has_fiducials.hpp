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

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

namespace sight::data
{

class has_fiducials
{
public:

    explicit has_fiducials(core::com::signals& _signals);

    struct signals
    {
        using GroupAdded = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t GROUP_ADDED = "groupAdded";

        using GroupRemoved = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t GROUP_REMOVED = "groupRemoved";

        using PointAdded = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t POINT_ADDED = "pointAdded";

        using PointRemoved = core::com::signal<void (std::string, std::size_t)>;
        static inline const core::com::signals::key_t POINT_REMOVED = "pointRemoved";

        using PointInserted = core::com::signal<void (std::string, std::size_t)>;
        static inline const core::com::signals::key_t POINT_INSERTED = "pointInserted";

        using PointModified = core::com::signal<void (std::string, std::size_t)>;
        static inline const core::com::signals::key_t POINT_MODIFIED = "pointModified";

        using GroupModified = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t GROUP_MODIFIED = "groupModified";

        using GroupRenamed = core::com::signal<void (std::string _old, std::string _new)>;
        static inline const core::com::signals::key_t GROUP_RENAMED = "groupRenamed";

        using PointSelected = core::com::signal<void (std::string _name, std::size_t _index)>;
        static inline const core::com::signals::key_t POINT_SELECTED = "pointSelected";

        using PointDeselected = core::com::signal<void (std::string _name, std::size_t _index)>;
        static inline const core::com::signals::key_t POINT_DESELECTED = "pointDeselected";
    };

private:

    const signals::GroupAdded::sptr M_GROUP_ADDED {std::make_shared<signals::GroupAdded>()};
    const signals::GroupRemoved::sptr M_GROUP_REMOVED {std::make_shared<signals::GroupRemoved>()};
    const signals::PointAdded::sptr M_POINT_ADDED {std::make_shared<signals::PointAdded>()};
    const signals::PointRemoved::sptr M_POINT_REMOVED {std::make_shared<signals::PointRemoved>()};
    const signals::PointInserted::sptr M_POINT_INSERTED {std::make_shared<signals::PointInserted>()};
    const signals::PointModified::sptr M_POINT_MODIFIED {std::make_shared<signals::PointModified>()};
    const signals::GroupModified::sptr M_GROUP_MODIFIED {std::make_shared<signals::GroupModified>()};
    const signals::GroupRenamed::sptr M_GROUP_RENAMED {std::make_shared<signals::GroupRenamed>()};
    const signals::PointSelected::sptr M_POINT_SELECTED {std::make_shared<signals::PointSelected>()};
    const signals::PointDeselected::sptr M_POINT_DESELECTED {std::make_shared<signals::PointDeselected>()};
};

} // namespace sight::data
