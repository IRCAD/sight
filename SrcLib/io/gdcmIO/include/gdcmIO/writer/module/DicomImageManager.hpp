/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMIMAGEMANAGER_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMIMAGEMANAGER_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/writer/module/DicomModule.hxx"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class handles image storage in DICOM file(s).
 *
 * It will call generic or specific image writers (like DicomCTImageWriter)
 * depending on the modality of series.
 *
 * @class   DicomImageManager
 */
class GDCMIO_CLASS_API DicomImageManager : public DicomModule< ::fwData::Image >
{

public :

    GDCMIO_API DicomImageManager();

    GDCMIO_API ~DicomImageManager();

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

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_MODULE_DICOMIMAGEMANAGER_HPP__
