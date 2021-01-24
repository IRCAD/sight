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

#pragma once

#include "fwDataTools/config.hpp"

#include <core/base.hpp>
#include <core/tools/Type.hpp>

#include <data/Image.hpp>

#include <fwMath/Compare.hpp>

namespace fwDataTools
{

/**
 * @brief   This class contains helper to generate and compare images.
 */
class Image
{

public:

    /**
     * @brief Apply roi to image
     *
     * set input image voxel to zero where roi voxel value is zero
     */
    FWDATATOOLS_API static void applyRoi( data::Image::sptr image,
                                          data::Image::sptr roi );

    /**
     * @brief Check if 'imgRoiApplyed' is the result of 'roi' applyed to 'image'
     *
     */
    FWDATATOOLS_API static bool isRoiApplyed( data::Image::sptr image,
                                              data::Image::sptr imgRoiApplyed,
                                              data::Image::sptr roi );

    /**
     * @brief Merge mask in image imgDest: put value 'val' in imgDest when mask value != 0
     */
    template<typename IMG_DEST_TYPE, typename MASK_TYPE>
    void mergeMask(const data::Image::sptr& imgDest, const data::Image::csptr& mask, IMG_DEST_TYPE val );

};

//------------------------------------------------------------------------------

template<typename IMG_DEST_TYPE, typename MASK_TYPE>
void Image::mergeMask(const data::Image::sptr& imgDest, const data::Image::csptr& mask, IMG_DEST_TYPE val )
{
    typedef IMG_DEST_TYPE ImgDestType;
    typedef MASK_TYPE MaskType;
    SLM_ASSERT( "Image dest has not correct type", imgDest->getType().isOfType< ImgDestType >());
    SLM_ASSERT( "Image mask has not correct type", mask->getType().isOfType< MaskType >());

    SLM_ASSERT( "Images have not the same size", imgDest->getSize2() == mask->getSize2() );
    SLM_ASSERT( "Images have not the same spacing",
                ::fwMath::isContainerEqual(imgDest->getSpacing2(), mask->getSpacing2()) );
    SLM_ASSERT( "Images have not the same origin",
                ::fwMath::isContainerEqual(imgDest->getOrigin2(), mask->getOrigin2()) );

    const auto imgDumpLock  = imgDest->lock();
    const auto maskDumpLock = mask->lock();

    auto imgItr          = imgDest->begin< ImgDestType >();
    const auto imgItrEnd = imgDest->end< ImgDestType >();

    auto maskItr          = mask->begin< MaskType >();
    const auto maskItrEnd = mask->end< MaskType >();

    for (; imgItr != imgItrEnd && maskItr != maskItrEnd; ++imgItr, ++maskItr)
    {
        if (*maskItr != 0)
        {
            *imgItr = val;
        }
    }
}

} // namespace fwDataTools
