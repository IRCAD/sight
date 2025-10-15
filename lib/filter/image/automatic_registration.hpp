/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include <sight/filter/image/config.hpp>

#include "filter/image/metric.hpp"

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <functional>

namespace sight::filter::image
{

using multi_resolution_parameters_t = std::vector<std::pair<std::size_t, double> >;

struct registration_params_t
{
    multi_resolution_parameters_t multi_resolution_parameters;
    std::size_t max_iterations;
    double min_step;
    double sampling_percentage;
    metric_t metric;
    bool enable_logging;
};

SIGHT_FILTER_IMAGE_API void perform_automatic_registration(
    const data::image& target,
    const data::image& reference,
    data::matrix4& transform,
    const registration_params_t& params
);

} // namespace sight::filter::image
