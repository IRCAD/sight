/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <map>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#include <boost/foreach.hpp>

#include <fwTools/Combinatory.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>


#include "fwDataTools/Image.hpp"


namespace fwDataTools
{

//------------------------------------------------------------------------------

Image::Image()
{}

//------------------------------------------------------------------------------

Image::~Image()
{}

//------------------------------------------------------------------------------

void Image::initRand()
{
    std::srand(std::time(NULL));
}

//------------------------------------------------------------------------------

void Image::generateImage(::fwData::Image::sptr image,
                          ::fwData::Image::SizeType size,
                          std::vector<double> spacing,
                          std::vector<double> origin,
                          ::fwTools::Type type)
{
    image->setSpacing(spacing);
    image->setOrigin(origin);
    image->setSize(size);
    image->setType(type);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    std::fill(array->begin(), array->end(), 0);
}

//------------------------------------------------------------------------------

void Image::generateRandomImage(::fwData::Image::sptr image, ::fwTools::Type type)
{
    ::boost::uint8_t dim = 3;
    image->setType(type);

    ::fwData::Image::SizeType size(3);
    size[0] = rand()%100 + 2;
    size[1] = rand()%100 + 2;
    size[2] = rand()%100 + 2;
    image->setSize(size);

    std::vector< double > spacing(3);
    spacing[0] = (rand()%200 +1) / 100.;
    spacing[1] = (rand()%200 +1) / 100.;
    spacing[2] = (rand()%200 +1) / 100.;
    image->setSpacing(spacing);

    std::vector< double > origin(3);
    origin[0] = (rand()%200 - 100) / 10.;
    origin[1] = (rand()%200 - 100) / 10.;
    origin[2] = (rand()%200 - 100) / 10.;
    image->setOrigin(origin);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    randomizeArray(array);
}

//------------------------------------------------------------------------------

void Image::randomizeArray(::fwData::Array::sptr array)
{
    char *iter = array->begin< char >();

    for (; iter != array->end< char >() ; ++iter)
    {
        *iter = rand()%256;
    }
}

//------------------------------------------------------------------------------

bool Image::compareImage(::fwData::Image::sptr image1, ::fwData::Image::sptr image2, double spacingTolerance, double originTolerance, std::string errorPrefix )
{
    bool compare = true;

    if ( ! image1 && image2 || image1 && ! image2 )
    {
        compare &= false;
        OSLM_ERROR( errorPrefix << "Images are not equivalent (one image has a null sptr)");
    }
    else if ( image1 && image2 )
    {
        compare &= image1->getNumberOfDimensions() == image2->getNumberOfDimensions();
        OSLM_ERROR_IF( errorPrefix << "Images have not the same dimension : " << image1->getNumberOfDimensions() << " != " << image2->getNumberOfDimensions(),
                image1->getNumberOfDimensions() != image2->getNumberOfDimensions());


        compare &= image1->getSize() == image2->getSize();
        for( unsigned int k = 0; k < image1->getNumberOfDimensions(); ++k )
        {
            bool sizeAreEquals = image1->getSize()[k] == image2->getSize()[k];

            OSLM_ERROR_IF( errorPrefix << "Images have not the same size : ( size[" << k << "] => " <<
                    image1->getSize()[k] << " != " << image2->getSize()[k], ! sizeAreEquals );

        }

        for( unsigned int k = 0; k < image1->getNumberOfDimensions(); ++k )
        {
            bool spacingAreEquals =
                    image1->getSpacing()[k] - spacingTolerance <= image2->getSpacing()[k] &&
                    image1->getSpacing()[k] + spacingTolerance >= image2->getSpacing()[k];

            OSLM_ERROR_IF( errorPrefix << "Images have not the same spacing : spacing[" << k << "] => " <<
                    image1->getSpacing()[k] << " != " << image2->getSpacing()[k], ! spacingAreEquals );

            compare &= spacingAreEquals;
        }

        for( unsigned int k = 0; k < image1->getNumberOfDimensions(); ++k )
        {
            bool originAreEquals =
                    image1->getOrigin()[k] - originTolerance <= image2->getOrigin()[k] &&
                    image1->getOrigin()[k] + originTolerance >= image2->getOrigin()[k];

            OSLM_ERROR_IF( errorPrefix << "Images have not the same origin : origin[" << k << "] => " <<
                    image1->getOrigin()[k] << " != " << image2->getOrigin()[k], ! originAreEquals );

            compare &= originAreEquals;
        }

        compare &= image1->getType() == image2->getType();
        OSLM_ERROR_IF( errorPrefix << "Images have not the same type : " << image1->getType().string() << " != " << image2->getType().string(),
                image1->getType() != image2->getType());

        compare &= image1->getSizeInBytes() == image2->getSizeInBytes();
        OSLM_ERROR_IF( errorPrefix << "Images have not the same size in bytes : " << image1->getSizeInBytes() << " != " << image2->getSizeInBytes(),
                image1->getSizeInBytes() != image2->getSizeInBytes());


        compare &= ::fwDataTools::Image::compareArray(image1->getDataArray(), image2->getDataArray(), "Image array :");
    }

    return compare;
}

//------------------------------------------------------------------------------

bool Image::compareImage(::fwData::Image::sptr image1,
                         ::fwData::Image::SizeType size,
                         std::vector<double> spacing,
                         std::vector<double> origin,
                         ::fwTools::Type type)
{
    bool compare = true;

    compare &= image1->getNumberOfDimensions() == size.size();
    OSLM_ERROR_IF("Images have not the same dimension : " << image1->getNumberOfDimensions() << " != " << size.size(),
            image1->getNumberOfDimensions() != size.size());

    compare &= image1->getSize() == size;
    OSLM_ERROR_IF("Images have not the same size",image1->getSize() != size);

    compare &= image1->getSpacing() == spacing;
    OSLM_ERROR_IF("Images have not the same spacing", image1->getSpacing() != spacing);

    compare &= image1->getOrigin() == origin;
    OSLM_ERROR_IF("Images have not the same origin", image1->getOrigin() != origin);

    compare &= image1->getType() == type;
    OSLM_ERROR_IF("Images have not the same type : " << image1->getType().string() << " != " << type.string(),
            image1->getType() != type);

    return compare;
}

//------------------------------------------------------------------------------

bool Image::compareArray(::fwData::Array::sptr array1, ::fwData::Array::sptr array2, std::string errorPrefix )
{
    bool compare = true;

    if ( ! array1 && array2 || array1 && ! array2 )
    {
        compare &= false;
        OSLM_ERROR( errorPrefix << "Arrays are not equivalent (one array has a null sptr)");
    }
    else if ( array1 && array2 )
    {
        compare &= array1->getSizeInBytes() == array2->getSizeInBytes();
        OSLM_ERROR_IF( errorPrefix << "Arrays have not same size in bytes", array1->getSizeInBytes() != array2->getSizeInBytes());

        compare &= array1->getElementSizeInBytes() ==  array2->getElementSizeInBytes();
        OSLM_ERROR_IF( errorPrefix << "Arrays have not same element size in bytes", array1->getElementSizeInBytes() !=  array2->getElementSizeInBytes());

        compare &= array1->getNumberOfDimensions() ==  array2->getNumberOfDimensions();
        OSLM_ERROR_IF( errorPrefix << "Arrays have not same number of dimensions", array1->getNumberOfDimensions() !=  array2->getNumberOfDimensions());

        compare &= array1->getSize() == array2->getSize();
        OSLM_ERROR_IF(errorPrefix <<  "Arrays have not same size", array1->getSize() != array2->getSize());

        compare &= array1->getNumberOfComponents() ==  array2->getNumberOfComponents();
        OSLM_ERROR_IF(errorPrefix <<  "Arrays have not same number of components", array1->getNumberOfComponents() !=  array2->getNumberOfComponents());

        /*
    compare &= array1->getIsBufferOwner() ==  array2->getIsBufferOwner();
    OSLM_ERROR_IF("Arrays have not same buffer owner : " << array1->getIsBufferOwner() << " != " << array2->getIsBufferOwner(),
            array1->getIsBufferOwner() !=  array2->getIsBufferOwner());
         */

        compare &= array1->getStrides() == array2->getStrides();
        OSLM_ERROR_IF( errorPrefix << "Arrays have not same strides", array1->getStrides() != array2->getStrides());

        compare &= array1->getType().string() == array2->getType().string();
        OSLM_ERROR_IF( errorPrefix << "Arrays have not same type", array1->getType().string() != array2->getType().string());

        compare &= array1->getType().sizeOf() == array2->getType().sizeOf();
        OSLM_ERROR_IF( errorPrefix << "Arrays have not same type", array1->getType().sizeOf() != array2->getType().sizeOf());

        if(array1)
        {
            char *iter1 = array1->begin<char>();
            char *iter2 = array2->begin<char>();

            for (; iter1 != array1->end<char>() ; ++iter1, ++iter2)
            {
                if ((*iter1 != *iter2))
                {
                    compare = false;
                    OSLM_ERROR( errorPrefix << "Array values are different");
                    break;
                }
            }
        }
    }
    return compare;
}

//------------------------------------------------------------------------------

struct RoiApplyerParam
{
    ::fwData::Image::sptr img;
    ::fwData::Image::sptr roi;
};

template<typename IMAGE_TYPE>
struct RoiApplyer
{
    template<typename ROI_TYPE>
        void operator()( RoiApplyerParam & p )
        {
            typedef IMAGE_TYPE ImgType;
            typedef ROI_TYPE   RoiType;

            SLM_ASSERT( "Null image pointer" , p.img && p.roi);

            ::fwData::Array::sptr imgData;
            ::fwData::Array::sptr roiData;
            imgData = p.img->getDataArray();
            roiData = p.roi->getDataArray();

            SLM_ASSERT( "Null data array pointer" , imgData && roiData);
            SLM_ASSERT( "Null data buffers" , imgData->getBuffer() && roiData->getBuffer());

            ImgType *imIt = imgData->begin<ImgType>();
            RoiType *roiIt = roiData->begin<RoiType>();

            const ImgType *imEnd = imIt + imgData->getNumberOfElements();

            for ( ; imIt != imEnd ; ++imIt, ++roiIt)
            {
                if (*roiIt == 0)
                {
                    *imIt = 0;
                }
            }
        }
};

struct RoiApplyerCaller
{
    template<typename IMAGE_TYPE>
    void operator()( RoiApplyerParam & p )
    {
        fwTools::Dispatcher< ::fwTools::IntrinsicTypes , RoiApplyer<IMAGE_TYPE> >::invoke( p.roi->getPixelType() , p );
    }
};


void Image::applyRoi( ::fwData::Image::sptr image, ::fwData::Image::sptr roi )
{
    SLM_ASSERT( "Null image pointers", image && roi);
    SLM_ASSERT( "Images have different size", image->getSize() == roi->getSize());

    using namespace boost;

    RoiApplyerParam param;
    param.img = image;
    param.roi = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    fwTools::Dispatcher< ::fwTools::IntrinsicTypes , RoiApplyerCaller >::invoke( image->getPixelType() , param );
}


//------------------------------------------------------------------------------

struct RoiTesterParam
{
    ::fwData::Image::sptr img;
    ::fwData::Image::sptr imgRoiApplyed;
    ::fwData::Image::sptr roi;
    bool result;
};

template<typename IMAGE_TYPE>
struct RoiTester
{

