/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_FRAMEOFREFERENCE_HPP__
#define __FWGDCMIO_WRITER_IE_FRAMEOFREFERENCE_HPP__

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwMedData/Series.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Frame Of Reference Information Entity class
 */
class FWGDCMIO_CLASS_API FrameOfReference : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API FrameOfReference(const SPTR(::gdcm::Writer)& writer,
                                  const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                  const ::fwMedData::Series::sptr& series,
                                  const ::fwLog::Logger::sptr& logger = nullptr,
                                  ProgressCallback progress = nullptr,
                                  CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~FrameOfReference();

    /**
     * @brief Write Frame of Reference Module tags
     * @see PS 3.3 C.7.4.1
     */
    FWGDCMIO_API virtual void writeFrameOfReferenceModule();
};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IE_FRAMEOFREFERENCE_HPP__
