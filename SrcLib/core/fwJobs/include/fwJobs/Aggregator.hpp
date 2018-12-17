/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#ifndef __FWJOBS_AGGREGATOR_HPP__
#define __FWJOBS_AGGREGATOR_HPP__

#include "fwJobs/config.hpp"
#include "fwJobs/IJob.hpp"

#include <fwCore/macros.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace fwJobs
{

/**
 * @brief This class manages an aggregation of IJob.
 */
class FWJOBS_CLASS_API Aggregator : public IJob
{
public:

    /**
     * @name Typedefs
     * @{ */
    typedef SPTR (::fwJobs::Aggregator) sptr;
    typedef WPTR (::fwJobs::Aggregator) wptr;

    /// Aggregator container type
    typedef ::boost::multi_index_container< ::fwJobs::IJob::sptr,
                                            ::boost::multi_index::indexed_by<
                                                ::boost::multi_index::random_access<>,
                                                ::boost::multi_index::hashed_unique<
                                                    ::boost::multi_index::identity< ::fwJobs::IJob::sptr >
                                                    >
                                                >
                                            > IJobSeq;
    /**  @} */

    /**
     * @brief Create a new Aggregator smart pointer
     *
     * @param name The job's name
     *
     * @return A pointer to a new Aggregator
     */
    FWJOBS_API static sptr New(const std::string& name = "");

    /// Default constructor. The name is initialized with an empty string
    FWJOBS_API Aggregator();

    /**
     * @brief Constructor
     *
     * @param name The job's name
     */
    FWJOBS_API Aggregator(const std::string& name);

    /**
     * @brief Add an IJob to the Aggregator
     * @warning Be careful not to add an aggregator to itself, directly or
     * indirectly (through other aggregators for instance).
     *
     * @param iJob the IJob to add to the Aggregator
     * @param weight added IJob's weight. Avoid values lower than 0.01
     */
    FWJOBS_API void add(const ::fwJobs::IJob::sptr & iJob, double weight = 1.);

    /**
     * @brief Retrieve sub job sequence of the Aggregator
     *
     * @return The job aggregator container
     */
    FWJOBS_API IJobSeq getSubJobs();


protected:

    /**
     * @brief Run all the jobs of the Aggregator
     *
     * @return future of all the IJobs of the Aggregator
     */
    FWJOBS_API SharedFuture runImpl();

private:

    /// The IJob container
    IJobSeq m_jobs;

    /// Hold sub job info
    struct JobInfo
    {
        std::uint64_t doneWork;
        std::uint64_t totalWork;
        std::uint64_t lastValue;

        JobInfo() : doneWork(0), totalWork(0), lastValue(0)
        {
        }

        JobInfo(std::uint64_t d, std::uint64_t t) :
            doneWork(d),
            totalWork(t),
            lastValue(0)
        {
        }

        JobInfo( const IJob &iJob ) :
            doneWork(iJob.getDoneWorkUnits()),
            totalWork(iJob.getTotalWorkUnits()),
            lastValue(0)
        {
        }

        double progress() const
        {
            return ( 0 == totalWork ) ? 1. : double(doneWork)/totalWork;
        }
    };

    /// Job info map type
    typedef std::map< fwJobs::IJob*, JobInfo > JobInfoMap;

    /// Map containing job info
    JobInfoMap m_jobInfo;
};

} //namespace fwJobs

#endif //__FWJOBS_AGGREGATOR_HPP__
