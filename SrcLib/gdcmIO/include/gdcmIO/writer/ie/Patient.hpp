/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_PATIENT_HPP__
#define __GDCMIO_WRITER_IE_PATIENT_HPP__

#include <fwMedData/Patient.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class Patient
 * @brief Patient Information Entity class
 */
class GDCMIO_CLASS_API Patient : public ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::Patient >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] patient Patient data
     */
    GDCMIO_API Patient(SPTR(::gdcm::Writer) writer,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwMedData::Patient::sptr patient);

    /// Destructor
    GDCMIO_API virtual ~Patient();

    /**
     * @brief Write Patient Module tags
     * @see PS 3.3 C.7.1.1
     */
    GDCMIO_API virtual void writePatientModule();
};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_IE_PATIENT_HPP__
