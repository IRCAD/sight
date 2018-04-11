/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/Study.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Study Information Entity class
 */
class FWGDCMIO_CLASS_API Study : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Study >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read study.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] study Study data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Study(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                       const SPTR(::gdcm::Reader)& reader,
                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                       const ::fwMedData::Study::sptr& study,
                       const ::fwLog::Logger::sptr& logger = nullptr,
                       ProgressCallback progress           = nullptr,
                       CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Study();

    /**
     * @brief Read General Study Module tags
     * @see PS 3.3 C.7.2.1
     */
    FWGDCMIO_API virtual void readGeneralStudyModule();

    /**
     * @brief Read Patient Study Module tags
     * @see PS 3.3 C.7.2.2
     */
    FWGDCMIO_API virtual void readPatientStudyModule();

};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
