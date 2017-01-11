/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICTIONARYSEGMENT_HPP__
#define __FWGDCMIO_HELPER_DICTIONARYSEGMENT_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief This class implement a segmentation dictionary.
 * It allows to get a complete identification of a segmentation from the organ name of a ::fwData::Reconstruction.
 * @note Currently, this identification has to be done with tools of this class.
 */
class FWGDCMIO_CLASS_API DictionarySegment
{
public:

    /**
     * @brief Get anatomic region sequence from label of an organ.
     * @see Tag (0x0008,0x2218)
     * @note Returned null pointer means organ name is unknown.
     * @param[in] label Label from witch the anatomic region is guessed.
     */
    FWGDCMIO_API static const ::fwGdcmIO::container::DicomCodedAttribute* guessAnatRegionFromLabel(
        const std::string& label);

    /**
     * @brief Get segmented property category code sequence from label of an organ.
     * @see Tag (0x0062,0x0003)
     * @note Returned null pointer means organ name is unknown.
     * @param[in] label Label from witch the property category is guessed.
     */
    FWGDCMIO_API static const ::fwGdcmIO::container::DicomCodedAttribute* guessPropCategoryFromLabel(
        const std::string& label);

    /**
     * @brief Get segmented property type code sequence from label of an organ.
     * @see Tag (0x0062,0x000F)
     * @note Returned null pointer means organ name is unknown.
     * @param[in] label Label from witch the property type is guessed.
     */
    FWGDCMIO_API static const ::fwGdcmIO::container::DicomCodedAttribute* guessPropTypeFromLabel(
        const std::string& label);

};

} //namespace helper
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_HELPER_DICTIONARYSEGMENT_HPP__ */
