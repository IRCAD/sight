/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <sight/core/config.hpp>

#include "core/progress/monitor.hpp"

#include <core/macros.hpp>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index_container.hpp>

#include <cstdint>
#include <string>

namespace sight::core::progress
{

/**
 * @brief This class manages an aggregation of monitor.
 */
class SIGHT_CORE_CLASS_API aggregator : public monitor
{
public:

    /**
     * @name Typedefs
     * @{ */
    using sptr = std::shared_ptr<core::progress::aggregator>;
    using wptr = std::weak_ptr<core::progress::aggregator>;

    /// aggregator container type
    using progress_seq = boost::multi_index_container<core::progress::monitor::sptr,
                                                      boost::multi_index::indexed_by<boost::multi_index::random_access<>,
                                                                                     boost::multi_index::hashed_unique<boost::multi_index::identity<core::progress::monitor::sptr> > > >;
    /**  @} */

    /// Default constructor. The name is initialized with an empty string
    SIGHT_CORE_API aggregator() = default;

    /**
     * @brief Constructor
     *
     * @param _name The progress's name
     */
    SIGHT_CORE_API explicit aggregator(const std::string& _name);

    /**
     * @brief Add an monitor to the aggregator
     * @warning Be careful not to add an aggregator to itself, directly or
     * indirectly (through other aggregators for instance).
     *
     * @param _progress the monitor to add to the aggregator
     * @param _weight added monitor's weight. Avoid values lower than 0.01
     */
    SIGHT_CORE_API void add(const core::progress::monitor::sptr& _progress, double _weight = 1.);

private:

    /// The monitor container
    progress_seq m_progress_seq;

    /// Hold sub progress info
    struct progress_info
    {
        std::uint64_t done_work {0};
        std::uint64_t total_work {0};
        std::uint64_t last_value {0};

        progress_info() = default;

        progress_info(std::uint64_t _d, std::uint64_t _t) :
            done_work(_d),
            total_work(_t)
        {
        }

        explicit progress_info(const monitor& _i_progress) :
            done_work(_i_progress.get_done_work_units()),
            total_work(_i_progress.get_total_work_units())
        {
        }

        //------------------------------------------------------------------------------

        [[nodiscard]] double progress() const
        {
            return (0 == total_work) ? 1. : static_cast<double>(done_work) / static_cast<double>(total_work);
        }
    };

    /// progress info map type
    using progress_info_map = std::map<core::progress::monitor*, progress_info>;

    /// Map containing progress info
    progress_info_map m_progress_info;
};

} //namespace sight::core::progress
