/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMIMAGEWRITERMANAGER_HPP_
#define _GDCMIO_DICOMIMAGEWRITERMANAGER_HPP_

#include <fwData/Acquisition.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class handles image storage in DICOM file(s).
 *
 * It will call generic or specific image writers (like DicomCTImageWriter)
 * depending on the modality of series.
 *
 * @class   DicomImageWriterManager
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomImageWriterManager : public DicomModuleWriter< ::fwData::Acquisition >
{

public :

    GDCMIO_API DicomImageWriterManager();

    GDCMIO_API ~DicomImageWriterManager();

    /**
     * @brief   Load and start appropriate image writing tools.
     * This is one of last managers of the writing chain. The loaded tools will
     * write one or more files.
     *
     * @note    Currently, it handles common image writing.
     *          So, created image does not follow properly the DICOM norm.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     * @param   a_path  Folder path where image will be saved.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path);

};

}   // namespace writer

}   // namespace gdcmIO

#endif // _GDCMIO_DICOMIMAGEWRITERMANAGER_HPP_
