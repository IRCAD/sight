/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "Histogram.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/thread/RegionThreader.hpp>

#include <numeric>

namespace sight::data::helper
{

/**
 * @brief Functor use to compute the histogram of the image.
 */
struct ComputeHistogramFunctor
{
    /// Parameters of the functor.
    struct Parameter
    {
        data::image::csptr image;
        double binsWidth {};
        std::vector<double> o_histogram;
        double o_min {std::numeric_limits<double>::max()};
        double o_max {std::numeric_limits<double>::lowest()};
    };

    using vector_t = std::vector<std::vector<double> >;

    //------------------------------------------------------------------------------

    template<class T>
    static void countPixels(
        const data::image::const_iterator<T>& imgBegin,
        vector_t& values,
        T min,
        double invBinsWidth,
        std::ptrdiff_t regionMin,
        std::ptrdiff_t regionMax,
        std::size_t i
)
    {
        const data::image::const_iterator<T> begin = imgBegin + regionMin;
        const data::image::const_iterator<T> end   = imgBegin + regionMax;
        for(auto itr = begin ; itr != end ; ++itr)
        {
            const auto index = static_cast<std::size_t>(static_cast<double>(*itr - min) * invBinsWidth);
            ++values[i][index];
        }
    }

    //------------------------------------------------------------------------------

    template<class IMAGETYPE>
    void operator()(Parameter& param)
    {
        data::image::csptr image = param.image;

        const auto dumpLock = image->dump_lock();

        IMAGETYPE min = std::numeric_limits<IMAGETYPE>::max();
        IMAGETYPE max = std::numeric_limits<IMAGETYPE>::min();
        {
            data::helper::MedicalImage::getMinMax(image, min, max);

            const double invBinsWidth = 1 / param.binsWidth;

            if(max > min)
            {
                vector_t values;
                std::size_t size = static_cast<std::size_t>(static_cast<double>(max - min) * invBinsWidth) + 1;

                sight::data::thread::RegionThreader rt;
                values.resize(rt.numberOfThread());
                for(auto& v : values)
                {
                    v.resize(size, 0);
                }

                rt(
                    [capture0 = image->cbegin<IMAGETYPE>(), &values, min, invBinsWidth](std::ptrdiff_t PH1,
                                                                                        std::ptrdiff_t PH2,
                                                                                        std::size_t PH3, auto&& ...)
                    {
                        return ComputeHistogramFunctor::countPixels<IMAGETYPE>(
                            capture0,
                            values,
                            min,
                            invBinsWidth,
                            PH1,
                            PH2,
                            PH3
                        );
                    },
                    image->cend<IMAGETYPE>() - image->cbegin<IMAGETYPE>()
                );

                param.o_histogram.resize(size, 0);

                double numPixels = 0;
                for(std::size_t i = 0 ; i < size ; ++i)
                {
                    for(const auto& v : values)
                    {
                        param.o_histogram[i] += v[i];
                    }

                    numPixels += param.o_histogram[i];
                }

                SIGHT_ASSERT("The number of pixels should not be null", numPixels != 0);
                for(std::size_t i = 0 ; i < size ; ++i)
                {
                    param.o_histogram[i] /= numPixels;
                }

                param.o_min = static_cast<double>(min);
                param.o_max = static_cast<double>(max);
            }
        }
    }
};

//------------------------------------------------------------------------------

void Histogram::compute()
{
    ComputeHistogramFunctor::Parameter param;
    param.image     = m_image;
    param.binsWidth = 1.;

    core::type type = m_image->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, ComputeHistogramFunctor>::invoke(type, param);

    m_values = std::move(param.o_histogram);
    m_max    = param.o_max;
    m_min    = param.o_min;
}

//------------------------------------------------------------------------------

Histogram::histogram_t Histogram::sample(std::size_t _binWidth) const
{
    const auto binWidth          = static_cast<std::ptrdiff_t>(_binWidth);
    const std::ptrdiff_t numBins = static_cast<std::ptrdiff_t>(m_max - m_min) / binWidth + 1;

    const auto begin  = m_values.begin();
    std::ptrdiff_t it = 0;

    histogram_t histogram;
    for(ptrdiff_t i = 0 ; i < numBins ; ++i)
    {
        histogram.push_back(
            std::accumulate(
                begin + it,
                begin + std::min(it + binWidth, static_cast<std::ptrdiff_t>(m_values.size())),
                0.
            )
        );
        it += binWidth;
    }

    return histogram;
}

//------------------------------------------------------------------------------

} // namespace sight::data::helper
