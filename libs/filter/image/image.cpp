/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "image.hpp"

#include <core/tools/dispatcher.hpp>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

struct RoiApplierParam
{
    data::image::sptr img;
    data::image::sptr roi;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct RoiApplier
{
    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()(RoiApplierParam& p)
    {
        using ImgType = IMAGE_TYPE;
        using RoiType = ROI_TYPE;

        SIGHT_ASSERT("Null image pointer", p.img && p.roi);

        const auto imgDumpLock = p.img->dump_lock();
        const auto roiDumpLock = p.roi->dump_lock();

        SIGHT_ASSERT("Null data buffers", p.img->buffer() && p.roi->buffer());

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
        core::tools::dispatcher<core::tools::supported_dispatcher_types, RoiApplier<IMAGE_TYPE> >::invoke(
            p.roi->getType(),
            p
        );
    }
};

//------------------------------------------------------------------------------

void applyRoi(data::image::sptr image, data::image::sptr roi)
{
    SIGHT_ASSERT("Null image pointers", image && roi);
    SIGHT_ASSERT("images have different size", image->size() == roi->size());

    RoiApplierParam param;
    param.img = image;
    param.roi = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::dispatcher<core::tools::supported_dispatcher_types, RoiApplierCaller>::invoke(
        image->getType(),
        param
    );
}

//------------------------------------------------------------------------------

struct RoiTesterParam
{
    data::image::sptr img;
    data::image::sptr imgRoiApplied;
    data::image::sptr roi;
    bool result {};
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

        using ImgType = IMAGE_TYPE;
        using RoiType = ROI_TYPE;

        const auto imgDumpLock           = p.img->dump_lock();
        const auto imgRoiAppliedDumpLock = p.imgRoiApplied->dump_lock();
        const auto roiDumpLock           = p.roi->dump_lock();

        SIGHT_ASSERT(
            "Null data buffers",
            p.img->buffer() && p.roi->buffer() && p.imgRoiApplied->buffer()
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
        core::tools::dispatcher<core::tools::supported_dispatcher_types, RoiTester<IMAGE_TYPE> >::invoke(
            p.roi->getType(),
            p
        );
    }
};

//------------------------------------------------------------------------------

bool isRoiApplied(data::image::sptr image, data::image::sptr roi, data::image::sptr imgRoiApplied)
{
    SIGHT_ASSERT("Null image pointers", image && imgRoiApplied && roi);
    SIGHT_ASSERT(
        "images have different size",
        image->size() == imgRoiApplied->size() && image->size() == roi->size()
    );

    RoiTesterParam param;
    param.img           = image;
    param.imgRoiApplied = imgRoiApplied;
    param.roi           = roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::dispatcher<core::tools::supported_dispatcher_types, RoiTesterCaller>::invoke(
        image->getType(),
        param
    );

    return param.result;
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image
