/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_PATIENT_HPP__
#define __FWGDCMIO_WRITER_IE_PATIENT_HPP__

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwMedData/Patient.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Patient Information Entity class
 */
class FWGDCMIO_CLASS_API Patient : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Patient >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] patient Patient data
     */
    FWGDCMIO_API Patient(SPTR(::gdcm::Writer)writer,
                         SPTR(::fwGdcmIO::container::DicomInstance)instance,
                         ::fwMedData::Patient::sptr patient);

    /// Destructor
    FWGDCMIO_API virtual ~Patient();

    /**
     * @brief Write Patient Module tags
     * @see PS 3.3 C.7.1.1
     */
    FWGDCMIO_API virtual void writePatientModule();
};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IE_PATIENT_HPP__
