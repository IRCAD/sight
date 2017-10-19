/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

