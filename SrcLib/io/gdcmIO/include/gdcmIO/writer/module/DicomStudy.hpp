/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMSTUDY_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMSTUDY_HPP__

#include <fwMedData/Study.hpp>

#include "gdcmIO/writer/module/DicomModule.hxx"
#include "gdcmIO/container/DicomInstance.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class write DICOM study Modules in a data set.
 *
 * @class   DicomStudy
 */
class GDCMIO_CLASS_API DicomStudy : public DicomModule< ::fwMedData::Study >
{

public:

    GDCMIO_API DicomStudy();

    GDCMIO_API virtual ~DicomStudy();

    /**
     * @brief   Write General Study Module in a data set.
     *
     * @see     PS 3.3 C.7.2.1
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API virtual void write(::gdcm::DataSet & a_gDs);

    /**
     * @brief   Set the index of the study to save.
     */
    GDCMIO_API void setStudyID(const unsigned int a_studyID);

};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_MODULE_DICOMSTUDY_HPP__ */
