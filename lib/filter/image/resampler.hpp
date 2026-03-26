/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <filter/image/types.hpp>

#include <optional>

namespace sight::filter::image
{

/**
 * @brief The resampler class
 */
class SIGHT_FILTER_IMAGE_CLASS_API resampler
{
public:

    using parameters_t = std::tuple<data::image::size_t,
                                    data::image::origin_t,
                                    data::image::orientation_t,
                                    data::image::spacing_t,
                                    filter::image::interpolation_t>;

    /**
     * @brief transforms and resamples an image using ITK.
     * @param[in] _in_image     the input data::image.
     * @param[out] _out_image   the resulting transformed image.
     * @param[in] _trf          transform applied to the input.
     * @param[in] _pre_transform if true, the transform is applied before resampling, otherwise after.
     * @param[in] _params       set the desired origin, spacing, size and interpolation.
     */
    static SIGHT_FILTER_IMAGE_API void resample(
        const data::image::csptr& _in_image,
        const data::image::sptr& _out_image,
        const data::matrix4::csptr& _trf,
        bool _pre_transform                 = false,
        std::optional<parameters_t> _params = std::nullopt
    );

    /**
     * @brief transforms and resamples the image into a new grid big enough so it won't crop the input image.
     * @param _img image to resample.
     * @param _trf transform applied to the image.
     * @param _output_spacing desired sampling rate.
     * @param _interpolation interpolation method to use for resampling.
     * @param _pre_transform if true, the transform is applied before resampling, otherwise after.
     * @return resampled image.
     */
    static SIGHT_FILTER_IMAGE_API data::image::sptr resample(
        const data::image::csptr& _img,
        const data::matrix4::csptr& _trf,
        const data::image::spacing_t& _output_spacing,
        filter::image::interpolation_t _interpolation = filter::image::interpolation_t::LINEAR,
        bool _pre_transform                           = false
    );
};

} // namespace sight::filter::image
