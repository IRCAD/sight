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

#pragma once

#include "filter/image/config.hpp"

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
FILTER_IMAGE_API void applyRoi(data::image::sptr image, data::image::sptr roi);

/**
 * @brief Check if 'imgRoiApplied' is the result of 'roi' Applied to 'image'
 *
 */
FILTER_IMAGE_API bool isRoiApplied(
    data::image::sptr image,
    data::image::sptr roi,
    data::image::sptr imgRoiApplied
);

/**
 * @brief Merge mask in image imgDest: put value 'val' in imgDest when mask value != 0
 */
template<typename IMG_DEST_TYPE, typename MASK_TYPE>
void mergeMask(const data::image::sptr& imgDest, const data::image::csptr& mask, IMG_DEST_TYPE val)
{
    typedef IMG_DEST_TYPE ImgDestType;
    typedef MASK_TYPE MaskType;
    SIGHT_ASSERT("image dest has not correct type", imgDest->getType() == core::type::get<ImgDestType>());
    SIGHT_ASSERT("image mask has not correct type", mask->getType() == core::type::get<MaskType>());

    SIGHT_ASSERT("images have not the same size", imgDest->size() == mask->size());
    SIGHT_ASSERT(
        "images have not the same spacing",
        core::tools::is_equal(imgDest->getSpacing(), mask->getSpacing())
    );
    SIGHT_ASSERT(
        "images have not the same origin",
        core::tools::is_equal(imgDest->getOrigin(), mask->getOrigin())
    );

    const auto imgDumpLock  = imgDest->dump_lock();
    const auto maskDumpLock = mask->dump_lock();

    auto imgItr          = imgDest->begin<ImgDestType>();
    const auto imgItrEnd = imgDest->end<ImgDestType>();

    auto maskItr          = mask->begin<MaskType>();
    const auto maskItrEnd = mask->end<MaskType>();

    for( ; imgItr != imgItrEnd && maskItr != maskItrEnd ; ++imgItr, ++maskItr)
    {
        if(*maskItr != 0)
        {
            *imgItr = val;
        }
    }
}

} // namespace sight::filter::image
