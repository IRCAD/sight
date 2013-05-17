/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMDICTIONARYSEGMENT_HPP__
#define __GDCMIO_CONTAINER_DICOMDICTIONARYSEGMENT_HPP__

#include "gdcmIO/config.hpp"
#include "gdcmIO/container/DicomCodedAttribute.hpp"

namespace gdcmIO
{
namespace container
{

/**
 * @brief   This class implement a segmentation dictionary.
 *
 * It allows to get a complete identification of a segmentation
 * from the organ name of a fwData::Reconstruction.
 *
 * @note    Currently, this identification has to be done with tools of
 *          this class.
 *
 * Concept : Singleton
 *
 * @class   DicomDictionarySegment.
 */
class GDCMIO_CLASS_API DicomDictionarySegment
{
public:

    /**
     * @brief   Get anatomic region sequence from label of an organ.
     *
     * @see     Tag (0x0008,0x2218)
     *
     * @note    Returned null pointer means organ name is unknown.
     */
    GDCMIO_API static const DicomCodedAttribute * guessAnatRegionFromLabel(const std::string& a_label);

    /**
     * @brief   Get segmented property category code sequence from label of an organ.
     *
     * @see     Tag (0x0062,0x0003)
     *
     * @note    Returned null pointer means organ name is unknown.
     */
    GDCMIO_API static const DicomCodedAttribute * guessPropCategoryFromLabel(const std::string& a_label);

    /**
     * @brief   Get segmented property type code sequence from label of an organ.
     *
     * @see     Tag (0x0062,0x000F)
     *
     * @note    Returned null pointer means organ name is unknown.
     */
    GDCMIO_API static const DicomCodedAttribute * guessPropTypeFromLabel(const std::string& a_label);

};

} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMDICTIONARYSEGMENT_HPP__ */
