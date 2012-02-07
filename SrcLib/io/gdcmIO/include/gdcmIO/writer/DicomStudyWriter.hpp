/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSTUDYWRITER_HPP_
#define _GDCMIO_DICOMSTUDYWRITER_HPP_

#include <fwData/Study.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"
#include "gdcmIO/DicomInstance.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class write DICOM study Modules in a data set.
 *
 * @class   DicomStudyWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomStudyWriter : public DicomModuleWriter< ::fwData::Study >
{

public:

    GDCMIO_API DicomStudyWriter();

    GDCMIO_API virtual ~DicomStudyWriter();

    /**
     * @brief   Write General Study Module in a data set.
     *
     * @see     PS 3.3 C.7.2.1
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API virtual void write(::gdcm::DataSet & a_gDs);


    GDCMIO_API fwGettersSettersDocMacro(StudyID, studyID, std::string, The index of study);

    /**
     * @brief   Set the index of the study to save.
     */
    GDCMIO_API void setStudyID(const unsigned int a_studyID);

private:

    std::string m_studyID;  ///< Index of the study (eg : 1 or 2 or ...)
};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMSTUDYWRITER_HPP_ */
