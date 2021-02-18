/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene2d/processing/SComputeHistogram.hpp"

#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

namespace sight::modules::viz::scene2d
{
namespace processing
{

/**
 * @brief Functor use to compute the histogram of the image.
 */
struct ComputeHistogramFunctor
{
    /// Parameters of the functor.
    struct Parameter
    {
        data::Image::csptr image;
        data::Histogram::sptr histogram;
        float binsWidth;
    };

    //------------------------------------------------------------------------------

    template<class IMAGETYPE>
    void operator()(Parameter& param)
    {
        data::Image::csptr image        = param.image;
        data::Histogram::sptr histogram = param.histogram;

        const auto dumpLock = image->lock();

        IMAGETYPE min = std::numeric_limits<IMAGETYPE>::max();
        IMAGETYPE max = std::numeric_limits<IMAGETYPE>::min();

        data::tools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
        SLM_ASSERT("Wrong image", max > min);
        if( max > min )
        {
            histogram->initialize( static_cast<float>(min), static_cast<float>(max),
                                   static_cast<float>(param.binsWidth) );

            auto itr          = image->begin<IMAGETYPE>();
            const auto itrEnd = image->end<IMAGETYPE>();

            for(; itr != itrEnd; ++itr)
            {
                histogram->addPixel( static_cast< float >( *itr ) );
            }
        }

    }
};

}   // namespace processing
}   // namespace sight::modules::viz::scene2d
