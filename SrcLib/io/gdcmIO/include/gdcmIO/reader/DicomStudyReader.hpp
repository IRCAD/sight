/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSTUDYREADER_HPP_
#define _GDCMIO_DICOMSTUDYREADER_HPP_

#include <fwData/Study.hpp>

#include "gdcmIO/reader/DicomFilesReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class read a complete tree of a study from DICOM file.
 *
 * It will sort series of the study, launch DicomSeriesReader and read
 * study information.
 *
 * @class   DicomStudyReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomStudyReader : public DicomFilesReader< ::fwData::Study >
{

public :

    GDCMIO_API DicomStudyReader();

    GDCMIO_API ~DicomStudyReader();

    /**
     * @brief   Read one study from DICOM file.
     *
     * It will sort series of study. Then, it loads and
     * starts series reading tools.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw(::fwTools::Failed);

private :

    /**
     * @brief   Read a study from a DICOM file.
     *
     * @note    setReader() have to be called before this method.
     */
    void readStudy();

};

}  // namespace reader

}  // namespace gdcmIO

#endif /*_GDCMIO_DICOMSTUDYREADER_HPP_*/
