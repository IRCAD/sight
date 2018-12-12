/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDATATOOLS_IMAGE_HPP__
#define __FWDATATOOLS_IMAGE_HPP__

#include "fwDataTools/config.hpp"
#include <fwDataTools/helper/Array.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwMath/Compare.hpp>

#include <fwTools/Type.hpp>

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
    FWDATATOOLS_API static void applyRoi( ::fwData::Image::sptr image,
                                          ::fwData::Image::sptr roi );

    /**
     * @brief Check if 'imgRoiApplyed' is the result of 'roi' applyed to 'image'
     *
     */
    FWDATATOOLS_API static bool isRoiApplyed( ::fwData::Image::sptr image,
                                              ::fwData::Image::sptr imgRoiApplyed,
                                              ::fwData::Image::sptr roi );

    /**
     * @brief Merge mask in image imgDest: put value 'val' in imgDest when mask value != 0
     */
    template<typename IMG_DEST_TYPE, typename MASK_TYPE>
    void mergeMask(::fwData::Image::sptr imgDest, ::fwData::Image::sptr mask, IMG_DEST_TYPE val );

};

//------------------------------------------------------------------------------

template<typename IMG_DEST_TYPE, typename MASK_TYPE>
void Image::mergeMask(::fwData::Image::sptr imgDest, ::fwData::Image::sptr mask, IMG_DEST_TYPE val )
{
    typedef IMG_DEST_TYPE ImgDestType;
    typedef MASK_TYPE MaskType;
    SLM_ASSERT( "Image dest has not correct type", imgDest->getType().isOfType< ImgDestType >());
    SLM_ASSERT( "Image mask has not correct type", mask->getType().isOfType< MaskType >());

    SLM_ASSERT( "Images have not the same size", imgDest->getSize() == mask->getSize() );
    SLM_ASSERT( "Images have not the same spacing",
                ::fwMath::isContainerEqual(imgDest->getSpacing(), mask->getSpacing()) );
    SLM_ASSERT( "Images have not the same origin",
                ::fwMath::isContainerEqual(imgDest->getOrigin(), mask->getOrigin()) );

    ::fwData::Array::sptr imgData;
    ::fwData::Array::sptr maskData;
    imgData  = imgDest->getDataArray();
    maskData = mask->getDataArray();

    ::fwDataTools::helper::Array imgHelper(imgData);
    ::fwDataTools::helper::Array maskHelper(maskData);

    ImgDestType* imgIt = imgHelper.begin<ImgDestType>();
    MaskType* maskIt   = maskHelper.begin<MaskType>();

    const ImgDestType* imgEnd = imgIt + maskData->getNumberOfElements();

    for (; imgIt != imgEnd; ++imgIt, ++maskIt)
    {
        if (*maskIt != 0)
        {
            *imgIt = val;
        }
    }
}

} // namespace fwDataTools

#endif // __FWDATATOOLS_IMAGE_HPP__
