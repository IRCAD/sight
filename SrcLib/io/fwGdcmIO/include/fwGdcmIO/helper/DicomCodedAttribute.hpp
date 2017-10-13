/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMCODEDATTRIBUTE_HPP__
#define __FWGDCMIO_HELPER_DICOMCODEDATTRIBUTE_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

#include <gdcmSegment.h>

namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief Helper class to manage DICOM coded attributes
 */
class FWGDCMIO_CLASS_API DicomCodedAttribute
{
public:

    /// DicomCodedAttribute container type
    typedef std::vector< ::fwGdcmIO::container::DicomCodedAttribute > DicomCodedAttributeVectorType;

    /**
     * @brief Convert an entry of the form '(AAA;BBB;CCC)' into a list of DicomCodedAttributes.
     * Several DicomCodedAttributes can be detected in a single entry.
     * @param[in] entry Entry containing one or several coded attributes.
     */
    FWGDCMIO_API static DicomCodedAttributeVectorType convertEntryToCodedAttribute(const std::string& entry);

    /**
     * @brief Convert an entry of the form '(AAA;BBB;CCC)' into GDCM's BasicCodedEntryVector.
     * Several DicomCodedAttributes can be detected in a single entry.
     * @param[in] entry Entry containing one or several coded attributes.
     */
    FWGDCMIO_API static ::gdcm::Segment::BasicCodedEntryVector
        convertEntryToGDCMCodedAttribute(const std::string& entry);

    /**
     * @brief Retrieve '(AAA;BBB;CCC)' patterns inside of the entry string.
     * Remove spaces or other characters that might be present between matches.
     * If a single value is requested, check the uniqueness of the value.
     * @param[in,out] entry Value that shall be verified and formatted
     * @param[in] multipleValue If set to 'false', check the uniqueness of the value
     */
    FWGDCMIO_API static bool checkAndFormatEntry(std::string& entry, bool multipleValue = false);

};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_DICOMCODEDATTRIBUTE_HPP__

