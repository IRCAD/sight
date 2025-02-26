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

#include "histogram.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>
#include <data/thread/region_threader.hpp>

#include <numeric>

namespace sight::data::helper
{

/**
 * @brief Functor use to compute the histogram of the image.
 */
struct computehistogram_functor
{
    /// Parameters of the functor.
    struct parameter
    {
        data::image::csptr image;
        double bins_width {};
        std::vector<double> o_histogram;
        double o_min {std::numeric_limits<double>::max()};
        double o_max {std::numeric_limits<double>::lowest()};
    };

    using vector_t = std::vector<std::vector<double> >;

    //------------------------------------------------------------------------------

    template<class T>
    static void count_pixels(
        const data::image::const_iterator<T>& _img_begin,
        vector_t& _values,
        T _min,
        double _inv_bins_width,
        std::ptrdiff_t _region_min,
        std::ptrdiff_t _region_max,
        std::size_t _i
)
    {
        const data::image::const_iterator<T> begin = _img_begin + _region_min;
        const data::image::const_iterator<T> end   = _img_begin + _region_max;
        for(auto itr = begin ; itr != end ; ++itr)
        {
            const auto index = static_cast<std::size_t>(static_cast<double>(*itr - _min) * _inv_bins_width);
            ++_values[_i][index];
        }
    }

    //------------------------------------------------------------------------------

    template<class IMAGETYPE>
    void operator()(parameter& _param)
    {
        data::image::csptr image = _param.image;

        const auto dump_lock = image->dump_lock();
        {
            const auto& [min, max] = data::helper::medical_image::get_min_max<IMAGETYPE>(image);

            const double inv_bins_width = 1 / _param.bins_width;

            if(max > min)
            {
                vector_t values;
                std::size_t size = static_cast<std::size_t>(static_cast<double>(max - min) * inv_bins_width) + 1;

                sight::data::thread::region_threader rt;
                values.resize(rt.number_of_thread());
                for(auto& v : values)
                {
                    v.resize(size, 0);
                }

                rt(
                    [begin = image->cbegin<IMAGETYPE>(), &values, min = min, inv_bins_width]
                    (std::ptrdiff_t _p_h1, std::ptrdiff_t _p_h2, std::size_t _p_h3, auto&& ...)
                    {
                        return computehistogram_functor::count_pixels<IMAGETYPE>(
                            begin,
                            values,
                            min,
                            inv_bins_width,
                            _p_h1,
                            _p_h2,
                            _p_h3
                        );
                    },
                    image->cend<IMAGETYPE>() - image->cbegin<IMAGETYPE>()
                );

                _param.o_histogram.resize(size, 0);

                double num_pixels = 0;
                for(std::size_t i = 0 ; i < size ; ++i)
                {
                    for(const auto& v : values)
                    {
                        _param.o_histogram[i] += v[i];
                    }

                    num_pixels += _param.o_histogram[i];
                }

                SIGHT_ASSERT("The number of pixels should not be null", num_pixels != 0);
                for(std::size_t i = 0 ; i < size ; ++i)
                {
                    _param.o_histogram[i] /= num_pixels;
                }

                _param.o_min = static_cast<double>(min);
                _param.o_max = static_cast<double>(max);
            }
        }
    }
};

//------------------------------------------------------------------------------

void histogram::compute()
{
    computehistogram_functor::parameter param;
    param.image      = m_image;
    param.bins_width = 1.;

    core::type type = m_image->type();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, computehistogram_functor>::invoke(type, param);

    m_values = std::move(param.o_histogram);
    m_max    = param.o_max;
    m_min    = param.o_min;
}

//------------------------------------------------------------------------------

histogram::histogram_t histogram::sample(std::size_t _bin_width) const
{
    const auto bin_width          = static_cast<std::ptrdiff_t>(_bin_width);
    const std::ptrdiff_t num_bins = static_cast<std::ptrdiff_t>(m_max - m_min) / bin_width + 1;

    const auto begin  = m_values.begin();
    std::ptrdiff_t it = 0;

    histogram_t histogram;
    for(ptrdiff_t i = 0 ; i < num_bins ; ++i)
    {
        histogram.push_back(
            std::accumulate(
                begin + it,
                begin + std::min(it + bin_width, static_cast<std::ptrdiff_t>(m_values.size())),
                0.
            )
        );
        it += bin_width;
    }

    return histogram;
}

//------------------------------------------------------------------------------

} // namespace sight::data::helper
