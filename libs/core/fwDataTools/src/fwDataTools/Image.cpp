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

#include "fwDataTools/Image.hpp"

#include <core/tools/Combinatory.hpp>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

namespace fwDataTools
{

//------------------------------------------------------------------------------

struct RoiApplyerParam
{
    data::Image::sptr img;
    data::Image::sptr roi;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiApplyer
{
    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()( RoiApplyerParam& p )
    {
        typedef IMAGE_TYPE ImgType;
        typedef ROI_TYPE RoiType;

        SLM_ASSERT( "Null image pointer", p.img && p.roi);

        const auto imgDumpLock = p.img->lock();
        const auto roiDumpLock = p.roi->lock();

        SLM_ASSERT( "Null data buffers", p.img->getBuffer() && p.roi->getBuffer());

        auto imIt         = p.img->begin<ImgType>();
        const auto imEnd  = p.img->end<ImgType>();
        auto roiIt        = p.roi->begin<RoiType>();
        const auto roiEnd = p.roi->end<RoiType>();

        for (; imIt != imEnd && roiIt != roiEnd; ++imIt, ++roiIt)
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
    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()( RoiApplyerParam& p )
    {
        core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RoiApplyer<IMAGE_TYPE> >::invoke(
            p.roi->getType(), p );
    }
};

//------------------------------------------------------------------------------

void Image::applyRoi( data::Image::sptr image, data::Image::sptr roi )
{
    SLM_ASSERT( "Null image pointers", image && roi);
    SLM_ASSERT( "Images have different size", image->getSize2() == roi->getSize2());

    using namespace boost;

    RoiApplyerParam param;
    param.img = image;
    param.roi = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RoiApplyerCaller >::invoke( image->getType(),
                                                                                                param );
}

//------------------------------------------------------------------------------

struct RoiTesterParam
{
    data::Image::sptr img;
    data::Image::sptr imgRoiApplyed;
    data::Image::sptr roi;
    bool result;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiTester
{

    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()( RoiTesterParam& p )
    {
        bool& result = p.result;
        result = true;

        typedef IMAGE_TYPE ImgType;
        typedef ROI_TYPE RoiType;

        const auto imgDumpLock           = p.img->lock();
        const auto imgRoiAppliedDumpLock = p.imgRoiApplyed->lock();
        const auto roiDumpLock           = p.roi->lock();

        SLM_ASSERT( "Null data buffers",
                    p.img->getBuffer() && p.roi->getBuffer() && p.imgRoiApplyed->getBuffer() );

        auto imIt        = p.img->begin<ImgType>();
        const auto imEnd = p.img->end<ImgType>();
        auto roiIt       = p.roi->begin<RoiType>();
        auto imRoiIt     = p.imgRoiApplyed->begin<ImgType>();

        for (; result && imIt != imEnd; ++imIt, ++roiIt, ++imRoiIt)
        {
            result = result && ( (*roiIt == 0) ? (*imRoiIt == 0) : (*imIt == *imRoiIt) );
        }
    }
};

//------------------------------------------------------------------------------

struct RoiTesterCaller
{
    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()( RoiTesterParam& p )
    {
        core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RoiTester<IMAGE_TYPE> >::invoke(
            p.roi->getType(),
            p );
    }
};

//------------------------------------------------------------------------------

bool Image::isRoiApplyed( data::Image::sptr image, data::Image::sptr roi, data::Image::sptr imgRoiApplyed )
{
    SLM_ASSERT( "Null image pointers", image && imgRoiApplyed && roi);
    SLM_ASSERT( "Images have different size",
                image->getSize2() == imgRoiApplyed->getSize2() && image->getSize2() == roi->getSize2());

    RoiTesterParam param;
    param.img           = image;
    param.imgRoiApplyed = imgRoiApplyed;
    param.roi           = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RoiTesterCaller >::invoke( image->getType(),
                                                                                               param );

    return param.result;
}

//------------------------------------------------------------------------------

} // namespace fwDataTools
