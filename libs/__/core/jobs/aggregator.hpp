/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"
#include "core/jobs/base.hpp"

#include <core/macros.hpp>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index_container.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace sight::core::jobs
{

/**
 * @brief This class manages an aggregation of base.
 */
class CORE_CLASS_API aggregator : public base
{
public:

    /**
     * @name Typedefs
     * @{ */
    using sptr = std::shared_ptr<core::jobs::aggregator>;
    using wptr = std::weak_ptr<core::jobs::aggregator>;

    /// aggregator container type
    using job_seq = boost::multi_index_container<core::jobs::base::sptr,
                                                 boost::multi_index::indexed_by<boost::multi_index::random_access<>,
                                                                                boost::multi_index::hashed_unique<boost::multi_index::identity<core::jobs::base::sptr> > > >;
    /**  @} */

    /// Default constructor. The name is initialized with an empty string
    CORE_API aggregator();

    /**
     * @brief Constructor
     *
     * @param name The job's name
     */
    CORE_API aggregator(const std::string& _name);

    /**
     * @brief Add an base to the aggregator
     * @warning Be careful not to add an aggregator to itself, directly or
     * indirectly (through other aggregators for instance).
     *
     * @param job the base to add to the aggregator
     * @param weight added base's weight. Avoid values lower than 0.01
     */
    CORE_API void add(const core::jobs::base::sptr& _i_job, double _weight = 1.);

    /**
     * @brief Retrieve sub job sequence of the aggregator
     *
     * @return The job aggregator container
     */
    CORE_API job_seq get_sub_jobs();

protected:

    /**
     * @brief Run all the jobs of the aggregator
     *
     * @return future of all the IJobs of the aggregator
     */
    CORE_API shared_future run_impl() override;

private:

    /// The base container
    job_seq m_jobs;

    /// Hold sub job info
    struct job_info
    {
        std::uint64_t done_work {0};
        std::uint64_t total_work {0};
        std::uint64_t last_value {0};

        job_info()
        = default;

        job_info(std::uint64_t _d, std::uint64_t _t) :
            done_work(_d),
            total_work(_t)
        {
        }

        job_info(const base& _i_job) :
            done_work(_i_job.get_done_work_units()),
            total_work(_i_job.get_total_work_units())
        {
        }

        //------------------------------------------------------------------------------

        [[nodiscard]] double progress() const
        {
            return (0 == total_work) ? 1. : static_cast<double>(done_work) / static_cast<double>(total_work);
        }
    };

    /// Job info map type
    using job_info_map = std::map<core::jobs::base*, job_info>;

    /// Map containing job info
    job_info_map m_job_info;
};

} //namespace sight::core::jobs
