/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/config.hpp"

#include <string>

namespace sight::data
{
namespace fieldHelper
{
/**
 * @brief   Centralization of all identifiers used for communication and storage of dynamic attributes
 * @note    probably not dynamic enough but allows at least to have an overview of information that can be exchanged
 */
struct DATA_CLASS_API Image
{
    /// position oriented IDs
    DATA_API const static std::string position;

    /// visualization oriented IDs
    /// @{
    DATA_API static const std::string localGui;
    DATA_API static const std::string m_axialSliceCountId;
    DATA_API static const std::string m_frontalSliceCountId;
    DATA_API static const std::string m_sagittalSliceCountId;
    DATA_API static const std::string m_axialSliceIndexId;
    DATA_API static const std::string m_frontalSliceIndexId;
    DATA_API static const std::string m_sagittalSliceIndexId;
    DATA_API static const std::string m_transferFunctionCompositeId;
    DATA_API static const std::string m_globalMatrixId;
    DATA_API static const std::string m_errorId;
    DATA_API static const std::string m_imageLandmarksId;
    DATA_API static const std::string m_imageDistancesId;
    DATA_API static const std::string m_distanceVisibility;
    DATA_API static const std::string m_commentId;
    DATA_API static const std::string m_colorId;
    DATA_API static const std::string m_labelId;         ///< to assign a label
    DATA_API static const std::string m_relatedServiceId; ///< to store uid (simple) of service which create the
    // object
    DATA_API static const std::string m_imageLabelId;
    DATA_API static const std::string m_acquisitionCountId;
    /// @}

    /// processing oriented IDs
    /// @{
    DATA_API static const std::string m_roi;
    /// @}

    /// processing oriented data
    /// @{
    DATA_API static const std::string DATA_IS_LOCKED;
    /// @}
};

} // fieldHelper

}
