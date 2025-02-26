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

#include "fiducials_series.hpp"

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
        using group_added = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t GROUP_ADDED = "groupAdded";

        using group_removed = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t GROUP_REMOVED = "groupRemoved";

        using point_added = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t POINT_ADDED = "point_added";

        using point_removed = core::com::signal<void (std::string, std::size_t)>;
        static inline const core::com::signals::key_t POINT_REMOVED = "point_removed";

        using point_inserted = core::com::signal<void (std::string, std::size_t)>;
        static inline const core::com::signals::key_t POINT_INSERTED = "pointInserted";

        using point_modified = core::com::signal<void (std::string, std::size_t)>;
        static inline const core::com::signals::key_t POINT_MODIFIED = "pointModified";

        using group_modified = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t GROUP_MODIFIED = "groupModified";

        using group_renamed = core::com::signal<void (std::string _old, std::string _new)>;
        static inline const core::com::signals::key_t GROUP_RENAMED = "groupRenamed";

        using point_selected = core::com::signal<void (std::string _name, std::size_t _index)>;
        static inline const core::com::signals::key_t POINT_SELECTED = "pointSelected";

        using point_deselected = core::com::signal<void (std::string _name, std::size_t _index)>;
        static inline const core::com::signals::key_t POINT_DESELECTED = "pointDeselected";
    };

    inline fiducials_series::csptr get_fiducials() const;
    inline fiducials_series::sptr get_fiducials();

protected:

    /// Contains the associated Spatial Fiducials file
    fiducials_series::sptr m_fiducials_series {std::make_shared<fiducials_series>()};

private:

    const signals::group_added::sptr m_group_added {std::make_shared<signals::group_added>()};
    const signals::group_removed::sptr m_group_removed {std::make_shared<signals::group_removed>()};
    const signals::point_added::sptr m_point_added {std::make_shared<signals::point_added>()};
    const signals::point_removed::sptr m_point_removed {std::make_shared<signals::point_removed>()};
    const signals::point_inserted::sptr m_point_inserted {std::make_shared<signals::point_inserted>()};
    const signals::point_modified::sptr m_point_modified {std::make_shared<signals::point_modified>()};
    const signals::group_modified::sptr m_group_modified {std::make_shared<signals::group_modified>()};
    const signals::group_renamed::sptr m_group_renamed {std::make_shared<signals::group_renamed>()};
    const signals::point_selected::sptr m_point_selected {std::make_shared<signals::point_selected>()};
    const signals::point_deselected::sptr m_point_deselected {std::make_shared<signals::point_deselected>()};
};

//------------------------------------------------------------------------------

fiducials_series::csptr has_fiducials::get_fiducials() const
{
    return m_fiducials_series;
}

//------------------------------------------------------------------------------

fiducials_series::sptr has_fiducials::get_fiducials()
{
    return m_fiducials_series;
}

} // namespace sight::data
