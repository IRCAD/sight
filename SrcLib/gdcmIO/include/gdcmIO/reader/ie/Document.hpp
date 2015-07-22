/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_DOCUMENT_HPP__
#define __GDCMIO_READER_IE_DOCUMENT_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRNumNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace fwData
{
class Point;
}

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Document
 * @brief Document Information Entity class
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is TID 1400 (taken from PS 3.16) for distances
 * and based on TID 320 (taken from PS 3.16) for landmarks.
 */
class GDCMIO_CLASS_API Document : public ::gdcmIO::reader::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read landmarks and distances.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    GDCMIO_API Document(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                        SPTR(::gdcm::Reader)reader,
                        SPTR(::gdcmIO::container::DicomInstance)instance,
                        ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~Document();

    /**
     * @brief Read the Structured Report
     */
    GDCMIO_API void readSR() throw (::gdcmIO::exception::Failed);

};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_IE_DOCUMENT_HPP__ */
