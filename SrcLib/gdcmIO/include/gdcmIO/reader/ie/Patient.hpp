/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_PATIENT_HPP__
#define __GDCMIO_READER_IE_PATIENT_HPP__

#include <fwMedData/Patient.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Patient
 * @brief Patient Information Entity class
 */
class GDCMIO_CLASS_API Patient : public ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Patient >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read patient.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] patient Patient data
     */
    GDCMIO_API Patient(SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwMedData::Patient::sptr patient);

    /// Destructor
    GDCMIO_API virtual ~Patient();

    /**
     * @brief Read Patient Module tags
     * @see PS 3.3 C.7.1.1
     */
    GDCMIO_API virtual void readPatientModule();
};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif // __GDCMIO_READER_IE_PATIENT_HPP__
