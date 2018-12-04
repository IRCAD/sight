/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwDataTools/fieldHelper/Image.hpp"

namespace fwDataTools
{

namespace fieldHelper
{

const std::string Image::DATA_IS_LOCKED                 ( "DATA_IS_LOCKED" );

const std::string Image::position                       ( "position" );

const std::string Image::localGui                       ( "Local GUI" );
const std::string Image::m_axialSliceCountId            ( "Axial Slice Count" );
const std::string Image::m_frontalSliceCountId          ( "Frontal Slice Count" );
const std::string Image::m_sagittalSliceCountId         ( "Sagittal Slice Count" );
const std::string Image::m_axialSliceIndexId            ( "Axial Slice Index" );
const std::string Image::m_frontalSliceIndexId          ( "Frontal Slice Index" );
const std::string Image::m_sagittalSliceIndexId         ( "Sagittal Slice Index" );
const std::string Image::m_transferFunctionCompositeId  ( "m_transferFunctionCompositeId" );
const std::string Image::m_imageLandmarksId             ("m_imageLandmarksId");
const std::string Image::m_imageDistancesId             ("m_imageDistancesId");
const std::string Image::m_commentId                    ("m_commentId");
const std::string Image::m_labelId                      ("m_labelId");
const std::string Image::m_colorId                      ("m_colorId");
const std::string Image::m_relatedServiceId             ("m_relatedServiceId");
const std::string Image::m_imageLabelId                 ("m_imageLabelId");
const std::string Image::m_acquisitionCountId           ("m_acquisitionCountId");

const std::string Image::m_globalMatrixId               ("Global Matrix");
const std::string Image::m_errorId                      ("Error");

const std::string Image::m_roi                          ( "ROI" );

} // fieldHelper

}
