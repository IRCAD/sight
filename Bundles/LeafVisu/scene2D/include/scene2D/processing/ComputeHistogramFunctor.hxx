#ifndef __SCENE2D_PROCESSING_COMPUTEHISTOGRAMFUNCTOR_HXX__
#define __SCENE2D_PROCESSING_COMPUTEHISTOGRAMFUNCTOR_HXX__

#include <fwComEd/helper/Image.hpp>
#include <fwComEd/helper/Array.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include "scene2D/processing/SComputeHistogram.hpp"

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
        ::fwData::Image::sptr  image;
        ::fwData::Histogram::sptr histogram;
        float binsWidth;
    };

    template<class IMAGETYPE>
    void operator()(Parameter &param)
    {
        ::fwData::Image::sptr image = param.image;
        ::fwData::Histogram::sptr histogram = param.histogram;

        ::fwComEd::helper::Image imgHelper(image);

        IMAGETYPE min = std::numeric_limits<IMAGETYPE>::max();
        IMAGETYPE max = std::numeric_limits<IMAGETYPE>::min();

        ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
        SLM_ASSERT("Wrong image", max > min) ;
        if( max > min )
        {
            histogram->initialize( min, max, param.binsWidth );

            ::fwData::Array::sptr array = image->getDataArray();
            ::fwComEd::helper::Array arrayHelper(array);

            IMAGETYPE* itr = arrayHelper.begin<IMAGETYPE>();
            IMAGETYPE* itrEnd = arrayHelper.end<IMAGETYPE>();

            for(; itr!= itrEnd; ++itr)
            {
                histogram->addPixel( static_cast< float >( *itr ) );
            }
        }

    }
};

}   // namespace processing
}   // namespace scene2D

#endif  /* __SCENE2D_PROCESSING_COMPUTEHISTOGRAMFUNCTOR_HXX__ */

