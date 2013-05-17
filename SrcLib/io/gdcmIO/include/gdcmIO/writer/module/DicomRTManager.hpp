/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMRTMANAGER_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMRTMANAGER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/module/DicomModule.hxx"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class handles RT documents.
 *
 * It will load and start appropriated writing tools.
 *
 * @note    Currently, it just handles Surface Segmentation Storage.
 *
 * @class   DicomRTManager
 */
class GDCMIO_CLASS_API DicomRTManager : public DicomModule< ::fwMedData::ModelSeries >
{

public :

    GDCMIO_API DicomRTManager();

    GDCMIO_API ~DicomRTManager();

    /**
     * @brief   Load and start appropriate image writing tools.
     *
     * This is one last manager of the writing chain. The loaded tools will
     * write one or more files.
     *
     * @note    Currently, it just writes one RT document.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     * @param   a_path  Folder path where RT document will be saved.
     */
    GDCMIO_API void write(::gdcm::DataSet &                 a_gDs,
                          const ::boost::filesystem::path & a_path) throw(::gdcmIO::exception::Failed);

};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_MODULE_DICOMRTMANAGER_HPP__
