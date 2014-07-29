/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_FRAMEOFREFERENCE_HPP__
#define __GDCMIO_WRITER_IE_FRAMEOFREFERENCE_HPP__

#include <fwMedData/Series.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class FrameOfReference
 * @brief Frame Of Reference Information Entity class
 */
class GDCMIO_CLASS_API FrameOfReference : public ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     */
    GDCMIO_API FrameOfReference(SPTR(::gdcm::Writer) writer,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwMedData::Series::sptr series);

    /// Destructor
    GDCMIO_API virtual ~FrameOfReference();

    /**
     * @brief Write Frame of Reference Module tags
     * @see PS 3.3 C.7.4.1
     */
    GDCMIO_API virtual void writeFrameOfReferenceModule();
};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_MODULE_FRAMEOFREFERENCE_HPP__
