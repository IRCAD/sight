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

#include "data/config.hpp"

#include <data/Image.hpp>

#include <vector>

namespace sight::data
{

namespace helper
{

/**
 * @brief Helper class used to compute an image histogram.
 *
 */
class DATA_CLASS_API Histogram
{
public:

    using histogram_t = std::vector<double>;

    Histogram(sight::data::Image::csptr image) :
        m_image(image)
    {
    }

    /// Computes the number of pixels for every intensity
    DATA_API void compute();

    /// Samples the histogram given a bin width
    DATA_API histogram_t sample(size_t _binWidth) const;

    double max() const;
    double min() const;

private:

    sight::data::Image::csptr m_image;
    histogram_t m_values;
    double m_max {std::numeric_limits<double>::lowest()};
    double m_min {std::numeric_limits<double>::max()};
};

//------------------------------------------------------------------------------

inline double Histogram::max() const
{
    return m_max;
}

//------------------------------------------------------------------------------

inline double Histogram::min() const
{
    return m_min;
}

} // namespace helper

} // namespace sight::data
