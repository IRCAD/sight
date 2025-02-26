/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/filter/image/config.hpp>

#include <core/base.hpp>
#include <core/type.hpp>

#include <data/image.hpp>

// Contains helper to generate and compare images.
namespace sight::filter::image
{

/**
 * @brief Apply roi to image
 *
 * set input image voxel to zero where roi voxel value is zero
 */
SIGHT_FILTER_IMAGE_API void apply_roi(data::image::sptr _image, data::image::sptr _roi);

/**
 * @brief Check if 'imgRoiApplied' is the result of 'roi' Applied to 'image'
 *
 */
SIGHT_FILTER_IMAGE_API bool is_roi_applied(
    data::image::sptr _image,
    data::image::sptr _roi,
    data::image::sptr _img_roi_applied
);

/**
 * @brief Merge mask in image imgDest: put value 'val' in imgDest when mask value != 0
 */
template<typename IMG_DEST_TYPE, typename MASK_TYPE>
void merge_mask(const data::image::sptr& _img_dest, const data::image::csptr& _mask, IMG_DEST_TYPE _val)
{
    typedef IMG_DEST_TYPE img_dest_t;
    typedef MASK_TYPE mask_t;
    SIGHT_ASSERT("image dest has not correct type", _img_dest->type() == core::type::get<img_dest_t>());
    SIGHT_ASSERT("image mask has not correct type", _mask->type() == core::type::get<mask_t>());

    SIGHT_ASSERT("images have not the same size", _img_dest->size() == _mask->size());
    SIGHT_ASSERT(
        "images have not the same spacing",
        core::is_equal(_img_dest->spacing(), _mask->spacing())
    );
    SIGHT_ASSERT(
        "images have not the same origin",
        core::is_equal(_img_dest->origin(), _mask->origin())
    );

    const auto img_dump_lock  = _img_dest->dump_lock();
    const auto mask_dump_lock = _mask->dump_lock();

    auto img_itr           = _img_dest->begin<img_dest_t>();
    const auto img_itr_end = _img_dest->end<img_dest_t>();

    auto mask_itr           = _mask->begin<mask_t>();
    const auto mask_itr_end = _mask->end<mask_t>();

    for( ; img_itr != img_itr_end && mask_itr != mask_itr_end ; ++img_itr, ++mask_itr)
    {
        if(*mask_itr != 0)
        {
            *img_itr = _val;
        }
    }
}

} // namespace sight::filter::image
