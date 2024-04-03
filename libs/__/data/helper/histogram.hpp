/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include <data/image.hpp>

#include <vector>

namespace sight::data::helper
{

/**
 * @brief Helper class used to compute an image histogram.
 *
 */
class SIGHT_DATA_CLASS_API histogram
{
public:

    using histogram_t = std::vector<double>;

    histogram(sight::data::image::csptr _image) :
        m_image(_image)
    {
    }

    /// Computes the number of pixels for every intensity
    SIGHT_DATA_API void compute();

    /// Samples the histogram given a bin width
    [[nodiscard]] SIGHT_DATA_API histogram_t sample(size_t _bin_width) const;

    [[nodiscard]] double max() const;
    [[nodiscard]] double min() const;

private:

    sight::data::image::csptr m_image;
    histogram_t m_values;
    double m_max {std::numeric_limits<double>::lowest()};
    double m_min {std::numeric_limits<double>::max()};
};

//------------------------------------------------------------------------------

inline double histogram::max() const
{
    return m_max;
}

//------------------------------------------------------------------------------

inline double histogram::min() const
{
    return m_min;
}

} // namespace sight::data::helper
