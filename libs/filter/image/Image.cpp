/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "Image.hpp"

#include <core/tools/Dispatcher.hpp>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

struct RoiApplierParam
{
    data::Image::sptr img;
    data::Image::sptr roi;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiApplier
{
    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()(RoiApplierParam& p)
    {
        typedef IMAGE_TYPE ImgType;
        typedef ROI_TYPE RoiType;

        SIGHT_ASSERT("Null image pointer", p.img && p.roi);

        const auto imgDumpLock = p.img->dump_lock();
        const auto roiDumpLock = p.roi->dump_lock();

        SIGHT_ASSERT("Null data buffers", p.img->getBuffer() && p.roi->getBuffer());

        auto imIt         = p.img->begin<ImgType>();
        const auto imEnd  = p.img->end<ImgType>();
        auto roiIt        = p.roi->begin<RoiType>();
        const auto roiEnd = p.roi->end<RoiType>();

        for( ; imIt != imEnd && roiIt != roiEnd ; ++imIt, ++roiIt)
        {
            if(*roiIt == 0)
            {
                *imIt = 0;
            }
        }
    }
};

//------------------------------------------------------------------------------

struct RoiApplierCaller
{
    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()(RoiApplierParam& p)
    {
        core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, RoiApplier<IMAGE_TYPE> >::invoke(
            p.roi->getType(),
            p
        );
    }
};

//------------------------------------------------------------------------------

void applyRoi(data::Image::sptr image, data::Image::sptr roi)
{
    SIGHT_ASSERT("Null image pointers", image && roi);
    SIGHT_ASSERT("Images have different size", image->getSize() == roi->getSize());

    using namespace boost;

    RoiApplierParam param;
    param.img = image;
    param.roi = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, RoiApplierCaller>::invoke(
        image->getType(),
        param
    );
}

//------------------------------------------------------------------------------

struct RoiTesterParam
{
    data::Image::sptr img;
    data::Image::sptr imgRoiApplied;
    data::Image::sptr roi;
    bool result;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiTester
{
    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()(RoiTesterParam& p)
    {
        bool& result = p.result;
        result = true;

        typedef IMAGE_TYPE ImgType;
        typedef ROI_TYPE RoiType;

        const auto imgDumpLock           = p.img->dump_lock();
        const auto imgRoiAppliedDumpLock = p.imgRoiApplied->dump_lock();
        const auto roiDumpLock           = p.roi->dump_lock();

        SIGHT_ASSERT(
            "Null data buffers",
            p.img->getBuffer() && p.roi->getBuffer() && p.imgRoiApplied->getBuffer()
        );

        auto imIt        = p.img->begin<ImgType>();
        const auto imEnd = p.img->end<ImgType>();
        auto roiIt       = p.roi->begin<RoiType>();
        auto imRoiIt     = p.imgRoiApplied->begin<ImgType>();

        for( ; result && imIt != imEnd ; ++imIt, ++roiIt, ++imRoiIt)
        {
            result = result && ((*roiIt == 0) ? (*imRoiIt == 0) : (*imIt == *imRoiIt));
        }
    }
};

//------------------------------------------------------------------------------

struct RoiTesterCaller
{
    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()(RoiTesterParam& p)
    {
        core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, RoiTester<IMAGE_TYPE> >::invoke(
            p.roi->getType(),
            p
        );
    }
};

//------------------------------------------------------------------------------

bool isRoiApplied(data::Image::sptr image, data::Image::sptr roi, data::Image::sptr imgRoiApplied)
{
    SIGHT_ASSERT("Null image pointers", image && imgRoiApplied && roi);
    SIGHT_ASSERT(
        "Images have different size",
        image->getSize() == imgRoiApplied->getSize() && image->getSize() == roi->getSize()
    );

    RoiTesterParam param;
    param.img           = image;
    param.imgRoiApplied = imgRoiApplied;
    param.roi           = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, RoiTesterCaller>::invoke(
        image->getType(),
        param
    );

    return param.result;
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image