    template<typename ROI_TYPE>
        void operator()( RoiTesterParam & p )
        {
            bool &result = p.result;
            result = true;

            typedef IMAGE_TYPE ImgType;
            typedef ROI_TYPE RoiType;

            ::fwData::Array::sptr imgData;
            ::fwData::Array::sptr imgRoiApplyedData;
            ::fwData::Array::sptr roiData;

            imgData = p.img->getDataArray();
            imgRoiApplyedData = p.imgRoiApplyed->getDataArray();
            roiData = p.roi->getDataArray();

            SLM_ASSERT( "Null data array pointer", imgData && roiData && imgRoiApplyedData);
            SLM_ASSERT( "Null data buffers", imgData->getBuffer() && roiData->getBuffer() && imgRoiApplyedData->getBuffer() );

            ImgType *imIt     = imgData->begin<ImgType>();
            ImgType *imRoiIt  = imgRoiApplyedData->begin<ImgType>();
            RoiType *roiIt    = roiData->begin<RoiType>();

            const ImgType *imEnd = imIt + imgData->getNumberOfElements();

            for ( ; result && imIt != imEnd ; ++imIt, ++roiIt, ++imRoiIt)
            {
                result = result && ( (*roiIt == 0) ? (*imRoiIt == 0) : (*imIt == *imRoiIt) );
            }
        }
};


struct RoiTesterCaller
{
    template<typename IMAGE_TYPE>
    void operator()( RoiTesterParam & p )
    {
        fwTools::Dispatcher< ::fwTools::IntrinsicTypes , RoiTester<IMAGE_TYPE> >::invoke( p.roi->getPixelType() , p );
    }
};

bool Image::isRoiApplyed( ::fwData::Image::sptr image, ::fwData::Image::sptr roi, ::fwData::Image::sptr imgRoiApplyed )
{
    SLM_ASSERT( "Null image pointers", image && imgRoiApplyed && roi);
    SLM_ASSERT( "Images have different size", image->getSize() == imgRoiApplyed->getSize() && image->getSize() == roi->getSize());

    RoiTesterParam param;
    param.img = image;
    param.imgRoiApplyed = imgRoiApplyed;
    param.roi = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    fwTools::Dispatcher< ::fwTools::IntrinsicTypes , RoiTesterCaller >::invoke( image->getPixelType() , param );

    return param.result;
}

} // namespace fwDataTools
