/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_STUDY_HPP__
#define __GDCMIO_READER_IE_STUDY_HPP__

#include <fwMedData/Study.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Study
 * @brief Study Information Entity class
 */
class GDCMIO_CLASS_API Study : public ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Study >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read study.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] study Study data
     */
    GDCMIO_API Study(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                     SPTR(::gdcm::Reader)reader,
                     SPTR(::gdcmIO::container::DicomInstance)instance,
                     ::fwMedData::Study::sptr study);

    /// Destructor
    GDCMIO_API virtual ~Study();

    /**
     * @brief Read General Study Module tags
     * @see PS 3.3 C.7.2.1
     */
    GDCMIO_API virtual void readGeneralStudyModule();

    /**
     * @brief Read Patient Study Module tags
     * @see PS 3.3 C.7.2.2
     */
    GDCMIO_API virtual void readPatientStudyModule();

};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_IE_STUDY_HPP__ */
