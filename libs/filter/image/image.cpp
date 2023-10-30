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

struct roi_applier_param
{
    data::image::sptr img;
    data::image::sptr roi;
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct roi_applier
{
    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()(roi_applier_param& _p)
    {
        using img_t = IMAGE_TYPE;
        using roi_t = ROI_TYPE;

        SIGHT_ASSERT("Null image pointer", _p.img && _p.roi);

        const auto img_dump_lock = _p.img->dump_lock();
        const auto roi_dump_lock = _p.roi->dump_lock();

        SIGHT_ASSERT("Null data buffers", _p.img->buffer() && _p.roi->buffer());

        auto im_it         = _p.img->begin<img_t>();
        const auto im_end  = _p.img->end<img_t>();
        auto roi_it        = _p.roi->begin<roi_t>();
        const auto roi_end = _p.roi->end<roi_t>();

        for( ; im_it != im_end && roi_it != roi_end ; ++im_it, ++roi_it)
        {
            if(*roi_it == 0)
            {
                *im_it = 0;
            }
        }
    }
};

//------------------------------------------------------------------------------

struct roi_applier_caller
{
    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()(roi_applier_param& _p)
    {
        core::tools::dispatcher<core::tools::supported_dispatcher_types, roi_applier<IMAGE_TYPE> >::invoke(
            _p.roi->type(),
            _p
        );
    }
};

//------------------------------------------------------------------------------

void apply_roi(data::image::sptr _image, data::image::sptr _roi)
{
    SIGHT_ASSERT("Null image pointers", _image && _roi);
    SIGHT_ASSERT("images have different size", _image->size() == _roi->size());

    roi_applier_param param;
    param.img = _image;
    param.roi = _roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::dispatcher<core::tools::supported_dispatcher_types, roi_applier_caller>::invoke(
        _image->type(),
        param
    );
}

//------------------------------------------------------------------------------

struct roi_tester_param
{
    data::image::sptr img;
    data::image::sptr img_roi_applied;
    data::image::sptr roi;
    bool result {};
};

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
struct roi_tester
{
    //------------------------------------------------------------------------------

    template<typename ROI_TYPE>
    void operator()(roi_tester_param& _p)
    {
        bool& result = _p.result;
        result = true;

        using img_t = IMAGE_TYPE;
        using roi_t = ROI_TYPE;

        const auto img_dump_lock             = _p.img->dump_lock();
        const auto img_roi_applied_dump_lock = _p.img_roi_applied->dump_lock();
        const auto roi_dump_lock             = _p.roi->dump_lock();

        SIGHT_ASSERT(
            "Null data buffers",
            _p.img->buffer() && _p.roi->buffer() && _p.img_roi_applied->buffer()
        );

        auto im_it        = _p.img->begin<img_t>();
        const auto im_end = _p.img->end<img_t>();
        auto roi_it       = _p.roi->begin<roi_t>();
        auto im_roi_it    = _p.img_roi_applied->begin<img_t>();

        for( ; result && im_it != im_end ; ++im_it, ++roi_it, ++im_roi_it)
        {
            result = result && ((*roi_it == 0) ? (*im_roi_it == 0) : (*im_it == *im_roi_it));
        }
    }
};

//------------------------------------------------------------------------------

struct roi_tester_caller
{
    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()(roi_tester_param& _p)
    {
        core::tools::dispatcher<core::tools::supported_dispatcher_types, roi_tester<IMAGE_TYPE> >::invoke(
            _p.roi->type(),
            _p
        );
    }
};

//------------------------------------------------------------------------------

bool is_roi_applied(data::image::sptr _image, data::image::sptr _roi, data::image::sptr _img_roi_applied)
{
    SIGHT_ASSERT("Null image pointers", _image && _img_roi_applied && _roi);
    SIGHT_ASSERT(
        "images have different size",
        _image->size() == _img_roi_applied->size() && _image->size() == _roi->size()
    );

    roi_tester_param param;
    param.img             = _image;
    param.img_roi_applied = _img_roi_applied;
    param.roi             = _roi;

    // Due to link failure, we use two dispatcher calls instead of one with a cross-product type list
    core::tools::dispatcher<core::tools::supported_dispatcher_types, roi_tester_caller>::invoke(
        _image->type(),
        param
    );

    return param.result;
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image
