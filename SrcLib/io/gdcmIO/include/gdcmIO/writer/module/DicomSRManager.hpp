/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMSRMANAGER_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMSRMANAGER_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/writer/module/DicomModule.hxx"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class handles SR content to save.
 *
 * It will call SR writer.
 *
 * @class   DicomSRManager
 */
class GDCMIO_CLASS_API DicomSRManager : public DicomModule< ::fwData::Image >
{

public :

    GDCMIO_API DicomSRManager();

    GDCMIO_API ~DicomSRManager();

    /**
     * @brief   Load and start appropriate SR writing tools.
     *
     * This is one of last managers of the writing chain. The loaded tools will
     * write one file.
     *
     * @note    Currently, it just writes one SR document.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     * @param   a_path  Folder path where SR document will be saved.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path);

};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_MODULE_DICOMSRMANAGER_HPP__
