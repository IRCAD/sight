/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <optional>

namespace sight::filter::image
{

/**
 * @brief The resampler class
 */
class SIGHT_FILTER_IMAGE_CLASS_API resampler
{
public:

    /**
     * @brief transforms and resamples an image using ITK.
     * @param[in] _in_image     the input data::image.
     * @param[out] _out_image   the resulting transformed image.
     * @param[in] _trf          transform applied to the input.
     * @param[in] _parameters   set the desired origin, spacing and size.
     */
    static SIGHT_FILTER_IMAGE_API void resample(
        const data::image::csptr& _in_image,
        const data::image::sptr& _out_image,
        const data::matrix4::csptr& _trf,
        std::optional<std::tuple<data::image::size_t,
                                 data::image::origin_t,
                                 data::image::orientation_t,
                                 data::image::spacing_t> > _parameters = std::nullopt
    );

    /**
     * @brief transforms and resamples the image into a new grid big enough so it won't crop the input image.
     * @param _img image to resample.
     * @param _trf transform applied to the image.
     * @param _output_spacing desired sampling rate.
     * @return resampled image.
     */
    static SIGHT_FILTER_IMAGE_API data::image::sptr resample(
        const data::image::csptr& _img,
        const data::matrix4::csptr& _trf,
        const data::image::spacing_t& _output_spacing
    );
};

} // namespace sight::filter::image
