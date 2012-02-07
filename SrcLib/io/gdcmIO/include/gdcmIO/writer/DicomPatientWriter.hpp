/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMPATIENTWRITER_HPP_
#define _GDCMIO_DICOMPATIENTWRITER_HPP_

#include <fwData/Patient.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class write DICOM patient modules in a data set.
 *
 * @class   DicomPatientWriter
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomPatientWriter : public DicomModuleWriter< ::fwData::Patient >
{

public :

    GDCMIO_API DicomPatientWriter();

    GDCMIO_API ~DicomPatientWriter();

    /**
     * @brief   Write General Patient Module in a data set.
     *
     * @see     PS 3.3 C.7.1.1
     *
     * @note    setObject() have to be called before this method.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API virtual void write(::gdcm::DataSet & a_gDs);
};

}   // namespace writer

}   // namespace gdcmIO

#endif // _GDCMIO_DICOMPATIENTWRITER_HPP_
