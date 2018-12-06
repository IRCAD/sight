/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __SCENE2D_PROCESSING_COMPUTEHISTOGRAMFUNCTOR_HXX__
#define __SCENE2D_PROCESSING_COMPUTEHISTOGRAMFUNCTOR_HXX__

#include "scene2D/processing/SComputeHistogram.hpp"

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

namespace scene2D
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
        ::fwData::Image::csptr image;
        ::fwData::Histogram::sptr histogram;
        float binsWidth;
    };

    //------------------------------------------------------------------------------

    template<class IMAGETYPE>
    void operator()(Parameter& param)
    {
        ::fwData::Image::csptr image        = param.image;
        ::fwData::Histogram::sptr histogram = param.histogram;

        ::fwDataTools::helper::ImageGetter imgHelper(image);

        IMAGETYPE min = std::numeric_limits<IMAGETYPE>::max();
        IMAGETYPE max = std::numeric_limits<IMAGETYPE>::min();

        ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
        SLM_ASSERT("Wrong image", max > min);
        if( max > min )
        {
            histogram->initialize( min, max, param.binsWidth );

            ::fwData::Array::sptr array = image->getDataArray();
            ::fwDataTools::helper::Array arrayHelper(array);

            IMAGETYPE* itr    = arrayHelper.begin<IMAGETYPE>();
            IMAGETYPE* itrEnd = arrayHelper.end<IMAGETYPE>();

            for(; itr != itrEnd; ++itr)
            {
                histogram->addPixel( static_cast< float >( *itr ) );
            }
        }

    }
};

}   // namespace processing
}   // namespace scene2D

#endif  /* __SCENE2D_PROCESSING_COMPUTEHISTOGRAMFUNCTOR_HXX__ */

