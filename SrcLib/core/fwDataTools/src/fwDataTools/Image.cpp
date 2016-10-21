/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/Image.hpp"

#include <fwDataTools/helper/Array.hpp>

#include <fwTools/Combinatory.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

namespace fwDataTools
{

//------------------------------------------------------------------------------

struct RoiApplyerParam
{
    ::fwData::Image::sptr img;
    ::fwData::Image::sptr roi;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiApplyer
{
    template<typename ROI_TYPE>
    void operator()( RoiApplyerParam& p )
    {
        typedef IMAGE_TYPE ImgType;
        typedef ROI_TYPE RoiType;

        SLM_ASSERT( "Null image pointer", p.img && p.roi);

        ::fwData::Array::sptr imgData;
        ::fwData::Array::sptr roiData;
        imgData = p.img->getDataArray();
        roiData = p.roi->getDataArray();


        ::fwDataTools::helper::Array imgHelper(imgData);
        ::fwDataTools::helper::Array roiHelper(roiData);
        SLM_ASSERT( "Null data array pointer", imgData && roiData);
        SLM_ASSERT( "Null data buffers", imgHelper.getBuffer() && roiHelper.getBuffer());

        ImgType* imIt  = imgHelper.begin<ImgType>();
        RoiType* roiIt = roiHelper.begin<RoiType>();

        const ImgType* imEnd = imIt + imgData->getNumberOfElements();

        for (; imIt != imEnd; ++imIt, ++roiIt)
        {
            if (*roiIt == 0)
            {
                * imIt = 0;
            }
        }
    }
};

//------------------------------------------------------------------------------

struct RoiApplyerCaller
{
    template<typename IMAGE_TYPE>
    void operator()( RoiApplyerParam& p )
    {
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RoiApplyer<IMAGE_TYPE> >::invoke( p.roi->getPixelType(), p );
    }
};

//------------------------------------------------------------------------------

void Image::applyRoi( ::fwData::Image::sptr image, ::fwData::Image::sptr roi )
{
    SLM_ASSERT( "Null image pointers", image && roi);
    SLM_ASSERT( "Images have different size", image->getSize() == roi->getSize());

    using namespace boost;

    RoiApplyerParam param;
    param.img = image;
    param.roi = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RoiApplyerCaller >::invoke( image->getPixelType(), param );
}


//------------------------------------------------------------------------------

struct RoiTesterParam
{
    ::fwData::Image::sptr img;
    ::fwData::Image::sptr imgRoiApplyed;
    ::fwData::Image::sptr roi;
    bool result;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiTester
{

    template<typename ROI_TYPE>
    void operator()( RoiTesterParam& p )
    {
        bool& result = p.result;
        result = true;

        typedef IMAGE_TYPE ImgType;
        typedef ROI_TYPE RoiType;

        ::fwData::Array::sptr imgData;
        ::fwData::Array::sptr imgRoiApplyedData;
        ::fwData::Array::sptr roiData;

        imgData           = p.img->getDataArray();
        imgRoiApplyedData = p.imgRoiApplyed->getDataArray();
        roiData           = p.roi->getDataArray();

        ::fwDataTools::helper::Array imgHelper(imgData);
        ::fwDataTools::helper::Array roiHelper(roiData);
        ::fwDataTools::helper::Array imgRoiApplyedHelper(imgRoiApplyedData);

        SLM_ASSERT( "Null data array pointer", imgData && roiData && imgRoiApplyedData);
        SLM_ASSERT( "Null data buffers",
                    imgHelper.getBuffer() && roiHelper.getBuffer() && imgRoiApplyedHelper.getBuffer() );

        ImgType* imIt    = imgHelper.begin<ImgType>();
        ImgType* imRoiIt = imgRoiApplyedHelper.begin<ImgType>();
        RoiType* roiIt   = roiHelper.begin<RoiType>();

        const ImgType* imEnd = imIt + imgData->getNumberOfElements();

        for (; result && imIt != imEnd; ++imIt, ++roiIt, ++imRoiIt)
        {
            result = result && ( (*roiIt == 0) ? (*imRoiIt == 0) : (*imIt == *imRoiIt) );
        }
    }
};

//------------------------------------------------------------------------------

struct RoiTesterCaller
{
    template<typename IMAGE_TYPE>
    void operator()( RoiTesterParam& p )
    {
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RoiTester<IMAGE_TYPE> >::invoke( p.roi->getPixelType(), p );
    }
};

//------------------------------------------------------------------------------

bool Image::isRoiApplyed( ::fwData::Image::sptr image, ::fwData::Image::sptr roi, ::fwData::Image::sptr imgRoiApplyed )
{
    SLM_ASSERT( "Null image pointers", image && imgRoiApplyed && roi);
    SLM_ASSERT( "Images have different size",
                image->getSize() == imgRoiApplyed->getSize() && image->getSize() == roi->getSize());

    RoiTesterParam param;
    param.img           = image;
    param.imgRoiApplyed = imgRoiApplyed;
    param.roi           = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RoiTesterCaller >::invoke( image->getPixelType(), param );

    return param.result;
}

} // namespace fwDataTools
