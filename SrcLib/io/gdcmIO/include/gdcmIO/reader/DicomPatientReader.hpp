/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMPATIENTREADER_HPP_
#define _GDCMIO_DICOMPATIENTREADER_HPP_

#include <fwData/Patient.hpp>

#include "gdcmIO/reader/DicomFilesReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class read a complete tree of a patient from DICOM file(s).
 *
 * It will sort studies of the patient, launch DicomStudyReader and read
 * patient information.
 *
 * @class   DicomPatientReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomPatientReader : public DicomFilesReader< ::fwData::Patient >
{

public :

    GDCMIO_API DicomPatientReader();

    GDCMIO_API ~DicomPatientReader();

    /**
     * @brief   Read one patient from DICOM file.
     * It will sort studies of patient. Then, it loads and
     * starts study reading tools.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw(::fwTools::Failed);

private :

    /**
     * @brief   Read patient data from a DICOM file.
     *
     * @note    setReader() have to be called before this method.
     */
    void readPatient();

};

}  // namespace reader

}  // namespace gdcmIO

#endif /*_GDCMIO_DICOMPATIENTREADER_HPP_*/
