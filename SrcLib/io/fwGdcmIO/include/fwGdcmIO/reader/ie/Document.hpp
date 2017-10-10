/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_DOCUMENT_HPP__
#define __FWGDCMIO_READER_IE_DOCUMENT_HPP__

#include "fwGdcmIO/container/sr/DicomSRNumNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/reader/ie/InformationEntity.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/Image.hpp>

namespace fwData
{
class Point;
}

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Document Information Entity class
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is TID 1400 (taken from PS 3.16) for distances
 * and based on TID 320 (taken from PS 3.16) for landmarks.
 */
class FWGDCMIO_CLASS_API Document : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read landmarks and distances.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Document(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                          const SPTR(::gdcm::Reader)& reader,
                          const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                          const ::fwData::Image::sptr& image,
                          const ::fwLog::Logger::sptr& logger = nullptr,
                          ProgressCallback progress = nullptr,
                          CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Document();

    /**
     * @brief Read the Structured Report
     */
    FWGDCMIO_API void readSR() throw (::fwGdcmIO::exception::Failed);

};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_IE_DOCUMENT_HPP__ */
