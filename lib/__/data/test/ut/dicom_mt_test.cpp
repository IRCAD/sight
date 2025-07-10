/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "dicom_mt_test.hpp"

#include <data/image_series.hpp>

#include <chrono>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::dicom_mt_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void dicom_mt_test::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void dicom_mt_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void dicom_mt_test::concurrent_read_test()
{
    static constexpr int s_NUM_THREADS = 32;

    // Setup the test data
    data::image_series::sptr series = std::make_shared<data::image_series>();
    series->set_sop_keyword(dicom::sop::Keyword::EnhancedUSVolumeStorage);

    {
        auto dump_lock = series->dump_lock();
        series->resize({8, 8, 8}, core::type::UINT8, data::image::pixel_format_t::gray_scale);
        std::fill(series->begin<std::uint8_t>(), series->end<std::uint8_t>(), std::uint8_t(0));
    }

    // Set the frame acquisition time point
    series->set_frame_acquisition_time_point(std::chrono::system_clock::now(), 0);
    const auto now = *series->get_frame_acquisition_time_point(0);

    // Launch a bunch of threads to read the series
    std::vector<std::future<void> > futures;

    // shared mutex to simulate the lock_ptr from the service
    std::shared_mutex mutex;

    futures.reserve(s_NUM_THREADS);
    for(int i = 0 ; i < s_NUM_THREADS ; ++i)
    {
        futures.emplace_back(
            std::async(
                std::launch::async,
                [series, now, &mutex]
            {
                for(int j = 0 ; j < 1000 ; ++j)
                {
                    std::shared_lock lock(mutex);

                    // Access the series as const
                    const auto& time_point = std::as_const(*series).get_frame_acquisition_time_point(0);

                    // Check the time point
                    CPPUNIT_ASSERT(time_point);
                    CPPUNIT_ASSERT(now == *time_point);
                }
            })
        );
    }

    // Wait for all the futures to complete
    for(auto& future : futures)
    {
        future.get();
    }

    // Retry with non const access
    futures.clear();
    futures.reserve(s_NUM_THREADS);

    for(int i = 0 ; i < s_NUM_THREADS ; ++i)
    {
        futures.emplace_back(
            std::async(
                std::launch::async,
                [series, now, &mutex]
            {
                for(int j = 0 ; j < 1000 ; ++j)
                {
                    std::shared_lock lock(mutex);

                    // Access the series as const
                    const auto& time_point = series->get_frame_acquisition_time_point(0);

                    // Check the time point
                    CPPUNIT_ASSERT(time_point);
                    CPPUNIT_ASSERT(now == *time_point);
                }
            })
        );
    }

    // Wait for all the futures to complete
    for(auto& future : futures)
    {
        future.get();
    }
}

} // namespace sight::data::ut
