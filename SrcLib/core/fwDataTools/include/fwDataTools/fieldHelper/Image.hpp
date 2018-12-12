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

#ifndef __FWDATATOOLS_FIELDHELPER_IMAGE_HPP__
#define __FWDATATOOLS_FIELDHELPER_IMAGE_HPP__

#include "fwDataTools/config.hpp"

#include <string>

namespace fwDataTools
{
namespace fieldHelper
{
/**
 * @brief   Centralization of all identifiers used for communication and storage of dynamic attributes
 * @note    probably not dynamic enough but allows at least to have an overview of information that can be exchanged
 */
struct FWDATATOOLS_CLASS_API Image
{
    /// position oriented IDs
    FWDATATOOLS_API const static std::string position;

    /// visualization oriented IDs
    /// @{
    FWDATATOOLS_API static const std::string localGui;
    FWDATATOOLS_API static const std::string m_axialSliceCountId;
    FWDATATOOLS_API static const std::string m_frontalSliceCountId;
    FWDATATOOLS_API static const std::string m_sagittalSliceCountId;
    FWDATATOOLS_API static const std::string m_axialSliceIndexId;
    FWDATATOOLS_API static const std::string m_frontalSliceIndexId;
    FWDATATOOLS_API static const std::string m_sagittalSliceIndexId;
    FWDATATOOLS_API static const std::string m_transferFunctionCompositeId;
    FWDATATOOLS_API static const std::string m_globalMatrixId;
    FWDATATOOLS_API static const std::string m_errorId;
    FWDATATOOLS_API static const std::string m_imageLandmarksId;
    FWDATATOOLS_API static const std::string m_imageDistancesId;
    FWDATATOOLS_API static const std::string m_commentId;
    FWDATATOOLS_API static const std::string m_colorId;
    FWDATATOOLS_API static const std::string m_labelId;         ///< to assign a label
    FWDATATOOLS_API static const std::string m_relatedServiceId; ///< to store uid (simple) of service which create the object
    FWDATATOOLS_API static const std::string m_imageLabelId;
    FWDATATOOLS_API static const std::string m_acquisitionCountId;
    /// @}

    /// processing oriented IDs
    /// @{
    FWDATATOOLS_API static const std::string m_roi;
    /// @}


    /// processing oriented data
    /// @{
    FWDATATOOLS_API static const std::string DATA_IS_LOCKED;
    /// @}
};

} // fieldHelper

}

#endif /*__FWDATATOOLS_FIELDHELPER_IMAGE_HPP__*/
