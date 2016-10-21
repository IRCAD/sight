/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
