/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_STUDY_HPP__
#define __GDCMIO_WRITER_IE_STUDY_HPP__

#include <fwMedData/Study.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class Study
 * @brief Study Information Entity class
 */
class GDCMIO_CLASS_API Study : public ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::Study >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] study Study data
     */
    GDCMIO_API Study(SPTR(::gdcm::Writer)writer,
                     SPTR(::gdcmIO::container::DicomInstance)instance,
                     ::fwMedData::Study::sptr study);

    /// Destructor
    GDCMIO_API virtual ~Study();

    /**
     * @brief Write General Study Module tags
     * @see PS 3.3 C.7.2.1
     */
    GDCMIO_API virtual void writeGeneralStudyModule();

    /**
     * @brief Write Patient Study Module tags
     * @see PS 3.3 C.7.2.2
     */
    GDCMIO_API virtual void writePatientStudyModule();

};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_IE_STUDY_HPP__ */
