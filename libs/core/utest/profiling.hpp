/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <chrono>
#include <cmath>
#include <functional>
#include <numeric>
#include <optional>

/// @note std::source_location is not yet supported by clang...
/// Profile a function with multiple calls
#define SIGHT_PROFILE_FUNC(Func, ...) \
    sight::utest::profile_func(SIGHT_SOURCE_FILE, __LINE__, Func __VA_OPT__(, ) __VA_ARGS__)

namespace sight::utest
{

//------------------------------------------------------------------------------

template<typename Func, typename Unit = std::chrono::milliseconds>
static inline std::tuple<Unit, Unit> profile_func(
    const char* const SOURCE,
    const int LINE,
    Func f,
    std::size_t iteration                     = 3,
    const std::optional<std::string>& label   = std::nullopt,
    const std::optional<double>& target_ratio = std::nullopt
)
{
    // Just to have at least one iteration
    iteration = std::max(std::size_t(1), iteration);

    std::vector<std::chrono::steady_clock::duration> durations;
    durations.reserve(iteration);

    for(std::size_t i = 0 ; i < iteration ; ++i)
    {
        // Register the current clock value
        const auto& start = std::chrono::steady_clock::now();

        // Execute the function
        f(i);

        // Compute the elapsed time
        durations.emplace_back(std::chrono::steady_clock::now() - start);
    }

    // Compute mean
    const auto& mean = std::accumulate(
        durations.begin(),
        durations.end(),
        std::chrono::steady_clock::duration()
                       ) / durations.size();

    // Compute variance
    const double variance = std::accumulate(
        durations.begin(),
        durations.end(),
        0.0,
        [&](auto a, const auto& b)
        {
            const double diff = double(b.count()) - double(mean.count());
            return std::move(a) + (diff * diff);
        }) / double(durations.size());

    // Compute standard error
    const double standard_error = std::sqrt(variance);

    using d_unit     = std::chrono::duration<double, typename Unit::period>;
    using d_duration = std::chrono::duration<double, typename std::chrono::steady_clock::duration::period>;

    // Convert standard error to the same unit as "mean"
    const auto converted_error = d_duration(standard_error);

    // Convert standard error to double in the target unit
    const double mean_as_double  = std::chrono::duration_cast<d_unit>(mean).count();
    const double error_as_double = std::chrono::duration_cast<d_unit>(converted_error).count();

    bool accurate = true;

    if(target_ratio)
    {
        // Check if the standard error is not too big which means the measured time is not very accurate
        const double error_ratio = error_as_double / mean_as_double;
        accurate = error_ratio < *target_ratio;

        // Max 1000 iteration in "auto" mode...
        if(!accurate && iteration < 1000)
        {
            // Increase the number of iteration to have a more accurate result
            iteration *= 2;

            std::cerr
            << "\nInaccurate results (error ratio = "
            << error_ratio
            << ") > (target ratio = "
            << *target_ratio
            << "). Retrying with "
            << iteration
            << " iterations."
            << std::endl;

            return profile_func(SOURCE, LINE, f, iteration, label, target_ratio);
        }
    }

    if(label)
    {
        const auto unit = []
                          {
                              if constexpr(std::is_same_v<Unit, std::chrono::nanoseconds>)
                              {
                                  return "ns";
                              }

                              if constexpr(std::is_same_v<Unit, std::chrono::microseconds>)
                              {
                                  return "µs";
                              }

                              if constexpr(std::is_same_v<Unit, std::chrono::milliseconds>)
                              {
                                  return "ms";
                              }

                              if constexpr(std::is_same_v<Unit, std::chrono::seconds>)
                              {
                                  return "s";
                              }

                              if constexpr(std::is_same_v<Unit, std::chrono::minutes>)
                              {
                                  return "min";
                              }

                              if constexpr(std::is_same_v<Unit, std::chrono::hours>)
                              {
                                  return "h";
                              }
                          };

        std::stringstream stream;
        stream
        << *label
        << " : average time ("
        << durations.size()
        << " iterations) = "
        << mean_as_double
        << " " << unit()
        << ", standard error = "
        << error_as_double
        << " " << unit()
        << (accurate || !target_ratio ? "" : " - Not accurate !");

        // We do not use macro because of __FILE__ and __LINE__
        sight::core::log::SpyLogger::get().info(
            stream.str(),
            SOURCE,
            LINE
        );
    }

    return std::make_tuple(std::chrono::duration_cast<Unit>(mean), std::chrono::duration_cast<Unit>(converted_error));
}

} // namespace sight::utest
