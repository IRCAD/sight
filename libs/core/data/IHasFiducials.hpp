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

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

namespace sight::data
{

class IHasFiducials
{
public:

    explicit IHasFiducials(core::com::Signals& signals);

    struct signals
    {
        using GroupAdded = core::com::Signal<void (std::string)>;
        static inline const core::com::Signals::SignalKeyType GROUP_ADDED = "groupAdded";

        using GroupRemoved = core::com::Signal<void (std::string)>;
        static inline const core::com::Signals::SignalKeyType GROUP_REMOVED = "groupRemoved";

        using PointAdded = core::com::Signal<void (std::string)>;
        static inline const core::com::Signals::SignalKeyType POINT_ADDED = "pointAdded";

        using PointRemoved = core::com::Signal<void (std::string, std::size_t)>;
        static inline const core::com::Signals::SignalKeyType POINT_REMOVED = "pointRemoved";

        using PointInserted = core::com::Signal<void (std::string, std::size_t)>;
        static inline const core::com::Signals::SignalKeyType POINT_INSERTED = "pointInserted";

        using PointModified = core::com::Signal<void (std::string, std::size_t)>;
        static inline const core::com::Signals::SignalKeyType POINT_MODIFIED = "pointModified";

        using GroupModified = core::com::Signal<void (std::string)>;
        static inline const core::com::Signals::SignalKeyType GROUP_MODIFIED = "groupModified";

        using GroupRenamed = core::com::Signal<void (std::string old, std::string _new)>;
        static inline const core::com::Signals::SignalKeyType GROUP_RENAMED = "groupRenamed";

        using PointSelected = core::com::Signal<void (std::string name, std::size_t index)>;
        static inline const core::com::Signals::SignalKeyType POINT_SELECTED = "pointSelected";

        using PointDeselected = core::com::Signal<void (std::string name, std::size_t index)>;
        static inline const core::com::Signals::SignalKeyType POINT_DESELECTED = "pointDeselected";
    };

private:

    const signals::GroupAdded::sptr m_groupAdded {std::make_shared<signals::GroupAdded>()};
    const signals::GroupRemoved::sptr m_groupRemoved {std::make_shared<signals::GroupRemoved>()};
    const signals::PointAdded::sptr m_pointAdded {std::make_shared<signals::PointAdded>()};
    const signals::PointRemoved::sptr m_pointRemoved {std::make_shared<signals::PointRemoved>()};
    const signals::PointInserted::sptr m_pointInserted {std::make_shared<signals::PointInserted>()};
    const signals::PointModified::sptr m_pointModified {std::make_shared<signals::PointModified>()};
    const signals::GroupModified::sptr m_groupModified {std::make_shared<signals::GroupModified>()};
    const signals::GroupRenamed::sptr m_groupRenamed {std::make_shared<signals::GroupRenamed>()};
    const signals::PointSelected::sptr m_pointSelected {std::make_shared<signals::PointSelected>()};
    const signals::PointDeselected::sptr m_pointDeselected {std::make_shared<signals::PointDeselected>()};
};

} // namespace sight::data
