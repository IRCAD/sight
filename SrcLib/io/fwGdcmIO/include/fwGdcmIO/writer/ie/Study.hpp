/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwMedData/Study.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Study Information Entity class
 */
class FWGDCMIO_CLASS_API Study : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Study >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] study Study data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Study(const SPTR(::gdcm::Writer)& writer,
                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                       const ::fwMedData::Study::csptr& study,
                       const ::fwLog::Logger::sptr& logger = nullptr,
                       ProgressCallback progress           = nullptr,
                       CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Study();

    /**
     * @brief Write General Study Module tags
     * @see PS 3.3 C.7.2.1
     */
    FWGDCMIO_API virtual void writeGeneralStudyModule();

    /**
     * @brief Write Patient Study Module tags
     * @see PS 3.3 C.7.2.2
     */
    FWGDCMIO_API virtual void writePatientStudyModule();

};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
